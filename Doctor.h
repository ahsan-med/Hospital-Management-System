#pragma once
#include "Person.h"
#include <iostream>
using namespace std;

class Doctor : public Person {
private:
    string specialization; 
    float  fee;

public:
    Doctor();
    Doctor(int, string, string, string, string, float);
    Doctor(const Doctor& other);
    Doctor& operator=(const Doctor& other);

    // Overloaded operators
    bool operator==(const Doctor& other) const;
    friend ostream& operator<<(ostream& out, const Doctor& d);

    // Pure virtual implementations
    void displayMenu() override;
    string getRole() const override;

    // Getters
    string getSpecialization() const;
    float getFee() const;

    // Setters
    void setSpecialization(string);
    void setFee(float);

    string getSpec() const;
    bool matchesSpec(const string& s) const;
};