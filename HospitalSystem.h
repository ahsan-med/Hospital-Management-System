#pragma once
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "Storage.h"
#include <string>
using namespace std;

class HospitalSystem {
private:
    // Time/date helper functions 
    string getTodayDate()  const;   // DD-MM-YYYY
    string getTimestamp()  const;   // DD-MM-YYYY HH:MM:SS

    // Slot / search helper function
    bool isSlotAvailable(int doctorID, const string& date, const string& slot) const;
    Bill* findBillByAppointment(int appointmentID) const;
    Prescription* findPrescByAppointment(int appointmentID) const;

    int countUnpaidBills(int patientID) const;
    bool hasPendingAppointments(int patientID) const;
    bool doctorHasPending(int doctorID) const;

    // sorting functions
    void sortAppointmentsAsc(Appointment** arr, int n) const;
    void sortAppointmentsDesc(Appointment** arr, int n) const;
    void sortAppointmentsBySlot(Appointment** arr, int n) const;
    void sortPrescriptionsDesc(Prescription** arr, int n) const;

    // Security log event function
    void logSecurityEvent(const string& role, const string& enteredID, const string& result) const;

    //Time Slot static variables for fixed number of slots and slot counts array
    static const int SLOT_COUNT = 8;
    static const string TIME_SLOTS[SLOT_COUNT];

public:
    HospitalSystem();
    ~HospitalSystem();

    // no copy
    HospitalSystem(const HospitalSystem&) = delete;
    HospitalSystem& operator=(const HospitalSystem&) = delete;

    // Startup / shutdown
    void loadAll();   // goes to FileHandler::getInstance()->loadAll()
    void saveAll();   // goes to FileHandler::getInstance()->saveAll()

    //Login
    Patient* loginPatient(int id, const string& password);
    Doctor* loginDoctor(int id, const string& password);
    Admin* loginAdmin(int id, const string& password);

    // Patient operations 
    bool registerPatient(const string& name, int age, char gender, const string& contact, const string& password, double balance = 0.0);
    Patient* findPatient(int id) const;

    // Doctor operations
    bool addDoctor(const string& name, const string& spec, const string& contact, const string& password, float fee);
    bool removeDoctor(int doctorID);
    Doctor* findDoctor(int id) const;

    // Appointment operations
    int bookAppointment(int patientID, int doctorID, const string& date, const string& slot);
    bool cancelAppointment(int appointmentID, int patientID);
    bool completeAppointment(int appointmentID, int doctorID);
    bool markNoShow(int appointmentID, int doctorID);
    Appointment* findAppointment(int id) const;

    //Billing operations functions
    bool payBill(int billID, int patientID);
    void topUp(int patientID, double amount);
    Bill* findBill(int id) const;

    //Prescription operations functions
    int writePrescription(int appointmentID, int doctorID, const string& medicines, const string& notes);
    Prescription* findPrescription(int id) const;

    //Admin operations functions
    bool dischargePatient(int patientID);
};
