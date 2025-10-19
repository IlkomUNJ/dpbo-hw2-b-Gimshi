#ifndef TRANSACTION_H
#define TRANSACTION_H


#include <string>
#include <vector>
using namespace std;

enum TransactionStatus {
    PENDING,     
    PAID,         
    COMPLETED,    
    CANCELLED   
};

class TransactionItem {
private:
    int itemId;
    string itemName;
    int quantity;
    double pricePerUnit;

public:
    TransactionItem(int id, const string& name, int qty, double price)
        : itemId(id), itemName(name), quantity(qty), pricePerUnit(price) {}

    int getItemId() const { return itemId; }
    string getItemName() const { return itemName; }
    int getQuantity() const { return quantity; }
    double getPricePerUnit() const { return pricePerUnit; }
    double getTotalPrice() const { return quantity * pricePerUnit; }
};

class Transaction {
private:
    static int nextTransactionId;
    int transactionId;
    int buyerId;
    string buyerName;
    int sellerId;
    string sellerName;
    vector<TransactionItem> items;
    double totalAmount;
    TransactionStatus status;
    std::string date;

public:
    Transaction(int bId, const string& bName, int sId, const string& sName, const std::string& date = "");


    int getTransactionId() const;
    int getBuyerId() const;
    string getBuyerName() const;
    int getSellerId() const;
    string getSellerName() const;
    const vector<TransactionItem>& getItems() const;
    double getTotalAmount() const;
    TransactionStatus getStatus() const;
    string getStatusString() const;
    const std::string& getDate() const { return date; }
    void setDate(const std::string& d) { date = d; }

 
    void setStatus(TransactionStatus newStatus);


    void addItem(int itemId, const string& itemName, int quantity, double price);
    void calculateTotal();
    void printTransactionDetails() const;
};

#endif // TRANSACTION_H
