#include <iostream>
#include "Appointment.h"
#include "Validator.h"
using namespace std;

Appointment::Appointment() : appointmentID(0), patientID(0), doctorID(0), date(" "), timeSlot(" "), status("pending") {}
Appointment::Appointment(int appID, int patID, int docID, string d, string slot, string st) 
    : appointmentID(appID), patientID(patID), doctorID(docID), date(d), timeSlot(slot), status(st) {
}
Appointment::Appointment(const Appointment& other) : appointmentID(other.appointmentID), patientID(other.patientID), doctorID(other.doctorID), date(other.date), timeSlot(other.timeSlot), status(other.status) {}
Appointment& Appointment::operator=(const Appointment& other) {
    if (this == &other) return *this;
    appointmentID = other.appointmentID;
    patientID = other.patientID;
    doctorID = other.doctorID;
    date = other.date;
    timeSlot = other.timeSlot;
    status = other.status;
    return *this;
}
//check conflict, same doctor, same date, same slot, neither cancelled
bool Appointment::operator==(const Appointment& other) const {
    if (doctorID != other.doctorID) return false;
    if (!Validator::stringEqual(date, other.date))     return false;
    if (!Validator::stringEqual(timeSlot, other.timeSlot)) return false;
    if (Validator::stringEqual(status, "cancelled")) return false;
    if (Validator::stringEqual(other.status, "cancelled")) return false;
    return true;
}

ostream& operator<<(ostream& out, const Appointment& a) {
    out << "Appt[" << a.appointmentID << "] " << "Patient:" << a.patientID << " Doctor:" << a.doctorID << " Date:" << a.date << " Slot:" << a.timeSlot << " Status:" << a.status;
    return out;
}

int Appointment::getID() const { return appointmentID;}
int Appointment::getAppointmentID() const { return appointmentID; }
int Appointment::getPatientID() const { return patientID; }
int Appointment::getDoctorID() const { return doctorID; }
string Appointment::getDate() const { return date; }
string Appointment::getTimeSlot() const { return timeSlot; }
string Appointment::getStatus() const { return status; }

void Appointment::setStatus(string s) { status = s; }
void Appointment::setDate(string d) { date = d; }
void Appointment::setTimeSlot(string t) { timeSlot = t; }
