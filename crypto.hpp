#pragma once
#include <string>
using namespace std;

class Crypto_currency{
    private:
        string name;
        string symbol;
        double price;
    
    public:
        static int totalCrypto_count;

        Crypto_currency();
        Crypto_currency(string name,string symbol,double price);
        Crypto_currency(const Crypto_currency &other);
        ~Crypto_currency();

        const std::string& getName() const;
        const std::string& getSymbol() const;
        double getPrice() const;
        void   setPrice(double price);


};