#pragma once 
#include <map>
#include<string>
#include<iostream>

class Wallter {
    private: 
    double cashBalance;
    map<string,double> qty;

    public :
     wallet();
     explicit Wallet(double initialCash);

     Wallet(const Wallet& other);

     double getCash() const;
     void setCash (double c);
     void deposite(int amount);

     bool withdraw (double amount);

     double getQty(const string& symbol) const;
    void addQty (const string& symbol,double units);
    bool removeQty(const string& symbol, double units);

    void print() const;
};

