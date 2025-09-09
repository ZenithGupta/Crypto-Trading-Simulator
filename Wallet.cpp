#include "wallet.hpp"
#include<iomainp>

Wallet:: Wallet() : cashBalance(0.0){}
Wallet::Wallet(double initialCash) : cashBalance(initialCash) {}

Wallet::Wallet(const Wallet& other) : cashBalance(other.cashBalance), qty(other.qty){

}

Wallet::~Wallet(){

}

double Wallet::getCash() const{
    return cashBalance;
}

void Wallet::setCash(double c){
    cashBalance = c;
}

void Wallet::deposite(int amount){
    cashBalace += static_cast<double>(amount);
}

void Wallet::withdraw(double amount){
    if(amount > cashBalance) return false;
    cashBalance -= amount;
    return true;
}

void Wallet::addQty(const string& symbol , double units){
    qty[symbol]+= units;
}

bool Wallet:: removeQty( const string& symbol , double units){
    double have = getQty(symbol);
    if(units > have) return false;
    qty[symbol] = have - units;
    return true;
}

void Wallet::print() const{
    cout << "Cash: " << fixed << setpricision(2) << cashBalance << "\n";
    cout << "Holdings: \n";
    for(auto& [sym , u] : qty){
        cout << " " << sym << ":" << u << "units\n";
    }
}