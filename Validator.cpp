#define _CRT_SECURE_NO_WARNINGS
#include "Validator.h"
#include <iostream>
using namespace std;

bool Validator::isValidContact(const string contact) {
    if (contact.length() != 11) {
        cout << "Error, Must be exactly 11 characters.\n";
        return false;
    }
    for (char c : contact) {
        if (!isdigit(c)) {
            std::cout << "Error, Contact number can only contain digits.\n";
            return false;
        }
    }
    return true; 
}
bool Validator::isValidPassword(const string password) {
    return (password.length() >= 6);
}
bool Validator::stringEqual(const string& a, const string& b) {
    return a == b;
}
bool Validator::isValidLength(const string& text, size_t maxLength) {
    return text.length() <= maxLength;
}
bool Validator::isValidID(int id) {
    return id > 0;
}
bool Validator::isPositiveFloat(float value) {
    return value > 0.0f;
}
bool Validator::isValidMenuChoice(int choice, int min, int max) {
    return (choice >= min && choice <= max);
}

// Case insensitive equality
bool Validator::stringEqualCI(const string& a, const string& b) {
    if (a.length() != b.length()) return false;
    for (int i = 0; i < (int)a.length(); i++) {
        char ca = a[i], cb = b[i];
        // Manual tolower
        if (ca >= 'A' && ca <= 'Z') ca = ca - 'A' + 'a';
        if (cb >= 'A' && cb <= 'Z') cb = cb - 'A' + 'a';
        if (ca != cb) return false;
    }
    return true;
}

//Date helpers
// Safely parse a 2-digit substring at position pos from string s
// Returns -1 if characters are not digits
static int parseTwoDigits(const string& s, int pos) {
    if (pos + 1 >= (int)s.length()) return -1;
    char c1 = s[pos], c2 = s[pos + 1];
    if (c1 < '0' || c1 > '9') return -1;
    if (c2 < '0' || c2 > '9') return -1;
    return (c1 - '0') * 10 + (c2 - '0');
}

// Safely parse a 4-digit year starting at position pos
static int parseFourDigits(const string& s, int pos) {
    if (pos + 3 >= (int)s.length()) return -1;
    int result = 0;
    for (int i = pos; i <= pos + 3; i++) {
        if (s[i] < '0' || s[i] > '9') return -1;
        result = result * 10 + (s[i] - '0');
    }
    return result;
}

int Validator::extractDay(const string& date) {
    // DD-MM-YYYY → length must be 10
    if (date.length() != 10) return -1;
    if (date[2] != '-' || date[5] != '-') return -1;
    return parseTwoDigits(date, 0);
}
int Validator::extractMonth(const string& date) {
    if (date.length() != 10) return -1;
    if (date[2] != '-' || date[5] != '-') return -1;
    return parseTwoDigits(date, 3);
}
int Validator::extractYear(const string& date) {
    if (date.length() != 10) return -1;
    if (date[2] != '-' || date[5] != '-') return -1;
    return parseFourDigits(date, 6);
}

int Validator::dateToInt(const string& date) {
    int d = extractDay(date);
    int m = extractMonth(date);
    int y = extractYear(date);
    if (d == -1 || m == -1 || y == -1) return -1;
    return y * 10000 + m * 100 + d;   // YYYYMMDD — naturally sortable
}

bool Validator::isValidDate(const string& date) {
    int d = extractDay(date);
    int m = extractMonth(date);
    int y = extractYear(date);

    if (d == -1 || m == -1 || y == -1) return false;
    if (d < 1 || d > 31)  return false;
    if (m < 1 || m > 12)  return false;

    // Year must be current year or later
    time_t now = time(nullptr);
    tm* t = localtime(&now);
    int currentYear = t->tm_year + 1900;

    if (y < currentYear) return false;

    return true;
}

//Time slot validation
bool Validator::isValidTimeSlot(const string& slot) {
    // 8 fixed daily slots
    const string validSlots[8] = {
        "09:00", "10:00", "11:00", "12:00",
        "13:00", "14:00", "15:00", "16:00"
    };
    for (int i = 0; i < 8; i++)
        if (stringEqual(slot, validSlots[i])) return true;
    return false;
}

bool Validator::isValidName(const string& name) {
    return !name.empty() && name.size() <= 50;
}

bool Validator::isPositiveFloat(const char* str) {
    if (!str || str[0] == '\0') return false;
    bool hasDot = false;
    int i = 0;
    if (str[0] == '-') return false;
    for (; str[i]; i++) {
        if (str[i] == '.' && !hasDot) { hasDot = true; continue; }
        if (str[i] < '0' || str[i] > '9') return false;
    }
    // must be > 0
    float val = 0.f;
    for (int j = 0; str[j]; j++) {
        if (str[j] != '.') val = val * 10 + (str[j] - '0');
    }
    return val > 0.f;
}