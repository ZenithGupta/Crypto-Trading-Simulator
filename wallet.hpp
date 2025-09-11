#pragma once 
#include <map>
#include<string>
#include<iostream>

class Wallet {
    private: 
    double cashBalance;
    map<string,double> qty;

    public :
     Wallet();
     explicit Wallet(double initialCash);

     Wallet(const Wallet& other);

     double getCash() const;
     void setCash (double c);
     void deposit(int amount);

     bool withdraw (double amount);

     double getQty(const string& symbol) const;
    void addQty (const string& symbol,double units);
    bool removeQty(const string& symbol, double units);

    void print() const;
};

