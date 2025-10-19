#include "transaction.h"
#include "datetime.h"
#include <iostream>
#include <iomanip>

using namespace std;


int Transaction::nextTransactionId = 1;


Transaction::Transaction(int bId, const string& bName, int sId, const string& sName, const std::string& date)
    : buyerId(bId), buyerName(bName), sellerId(sId), sellerName(sName), 
      totalAmount(0.0), status(PENDING), date(date) {
    transactionId = nextTransactionId++;
    if (this->date.empty()) {
        // Default to today if not provided - automatically gets current date from system
        this->date = dt::today();
    }
}


int Transaction::getTransactionId() const {
    return transactionId;
}

int Transaction::getBuyerId() const {
    return buyerId;
}

string Transaction::getBuyerName() const {
    return buyerName;
}

int Transaction::getSellerId() const {
    return sellerId;
}

string Transaction::getSellerName() const {
    return sellerName;
}

const vector<TransactionItem>& Transaction::getItems() const {
    return items;
}

double Transaction::getTotalAmount() const {
    return totalAmount;
}

TransactionStatus Transaction::getStatus() const {
    return status;
}

string Transaction::getStatusString() const {
    switch (status) {
        case PENDING: return "PENDING";
        case PAID: return "PAID";
        case COMPLETED: return "COMPLETED";
        case CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}


void Transaction::setStatus(TransactionStatus newStatus) {
    status = newStatus;
}


void Transaction::addItem(int itemId, const string& itemName, int quantity, double price) {
    items.emplace_back(itemId, itemName, quantity, price);
    calculateTotal();
}

void Transaction::calculateTotal() {
    totalAmount = 0.0;
    for (const auto& item : items) {
        totalAmount += item.getTotalPrice();
    }
}

void Transaction::printTransactionDetails() const {
    cout << "\n=== Transaction #" << transactionId << " ===" << endl;
    cout << "Status: " << getStatusString() << endl;
    cout << "Buyer: " << buyerName << " (ID: " << buyerId << ")" << endl;
    cout << "Seller: " << sellerName << " (ID: " << sellerId << ")" << endl;
        cout << "Date: " << date << endl;
        cout << "\nItems:" << endl;
    cout << "---------------------------------------" << endl;
    
    for (const auto& item : items) {
        cout << "  " << item.getItemName() 
             << " x" << item.getQuantity()
             << " @ $" << fixed << setprecision(2) << item.getPricePerUnit()
             << " = $" << fixed << setprecision(2) << item.getTotalPrice() << endl;
    }
    
    cout << "---------------------------------------" << endl;
    cout << "Total Amount: $" << fixed << setprecision(2) << totalAmount << endl;
}
