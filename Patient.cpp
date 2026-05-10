#include "Patient.h"
#include <iostream>
using namespace std;


Patient::Patient() : Person(), age(0), gender('M'), balance(0.0) {}
Patient::Patient(int id, string n, int age, char gender, string contact, string password, double balance)
    : Person(id, n, contact, password), age(age), gender(gender), balance(balance) {
}
Patient::Patient(const Patient& other) : Person(other), age(other.age), gender(other.gender), balance(other.balance) {}

//overloaded functions
Patient& Patient::operator=(const Patient& other) {
    if (this == &other) return *this;
    Person::operator=(other);
    age = other.age;
    gender = other.gender;
    balance = other.balance;
    return *this;
}
Patient& Patient::operator+=(double amount) {
    balance += amount;
    return *this;
}
Patient& Patient::operator-=(double amount) {
    balance -= amount;
    return *this;
}
bool Patient::operator==(const Patient& other) const {
    return id == other.id;
}
ostream& operator<<(ostream& out, const Patient& p) {
    out << "Patient[" << p.id << "] " << p.name << " | Age: " << p.age << " | Gender: " << p.gender << " | Contact: " << p.contact << " | Balance: PKR " << p.balance;
    return out;
}

void Patient::displayMenu() {}   // handled in GUI
string Patient::getRole() const { return "Patient"; }

int Patient::getAge() const { return age; }
char Patient::getGender() const { return gender; }
double Patient::getBalance() const { return balance; }

void Patient::setAge(int a) { age = a; }
void Patient::setGender(char g) { gender = g; }
void Patient::setBalance(double b) { balance = b; }

