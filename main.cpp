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

class Exchange {
private:
    std::vector<Crypto_currency> listings;

public:
    static int totalTrades;

    void add_crypto_listing(const Crypto_currency& c);
    Crypto_currency* find(const std::string& symbol);
    double priceOf(const std::string& symbol);
    bool isListingsEmpty() const;
    void print() const;
    const std::vector<Crypto_currency>& getListings() const;
};

int Exchange::totalTrades = 0;

void Exchange::add_crypto_listing(const Crypto_currency& c) { listings.push_back(c); }

Crypto_currency* Exchange::find(const std::string& symbol) {
    for (auto& crypto : listings) {
        if (crypto.getSymbol() == symbol) {
            return &crypto;
        }
    }
    return nullptr;
}

double Exchange::priceOf(const std::string& symbol) {
    Crypto_currency* crypto = find(symbol);
    return (crypto != nullptr) ? crypto->getPrice() : -1.0;
}

bool Exchange::isListingsEmpty() const { return listings.empty(); }

void Exchange::print() const {
    std::cout << "\n--- Crypto Exchange Listings ---\n";
    for (const auto& c : listings) {
        std::cout << "  " << std::setw(5) << c.getSymbol()
                  << "  " << std::setw(12) << c.getName()
                  << "  $" << std::fixed << std::setprecision(2) << c.getPrice() << "\n";
    }
    std::cout << "Total Trades on Exchange: " << totalTrades << "\n";
    std::cout << "-------------------------------\n";
}

const std::vector<Crypto_currency>& Exchange::getListings() const { return listings; }


class Trade {
protected:
    std::string symbol;
    double units;

public:
    Trade(const std::string& sym, double u);
    virtual ~Trade() = default;
    virtual bool execute(User& user, Exchange& ex) = 0;
};

Trade::Trade(const std::string& sym, double u) : symbol(sym), units(u) {}

class BuyTrade : public Trade {
public:
    BuyTrade(const std::string& sym, double u);
    bool execute(User& user, Exchange& ex) override;
};

BuyTrade::BuyTrade(const std::string& sym, double u) : Trade(sym, u) {}

bool BuyTrade::execute(User& user, Exchange& ex) {
    double px = ex.priceOf(symbol);
    if (px < 0) {
        std::cout << "Symbol not found.\n";
        return false;
    }
    double cost = px * units;
    if (!user.getWallet().withdraw(cost)) {
        std::cout << "Insufficient cash to complete purchase.\n";
        return false;
    }
    user.getWallet().addQty(symbol, units);
    Exchange::totalTrades++;
    std::cout << "SUCCESS: Bought " << units << " " << symbol << " for $" << std::fixed << std::setprecision(2) << cost << "\n";
    return true;
}

class SellTrade : public Trade {
public:
    SellTrade(const std::string& sym, double u);
    bool execute(User& user, Exchange& ex) override;
};

SellTrade::SellTrade(const std::string& sym, double u) : Trade(sym, u) {}

bool SellTrade::execute(User& user, Exchange& ex) {
    double px = ex.priceOf(symbol);
    if (px < 0) {
        std::cout << "Symbol not found.\n";
        return false;
    }
    if (!user.getWallet().removeQty(symbol, units)) {
        std::cout << "Insufficient units to sell.\n";
        return false;
    }
    double earnings = px * units;
    user.getWallet().deposit(earnings);
    Exchange::totalTrades++;
    std::cout << "SUCCESS: Sold " << units << " " << symbol << " for $" << std::fixed << std::setprecision(2) << earnings << "\n";
    return true;
}