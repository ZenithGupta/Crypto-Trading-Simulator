#include "wallet.hpp"
#include <iostream>
#include <iomanip>

Wallet::Wallet() : cashBalance(0.0) {}
Wallet::Wallet(double initialCash) : cashBalance(initialCash) {}

Wallet::Wallet(const Wallet& other) : cashBalance(other.cashBalance), qty(other.qty) {
}

Wallet::~Wallet() {
}

double Wallet::getCash() const {
    return cashBalance;
}

void Wallet::setCash(double c) {
    cashBalance = c;
}

void Wallet::deposit(int amount) {
    cashBalance += static_cast<double>(amount);
}

void Wallet::deposit(double amount) {
    cashBalance += amount;
}

bool Wallet::withdraw(double amount) {
    if (amount > cashBalance) return false;
    cashBalance -= amount;
    return true;
}

void Wallet::addQty(const std::string& symbol, double units) {
    qty[symbol] += units;
}

double Wallet::getQty(const std::string& symbol) const {
    if (qty.count(symbol)) {
        return qty.at(symbol);
    }
    return 0.0;
}

bool Wallet::removeQty(const std::string& symbol, double units) {
    double have = getQty(symbol);
    if (units > have) return false;
    qty[symbol] = have - units;
    return true;
}

void Wallet::print() const {
    std::cout << "Cash: $" << std::fixed << std::setprecision(2) << cashBalance << "\n";
    std::cout << "Holdings:\n";
   for (const auto& p : qty) {
    const std::string& sym = p.first;
    double u = p.second;
    if (u > 0) {
        std::cout << "  " << sym << ": " << u << " units\n";
    }
}
}