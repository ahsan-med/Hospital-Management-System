#include "Person.h"
#include <string>
#include "Validator.h"
using namespace std;

Person::Person() : id(0) {}
Person::Person(int id, string n, string c, string p) {
	this->id = id;
	name = n;
	contact = c;
	password = p;
}
Person::Person(const Person& other) : id(other.id), name(other.name), contact(other.contact), password(other.password) {}

//overloaded functions
Person& Person::operator=(const Person& other) {
	if (this == &other) 
		return *this;
	id = other.id;
	name = other.name;
	contact = other.contact;
	password = other.password;
	return *this;
}

bool Person::checkPassword(const string& pwd) const {
	return password == pwd;
}

int Person::getID() const { return id; }
string Person::getName() const { return name; }
string Person::getContact() const { return contact; }
string Person::getPassword() const { return password; }

void Person::setName(const string n) {
	name = n;
}
void Person::setContact(const string c) {
	if (Validator::isValidContact(c)) {
		contact = c;
	} 
}
void Person::setPassword(const string p) {
	if (Validator::isValidPassword(p)) {
		password = p;
	}
}