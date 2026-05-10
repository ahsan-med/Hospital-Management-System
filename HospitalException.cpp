#include "HospitalException.h"
#include <string>
#include <cstdio>
#include <iostream>
using namespace std;

//HospitalException
HospitalException::HospitalException() : message("") {}
HospitalException::HospitalException(const string& msg) : message(msg) {}
const char* HospitalException::what() const {
    return message.c_str();
}
HospitalException::~HospitalException() {}

//FileNotFoundException
FileNotFoundException::FileNotFoundException(const string& filename) : HospitalException("File not found: " + filename) {}

//InsufficientFundsException
InsufficientFundsException::InsufficientFundsException(float required, float available) {
    char buf[200];
    snprintf(buf, sizeof(buf), "Insufficient funds. Required: PKR %.2f, Available: PKR %.2f", required, available);
    message = string(buf);
}

InsufficientFundsException::InsufficientFundsException(const string& detail) : HospitalException(detail) {}

//InvalidInputException
InvalidInputException::InvalidInputException(const string& detail) : HospitalException("Invalid input: " + detail) {}

//SlotUnavailableException
SlotUnavailableException::SlotUnavailableException(const string& slot) : HospitalException("Time slot unavailable: " + slot) {}