#include "Admin.h"
#include <iostream>
using namespace std;

Admin::Admin() : Person() {}
Admin::Admin(int id, string n, string p) : Person(id, n, "", p) {}
Admin::Admin(const Admin& other) : Person(other) {}
Admin& Admin::operator=(const Admin& other) {
    if (this == &other) return *this;
    Person::operator=(other);
    return *this;
}
void Admin::displayMenu() {}
string Admin::getRole() const { return "Admin"; }