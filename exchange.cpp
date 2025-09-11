#include "exchange.hpp"
#include <iostream>
#include <iomanip>

int Exchange::totalTrades = 0;

Exchange::Exchange() = default;
Exchange::~Exchange() = default;

void Exchange::add_crypto_listing(const Crypto_currency& c) {
    listings.push_back(c);
}

Crypto_currency* Exchange::find(const std::string& symbol) {
    for (auto& c : listings) if (c.getSymbol() == symbol) return &c;
    return nullptr;
}

const Crypto_currency* Exchange::find(const std::string& symbol) const {
    for (auto& c : listings) {
        if (c.getSymbol() == symbol) {
            return &c;
        }
    }
    return nullptr;
}

bool Exchange::updatePrice(const std::string& symbol, double percent, bool increase) {
    if (auto* c = find(symbol)) {
        double p = c->getPrice();
        double delta = p * (percent / 100.0);
        c->setPrice(increase ? p + delta : p - delta);
        return true;
    }
    return false;
}

double Exchange::priceOf(const std::string& symbol) const {
    for (const auto& c : listings) {
        if (c.getSymbol() == symbol) return c.getPrice();
    }
    return -1.0;
}

void Exchange::print() const {
    std::cout << "Exchange Listings (" << Crypto_currency::totalCrypto_count << " cryptos):\n";
    for (auto& c : listings) {
        std::cout << "  " << std::setw(5) << c.getSymbol()
            << "  " << std::setw(12) << c.getName()
            << "  $" << std::fixed << std::setprecision(2) << c.getPrice() << "\n";
    }
    std::cout << "Total Trades Executed: " << totalTrades << "\n";
}