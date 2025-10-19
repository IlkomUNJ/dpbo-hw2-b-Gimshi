// seller.h
#pragma once
#include "buyer.h"
#include "item.h"
#include <string>
#include <vector>

using namespace std;

class seller : public Buyer {
private:
    int sellerId;
    string sellerName;
    vector<Item> items;

    bool idDisplayed(int itemId) const {
        return itemId > 0;
    }
public:
    seller() = default;
        seller(const Buyer& buyer, int sellerId, const string& sellerName)
        : Buyer(buyer.getId(), buyer.getName(), buyer.getEmail(), buyer.getPhone(), buyer.getAddress(), buyer.getAccount()),
          sellerId(sellerId), sellerName(sellerName) {}
    virtual ~seller() = default;
    int getSellerId() const { return sellerId; }
    const string& getStoreName() const { return sellerName; }
    const vector<Item>& getItems() const { return items; }
    vector<Item>& getItems() { return items; } 
    void addNewItem(int newId, const string& newName, int newQuantity, double newPrice) {
        Item newItem(newId, newName, newQuantity, newPrice);
        items.push_back(newItem);
    }
    void updateItem(int itemId, const string& newName, int newQuantity, double newPrice) {
        for (auto& item : items) {
            if (item.getId() == itemId) {
                item = Item(itemId, newName, newQuantity, newPrice); 
                break;
            }
        }
    }
    void makeItemVisibleToCustomer(int itemId) {
        for (auto& item : items) {
            if (item.getId() == itemId) {
                item.setDisplay(true);
                break;
            }
        }
    }
};