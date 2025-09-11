#include "trade.hpp"
#include <iostream>

using namespace std;

Trade::Trade() : symbol("N/A"), units(0), priceAtOrder(0) {}
Trade::Trade(string sym, double u, double p) : symbol(move(sym)), units(u), priceAtOrder(p) {}
Trade::Trade(const Trade& other) : symbol(other.symbol), units(other.units), priceAtOrder(other.priceAtOrder) {}
Trade::~Trade() = default;

const string& Trade::getSymbol() const { return symbol; }
double Trade::getUnits() const { return units; }
double Trade::getPriceAtOrder() const { return priceAtOrder; }

bool BuyTrade::execute(User& user, Exchange& ex) {
    double px = ex.priceOf(symbol);
    if (px < 0) { cout << "Symbol not found\n"; return false; }
    double cost = px * units;
    if (!user.getWallet().withdraw(cost)) {
        cout << "Insufficient cash to buy\n";
        return false;
    }
    user.getWallet().addQty(symbol, units);
    Exchange::totalTrades++;
    cout << "BUY " << units << " " << symbol << " @ $" << px << " OK\n";
    return true;
}

bool SellTrade::execute(User& user, Exchange& ex) {
    double px = ex.priceOf(symbol);
    if (px < 0) { cout << "Symbol not found\n"; return false; }
    if (!user.getWallet().removeQty(symbol, units)) {
        cout << "Insufficient units to sell\n";
        return false;
    }
    user.getWallet().deposit(px * units);
    Exchange::totalTrades++;
    cout << "SELL " << units << " " << symbol << " @ $" << px << " OK\n";
    return true;
}