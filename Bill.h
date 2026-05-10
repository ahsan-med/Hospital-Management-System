#pragma once
#include <iostream>
using namespace std;

class Bill {
private:
    int billID;
    int patientID;
    int appointmentID;
    float amount;
    string status;   // unpaid / paid / cancelled
    string date;     // DD-MM-YYYY

public:
    Bill();
    Bill(int, int, int, float, string, string);
    Bill(const Bill& other);
    Bill& operator=(const Bill& other);

    // Getters
    int getID() const;
    int getApptID() const;
    int getBillID() const;
    int getPatientID() const;
    int getAppointmentID() const;
    float getAmount() const;
    string getStatus() const;
    string getDate() const;

    // Setters
    void setStatus(string);
};