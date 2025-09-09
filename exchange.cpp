#include "exchange.hpp"
#include <iostream>
#include <iomanip>

int Exchange::totalTrades = 0;

Exchange::Exchange() = default;
Exchange::~Exchange() = default;

void Exchange::add_crypto_listing(const Crypto_currency& c){
    crypto_list.push_back(c);
}

Crypto_currency* Exchange::find(const string& symbol) {
    for (auto& c : listings) if (c.getSymbol() == symbol) return &c;
    return nullptr;
}

const Crypto* Exchange::find(const string& symbol) const {
    for (auto& c : listings) {
    if (c.getSymbol() == symbol) {
        return &c;
    }
    }
    return nullptr;
}

bool Exchange::updatePrice(const string& symbol, double percent, bool increase) {
    if (auto* c = find(symbol)) {
        double p = c->getPrice();
        double delta = p * (percent / 100.0);
        c->setPrice(increase ? p + delta : p - delta);
        return true;
    }
    return false;
}

double Exchange::priceOf(const string& symbol) const {
    for (const auto& c : listings) {
        if (c.getSymbol() == symbol) return c.getPrice();
    }
    return -1.0; 
}

void Exchange::print() const {
    cout << "Exchange Listings (" << Crypto_currency::totalCryptos << " cryptos):\n";
    for (auto& c : listings) {
        cout << "  " << setw(5) << c.getSymbol()
                  << "  " << setw(12) << c.getName()
                  << "  $" << fixed << setprecision(2) << c.getPrice() << "\n";
    }
    cout << "Total Trades Executed: " << totalTrades << "\n";
}



