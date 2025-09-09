#pragma once

#include <vector>
#include <string>
#include "crypto.hpp"
#include "user.hpp"
using namespace std;

class Exchange{
    private:
        vector<Crypto_currency> crypto_list;

    public:
    static int totalTrades;

    Exchange()
    ~Exchange();

    void add_crypto_listing(const Crypto_currency& c);
    Crypto_currency* find(const string& symbol)
    const Crypto_currency* find(const string& symbol) const;

    
     bool updatePrice(const std::string& symbol, double percent, bool increase);
     double priceOf(const string& symbol) const;

     void print() const;

};
