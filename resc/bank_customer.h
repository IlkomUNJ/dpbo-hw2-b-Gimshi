#ifndef BANK_CUSTOMER_H
#define BANK_CUSTOMER_H

#include <string>

using namespace std;

class BankCustomer {
private:
    int id;
    string name;
    double balance;

public:
    BankCustomer(int id, const string& name, double balance) : id(id), name(name), balance(balance) {}

    int getId() const;
    string getName() const;
    double getBalance() const;

    void printInfo() const;
    void setName(const string& name);
    void setBalance(double balance);
    void addBalance(int amout);
    bool withdrawBalance(double amout);
    bool isDormant() const;  // Check if account balance is 0 or below
};

#endif // BANK_CUSTOMER_H