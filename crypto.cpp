#include "crypto.hpp"
#include <utility>
using namespace std;

int Crypto_currency::totalCrypto_count = 0;

Crypto_currency::Crypto_currency()
                : name("Undefined"),symbol("N/A"),price(0.0){
                    ++totalCrypto_count;
                }

Crypto_currency::Crypto_currency(string input_name,string input_symbol,double input_price)
                :name(move(input_name)),symbol(move(input_symbol)),price(input_price){
                    ++totalCrypto_count;
                }

Crypto_currency::Crypto_currency(const Crypto_currency& other)
                 : name(other.name), symbol(other.symbol), price(other.price) {
                     ++totalCrypto_count;
}

Crypto_currency::~Crypto_currency() {
                --totalCrypto_count;
}

const string& Crypto_currency::getName() const{
        return name;
}

const string& Crypto_currency::getSymbol() const{
    return symbol;
}

double Crypto_currency::getPrice() const {
    return price;
}

void Crypto_currency::setPrice(double newPrice) {
    price = newPrice;
}