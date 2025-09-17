#pragma once
#include <string>
#include "user.hpp"
#include "crypto.hpp"
#include "MinimalMap.hpp"
#include "MinimalList.hpp"

class LimitOrder {
public:
    int orderId;
    std::string username;
    std::string symbol;
    double units;
    double desiredPrice;
    bool isBuyOrder;

    LimitOrder();
    LimitOrder(int id, std::string uname, std::string sym, double u, double price, bool isBuy);
    void display() const;
};

class LimitOrderManager {
private:
    MinimalMap<int, LimitOrder> orders;
    std::string filename;
    static int nextOrderId;

    static void loadNextOrderId();

public:
    LimitOrderManager(const std::string& fname = "limit_orders.txt");
    ~LimitOrderManager();

    void loadOrders();
    void saveOrders() const;
    void addOrder(const std::string& username, const std::string& symbol, double units, double price, bool isBuy);
    void displayUserOrders(const std::string& username) const;
    void checkAndExecuteOrders(User& user, MinimalList<Crypto_currency>& cryptos);
};