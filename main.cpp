#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>

class User;
class Exchange;
class AuthManager;
class LimitOrderManager;

class Crypto_currency {
private:
    std::string name;
    std::string symbol;
    double price;

public:
    Crypto_currency(const std::string& name, const std::string& symbol, double price);

    const std::string& getName() const;
    const std::string& getSymbol() const;
    double getPrice() const;
    void setPrice(double newPrice);
};

Crypto_currency::Crypto_currency(const std::string& name, const std::string& symbol, double price)
    : name(name), symbol(symbol), price(price) {}

const std::string& Crypto_currency::getName() const { return name; }
const std::string& Crypto_currency::getSymbol() const { return symbol; }
double Crypto_currency::getPrice() const { return price; }
void Crypto_currency::setPrice(double newPrice) { price = newPrice; }

class Wallet {
private:
    double cashBalance;
    std::map<std::string, double> holdings;

public:
    Wallet();
    explicit Wallet(double initialCash);

    double getCash() const;
    void deposit(double amount);
    bool withdraw(double amount);

    double getQty(const std::string& symbol) const;
    void addQty(const std::string& symbol, double units);
    bool removeQty(const std::string& symbol, double units);

    void print() const;
    const std::map<std::string, double>& getHoldings() const;
};

Wallet::Wallet() : cashBalance(0.0) {}
Wallet::Wallet(double initialCash) : cashBalance(initialCash) {}

double Wallet::getCash() const { return cashBalance; }

void Wallet::deposit(double amount) {
    if (amount > 0) {
        cashBalance += amount;
    }
}

bool Wallet::withdraw(double amount) {
    if (amount > 0 && amount <= cashBalance) {
        cashBalance -= amount;
        return true;
    }
    return false;
}

double Wallet::getQty(const std::string& symbol) const {
    auto it = holdings.find(symbol);
    return (it != holdings.end()) ? it->second : 0.0;
}

void Wallet::addQty(const std::string& symbol, double units) {
    if (units > 0) {
        holdings[symbol] += units;
    }
}

bool Wallet::removeQty(const std::string& symbol, double units) {
    if (units > 0 && getQty(symbol) >= units) {
        holdings[symbol] -= units;
        if (holdings[symbol] < 1e-9) { 
            holdings.erase(symbol);
        }
        return true;
    }
    return false;
}

void Wallet::print() const {
    std::cout << "Cash: $" << std::fixed << std::setprecision(2) << cashBalance << "\n";
    std::cout << "Holdings:\n";
    if (holdings.empty()) {
        std::cout << "  No holdings yet.\n";
    } else {
        for (const auto& pair : holdings) {
            std::cout << "  " << pair.first << ": " << pair.second << " units\n";
        }
    }
}

const std::map<std::string, double>& Wallet::getHoldings() const { return holdings; }


class User {
private:
    std::string name;
    Wallet wallet;

public:
    User(const std::string& name, double cash = 0.0);

    const std::string& getName() const;
    Wallet& getWallet();
    const Wallet& getWallet() const;
    void printSummary() const;
};

User::User(const std::string& name, double cash) : name(name), wallet(cash) {}
const std::string& User::getName() const { return name; }
Wallet& User::getWallet() { return wallet; }
const Wallet& User::getWallet() const { return wallet; }

void User::printSummary() const {
    std::cout << "\n--- User Portfolio ---\n";
    std::cout << "Welcome, " << name << "!\n";
    wallet.print();
    std::cout << "----------------------\n";
}