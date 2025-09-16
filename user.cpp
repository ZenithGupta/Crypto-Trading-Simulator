#include "user.hpp"
#include <iostream>
int User::nextId = 1;
User::User() : userId(nextId++), name("Guest"), wallet(0.0) {}

User::User(std::string name_, double cash) : userId(nextId++), name(std::move(name_)), wallet(cash) {}

User::User(const User& other) : userId(nextId++), name(other.name), wallet(other.getWallet()) {}

User::~User() {}

int User::getId() const { return userId; }
const std::string& User::getName() const { return name; }
void User::setName(const std::string& n) { name = n; }
Wallet& User::getWallet() { return wallet; }
const Wallet& User::getWallet() const { return wallet; }

void User::printSummary() const {
    std::cout << "User#" << userId << " - " << name << "\n";
    wallet.print();
}

double auditNetWorth(const User& u) {
    return u.wallet.getCash();
}