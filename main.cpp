#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <utility> // for std::pair
#include <stdexcept> // Required for standard exception types

using namespace std;

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

    bool operator==(const Crypto_currency& other) const { return symbol == other.symbol; }
    bool operator!=(const Crypto_currency& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& os, const Crypto_currency& c);
};

Crypto_currency::Crypto_currency(const std::string& name, const std::string& symbol, double price)
    : name(name), symbol(symbol), price(price) {}

const std::string& Crypto_currency::getName() const { return name; }
const std::string& Crypto_currency::getSymbol() const { return symbol; }
double Crypto_currency::getPrice() const { return price; }
void Crypto_currency::setPrice(double newPrice) { price = newPrice; }

inline std::ostream& operator<<(std::ostream& os, const Crypto_currency& c) {
    os << c.getSymbol() << " (" << c.getName() << ") $" << std::fixed << std::setprecision(2) << c.getPrice();
    return os;
}

class Wallet {
private:
    double cashBalance;
    std::map<std::string, double> holdings;

public:
    Wallet() : cashBalance(0.0) {}
    explicit Wallet(double initialCash) : cashBalance(initialCash) {}

    double getCash() const;
    void deposit(double amount);
    bool withdraw(double amount);

    double getQty(const std::string& symbol) const;
    void addQty(const std::string& symbol, double units);
    bool removeQty(const std::string& symbol, double units);

    void print() const;
    const std::map<std::string, double>& getHoldings() const;

    Wallet& operator+=(double amount) { deposit(amount); return *this; }
    Wallet& operator-=(double amount) { withdraw(amount); return *this; }

    Wallet& operator+=(const std::pair<std::string,double>& asset) { addQty(asset.first, asset.second); return *this; }
    Wallet& operator-=(const std::pair<std::string,double>& asset) { removeQty(asset.first, asset.second); return *this; }

    bool operator==(const Wallet& other) const { return cashBalance == other.cashBalance && holdings == other.holdings; }
    bool operator!=(const Wallet& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& os, const Wallet& w);
};

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

inline std::ostream& operator<<(std::ostream& os, const Wallet& w) {
    os << "Cash: $" << std::fixed << std::setprecision(2) << w.cashBalance << "\nHoldings:\n";
    if (w.holdings.empty()) {
        os << "  No holdings yet.\n";
    } else {
        for (const auto& h : w.holdings) {
            os << "  " << h.first << ": " << h.second << " units\n";
        }
    }
    return os;
}

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

    friend std::ostream& operator<<(std::ostream& os, const User& u);
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

inline std::ostream& operator<<(std::ostream& os, const User& u) {
    os << "--- User Portfolio ---\n";
    os << "User: " << u.name << "\n";
    os << u.wallet;
    os << "----------------------\n";
    return os;
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

class AuthManager {
private:
    const std::string user_file = "users.txt";
    unsigned long simpleHash(const std::string& str) const;

public:
    User* login();
    User* signUp();
    void saveUserData(const User& user) const;
    User* loadUserData(const std::string& username) const;
};

// Use a simple deterministic hash (djb2) that returns unsigned long
unsigned long AuthManager::simpleHash(const std::string& str) const {
    unsigned long hash = 5381;
    for (unsigned char c : str) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

User* AuthManager::login() {
    std::string username, password;

    try {
        std::ifstream file(user_file);
        if (!file) {
            std::cout << "No users have signed up yet.\n";
            return nullptr;
        }

        std::cout << "--- User Login ---\n";
        std::cout << "Enter username: ";
        std::cin >> username;
        std::cout << "Enter password: ";
        std::cout << "if forgot password write 1:";
        std::cin >> password;

        if (password == "1") {
            file.clear();
            file.seekg(0);
            std::string line;
            while (std::getline(file, line)) {
                std::stringstream ss(line);
                std::string stored_user;
                unsigned long stored_hash;
                ss >> stored_user >> stored_hash;
                if (username == stored_user) {
                    std::cout << "Stored password hash for user '" << username << "': " << stored_hash << "\n";
                    break;
                }
            }
            return nullptr;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string stored_user;
            unsigned long stored_hash;
            ss >> stored_user >> stored_hash;
            if (username == stored_user) {
                if (simpleHash(password) == stored_hash) {
                    std::cout << "Login successful! Welcome, " << username << ".\n";
                    return loadUserData(username);
                } else {
                    std::cout << "Invalid password.\n";
                    return nullptr;
                }
            }
        }
    } catch (const std::ifstream::failure& e) {
        std::cerr << "Exception opening/reading user file: " << e.what() << '\n';
        return nullptr;
    }
    std::cout << "User not found.\n";
    return nullptr;
}

User* AuthManager::signUp() {
    std::string username, password;
    std::cout << "--- User Sign Up ---\n";
    std::cout << "Choose a username: ";
    std::cin >> username;

    try {
        std::ifstream infile(user_file);
        if (infile) {
            std::string line;
            while (std::getline(infile, line)) {
                if (line.empty()) continue;
                std::stringstream ss(line);
                std::string stored_user;
                ss >> stored_user;
                if (username == stored_user) {
                    std::cout << "Username already exists. Please try another.\n";
                    return nullptr;
                }
            }
        }
        infile.close();

        std::cout << "Choose a password: \n Password should contain atleast size of 5 having character and digit\n";
        std::cin >> password;
        if (password.size() < 5) {
            std::cout << "Not a valid password\n";
            return nullptr;
        }

        std::ofstream outfile(user_file, std::ios::app);
        if (!outfile) {
            std::cerr << "Error: Could not open user file for writing.\n";
            return nullptr;
        }
        outfile << username << " " << simpleHash(password) << std::endl;

        std::cout << "Sign up successful! Welcome, " << username << ".\n";
        User* newUser = new User(username, 10000.0);
        saveUserData(*newUser);
        return newUser;
    } catch (const std::ios_base::failure& e) {
        std::cerr << "Exception handling user file: " << e.what() << '\n';
        return nullptr;
    }
     catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation failed: " << e.what() << '\n';
        return nullptr;
    }
}

void AuthManager::saveUserData(const User& user) const {
    std::string filename = user.getName() + "_wallet.csv";
    try {
        std::ofstream file(filename);
        if (!file) {
             std::cerr << "Error: Could not save user data for " << user.getName() << std::endl;
             return;
        }

        file << user.getWallet().getCash() << std::endl;
        for (const auto& holding : user.getWallet().getHoldings()) {
            file << holding.first << "," << holding.second << std::endl;
        }
    } catch (const std::ofstream::failure& e) {
        std::cerr << "Exception writing to user wallet file: " << e.what() << '\n';
    }
}

User* AuthManager::loadUserData(const std::string& username) const {
    std::string filename = username + "_wallet.csv";
    try {
        std::ifstream file(filename);
        if (!file) {
            User* newUser = new User(username, 10000.0);
            saveUserData(*newUser);
            return newUser;
        }

        double cash;
        file >> cash;
        User* user = new User(username, cash);

        std::string line;
        std::getline(file, line); // Consume rest of the first line
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string symbol;
            double units;
            std::getline(ss, symbol, ',');
            ss >> units;
            if (!symbol.empty()) {
                user->getWallet().addQty(symbol, units);
            }
        }
        return user;
    } catch (const std::ifstream::failure& e) {
        std::cerr << "Exception reading user wallet file: " << e.what() << '\n';
        return nullptr;
    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation failed: " << e.what() << '\n';
        return nullptr;
    }
}


class LimitOrder {
public:
    int orderId;
    std::string username;
    std::string symbol;
    double units;
    double desiredPrice;
    bool isBuyOrder;

    LimitOrder(int id, std::string uname, std::string sym, double u, double price, bool isBuy);
    void display() const;

    friend std::ostream& operator<<(std::ostream& os, const LimitOrder& lo);
};

LimitOrder::LimitOrder(int id, std::string uname, std::string sym, double u, double price, bool isBuy)
    : orderId(id), username(std::move(uname)), symbol(std::move(sym)), units(u), desiredPrice(price), isBuyOrder(isBuy) {}

void LimitOrder::display() const {
    std::cout << "ID: " << std::setw(4) << orderId
              << " | " << (isBuyOrder ? "BUY " : "SELL")
              << " | " << std::setw(5) << symbol
              << " | Units: " << std::setw(8) << std::fixed << std::setprecision(4) << units
              << " | Target Price: $" << std::setw(10) << std::fixed << std::setprecision(2) << desiredPrice << std::endl;
}

inline std::ostream& operator<<(std::ostream& os, const LimitOrder& lo) {
    os << "ID: " << std::setw(4) << lo.orderId
       << " | " << (lo.isBuyOrder ? "BUY " : "SELL")
       << " | " << std::setw(5) << lo.symbol
       << " | Units: " << std::setw(8) << std::fixed << std::setprecision(4) << lo.units
       << " | Target Price: $" << std::setw(10) << std::fixed << std::setprecision(2) << lo.desiredPrice;
    return os;
}

class LimitOrderManager {
private:
    std::vector<LimitOrder> orders;
    const std::string filename = "limit_orders.txt";
    const std::string id_filename = "order_id.txt";
    static int nextOrderId;

    void loadNextOrderId();
    void saveNextOrderId() const;
    void loadOrders();
    void saveOrders() const;

public:
    LimitOrderManager();
    ~LimitOrderManager();

    void addOrder(const std::string& username, const std::string& symbol, double units, double price, bool isBuy);
    void displayUserOrders(const std::string& username) const;
    void checkAndExecuteUserOrders(User& user, Exchange& ex);
    void checkAndExecuteAllOrders(Exchange& ex, AuthManager& auth);
};

int LimitOrderManager::nextOrderId = 1;

LimitOrderManager::LimitOrderManager() {
    try {
        loadNextOrderId();
        loadOrders();
    } catch (const std::exception& e) {
        std::cerr << "Error during LimitOrderManager initialization: " << e.what() << '\n';
    }
}

LimitOrderManager::~LimitOrderManager() {
    try {
        saveNextOrderId();
    } catch (const std::exception& e) {
        std::cerr << "Error during LimitOrderManager destruction: " << e.what() << '\n';
    }
}

void LimitOrderManager::loadNextOrderId() {
    try {
        std::ifstream idFile(id_filename);
        if (idFile) idFile >> nextOrderId;
        if (nextOrderId == 0) nextOrderId = 1;
    } catch (const std::ifstream::failure& e) {
        std::cerr << "Exception loading next order ID: " << e.what() << '\n';
    }
}

void LimitOrderManager::saveNextOrderId() const {
    try {
        std::ofstream idFile(id_filename);
        if (idFile) idFile << nextOrderId;
    } catch (const std::ofstream::failure& e) {
        std::cerr << "Exception saving next order ID: " << e.what() << '\n';
    }
}

void LimitOrderManager::loadOrders() {
    orders.clear();
    try {
        std::ifstream file(filename);
        if (!file) return;

        int id, isBuyInt;
        std::string username, symbol;
        double units, price;
        while (file >> id >> username >> symbol >> units >> price >> isBuyInt) {
            orders.emplace_back(id, username, symbol, units, price, (isBuyInt == 1));
        }
    } catch (const std::ifstream::failure& e) {
        std::cerr << "Exception loading limit orders: " << e.what() << '\n';
    }
}

void LimitOrderManager::saveOrders() const {
    try {
        std::ofstream file(filename);
        if (!file) return;

        for (const auto& order : orders) {
            file << order.orderId << " " << order.username << " " << order.symbol << " "
                 << order.units << " " << order.desiredPrice << " " << (order.isBuyOrder ? 1 : 0) << std::endl;
        }
    } catch (const std::ofstream::failure& e) {
        std::cerr << "Exception saving limit orders: " << e.what() << '\n';
    }
}

void LimitOrderManager::addOrder(const std::string& username, const std::string& symbol, double units, double price, bool isBuy) {
    try {
        orders.emplace_back(nextOrderId++, username, symbol, units, price, isBuy);
        saveOrders();
        std::cout << "Limit order placed successfully.\n";
    } catch (const std::bad_alloc& e) {
        std::cerr << "Memory allocation failed for new order: " << e.what() << '\n';
    }
}

void LimitOrderManager::displayUserOrders(const std::string& username) const {
    std::cout << "\n--- Your Pending Limit Orders ---\n";
    bool found = false;
    for (const auto& order : orders) {
        if (order.username == username) {
            order.display();
            found = true;
        }
    }
    if (!found) std::cout << "You have no pending limit orders.\n";
}

void LimitOrderManager::checkAndExecuteUserOrders(User& user, Exchange& ex) {
    bool ordersChanged = false;
    try {
        auto it = std::remove_if(orders.begin(), orders.end(), [&](LimitOrder& order) {
            if (order.username != user.getName()) return false;

            double currentPrice = ex.priceOf(order.symbol);
            if (currentPrice < 0) return false;

            bool shouldExecute = (order.isBuyOrder && currentPrice <= order.desiredPrice) ||
                                 (!order.isBuyOrder && currentPrice >= order.desiredPrice);

            if (shouldExecute) {
                std::cout << "\n[!] EXECUTING YOUR LIMIT ORDER ID: " << order.orderId << std::endl;
                bool success = order.isBuyOrder ? BuyTrade(order.symbol, order.units).execute(user, ex)
                                                : SellTrade(order.symbol, order.units).execute(user, ex);
                if (success) ordersChanged = true;
                else std::cout << "[!] Limit Order ID " << order.orderId << " failed (insufficient funds/units).\n";
                return success;
            }
            return false;
        });

        if (ordersChanged) {
            orders.erase(it, orders.end());
            saveOrders();
        }
    } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred while checking user orders: " << e.what() << '\n';
    }
}

void LimitOrderManager::checkAndExecuteAllOrders(Exchange& ex, AuthManager& auth) {
    bool ordersChanged = false;
    try {
        auto it = std::remove_if(orders.begin(), orders.end(), [&](LimitOrder& order) {
            double currentPrice = ex.priceOf(order.symbol);
            if (currentPrice < 0) return false;

            bool shouldExecute = (order.isBuyOrder && currentPrice <= order.desiredPrice) ||
                                 (!order.isBuyOrder && currentPrice >= order.desiredPrice);

            if (shouldExecute) {
                User* owner = auth.loadUserData(order.username);
                if (!owner) return false;

                std::cout << "\n[!] EXECUTING GLOBAL LIMIT ORDER ID: " << order.orderId << " for user " << order.username << std::endl;
                bool success = order.isBuyOrder ? BuyTrade(order.symbol, order.units).execute(*owner, ex)
                                                : SellTrade(order.symbol, order.units).execute(*owner, ex);

                if (success) {
                    auth.saveUserData(*owner);
                    ordersChanged = true;
                } else {
                    std::cout << "[!] Global Limit Order ID " << order.orderId << " failed.\n";
                }
                delete owner;
                return success;
            }
            return false;
        });

        if (ordersChanged) {
            orders.erase(it, orders.end());
            saveOrders();
        }
    } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred while checking all orders: " << e.what() << '\n';
    }
}

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

template <typename T>
T getNumericInput(const std::string& prompt) {
    T value;
    std::cout << prompt;
    while (!(std::cin >> value)) {
        std::cout << "Wrong input, enter a numeric value: ";
        clearInput();
    }
    return value;
}

void saveCryptoData(const Exchange& ex) {
    try {
        std::ofstream file("crypto_data.csv");
        if (!file) return;

        for (const auto& crypto : ex.getListings()) {
            file << crypto.getName() << "," << crypto.getSymbol() << "," << crypto.getPrice() << std::endl;
        }
    } catch (const std::ofstream::failure& e) {
        std::cerr << "Exception writing to crypto data file: " << e.what() << '\n';
    }
}

void loadCryptoData(Exchange& ex) {
    try {
        std::ifstream file("crypto_data.csv");
        if (!file) return;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string name, symbol, price_str;
            std::getline(ss, name, ',');
            std::getline(ss, symbol, ',');
            std::getline(ss, price_str);
            if (!name.empty() && !symbol.empty() && !price_str.empty()) {
                try {
                    ex.add_crypto_listing(Crypto_currency(name, symbol, std::stod(price_str)));
                } catch (const std::invalid_argument& ia) {
                    std::cerr << "Invalid argument: " << ia.what() << " for price: " << price_str << '\n';
                } catch (const std::out_of_range& oor) {
                    std::cerr << "Out of Range error: " << oor.what() << " for price: " << price_str << '\n';
                }
            }
        }
    } catch (const std::ifstream::failure& e) {
        std::cerr << "Exception reading crypto data file: " << e.what() << '\n';
    }
}

void seedExchange(Exchange& ex) {
    ex.add_crypto_listing(Crypto_currency("Bitcoin", "BTC", 60000.0));
    ex.add_crypto_listing(Crypto_currency("Ether", "ETH", 2500.0));
    ex.add_crypto_listing(Crypto_currency("Solana", "SOL", 150.0));
}

void adminMenu(Exchange& ex, AuthManager& auth, LimitOrderManager& limitManager) {
    while (true) {
        std::cout << "\n--- Admin Menu ---\n"
                  << "1) Update Crypto Price\n"
                  << "0) Logout\n> ";
        int choice = getNumericInput<int>("");

        if (choice == 0) break;

        if (choice == 1) {
            std::string sym;
            std::cout << "Update % for which symbol? ";
            std::cin >> sym;
            double pct = getNumericInput<double>("Percent change (+/-): ");
            int inc = getNumericInput<int>("Increase? (1=yes, 0=no): ");

            Crypto_currency* crypto = ex.find(sym);
            if (crypto) {
                double p = crypto->getPrice();
                double delta = p * (pct / 100.0);
                crypto->setPrice(inc == 1 ? p + delta : p - delta);
                std::cout << "[OK] " << sym << " is now $" << crypto->getPrice() << "\n";

                std::cout << "Checking all pending limit orders against new price...\n";
                limitManager.checkAndExecuteAllOrders(ex, auth);

            } else {
                std::cout << "[ERR] Symbol not found\n";
            }
        } else {
            std::cout << "Unknown option.\n";
        }
    }
}

void userMenu(User& user, Exchange& ex, AuthManager& auth, LimitOrderManager& limitManager) {
    while (true) {
        try {
            limitManager.checkAndExecuteUserOrders(user, ex);

            std::cout << "\n=========== USER MENU ============\n"
                      << "1) List Market\n"
                      << "2) View Portfolio\n"
                      << "3) Deposit Funds\n"
                      << "4) Buy Crypto (Market Order)\n"
                      << "5) Sell Crypto (Market Order)\n"
                      << "6) Place Limit Order\n"
                      << "7) View My Limit Orders\n"
                      << "0) Save & Logout\n> ";
            int choice = getNumericInput<int>("");

            if (choice == 0) {
                auth.saveUserData(user);
                std::cout << "Data saved. Logging out.\n";
                break;
            }

            switch (choice) {
                case 1: ex.print(); break;
                case 2: user.printSummary(); break;
                case 3: {
                    double amt = getNumericInput<double>("Enter amount to deposit: ");
                    user.getWallet().deposit(amt);
                    std::cout << "[OK] Deposited. New cash: $" << user.getWallet().getCash() << "\n";
                    break;
                }
                case 4: {
                    std::string sym;
                    std::cout << "Enter symbol to BUY (e.g., ETH): ";
                    std::cin >> sym;
                    double units = getNumericInput<double>("Enter units to buy: ");
                    BuyTrade(sym, units).execute(user, ex);
                    break;
                }
                case 5: {
                    std::string sym;
                    std::cout << "Enter symbol to SELL (e.g., ETH): ";
                    std::cin >> sym;
                    double units = getNumericInput<double>("Enter units to sell: ");
                    SellTrade(sym, units).execute(user, ex);
                    break;
                }
                case 6: {
                    std::string sym;
                    std::cout << "Place a new Limit Order\n";
                    std::cout << "Enter symbol (e.g., BTC): ";
                    std::cin >> sym;

                    if (ex.find(sym) == nullptr) {
                        std::cout << "Error: Symbol '" << sym << "' is not listed on the market.\n";
                        continue;
                    }
                    double units = getNumericInput<double>("Enter units: ");
                    double price = getNumericInput<double>("Enter target price: $");
                    int type = getNumericInput<int>("Is this a BUY or SELL order? (1 for Buy, 2 for Sell): ");

                    if (type == 1 || type == 2) {
                        limitManager.addOrder(user.getName(), sym, units, price, (type == 1));
                    } else {
                        std::cout << "Invalid order type.\n";
                    }
                    break;
                }
                case 7: {
                    limitManager.displayUserOrders(user.getName());
                    break;
                }
                default:
                    std::cout << "Unknown option.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "An error occurred in the user menu: " << e.what() << '\n';
            clearInput(); // Clear any bad input
        }
    }
}

// --- Main Application ---
int main() {
    try {
        Exchange ex;
        AuthManager auth;
        LimitOrderManager limitManager;

        loadCryptoData(ex);
        if (ex.isListingsEmpty()) {
            seedExchange(ex);
        }

        std::cout << "====== Crypto Trading Simulator ======\n";

        while (true) {
            std::cout << "\n--- Welcome ---\n"
                      << "1. Admin Login\n"
                      << "2. User Login\n"
                      << "3. User Sign Up\n"
                      << "0. Exit\n> ";
            int choice;

            if (!(std::cin >> choice)) {
                std::cout << "Invalid input. Please enter a number.\n";
                clearInput();
                continue;
            }

            if (choice == 0) {
                break;
            }

            switch(choice) {
                case 1: {
                    std::string user, pass;
                    std::cout << "--- Admin Login ---\n";
                    std::cout << "Username: "; std::cin >> user;
                    std::cout << "Password: "; std::cin >> pass;
                    if (user == "admin" && pass == "letmein") {
                        std::cout << "Admin login successful.\n";
                        adminMenu(ex, auth, limitManager);
                    } else {
                        std::cout << "Invalid admin credentials.\n";
                    }
                    break;
                }
                case 2: {
                    User* currentUser = auth.login();
                    if (currentUser) {
                        userMenu(*currentUser, ex, auth, limitManager);
                        delete currentUser;
                    }
                    break;
                }
                case 3: {
                    User* newUser = auth.signUp();
                    if (newUser) {
                        userMenu(*newUser, ex, auth, limitManager);
                        delete newUser;
                    }
                    break;
                }
                default:
                    std::cout << "Invalid choice.\n";
            }
        }

        saveCryptoData(ex);
        std::cout << "Crypto market data saved. Goodbye!\n";
    } catch (const std::exception& e) {
        std::cerr << "A critical error occurred: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}