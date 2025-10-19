#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <string>
#include <vector>
#include <memory>
#include "buyer.h"
#include "seller.h"
#include "bank_customer.h"
#include "transaction.h"


using namespace std;

struct AppState {
    vector<Buyer> buyers;
    vector<seller> sellers;
    vector<unique_ptr<BankCustomer>> bankAccounts;
    vector<Transaction> transactions;
    vector<Transaction> pendingOrders;
};


namespace store {
    bool ensure_data_dir(const string &path = "data");
    bool save_all(const AppState &state, const string &path = "data");
    bool load_all(AppState &state, const string &path = "data");
}

#endif // PERSISTENCE_H