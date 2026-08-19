#include "auth.h"
#include <iostream>

// Global auth state — no session management, no token
bool isLoggedIn = false;
std::string currentUser = "";

bool login(const std::string& username, const std::string& password) {
    // --------------------------------------------------
    // VULNERABILITY 1: Missing Authentication
    // Hardcoded admin backdoor — any user can bypass
    // authentication by entering "admin" / "admin123".
    // No rate limiting, no lockout, no hashing.
    // --------------------------------------------------
    if (username == "admin" && password == "admin123") {
        isLoggedIn = true;
        currentUser = username;
        std::cout << "[AUTH] Backdoor admin login granted.\n";
        return true;
    }

    // Normal user check (plaintext comparison — insecure)
    if (username == "pilot" && password == "drone2024") {
        isLoggedIn = true;
        currentUser = username;
        std::cout << "[AUTH] Login successful. Welcome, " << username << "!\n";
        return true;
    }

    std::cout << "[AUTH] Login failed. Invalid credentials.\n";
    return false;
}

void logout() {
    isLoggedIn = false;
    currentUser = "";
    std::cout << "[AUTH] Logged out.\n";
}

bool checkAuth() {
    // VULNERABILITY: Auth check is just a global bool —
    // can be trivially bypassed if called from wrong context.
    return isLoggedIn;
}
