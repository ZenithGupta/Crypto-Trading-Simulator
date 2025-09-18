#pragma once
#include <string>
#include <vector>
#include <stdexcept>

class AuthException : public std::runtime_error {
public:
    explicit AuthException(const std::string& message) : std::runtime_error(message) {}
};

class UserNotFoundException : public AuthException {
public:
    explicit UserNotFoundException(const std::string& username)
        : AuthException("Error: User '" + username + "' not found.") {}
};

class InvalidPasswordException : public AuthException {
public:
    InvalidPasswordException() : AuthException("Error: Invalid password.") {}
};

class UserAlreadyExistsException : public AuthException {
public:
    explicit UserAlreadyExistsException(const std::string& username)
        : AuthException("Error: User '" + username + "' already exists.") {}
};

class AuthManager {
private:
   
    struct User {
        std::string username;
        std::string hashedPassword;
    };

    std::vector<User> users;
    std::string filename;

    void loadUsers();
    void saveUsers() const;
    std::string hashPassword(const std::string& password) const;

public:
    
    AuthManager(const std::string& fname = "users.txt");
    
    ~AuthManager();

    bool login(const std::string& username, const std::string& password);
    void signUp(const std::string& username, const std::string& password);
    bool isAdmin(const std::string& username) const;
};