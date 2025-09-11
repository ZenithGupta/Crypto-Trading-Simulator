#pragma once
#include <string>
#include "exchange.hpp"
#include "user.hpp"

using namespace std;

class Trade {
protected:
    string symbol;
    double units;
    double priceAtOrder;

public:
    Trade();
    Trade(string sym, double units, double price);
    Trade(const Trade& other);
    virtual ~Trade();

    const string& getSymbol() const;
    double getUnits() const;
    double getPriceAtOrder() const;

    virtual bool execute(User& user, Exchange& ex) = 0;
};

class BuyTrade : public Trade {
public:
    using Trade::Trade;
    bool execute(User& user, Exchange& ex) override;
};

class SellTrade : public Trade {
public:
    using Trade::Trade;
    bool execute(User& user, Exchange& ex) override;
};