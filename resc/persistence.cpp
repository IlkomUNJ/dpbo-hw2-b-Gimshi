#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <memory>
#include <string>

#include "persistence.h"
#include "bank_customer.h"
#include "buyer.h"
#include "seller.h"
#include "transaction.h"

namespace fs = std::filesystem;

namespace store {

static std::string safe(const std::string &s) {
    std::string out = s;
    for (auto &c : out) if (c == '\n') c = ' ';
    return out;
}

bool ensure_data_dir(const std::string &path) {
    std::error_code ec;
    fs::create_directories(path, ec);
    return !ec;
}

bool save_all(const AppState &state, const std::string &path) {
    if (!ensure_data_dir(path)) return false;

    // buyers.txt: id|name|email|phone|address|hasAccount
    {
        std::ofstream f(path + "/buyers.txt");
        for (const auto &b : state.buyers) {
            f << b.getId() << '|' << safe(b.getName()) << '|' << safe(b.getEmail()) << '|' 
              << safe(b.getPhone()) << '|' << safe(b.getAddress()) << '|' 
              << ((b.getAccount() && b.getAccount()->getId()!=0)?1:0) << "\n";
        }
    }

    // sellers.txt: buyerId|sellerId|storeName
    {
        std::ofstream f(path + "/sellers.txt");
        for (const auto &s : state.sellers) {
            f << s.getId() << '|' << s.getSellerId() << '|' << safe(s.getStoreName()) << "\n";
        }
    }

    // accounts.txt: id|name|balance
    {
        std::ofstream f(path + "/accounts.txt");
        for (const auto &acc : state.bankAccounts) {
            if (!acc) continue;
            f << acc->getId() << '|' << safe(acc->getName()) << '|' << acc->getBalance() << "\n";
        }
    }

    // items.txt: sellerId|itemId|name|qty|price
    {
        std::ofstream f(path + "/items.txt");
        for (const auto &s : state.sellers) {
            for (const auto &item : s.getItems()) {
                f << s.getSellerId() << '|' << item.getId() << '|' << safe(item.getName()) << '|' << item.getQuantity() << '|' << item.getPrice() << "\n";
            }
        }
    }

    // transactions.txt: id|buyerId|buyerName|sellerId|sellerName|total|status|date
    {
        std::ofstream f(path + "/transactions.txt");
        for (const auto &t : state.transactions) {
            f << t.getTransactionId() << '|' << t.getBuyerId() << '|' << safe(t.getBuyerName()) << '|'
              << t.getSellerId() << '|' << safe(t.getSellerName()) << '|'
              << t.getTotalAmount() << '|' << t.getStatus() << '|' << safe(t.getDate()) << "\n";
        }
    }

    return true;
}

bool load_all(AppState& state, const std::string& path) {
    bool any = false;

    // accounts.txt
    {
        std::ifstream f(path + "/accounts.txt");
        if (f) {
            any = true;
            std::string line;
            while (std::getline(f, line)) {
                if (line.empty() || line[0] == '#') continue;
                std::istringstream iss(line);
                std::string tok;
                std::vector<std::string> cols;
                while (std::getline(iss, tok, '|')) cols.push_back(tok);
                if (cols.size() >= 3) {
                    int id = std::stoi(cols[0]);
                    std::string name = cols[1];
                    double bal = std::stod(cols[2]);
                    state.bankAccounts.push_back(std::make_unique<BankCustomer>(id, name, bal));
                }
            }
        }
    }

    // buyers.txt
    {
        std::ifstream f(path + "/buyers.txt");
        if (f) {
            any = true;
            std::string line;
            while (std::getline(f, line)) {
                if (line.empty() || line[0] == '#') continue;
                std::istringstream iss(line);
                std::string tok;
                std::vector<std::string> cols;
                while (std::getline(iss, tok, '|')) cols.push_back(tok);
                if (cols.size() >= 6) {
                    int id = std::stoi(cols[0]);
                    std::string name = cols[1];
                    std::string email = cols[2];
                    std::string phone = cols[3];
                    std::string address = cols[4];
                    int hasAcc = std::stoi(cols[5]);
                    BankCustomer* accPtr = nullptr;
                    if (hasAcc) {
                        for (auto &acc : state.bankAccounts) if (acc && acc->getId() == id) { accPtr = acc.get(); break; }
                    }
                    state.buyers.emplace_back(id, name, email, phone, address, accPtr);
                }
            }
        }
    }

    // sellers.txt
    {
        std::ifstream f(path + "/sellers.txt");
        if (f) {
            any = true;
            std::string line;
            while (std::getline(f, line)) {
                if (line.empty() || line[0] == '#') continue;
                std::istringstream iss(line);
                std::string tok;
                std::vector<std::string> cols;
                while (std::getline(iss, tok, '|')) cols.push_back(tok);
                if (cols.size() >= 3) {
                    int buyerId = std::stoi(cols[0]);
                    int sellerId = std::stoi(cols[1]);
                    std::string storeName = cols[2];
                    // find buyer
                    auto itb = std::find_if(state.buyers.begin(), state.buyers.end(), [buyerId](const Buyer& b){ return b.getId()==buyerId; });
                    if (itb != state.buyers.end()) {
                        state.sellers.emplace_back(*itb, sellerId, storeName);
                    }
                }
            }
        }
    }

    // transactions.txt
    {
        std::ifstream f(path + "/transactions.txt");
        if (f) {
            std::string line;
            while (std::getline(f, line)) {
                if (line.empty() || line[0] == '#') continue;
                std::istringstream iss(line);
                std::string tok;
                std::vector<std::string> cols;
                while (std::getline(iss, tok, '|')) cols.push_back(tok);
                if (cols.size() >= 8) {
                    // int id = std::stoi(cols[0]);  // Transaction ID not used for reconstruction
                    int buyerId = std::stoi(cols[1]);
                    std::string buyerName = cols[2];
                    int sellerId = std::stoi(cols[3]);
                    std::string sellerName = cols[4];
                    // double total = std::stod(cols[5]);  // Total calculated from items, not loaded
                    // std::string status = cols[6];  // Status not used in current constructor
                    std::string date = cols[7];
                    state.transactions.push_back(Transaction(buyerId, buyerName, sellerId, sellerName, date));
                }
            }
        }
    }

    // items.txt
    {
        std::ifstream f(path + "/items.txt");
        if (f) {
            std::string line;
            while (std::getline(f, line)) {
                if (line.empty() || line[0] == '#') continue;
                std::istringstream iss(line);
                std::string tok;
                std::vector<std::string> cols;
                while (std::getline(iss, tok, '|')) cols.push_back(tok);
                if (cols.size() >= 5) {
                    int sellerId = std::stoi(cols[0]);
                    int itemId = std::stoi(cols[1]);
                    std::string name = cols[2];
                    int qty = std::stoi(cols[3]);
                    double price = std::stod(cols[4]);
                    auto its = std::find_if(state.sellers.begin(), state.sellers.end(), [sellerId](const seller& s){ return s.getSellerId()==sellerId; });
                    if (its != state.sellers.end()) {
                        its->addNewItem(itemId, name, qty, price);
                    }
                }
            }
        }
    }

    return any;
}

} // namespace store
