#include "auth.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional> 

AuthManager::AuthManager(const std::string& fname) : filename(fname) {
    try {
        loadUsers();
    } catch (const std::exception& e) {
        std::cerr << "Notice: Could not load user file. Creating a new one.\n";
        std::ofstream newFile(filename);
        if (!newFile) {
            throw AuthException("FATAL: Could not create user file at " + filename);
        }
        newFile.close();
    }
}

AuthManager::~AuthManager() {
    try {
        saveUsers();
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: Could not save user data! " << e.what() << std::endl;
    }
}

void AuthManager::loadUsers() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return;
    }

    users.clear();
    std::string line;
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string username, hashedPassword;
        if (ss >> username >> hashedPassword) {
            users.push_back({username, hashedPassword});
        }
    }
    file.close();
}

void AuthManager::saveUsers() const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw AuthException("Could not open " + filename + " for writing.");
    }

    for (const auto& user : users) {
        file << user.username << " " << user.hashedPassword << std::endl;
    }
    file.close();
}

std::string AuthManager::hashPassword(const std::string& password) const {
    std::hash<std::string> hasher;
    std::stringstream ss;
    ss << std::hex << hasher(password);
    return ss.str();
}

bool AuthManager::login(const std::string& username, const std::string& password) {
    std::string hashedAttempt = hashPassword(password);
    for (const auto& user : users) {
        if (user.username == username) {
            if (user.hashedPassword == hashedAttempt) {
                return true; 
            } else {
                throw InvalidPasswordException();
            }
        }
    }
    throw UserNotFoundException(username);
}

void AuthManager::signUp(const std::string& username, const std::string& password) {
    for (const auto& user : users) {
        if (user.username == username) {
            throw UserAlreadyExistsException(username);
        }
    }

    std::string hashedPassword = hashPassword(password);
    users.push_back({username, hashedPassword});
    
    saveUsers(); 
}

bool AuthManager::isAdmin(const std::string& username) const {
    
    return username == "admin";
}