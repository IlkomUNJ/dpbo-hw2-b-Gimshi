#include "bank_customer.h"
#include <iostream>
#include <iomanip>

using namespace std;

string BankCustomer::getName() const {
    return this->name;
}

int BankCustomer::getId() const {
    return this->id ;
}

double BankCustomer::getBalance() const {
    return this->balance;
}

void BankCustomer::setBalance(double amount) {
    this->balance = amount;
}
 
void BankCustomer::addBalance(int amount) {
    bool wasDormant = isDormant();
    this->balance += amount;
    
    // Show reactivation message if account was dormant
    if (wasDormant && !isDormant()) {
        cout << "\nACCOUNT REACTIVATED!" << endl;
        cout << "   Your account is now active with balance: $" << fixed << setprecision(2) << this->balance << endl;
    }
}

bool BankCustomer::withdrawBalance(double amount){
    if (amount > this->balance) {
    cout << "Rejected: Insufficient funds!" << endl;
        return false;
    }
    this->balance -= amount;
    return true;
}

void BankCustomer::printInfo() const {
    cout << "Customer Name: " << this->name << endl;
    cout << "Customer ID: " << this->id << endl;
    cout << "Balance: $" << fixed << setprecision(2) << this->balance << endl;
    
    // Show dormant status if balance is 0 or below
    if (isDormant()) {
        cout << " STATUS: DORMANT ACCOUNT (Balance = $0)" << endl;
        cout << "    Please deposit money to reactivate your account." << endl;
    }
}

bool BankCustomer::isDormant() const {
    return this->balance <= 0;
}