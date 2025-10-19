#ifndef BUYER_H
#define BUYER_H

#include <cstddef>
#include <string>
#include "bank_customer.h"

using namespace std;

class Buyer {
private:
    int id;
    string name, email, phone, address;
    BankCustomer *account;

public:
    Buyer(int id, const string& name, const string& email, const string& phone, const string& address, BankCustomer *account0);

    int getId() const;
    string getName() const;
    string getEmail() const;
    string getPhone() const;
    string getAddress() const;
    BankCustomer* getAccount() const;

    void setId(int newId) { id = newId; }
    void setName(const string& newName);
    void setEmail(const string& newEmail);
    void setPhone(const string& newPhone);
    void setAddress(const string& newAddress);
    void setAccount(BankCustomer* acc);
    
    static bool isValidEmail(const string& email);
    static bool isValidPhone(const string& phone);
};

#endif // BUYER_H