#include "LimitOrder.hpp"
#include "trade.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "MinimalList.hpp"

int LimitOrderManager::nextOrderId = 1;

LimitOrder::LimitOrder() : orderId(0), units(0), desiredPrice(0), isBuyOrder(false) {}

LimitOrder::LimitOrder(int id, std::string uname, std::string sym, double u, double price, bool isBuy)
    : orderId(id), username(uname), symbol(sym), units(u), desiredPrice(price), isBuyOrder(isBuy) {}

void LimitOrder::display() const {
    std::cout << "ID: " << orderId
              << " | " << (isBuyOrder ? "BUY " : "SELL")
              << " | " << symbol
              << " | Units: " << units
              << " | Target Price: $" << desiredPrice << std::endl;
}

LimitOrderManager::LimitOrderManager(const std::string& fname) : filename(fname) {
    loadNextOrderId();
}

LimitOrderManager::~LimitOrderManager() {
    try {
        std::ofstream idFile("order_id.txt");
        if (idFile.is_open()) {
            idFile << nextOrderId;
        }
    } catch (...) {
        std::cerr << "Error: Could not save the next order ID." << std::endl;
    }
}

void LimitOrderManager::loadNextOrderId() {
    std::ifstream idFile("order_id.txt");
    if (idFile.is_open()) {
        idFile >> nextOrderId;
        if (nextOrderId == 0) nextOrderId = 1;
    }
}

void LimitOrderManager::loadOrders() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (getline(file, line)) {
        std::stringstream ss(line);
        int id;
        std::string username, symbol;
        double units, price;
        bool isBuy;

        if (ss >> id >> username >> symbol >> units >> price >> isBuy) {
            orders.insert(id, LimitOrder(id, username, symbol, units, price, isBuy));
        }
    }
}

void LimitOrderManager::saveOrders() const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("FATAL: Could not open " + filename + " for writing.");
    }

    MinimalList<int> keys = orders.get_keys();
    for (int i = 0; i < keys.size(); ++i) {
        int key = keys.at(i);
        LimitOrder* order = orders.find(key);
        if (order) {
            file << order->orderId << " " << order->username << " " << order->symbol << " "
                 << order->units << " " << order->desiredPrice << " " << order->isBuyOrder << std::endl;
        }
    }
}

void LimitOrderManager::addOrder(const std::string& username, const std::string& symbol, double units, double price, bool isBuy) {
    int newId = nextOrderId++;
    orders.insert(newId, LimitOrder(newId, username, symbol, units, price, isBuy));
    saveOrders();
}

void LimitOrderManager::displayUserOrders(const std::string& username) const {
    std::cout << "\n--- Your Pending Limit Orders ---\n";
    bool found = false;
    MinimalList<int> keys = orders.get_keys();
    for (int i = 0; i < keys.size(); ++i) {
        int key = keys.at(i);
        LimitOrder* order = orders.find(key);
        if (order && order->username == username) {
            order->display();
            found = true;
        }
    }
    if (!found) {
        std::cout << "You have no pending limit orders.\n";
    }
    std::cout << "---------------------------------\n";
}

void LimitOrderManager::checkAndExecuteOrders(User& user, MinimalList<Crypto_currency>& cryptos) {
    bool ordersChanged = false;
    MinimalList<int> keys = orders.get_keys();

    for (int i = 0; i < keys.size(); ++i) {
        int key = keys.at(i);
        LimitOrder* order = orders.find(key);
        if (!order || order->username != user.getName()) {
            continue;
        }

        bool executed = false;
        for (int j = 0; j < cryptos.size(); ++j) {
            Crypto_currency& crypto = cryptos.at(j);
            if (crypto.getSymbol() == order->symbol) {
                if ((order->isBuyOrder && crypto.getPrice() <= order->desiredPrice) ||
                    (!order->isBuyOrder && crypto.getPrice() >= order->desiredPrice)) {
                    
                    std::cout << "\n[!] Executing Limit Order ID: " << order->orderId << std::endl;
                    executed = true;
                }
                break;
            }
        }

        if (executed) {
            orders.remove(key);
            ordersChanged = true;
        }
    }

    if (ordersChanged) {
        saveOrders();
    }
}