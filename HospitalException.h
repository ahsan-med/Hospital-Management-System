#pragma once
#include <iostream>
using namespace std;

class HospitalException {
protected:
    string message;
public:
    HospitalException();
    HospitalException(const string& msg);
    virtual const char* what() const;
    virtual ~HospitalException();
};
class FileNotFoundException : public HospitalException {
public:
    FileNotFoundException(const string& filename);
};
class InsufficientFundsException : public HospitalException {
public:
    InsufficientFundsException(float required, float available);
    InsufficientFundsException(const string& detail);
};
class InvalidInputException : public HospitalException {
public:
    InvalidInputException(const string& detail);
};
class SlotUnavailableException : public HospitalException {
public:
    SlotUnavailableException(const string& slot);
};