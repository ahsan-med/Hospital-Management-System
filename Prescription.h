#pragma once
#include <iostream>
using namespace std;


class Prescription {
private:
    int prescriptionID;
    int appointmentID;
    int patientID;
    int doctorID;
    string date;
    string medicines;
    string notes;

public:
    Prescription();
    Prescription(int, int, int, int,string, string, string);
    Prescription(const Prescription& other);
    Prescription& operator=(const Prescription& other);

    // Getter Functions
    int getID() const;
    int getPrescriptionID() const;
    int getAppointmentID() const;
    int getPatientID() const;
    int getDoctorID() const;
    string getDate() const;
    string getMedicines() const;
    string getNotes() const;

    // Setter Functions
    bool setMedicines(const string& meds);
    bool setNotes(const string& n);
};

