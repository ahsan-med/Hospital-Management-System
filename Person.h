#pragma once
#include <iostream>
using namespace std;

class Person
{
protected:
	int id;
	string name;
	string contact;
	string password;
public:
	Person();
	Person(int, string, string, string);
	Person(const Person& other);
	Person& operator= (const Person& other);

	//Pure Virtual Functions
	virtual void displayMenu() = 0;
	virtual string getRole() const = 0;

	bool checkPassword(const string& pwd) const;

	//Getter Functions
	int getID() const;
	string getName() const;
	string getContact() const;
	string getPassword() const;

	//Setter Functions
	void setName(const string);
	void setContact(const string);
	void setPassword(const string);
};

