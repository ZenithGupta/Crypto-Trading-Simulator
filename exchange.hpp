#pragma once

#include <vector>
#include <string>
#include "crypto.hpp"
#include "user.hpp"

class Exchange;

void saveCryptoData(const Exchange& ex);

class Exchange {
private:
    std::vector<Crypto_currency> listings;

public:
    static int totalTrades;

    Exchange();
    ~Exchange();

    void add_crypto_listing(const Crypto_currency& c);
    Crypto_currency* find(const std::string& symbol);
    const Crypto_currency* find(const std::string& symbol) const;

    bool isListingsEmpty() const;

    bool updatePrice(const std::string& symbol, double percent, bool increase);
    double priceOf(const std::string& symbol) const;

    void print() const;
    
    friend void saveCryptoData(const Exchange& ex);
};