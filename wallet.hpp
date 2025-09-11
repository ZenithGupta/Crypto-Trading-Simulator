#pragma once
#include <map>
#include <string>
#include <iostream>

class Wallet {
private:
    double cashBalance;
    std::map<std::string, double> qty;

public:
    Wallet();
    explicit Wallet(double initialCash);
    Wallet(const Wallet& other);
    ~Wallet();


    double getCash() const;
    void setCash(double c);
    void deposit(double amount);
    void deposit(int amount);

    bool withdraw(double amount);

    double getQty(const std::string& symbol) const;
    void addQty(const std::string& symbol, double units);
    bool removeQty(const std::string& symbol, double units);

    void print() const;
};