#pragma once
#include "Person.h"
#include <iostream>
using namespace std;

class Admin : public Person {
public:
    Admin();
    Admin(int , string name, string password);
    Admin(const Admin& other);
    Admin& operator=(const Admin& other);

    void displayMenu() override;
    string getRole() const override;
};