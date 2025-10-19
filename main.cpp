#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include "resc/seller.h"
#include "resc/bank_customer.h"
#include "resc/buyer.h"
#include "resc/transaction.h"
#include "resc/datetime.h"
#include "resc/persistence.h"

enum PrimaryPrompt { LOGIN, REGISTER, EXIT_MAIN };

using namespace std;

void showBuyerMenu(Buyer* buyer, vector<seller>& sellers, vector<Transaction>& pendingOrders, 
                   vector<Transaction>& transactions, vector<Buyer>& buyers, 
                   vector<unique_ptr<BankCustomer>>& bankAccounts, int& sellerIdCounter);
void showSellerMenu(seller* sellerAccount, vector<seller>& sellers, vector<Transaction>& transactions, 
                    vector<Transaction>& pendingOrders, vector<Buyer>& buyers,
                    vector<unique_ptr<BankCustomer>>& bankAccounts);

int main() {
    vector<Buyer> buyers;
    vector<unique_ptr<BankCustomer>> bankAccounts;
    vector<Transaction> transactions;
    vector<Transaction> pendingOrders;
    vector<seller> sellers;

    AppState state;
    store::load_all(state, "database");
    buyers = state.buyers;
    sellers = state.sellers;
    bankAccounts.clear();
    for (auto &acc : state.bankAccounts) bankAccounts.push_back(move(acc));
    transactions = state.transactions;
    pendingOrders = state.pendingOrders;

    int buyerIdCounter = buyers.empty() ? 1 : (buyers.back().getId() + 1);
    int sellerIdCounter = sellers.empty() ? 1 : (sellers.back().getSellerId() + 1);

    PrimaryPrompt prompt = LOGIN;
    while (prompt != EXIT_MAIN) {
        cout << "\n========================================" << endl;
        cout << "   MARKETPLACE SYSTEM - MAIN MENU" << endl;
        cout << "========================================" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Exit" << endl;
        cout << "Select option: ";
        
        int choice;
        cin >> choice;
        prompt = static_cast<PrimaryPrompt>(choice - 1);

        switch (prompt) {
            case LOGIN: {
                int loginId;
                string loginName;
                
                cout << "\n=== LOGIN ===" << endl;
                cout << "Enter your ID: ";
                cin >> loginId;
                cin.ignore();
                cout << "Enter your Name: ";
                getline(cin, loginName);

                // Check if user is a SELLER
                auto sellerIt = find_if(sellers.begin(), sellers.end(),
                    [loginId, &loginName](const seller &s) {
                        return s.getId() == loginId && s.getName() == loginName;
                    });

                if (sellerIt != sellers.end()) {
                    // LOGIN AS SELLER
                    cout << "\n--- Login successful! Welcome, " << sellerIt->getName() << " (SELLER) ---" << endl;
                    cout << "Store: " << sellerIt->getStoreName() << endl;
                    showSellerMenu(&(*sellerIt), sellers, transactions, pendingOrders, buyers, bankAccounts);
                    continue;
                }

                // Check if user is a BUYER
                auto buyerIt = find_if(buyers.begin(), buyers.end(),
                    [loginId, &loginName](const Buyer &b) {
                        return b.getId() == loginId && b.getName() == loginName;
                    });

                if (buyerIt != buyers.end()) {
                    // LOGIN AS BUYER
                    cout << "\n--- Login successful! Welcome, " << buyerIt->getName() << " (BUYER) ---" << endl;
                    showBuyerMenu(&(*buyerIt), sellers, pendingOrders, transactions, buyers, bankAccounts, sellerIdCounter);
                    continue;
                }

                // User not found
                cout << "\n[X] Account not found! Please register first." << endl;
                break;
            }

            case REGISTER: {
                string name, email, phone, address;
                cin.ignore();
                
                cout << "\n=== BUYER REGISTRATION ===" << endl;
                cout << "Enter Name: ";
                getline(cin, name);
                
                bool validEmail = false;
                while (!validEmail) {
                    cout << "Enter Email: ";
                    getline(cin, email);
                    if (Buyer::isValidEmail(email)) {
                        validEmail = true;
                    } else {
                        cout << "[X] Invalid email! Must contain '@'. Try again." << endl;
                    }
                }
                
                bool validPhone = false;
                while (!validPhone) {
                    cout << "Enter Phone: ";
                    getline(cin, phone);
                    if (Buyer::isValidPhone(phone)) {
                        validPhone = true;
                    } else {
                        cout << "[X] Invalid phone! Must be digits only. Try again." << endl;
                    }
                }
                
                cout << "Enter Address: ";
                getline(cin, address);
                
                int newBuyerId = buyerIdCounter++;
                buyers.emplace_back(newBuyerId, name, email, phone, address, nullptr);
                // Save after mutation
                AppState state;
                state.buyers = buyers;
                state.sellers = sellers;
                for (auto &acc : bankAccounts) state.bankAccounts.push_back(make_unique<BankCustomer>(*acc));
                state.transactions = transactions;
                state.pendingOrders = pendingOrders;
                store::save_all(state, "database");
                cout << "\n--- Buyer registered successfully! ---" << endl;
                cout << "Your ID: " << newBuyerId << endl;
                cout << "Name: " << name << endl;
                cout << "\nPlease login to continue." << endl;
                break;
            }

            case EXIT_MAIN:
                cout << "\nThank you for using Marketplace System!" << endl;
                break;

            default:
                cout << "\n[X] Invalid option!" << endl;
                break;
        }
    }

    return 0;
}


// ========================================
// BUYER MENU
// ========================================
void showBuyerMenu(Buyer* buyer, vector<seller>& sellers, vector<Transaction>& pendingOrders, 
                   vector<Transaction>& transactions, vector<Buyer>& buyers, 
                   vector<unique_ptr<BankCustomer>>& bankAccounts, int& sellerIdCounter) {
    bool logout = false;
    
    while (!logout) {
        cout << "\n========================================" << endl;
        cout << "   BUYER MENU - " << buyer->getName() << endl;
        cout << "========================================" << endl;
        cout << "1. Account Status" << endl;
        cout << "2. Create Bank Account" << endl;
        cout << "3. Top Up Balance" << endl;
        cout << "4. Browse Stores" << endl;
        cout << "5. Place Order" << endl;
        cout << "6. Payment (Complete Pending Orders)" << endl;
        cout << "7. Upgrade to Seller" << endl;
        cout << "8. Delete Account" << endl;
        cout << "9. Logout" << endl;
        cout << "Select option: ";
        
        int choice;
        cin >> choice;

        switch (choice) {
            case 1: { // Account Status
                cout << "\n=== ACCOUNT STATUS ===" << endl;
                cout << "ID: " << buyer->getId() << endl;
                cout << "Name: " << buyer->getName() << endl;
                cout << "Email: " << buyer->getEmail() << endl;
                cout << "Phone: " << buyer->getPhone() << endl;
                cout << "Address: " << buyer->getAddress() << endl;
                cout << "Role: BUYER" << endl;

                if (buyer->getAccount() && buyer->getAccount()->getId() != 0) {
                    cout << "\n--- Bank Account ---" << endl;
                    buyer->getAccount()->printInfo();
                } else {
                    cout << "\n--- Bank Account ---" << endl;
                    cout << "[X] No bank account linked." << endl;
                }
                break;
            }

            case 2: { // Create Bank Account
                cout << "\n=== CREATE BANK ACCOUNT ===" << endl;
                if (buyer->getAccount() && buyer->getAccount()->getId() != 0) {
                    cout << "[X] You already have a bank account!" << endl;
                    buyer->getAccount()->printInfo();
                    break;
                }
                
                double deposit;
                cout << "Enter initial deposit: $";
                cin >> deposit;
                
                // Warn if creating dormant account
                if (deposit <= 0) {
                    cout << "\n⚠️  WARNING: Creating account with $0 balance." << endl;
                    cout << "    Your account will be DORMANT until you deposit money." << endl;
                }
                
                auto newBank = make_unique<BankCustomer>(buyer->getId(), buyer->getName(), deposit);
                bankAccounts.push_back(move(newBank));
                for (auto& b : buyers) {
                    if (b.getId() == buyer->getId()) {
                        b.setAccount(bankAccounts.back().get());
                        buyer = &b;
                        break;
                    }
                }
                // Save after mutation
                AppState state;
                state.buyers = buyers;
                state.sellers = sellers;
                for (auto &acc : bankAccounts) state.bankAccounts.push_back(make_unique<BankCustomer>(*acc));
                state.transactions = transactions;
                state.pendingOrders = pendingOrders;
                store::save_all(state, "database");
                cout << "\n--- Bank account created successfully! ---" << endl;
                buyer->getAccount()->printInfo();
                break;
            }

            case 3: { // Top Up Balance
                cout << "\n=== TOP UP BALANCE ===" << endl;
                if (!buyer->getAccount() || buyer->getAccount()->getId() == 0) {
                    cout << "[X] You don't have a bank account! Create one first." << endl;
                    break;
                }
                
                cout << "Current balance: $" << buyer->getAccount()->getBalance() << endl;
                if (buyer->getAccount()->isDormant()) {
                    cout << "⚠️  Your account is DORMANT. Please deposit to reactivate." << endl;
                }
                
                double topUpAmount;
                cout << "\nEnter amount to deposit: $";
                cin >> topUpAmount;
                
                if (topUpAmount <= 0) {
                    cout << "[X] Invalid amount! Must be greater than 0." << endl;
                    break;
                }
                
                buyer->getAccount()->addBalance(topUpAmount);
                
                // Save after mutation
                AppState state;
                state.buyers = buyers;
                state.sellers = sellers;
                for (auto &acc : bankAccounts) state.bankAccounts.push_back(make_unique<BankCustomer>(*acc));
                state.transactions = transactions;
                state.pendingOrders = pendingOrders;
                store::save_all(state, "database");
                
                cout << "\n--- Deposit successful! ---" << endl;
                cout << "New balance: $" << buyer->getAccount()->getBalance() << endl;
                break;
            }

            case 4: { // Browse Stores
                cout << "\n=== BROWSE STORES ===" << endl;
                if (sellers.empty()) {
                    cout << "[X] No sellers available yet." << endl;
                    break;
                }
                
                cout << "\nAvailable Sellers:" << endl;
                for (size_t i = 0; i < sellers.size(); i++) {
                    cout << (i + 1) << ". " << sellers[i].getStoreName() 
                         << " (Seller ID: " << sellers[i].getSellerId() << ")" << endl;
                }
                
                cout << "\nEnter seller number (0 to cancel): ";
                int sellerChoice;
                cin >> sellerChoice;
                
                if (sellerChoice > 0 && sellerChoice <= static_cast<int>(sellers.size())) {
                    const seller& chosenSeller = sellers[sellerChoice - 1];
                    const auto& items = chosenSeller.getItems();
                    
                    cout << "\n=== " << chosenSeller.getStoreName() << " Inventory ===" << endl;
                    if (items.empty()) {
                        cout << "[X] No items available in this store." << endl;
                    } else {
                        cout << "ID\tName\t\tQuantity\tPrice" << endl;
                        cout << "------------------------------------------------" << endl;
                        for (const auto& item : items) {
                            cout << item.getId() << "\t" 
                                 << item.getName() << "\t\t" 
                                 << item.getQuantity() << "\t\t$" 
                                 << item.getPrice() << endl;
                        }
                    }
                }
                break;
            }

            case 5: { // Place Order
                cout << "\n=== PLACE ORDER ===" << endl;
                if (sellers.empty()) {
                    cout << "[X] No sellers available yet." << endl;
                    break;
                }
                
                cout << "\nAvailable Sellers:" << endl;
                for (size_t i = 0; i < sellers.size(); i++) {
                    cout << (i + 1) << ". " << sellers[i].getStoreName() << endl;
                }
                
                cout << "\nSelect seller (0 to cancel): ";
                int selChoice;
                cin >> selChoice;
                
                if (selChoice <= 0 || selChoice > static_cast<int>(sellers.size())) {
                    cout << "Cancelled." << endl;
                    break;
                }
                
                seller& chosenSeller = sellers[selChoice - 1];
                const auto& items = chosenSeller.getItems();
                
                if (items.empty()) {
                    cout << "[X] This store has no items." << endl;
                    break;
                }
                
                cout << "\n=== " << chosenSeller.getStoreName() << " Items ===" << endl;
                cout << "ID\tName\t\tQuantity\tPrice" << endl;
                cout << "------------------------------------------------" << endl;
                for (const auto& item : items) {
                    cout << item.getId() << "\t" 
                         << item.getName() << "\t\t" 
                         << item.getQuantity() << "\t\t$" 
                         << item.getPrice() << endl;
                }
                
                string today = dt::today();  // Automatically gets today's date from system
                Transaction newOrder(buyer->getId(), buyer->getName(),
                                   chosenSeller.getSellerId(), chosenSeller.getStoreName(), today);
                
                char addMore = 'y';
                while (addMore == 'y' || addMore == 'Y') {
                    int itemId, qty;
                    cout << "\nEnter Item ID: ";
                    cin >> itemId;
                    
                    auto& mutableItems = chosenSeller.getItems();
                    auto itemIt = find_if(mutableItems.begin(), mutableItems.end(),
                        [itemId](const Item& i) { return i.getId() == itemId; });
                    
                    if (itemIt == mutableItems.end()) {
                        cout << "[X] Item not found!" << endl;
                    } else {
                        cout << "Enter quantity: ";
                        cin >> qty;
                        
                        if (qty > itemIt->getQuantity()) {
                            cout << "[X] Not enough stock! Available: " << itemIt->getQuantity() << endl;
                        } else {
                            newOrder.addItem(itemIt->getId(), itemIt->getName(), qty, itemIt->getPrice());
                            itemIt->setQuantity(itemIt->getQuantity() - qty);
                            cout << "--- Added to order! ---" << endl;
                        }
                    }
                    
                    cout << "Add more items? (y/n): ";
                    cin >> addMore;
                }
                
                if (newOrder.getItems().empty()) {
                    cout << "No items ordered." << endl;
                } else {
                    pendingOrders.push_back(newOrder);
                    cout << "\n--- Order created! Go to Payment to complete. ---" << endl;
                    newOrder.printTransactionDetails();
                }
                break;
            }

            case 6: { // Payment
                cout << "\n=== PAYMENT ===" << endl;
                if (pendingOrders.empty()) {
                    cout << "[X] No pending orders." << endl;
                    break;
                }
                
                vector<Transaction*> buyerOrders;
                for (auto& order : pendingOrders) {
                    if (order.getBuyerId() == buyer->getId()) {
                        buyerOrders.push_back(&order);
                    }
                }
                
                if (buyerOrders.empty()) {
                    cout << "[X] You have no pending orders." << endl;
                    break;
                }
                
                cout << "\nYour Pending Orders:" << endl;
                for (size_t i = 0; i < buyerOrders.size(); i++) {
                    cout << (i + 1) << ". Transaction #" << buyerOrders[i]->getTransactionId()
                         << " - $" << buyerOrders[i]->getTotalAmount()
                         << " to " << buyerOrders[i]->getSellerName() << endl;
                }
                
                cout << "\nSelect order to pay (0 to cancel): ";
                int orderChoice;
                cin >> orderChoice;
                
                if (orderChoice <= 0 || orderChoice > static_cast<int>(buyerOrders.size())) {
                    cout << "Cancelled." << endl;
                    break;
                }
                
                Transaction* orderToPay = buyerOrders[orderChoice - 1];
                
                if (!buyer->getAccount() || buyer->getAccount()->getId() == 0) {
                    cout << "[X] You don't have a bank account! Create one first." << endl;
                    break;
                }
                
                // Check if account is dormant
                if (buyer->getAccount()->isDormant()) {
                    cout << "\n❌ PAYMENT BLOCKED: Your account is DORMANT!" << endl;
                    cout << "   Current balance: $" << buyer->getAccount()->getBalance() << endl;
                    cout << "   Please deposit money first to reactivate your account." << endl;
                    cout << "\n   💡 Tip: Use 'Account Status' to see your account details." << endl;
                    break;
                }
                
                if (buyer->getAccount()->getBalance() < orderToPay->getTotalAmount()) {
                    cout << "[X] Insufficient balance! You have $" 
                         << buyer->getAccount()->getBalance()
                         << " but need $" << orderToPay->getTotalAmount() << endl;
                    break;
                }
                
                cout << "\n=== Payment Confirmation ===" << endl;
                orderToPay->printTransactionDetails();
                cout << "\nYour current balance: $" << buyer->getAccount()->getBalance() << endl;
                cout << "Confirm payment? (y/n): ";
                char confirm;
                cin >> confirm;
                
                if (confirm == 'y' || confirm == 'Y') {
                    buyer->getAccount()->withdrawBalance(orderToPay->getTotalAmount());
                    
                    auto sellerIt = find_if(sellers.begin(), sellers.end(),
                        [orderToPay](const seller& s) { 
                            return s.getSellerId() == orderToPay->getSellerId(); 
                        });
                    
                    if (sellerIt != sellers.end() && sellerIt->getAccount()) {
                        sellerIt->getAccount()->addBalance(orderToPay->getTotalAmount());
                    }
                    
                    orderToPay->setStatus(PAID);
                    transactions.push_back(*orderToPay);
                    
                    pendingOrders.erase(remove_if(pendingOrders.begin(), pendingOrders.end(),
                        [orderToPay](const Transaction& t) {
                            return t.getTransactionId() == orderToPay->getTransactionId();
                        }), pendingOrders.end());
                    
                    cout << "\n--- Payment successful! ---" << endl;
                    cout << "New balance: $" << buyer->getAccount()->getBalance() << endl;
                    
                    // Save all data after payment
                    AppState tempState;
                    tempState.buyers = buyers;
                    tempState.sellers = sellers;
                    tempState.bankAccounts = move(bankAccounts);
                    tempState.transactions = transactions;
                    tempState.pendingOrders = pendingOrders;
                    store::save_all(tempState, "database");
                    bankAccounts = move(tempState.bankAccounts);
                } else {
                    cout << "Payment cancelled." << endl;
                }
                break;
            }

            case 7: { // Upgrade to Seller
                cout << "\n=== UPGRADE TO SELLER ===" << endl;
                
                // Check if already seller
                auto checkSeller = find_if(sellers.begin(), sellers.end(),
                    [buyer](const seller& s) { return s.getId() == buyer->getId(); });
                
                if (checkSeller != sellers.end()) {
                    cout << "[X] You are already a seller!" << endl;
                    cout << "Please logout and login again to access seller features." << endl;
                    break;
                }
                
                if (!buyer->getAccount() || buyer->getAccount()->getId() == 0) {
                    cout << "[X] Cannot upgrade! You need a bank account first." << endl;
                    break;
                }
                
                string storeName;
                cin.ignore();
                cout << "Enter Store Name: ";
                getline(cin, storeName);
                
                int newSellerId = sellerIdCounter++;
                sellers.emplace_back(*buyer, newSellerId, storeName);
                // Save after mutation
                AppState state;
                state.buyers = buyers;
                state.sellers = sellers;
                for (auto &acc : bankAccounts) state.bankAccounts.push_back(make_unique<BankCustomer>(*acc));
                state.transactions = transactions;
                state.pendingOrders = pendingOrders;
                store::save_all(state, "database");
                cout << "\n--- Successfully upgraded to Seller! ---" << endl;
                cout << "Seller ID: " << newSellerId << endl;
                cout << "Store Name: " << storeName << endl;
                cout << "\n>>> Please LOGOUT and LOGIN again to access seller features. <<<" << endl;
                break;
            }

            case 8: { // Delete Account
                cout << "\n=== DELETE ACCOUNT ===" << endl;
                char confirm;
                cout << "Are you sure? This cannot be undone! (y/N): ";
                cin >> confirm;
                
                if (confirm == 'y' || confirm == 'Y') {
                    int delId = buyer->getId();
                    buyers.erase(remove_if(buyers.begin(), buyers.end(),
                        [delId](const Buyer& b) { return b.getId() == delId; }), buyers.end());
                    sellers.erase(remove_if(sellers.begin(), sellers.end(),
                        [delId](const seller& s) { return s.getId() == delId; }), sellers.end());
                    bankAccounts.erase(remove_if(bankAccounts.begin(), bankAccounts.end(),
                        [delId](const unique_ptr<BankCustomer>& acc) { return acc->getId() == delId; }), bankAccounts.end());
                    cout << "\n--- Account deleted. ---" << endl;
                    logout = true;
                } else {
                    cout << "Cancelled." << endl;
                }
                break;
            }

            case 9: { // Logout
                cout << "\n--- Logged out successfully. ---" << endl;
                logout = true;
                break;
            }

            default:
                cout << "\n[X] Invalid option!" << endl;
                break;
        }
    }
}


// ========================================
// SELLER MENU
// ========================================
void showSellerMenu(seller* sellerAccount, vector<seller>& sellers, vector<Transaction>& transactions, 
                    vector<Transaction>& pendingOrders, vector<Buyer>& buyers,
                    vector<unique_ptr<BankCustomer>>& bankAccounts) {
    bool logout = false;
    
    while (!logout) {
        cout << "\n========================================" << endl;
        cout << "   SELLER MENU - " << sellerAccount->getStoreName() << endl;
        cout << "========================================" << endl;
        cout << "1. Account Status" << endl;
        cout << "2. Check Inventory" << endl;
        cout << "3. Add Item to Inventory" << endl;
        cout << "4. Remove Item from Inventory" << endl;
        cout << "5. View All Orders" << endl;
        cout << "6. Delete Account" << endl;
        cout << "7. Logout" << endl;
        cout << "Select option: ";
        
        int choice;
        cin >> choice;

        switch (choice) {
            case 1: { // Account Status
                cout << "\n=== ACCOUNT STATUS ===" << endl;
                cout << "Buyer ID: " << sellerAccount->getId() << endl;
                cout << "Seller ID: " << sellerAccount->getSellerId() << endl;
                cout << "Name: " << sellerAccount->getName() << endl;
                cout << "Email: " << sellerAccount->getEmail() << endl;
                cout << "Phone: " << sellerAccount->getPhone() << endl;
                cout << "Address: " << sellerAccount->getAddress() << endl;
                cout << "Role: SELLER" << endl;
                cout << "Store Name: " << sellerAccount->getStoreName() << endl;
                cout << "Inventory Items: " << sellerAccount->getItems().size() << endl;

                if (sellerAccount->getAccount() && sellerAccount->getAccount()->getId() != 0) {
                    cout << "\n--- Bank Account ---" << endl;
                    sellerAccount->getAccount()->printInfo();
                } else {
                    cout << "\n--- Bank Account ---" << endl;
                    cout << "[X] No bank account linked." << endl;
                }
                break;
            }

            case 2: { // Check Inventory
                cout << "\n=== INVENTORY ===" << endl;
                const auto& items = sellerAccount->getItems();
                if (items.empty()) {
                    cout << "[X] No items in inventory." << endl;
                } else {
                    cout << "ID\tName\t\tQuantity\tPrice" << endl;
                    cout << "------------------------------------------------" << endl;
                    for (const auto& item : items) {
                        cout << item.getId() << "\t" 
                             << item.getName() << "\t\t" 
                             << item.getQuantity() << "\t\t$" 
                             << item.getPrice() << endl;
                    }
                }
                break;
            }

            case 3: { // Add Item
                cout << "\n=== ADD ITEM ===" << endl;
                int id, qty;
                string name;
                double price;
                
                cout << "Item ID: ";
                cin >> id;
                cin.ignore();
                cout << "Item Name: ";
                getline(cin, name);
                cout << "Quantity: ";
                cin >> qty;
                cout << "Price: $";
                cin >> price;
                
                sellerAccount->addNewItem(id, name, qty, price);
                cout << "\n--- Item added to inventory! ---" << endl;
                
                // Save all data
                AppState tempState;
                tempState.buyers = buyers;
                tempState.sellers = sellers;
                tempState.bankAccounts = move(bankAccounts);
                tempState.transactions = transactions;
                store::save_all(tempState, "database");
                bankAccounts = move(tempState.bankAccounts);
                break;
            }

            case 4: { // Remove Item
                cout << "\n=== REMOVE ITEM ===" << endl;
                const auto& items = sellerAccount->getItems();
                
                if (items.empty()) {
                    cout << "[X] No items in inventory." << endl;
                    break;
                }
                
                cout << "\nCurrent Inventory:" << endl;
                for (const auto& item : items) {
                    cout << "ID: " << item.getId() << " - " << item.getName() << endl;
                }
                
                int id;
                cout << "\nEnter Item ID to remove: ";
                cin >> id;
                
                auto& mutableItems = sellerAccount->getItems();
                auto it = find_if(mutableItems.begin(), mutableItems.end(),
                    [id](const Item& i) { return i.getId() == id; });
                
                if (it != mutableItems.end()) {
                    mutableItems.erase(it);
                    cout << "\n--- Item removed! ---" << endl;
                    
                    // Save all data
                    AppState tempState;
                    tempState.buyers = buyers;
                    tempState.sellers = sellers;
                    tempState.bankAccounts = move(bankAccounts);
                    tempState.transactions = transactions;
                    store::save_all(tempState, "database");
                    bankAccounts = move(tempState.bankAccounts);
                } else {
                    cout << "\n[X] Item not found!" << endl;
                }
                break;
            }

            case 5: { // View Orders
                cout << "\n=== ALL ORDERS ===" << endl;
                
                vector<Transaction*> sellerTransactions;
                for (auto& t : transactions) {
                    if (t.getSellerId() == sellerAccount->getSellerId()) {
                        sellerTransactions.push_back(&t);
                    }
                }
                
                for (auto& t : pendingOrders) {
                    if (t.getSellerId() == sellerAccount->getSellerId()) {
                        sellerTransactions.push_back(&t);
                    }
                }
                
                if (sellerTransactions.empty()) {
                    cout << "[X] No orders yet." << endl;
                } else {
                    cout << "Total Orders: " << sellerTransactions.size() << endl;
                    for (auto* t : sellerTransactions) {
                        t->printTransactionDetails();
                    }
                }
                break;
            }

            case 6: { // Delete Account
                cout << "\n=== DELETE ACCOUNT ===" << endl;
                char confirm;
                cout << "Are you sure? This will delete both buyer and seller accounts! (y/N): ";
                cin >> confirm;
                
                if (confirm == 'y' || confirm == 'Y') {
                    int delId = sellerAccount->getId();
                    buyers.erase(remove_if(buyers.begin(), buyers.end(),
                        [delId](const Buyer& b) { return b.getId() == delId; }), buyers.end());
                    sellers.erase(remove_if(sellers.begin(), sellers.end(),
                        [delId](const seller& s) { return s.getId() == delId; }), sellers.end());
                    bankAccounts.erase(remove_if(bankAccounts.begin(), bankAccounts.end(),
                        [delId](const unique_ptr<BankCustomer>& acc) { return acc->getId() == delId; }), bankAccounts.end());
                    cout << "\n--- Account deleted. ---" << endl;
                    logout = true;
                } else {
                    cout << "Cancelled." << endl;
                }
                break;
            }

            case 7: { // Logout
                cout << "\n--- Logged out successfully. ---" << endl;
                logout = true;
                break;
            }

            default:
                cout << "\n[X] Invalid option!" << endl;
                break;
        }
    }
}
