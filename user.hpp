#pragma once
#include <string>
#include "wallet.hpp"

class User {
private:
    const int   userId;     
    std::string name;
    Wallet      wallet;

    static int nextId;        
    friend double auditNetWorth(const User& u);

public:
    User();                                 
    User(std::string name, double cash);    
    User(const User& other);                
    ~User();

    int              getId()   const;
    const std::string& getName() const;
    void             setName(const std::string& n);
    Wallet&          getWallet();
    const Wallet&    getWallet() const;

    void printSummary() const;
};;
double auditNetWorth(const User& u);
