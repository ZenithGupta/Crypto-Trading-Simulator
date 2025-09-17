#include <iostream>
#include <limits>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "user.hpp"
#include "exchange.hpp"
#include "trade.hpp"

using namespace std;

void saveUserData(const User& user) {
    ofstream file("user_data.csv");
    if (file.is_open()) {
        file << user.getId() << "," << user.getName() << "," << user.getWallet().getCash() << "\n";
        file.close();
    }
}

void loadUserData(User*& user) {
    ifstream file("user_data.csv");
    if (file.is_open()) {
        string line;
        if (getline(file, line)) {
            stringstream ss(line);
            string id_str, name, cash_str;
            if (getline(ss, id_str, ',') && getline(ss, name, ',') && getline(ss, cash_str, ',')) {
                try {
                    double cash = stod(cash_str);
                    delete user;
                    user = new User(name, cash);
                } catch (const std::invalid_argument& e) {
                    cerr << "Error reading cash value from CSV." << endl;
                }
            }
        }
        file.close();
    }
}

void saveCryptoData(const Exchange& ex) {
    ofstream file("crypto_data.csv");
    if (file.is_open()) {
        for (const auto& crypto : ex.listings) {
            file << crypto.getSymbol() << "," << crypto.getName() << "," << crypto.getPrice() << "\n";
        }
        file.close();
    }
}

void loadCryptoData(Exchange& ex) {
    ifstream file("crypto_data.csv");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string symbol, name, price_str;
            if (getline(ss, symbol, ',') && getline(ss, name, ',') && getline(ss, price_str, ',')) {
                try {
                    double price = stod(price_str);
                    ex.add_crypto_listing(Crypto_currency(name, symbol, price));
                } catch (const std::invalid_argument& e) {
                    cerr << "Error reading price value from CSV." << endl;
                }
            }
        }
        file.close();
    }
}


class AdminGuard {
    static const string kUser;
    static const string kPass;

public:
    static bool loginInteractive() {
        string u, p;
        cout << "[Admin Login]\nUsername: ";
        cin >> u;
        cout << "Password: ";
        cin >> p;
        if (u == kUser && p == kPass) {
            cout << "[OK] Admin authenticated.\n";
            return true;
        }
        cout << "[ERR] Invalid credentials.\n";
        return false;
    }
};

const string AdminGuard::kUser = "admin";
const string AdminGuard::kPass = "letmein";
double netWorth(const User& u, const Exchange& ex,
    const vector<string>& symbols) {

    double worth = auditNetWorth(u);
    for (const auto& sym : symbols) {
        double units = u.getWallet().getQty(sym);
        if (units <= 0) continue;
        double px = ex.priceOf(sym);
        if (px > 0) {
            worth += units * px;
        }
    }
    return worth;
}

void seedExchange(Exchange& ex) {
    ex.add_crypto_listing(Crypto_currency("Bitcoin", "BTC", 60000.0));
    ex.add_crypto_listing(Crypto_currency("Ether", "ETH", 2500.0));
    ex.add_crypto_listing(Crypto_currency("Solana", "SOL", 150.0));

    ex.updatePrice("ETH", 10.0, true);
}

void printPortfolio(const User& u) {
    cout << "[Portfolio]\n";
    u.getWallet().print();
}

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main() {
    cout << "=== Crypto Trading Simulator (OOP Demo) ===\n\n";

    Exchange ex;
    loadCryptoData(ex);
    if(ex.isListingsEmpty()){
        seedExchange(ex);
    }

    User* user = nullptr;
    try {
        user = new User("Alice", 10000.00);
    } catch (const std::bad_alloc& e) {
        cerr << "Failed to allocate memory for User: " << e.what() << endl;
        return 1;
    }
    
    loadUserData(user);

    ex.print();
    user->printSummary();
    vector<string> universe = { "BTC", "ETH", "SOL" };

    while (true) {
        cout << "\n================ MENU ================\n"
            << "1) List Market\n"
            << "2) View Portfolio\n"
            << "3) Deposit Funds\n"
            << "4) Buy Crypto\n"
            << "5) Sell Crypto\n"
            << "6) Net Worth\n"
            << "8) Update Price (percentage) [ADMIN]\n"
            << "0) Exit\n> ";

        int choice;
        cin >> choice;

        if (cin.fail()) {
            cout << "Invalid input. Please enter a number." << endl;
            clearInput();
            continue;
        }


        if (choice == 0) break;

        switch (choice) {
        case 1:
            ex.print();
            break;
        case 2:
            printPortfolio(*user);
            break;
        case 3: {
            cout << "Enter amount to deposit (supports int or double): ";
            double amt;
            if (!(cin >> amt)) { 
                cout << "Invalid input. Please enter a number." << endl;
                clearInput(); 
                break; 
            }
            if (floor(amt) == amt) {
                user->getWallet().deposit((int)amt);
            }
            else {

                user->getWallet().deposit(amt);
            }
            cout << "[OK] Deposited. New cash: " << user->getWallet().getCash() << "\n";
            break;
        }
        case 4: {
            string sym; double units;
            cout << "Enter symbol to BUY (e.g., ETH): ";
            cin >> sym;
            cout << "Enter units to buy: ";
            if (!(cin >> units)) { 
                cout << "Invalid input. Please enter a number." << endl;
                clearInput(); 
                break; 
            }
            BuyTrade order(sym, units, 0);
            order.execute(*user, ex);
            break;
        }
        case 5: {
            string sym; double units;
            cout << "Enter symbol to SELL (e.g., ETH): ";
            cin >> sym;
            cout << "Enter units to sell: ";
            if (!(cin >> units)) { 
                cout << "Invalid input. Please enter a number." << endl;
                clearInput(); 
                break; 
            }
            SellTrade order(sym, units, 0);
            order.execute(*user, ex);
            break;
        }
        case 6: {
            double w = netWorth(*user, ex, universe);
            cout << "Net Worth: $" << w << "\n";
            break;
        }
        case 8: {
            if (!AdminGuard::loginInteractive()) break;
            string sym; double pct; int inc;
            cout << "Update % for which symbol? ";
            cin >> sym;
            cout << "Percent change (+/-): ";
            if (!(cin >> pct)) { 
                cout << "Invalid input. Please enter a number." << endl;
                clearInput(); 
                break; 
            }
            cout << "Increase? (1=yes, 0=no): ";
            if (!(cin >> inc)) { 
                cout << "Invalid input. Please enter a number." << endl;
                clearInput(); 
                break; 
            }
            if (ex.updatePrice(sym, pct, inc == 1)) {
                auto px = ex.priceOf(sym);
                cout << "[OK] " << sym << " is now $" << (px > 0 ? px : 0.0) << "\n";
            }
            else {
                cout << "[ERR] Symbol not found\n";
            }
            break;
        }
        default:
            cout << "Unknown option.\n";
        }
    }

    saveUserData(*user);
    saveCryptoData(ex);
    
    delete user;

    cout << "Done.\n";
    return 0;
}