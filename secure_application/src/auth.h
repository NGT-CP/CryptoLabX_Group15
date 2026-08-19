#ifndef AUTH_H
#define AUTH_H

#include <string>

// =====================================================
// VULNERABILITY: Missing Authentication
// - Admin backdoor bypass with hardcoded credentials
// - No session token validation after login
// - Login state stored in a simple global bool
// =====================================================

extern bool isLoggedIn;
extern std::string currentUser;

bool login(const std::string& username, const std::string& password);
void logout();
bool checkAuth();

#endif
