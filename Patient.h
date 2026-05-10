#pragma once
#include "Person.h"
#include <iostream>
using namespace std;

class Patient : public Person
{
private:
	int age;
	char gender;
	double balance;

public:
	Patient();
	Patient(int, string, int, char, string, string, double);
	Patient(const Patient& other);
	Patient& operator=(const Patient& other);

	// Overloaded operators
	Patient& operator+=(double amount);         
	Patient& operator-=(double amount);         
	bool operator==(const Patient& other) const; 
	friend ostream& operator<<(ostream& out, const Patient& p);

	//Pure virtual implementations
	void displayMenu() override;
	string getRole() const override;

	// Getters
	int getAge() const;
	char getGender() const;
	double getBalance() const;

	// Setters
	void setAge(int);
	void setGender(char);
	void setBalance(double);

};

