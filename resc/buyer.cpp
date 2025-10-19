#include "buyer.h"
#include <iostream>
#include <algorithm>
using namespace std;

Buyer::Buyer(int id, const string& name, const string& email, const string& phone, const string& address, BankCustomer* account) 
    : id(id), name(name), email(email), phone(phone), address(address), account(account) {}

int Buyer::getId() const {
    return id;
}

string Buyer::getName() const {
    return name;
}

string Buyer::getEmail() const {
    return email;
}

string Buyer::getPhone() const {
    return phone;
}

string Buyer::getAddress() const {
    return address;
}

BankCustomer* Buyer::getAccount() const {
    return account;
}

void Buyer::setName(const string& newName) {
    name = newName;
}

void Buyer::setEmail(const string& newEmail) {
    email = newEmail;
}

void Buyer::setPhone(const string& newPhone) {
    phone = newPhone;
}

void Buyer::setAddress(const string& newAddress) {
    address = newAddress;
}

void Buyer::setAccount(BankCustomer* acc) {
    account = acc;
}

bool Buyer::isValidEmail(const string& email) {
    return email.find('@') != string::npos;
}

bool Buyer::isValidPhone(const string& phone) {
    return !phone.empty() && all_of(phone.begin(), phone.end(), ::isdigit);
}