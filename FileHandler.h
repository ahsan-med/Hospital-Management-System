#pragma once
#include <fstream>
#include <string>
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "Storage.h"
using namespace std;

// UITable is used in loadSecurityLog function forward declartion here only.
class UITable;

class FileHandler {
public:
    //Singleton access
    static FileHandler* getInstance();

    //load/save 
    void loadAll();
    void saveAll();

    // Storage accessors, return references
    Storage<Patient>& loadPatients();
    Storage<Doctor>& loadDoctors();
    Storage<Admin>& loadAdmin();
    Storage<Appointment>& loadAppointments();
    Storage<Bill>& loadBills();
    Storage<Prescription>& loadPrescriptions();

    // Append, write one new record to file
    void appendPatient(const Patient& p);
    void appendDoctor(const Doctor& d);
    void appendAppointment(const Appointment& a);
    void appendBill(const Bill& b);
    void appendPrescription(const Prescription& p);

    // Update, rewrite full file 
    void updatePatient(const Patient& p);
    void updateDoctor(const Doctor& d);
    void updateAppointment(const Appointment& a);
    void updateBill(const Bill& b);

    //Delete Doctor
    void deleteDoctor(int id);

    // Max ID helpers
    int getMaxPatientID() const;
    int getMaxDoctorID() const;
    int getMaxAppointmentID() const;
    int getMaxBillID() const;
    int getMaxPrescID() const;

    // Business logic helpers
    bool prescriptionExists(int appointmentID) const;
    bool hasUnpaidBills(int patientID) const;
    bool hasPendingAppts(int patientID) const;
    void dischargePatient(int patientID);

    // Security log filing
    void logSecurityEvent(const string& role, const string& enteredID, const string& result);

    // UITable* is a pointer, full definition only needed in .cpp
    void loadSecurityLog(UITable& table);

    //File creation in the starting
    static void createFilesIfMissing();

private:
    FileHandler();
    static FileHandler* s_instance;

    Storage<Patient> m_patients;
    Storage<Doctor> m_doctors;
    Storage<Admin> m_admins;
    Storage<Appointment> m_appointments;
    Storage<Bill> m_bills;
    Storage<Prescription> m_prescriptions;

    bool m_loaded;

    // CSV write helpers
    void writePatientLine(ofstream& out, const Patient& p);
    void writeDoctorLine(ofstream& out, const Doctor& d);
    void writeAdminLine(ofstream& out, const Admin& a);
    void writeAppointmentLine(ofstream& out, const Appointment& a);
    void writeBillLine(ofstream& out, const Bill& b);
    void writePrescriptionLine(ofstream& out, const Prescription& p);

    string getTimestamp() const;
};