#include <iostream>
#include "Prescription.h"
#include "Validator.h"
using namespace std;


Prescription::Prescription() : prescriptionID(0), appointmentID(0), patientID(0), doctorID(0), date(" "), medicines(" "), notes(" ") {}
Prescription::Prescription(int presID, int appID, int patID, int docID, string d, string meds, string n)
    : prescriptionID(presID), appointmentID(appID), patientID(patID), doctorID(docID), date(d), medicines(meds), notes(n) 
{
    setMedicines(meds);
    setNotes(n);
}
Prescription::Prescription(const Prescription& other) : prescriptionID(other.prescriptionID), appointmentID(other.appointmentID), patientID(other.patientID), doctorID(other.doctorID), date(other.date), medicines(other.medicines), notes(other.notes) {}
Prescription& Prescription::operator=(const Prescription& other) {
    if (this == &other) return *this;
    prescriptionID = other.prescriptionID;
    appointmentID = other.appointmentID;
    patientID = other.patientID;
    doctorID = other.doctorID;
    date = other.date;
    medicines = other.medicines;
    notes = other.notes;
    return *this;
}

int Prescription::getID() const { return prescriptionID; }
int Prescription::getPrescriptionID() const { return prescriptionID; }
int Prescription::getAppointmentID() const { return appointmentID; }
int Prescription::getPatientID() const { return patientID; }
int Prescription::getDoctorID() const { return doctorID; }
string Prescription::getDate() const { return date; }
string Prescription::getMedicines() const { return medicines; }
string Prescription::getNotes() const { return notes; }

bool Prescription::setMedicines(const string& meds) {
    if (Validator::isValidLength(meds, 500)) {
        medicines = meds;
        return true;
    }
    cout << "Warning: Notes text too long. Cropped to 300 characters.\n";
    medicines = meds.substr(0, 300);
    return false;
}
bool Prescription::setNotes(const string& n) {
    if (Validator::isValidLength(n, 300)) {
        notes = n;
        return true;
    }
    cout << "Warning: Notes text too long. Cropped to 300 characters.\n";
    notes = n.substr(0, 300);
    return false;
}