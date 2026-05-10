#pragma once
#include <iostream>
using namespace std;

class Validator {
public:
    //Exactly 11 digits, all numeric
    static bool isValidContact(const string contact);

    // Minimum 6 characters
    static bool isValidPassword(const string password);

    static bool stringEqual(const string& a, const string& b);

    static bool isValidLength(const string& text, size_t maxLength);

    // Positive integer ID (> 0)
    static bool isValidID(int id);

    // Date in DD-MM-YYYY format.
    // Day 01-31, month 01-12, year >= currentYear.
    // Does NOT validate days-in-month precisely
    static bool isValidDate(const string& date);

    // One of the 8 fixed daily slots:
    // 09:00 10:00 11:00 12:00 13:00 14:00 15:00 16:00
    static bool isValidTimeSlot(const string& slot);

    // Positive float (> 0)
    static bool isPositiveFloat(float value);
    static bool isPositiveFloat(const char* str);

    // Integer menu choice within [min, max]
    static bool isValidMenuChoice(int choice, int min, int max);

    // Case-insensitive string equality (manual tolower, no library)
    static bool stringEqualCI(const string& a, const string& b);

    // Extract year from DD-MM-YYYY string (returns -1 if malformed)
    static int extractYear(const string& date);

    // Extract month from DD-MM-YYYY (returns -1 if malformed)
    static int extractMonth(const string& date);

    // Extract day from DD-MM-YYYY (returns -1 if malformed)
    static int extractDay(const string& date);

    // Convert date string DD-MM-YYYY to a comparable integer YYYYMMDD
    // Returns -1 if malformed. Used for sorting and difftime comparisons.
    static int dateToInt(const string& date);

    static bool isValidName(const string& name);

 
};
