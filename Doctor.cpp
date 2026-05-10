#include "Doctor.h"
#include <iostream>
using namespace std;

Doctor::Doctor() : Person(), specialization(nullptr), fee(0.0f) { 
    specialization = " ";
}
Doctor::Doctor(int id, string name, string spec, string contact, string password, float fee)
    : Person(id, name, contact, password), specialization(spec), fee(fee) {
}
Doctor::Doctor(const Doctor& other) : Person(other), specialization(other.specialization), fee(other.fee) {}

//overloaded operators
Doctor& Doctor::operator=(const Doctor& other) {
    if (this == &other) return *this;
    Person::operator=(other);
    specialization = other.specialization;
    fee = other.fee;
    return *this;
}
bool Doctor::operator==(const Doctor& other) const {
    return id == other.id;
}
ostream& operator<<(ostream& out, const Doctor& d) {
    out << "Doctor[" << d.id << "] " << d.name << " | Spec: " << d.specialization << " | Fee: PKR " << d.fee << " | Contact: " << d.contact;
    return out;
}

void Doctor::displayMenu() {}

string Doctor::getRole() const { return "Doctor"; }
string Doctor::getSpecialization() const { return specialization; }
float Doctor::getFee() const { return fee; }
void Doctor::setSpecialization(string s) {
    specialization = s;
}
void Doctor::setFee(float f) { fee = f; }
string Doctor::getSpec() const {
    return specialization;
}

bool Doctor::matchesSpec(const string& s) const {
    // case-insensitive manual compare
    if (s.size() != specialization.size()) return false;
    for (int i = 0; i < (int)s.size(); i++) {
        char a = s[i], b = specialization[i];
        if (a >= 'A' && a <= 'Z') a += 32;
        if (b >= 'A' && b <= 'Z') b += 32;
        if (a != b) return false;
    }
    return true;
}