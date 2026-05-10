#pragma once
#include <iostream>
using namespace std;

class Appointment {
private:
    int appointmentID;
    int patientID;
    int doctorID;
    string date;       // DD-MM-YYYY
    string timeSlot;   // HH:MM
    string status;     // pending / completed / noshow / cancelled

public:
    Appointment();
    Appointment(int, int, int, string, string, string);
    Appointment(const Appointment& other);
    Appointment& operator=(const Appointment& other);

    // == detects scheduling conflict: same doctor, same date, same slot, and neither is cancelled
    bool operator==(const Appointment& other) const;
    friend ostream& operator<<(ostream& out, const Appointment& a);

    // Getters
    int getID() const;
    int getAppointmentID() const;
    int getPatientID() const;
    int getDoctorID() const;
    string getDate() const;
    string getTimeSlot() const;
    string getStatus() const;

    // Setters
    void setStatus(string);
    void setDate(string);
    void setTimeSlot(string);
};