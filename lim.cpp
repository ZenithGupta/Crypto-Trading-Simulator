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

unsigned long AuthManager::simpleHash(const std::string& str) const {
    unsigned long hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

User* AuthManager::login() {
    std::string username, password;
    std::cout << "--- User Login ---\n";
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;

    std::ifstream file(user_file);
    if (!file) {
        std::cout << "No users have signed up yet.\n";
        return nullptr;
    }

    std::string line;
    while (std::getline(file, line)) {
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
    std::cout << "User not found.\n";
    return nullptr;
}

User* AuthManager::signUp() {
    std::string username, password;
    std::cout << "--- User Sign Up ---\n";
    std::cout << "Choose a username: ";
    std::cin >> username;

    std::ifstream infile(user_file);
    if (infile) {
        std::string line;
        while (std::getline(infile, line)) {
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

    std::cout << "Choose a password: ";
    std::cin >> password;

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
}

void AuthManager::saveUserData(const User& user) const {
    std::string filename = user.getName() + "_wallet.csv";
    std::ofstream file(filename);
    if (!file) {
         std::cerr << "Error: Could not save user data for " << user.getName() << std::endl;
         return;
    }
    
    file << user.getWallet().getCash() << std::endl;
    for (const auto& holding : user.getWallet().getHoldings()) {
        file << holding.first << "," << holding.second << std::endl;
    }
}

User* AuthManager::loadUserData(const std::string& username) const {
    std::string filename = username + "_wallet.csv";
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
    while(std::getline(file, line)) {
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
    loadNextOrderId();
    loadOrders();
}

LimitOrderManager::~LimitOrderManager() {
    saveNextOrderId();
}

void LimitOrderManager::loadNextOrderId() {
    std::ifstream idFile(id_filename);
    if (idFile) idFile >> nextOrderId;
    if (nextOrderId == 0) nextOrderId = 1;
}

void LimitOrderManager::saveNextOrderId() const {
    std::ofstream idFile(id_filename);
    if (idFile) idFile << nextOrderId;
}

void LimitOrderManager::loadOrders() {
    orders.clear();
    std::ifstream file(filename);
    if (!file) return;

    int id, isBuyInt;
    std::string username, symbol;
    double units, price;
    while (file >> id >> username >> symbol >> units >> price >> isBuyInt) {
        orders.emplace_back(id, username, symbol, units, price, (isBuyInt == 1));
    }
}

void LimitOrderManager::saveOrders() const {
    std::ofstream file(filename);
    if (!file) return;

    for (const auto& order : orders) {
        file << order.orderId << " " << order.username << " " << order.symbol << " "
             << order.units << " " << order.desiredPrice << " " << (order.isBuyOrder ? 1 : 0) << std::endl;
    }
}

void LimitOrderManager::addOrder(const std::string& username, const std::string& symbol, double units, double price, bool isBuy) {
    orders.emplace_back(nextOrderId++, username, symbol, units, price, isBuy);
    saveOrders();
    std::cout << "Limit order placed successfully.\n";
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
}

void LimitOrderManager::checkAndExecuteAllOrders(Exchange& ex, AuthManager& auth) {
    bool ordersChanged = false;
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
}