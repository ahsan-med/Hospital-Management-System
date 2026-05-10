#include <iostream>
#include "Bill.h"
using namespace std;

Bill::Bill() : billID(0), patientID(0), appointmentID(0), amount(0.0f), status("unpaid"), date(" ") {}
Bill::Bill(int bID, int pID, int aID, float amt, string st, string d) 
    : billID(bID), patientID(pID), appointmentID(aID), amount(amt), status(st), date(d) {
}
Bill::Bill(const Bill& other) : billID(other.billID), patientID(other.patientID), appointmentID(other.appointmentID), amount(other.amount), status(other.status), date(other.date) {}
Bill& Bill::operator=(const Bill& other) {
    if (this == &other) return *this;
    billID = other.billID;
    patientID = other.patientID;
    appointmentID = other.appointmentID;
    amount = other.amount;
    status = other.status;
    date = other.date;
    return *this;
}

int Bill::getID() const { return billID; }
int Bill::getApptID() const { return appointmentID; }
int Bill::getBillID() const { return billID; }
int Bill::getPatientID() const { return patientID; }
int Bill::getAppointmentID() const { return appointmentID; }
float Bill::getAmount() const { return amount; }
string Bill::getStatus() const { return status; }
string Bill::getDate() const { return date; }

void Bill::setStatus(string s) { status = s; }