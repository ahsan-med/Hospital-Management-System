#define _CRT_SECURE_NO_WARNINGS
#include "HospitalSystem.h"
#include "FileHandler.h"
#include "Validator.h"
#include "HospitalException.h"
#include <iostream>
#include <ctime>
#include <string>
using namespace std;

//Static member
const string HospitalSystem::TIME_SLOTS[HospitalSystem::SLOT_COUNT] = {
    "09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00"
};

//Constructor / Destructor
HospitalSystem::HospitalSystem() {}
HospitalSystem::~HospitalSystem() { saveAll(); }

// Private helper fucntions
string HospitalSystem::getTodayDate() const {
    time_t now = time(nullptr);
    char buf[11];
    strftime(buf, sizeof(buf), "%d-%m-%Y", localtime(&now));
    return string(buf);
}
string HospitalSystem::getTimestamp() const {
    time_t now = time(nullptr);
    char buf[20];
    strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", localtime(&now));
    return string(buf);
}
void HospitalSystem::logSecurityEvent(const string& role, const string& enteredID, const string& result) const {
    // delegate to the new singleton FileHandler
    FileHandler::getInstance()->logSecurityEvent(role, enteredID, result);
}

//Convenience aliases for the in-memory stores 
// (all calls below use these instead of repeating getInstance()->loadXxx())
#define FH FileHandler::getInstance()
#define PATS FH->loadPatients()
#define DOCS FH->loadDoctors()
#define ADMS FH->loadAdmin()
#define APPT FH->loadAppointments()
#define BILL FH->loadBills()
#define PRSC FH->loadPrescriptions()


bool HospitalSystem::isSlotAvailable(int doctorID, const string& date, const string& slot) const {
    for (int i = 0; i < APPT.size(); i++) {
        Appointment* a = APPT.getAt(i);
        if (!a) continue;
        if (a->getDoctorID() != doctorID) continue;
        if (!Validator::stringEqual(a->getDate(), date)) continue;
        if (!Validator::stringEqual(a->getTimeSlot(), slot)) continue;
        if (!Validator::stringEqual(a->getStatus(), "cancelled"))return false;
    }
    return true;
}
Bill* HospitalSystem::findBillByAppointment(int appointmentID) const {
    for (int i = 0; i < BILL.size(); i++) {
        Bill* b = BILL.getAt(i);
        if (b && b->getAppointmentID() == appointmentID) return b;
    }
    return nullptr;
}
Prescription* HospitalSystem::findPrescByAppointment(int appointmentID) const {
    for (int i = 0; i < PRSC.size(); i++) {
        Prescription* p = PRSC.getAt(i);
        if (p && p->getAppointmentID() == appointmentID) return p;
    }
    return nullptr;
}
int HospitalSystem::countUnpaidBills(int patientID) const {
    int count = 0;
    for (int i = 0; i < BILL.size(); i++) {
        Bill* b = BILL.getAt(i);
        if (b && b->getPatientID() == patientID
            && Validator::stringEqual(b->getStatus(), "unpaid"))
            count++;
    }
    return count;
}
bool HospitalSystem::hasPendingAppointments(int patientID) const {
    for (int i = 0; i < APPT.size(); i++) {
        Appointment* a = APPT.getAt(i);
        if (a && a->getPatientID() == patientID
            && Validator::stringEqual(a->getStatus(), "pending"))
            return true;
    }
    return false;
}
bool HospitalSystem::doctorHasPending(int doctorID) const {
    for (int i = 0; i < APPT.size(); i++) {
        Appointment* a = APPT.getAt(i);
        if (a && a->getDoctorID() == doctorID
            && Validator::stringEqual(a->getStatus(), "pending"))
            return true;
    }
    return false;
}

//Sorting functions
void HospitalSystem::sortAppointmentsAsc(Appointment** arr, int n) const {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (Validator::dateToInt(arr[j]->getDate()) >
                Validator::dateToInt(arr[j + 1]->getDate())) {
                Appointment* t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
            }
}
void HospitalSystem::sortAppointmentsDesc(Appointment** arr, int n) const {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (Validator::dateToInt(arr[j]->getDate()) <
                Validator::dateToInt(arr[j + 1]->getDate())) {
                Appointment* t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
            }
}
void HospitalSystem::sortAppointmentsBySlot(Appointment** arr, int n) const {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (arr[j]->getTimeSlot() > arr[j + 1]->getTimeSlot()) {
                Appointment* t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
            }
}
void HospitalSystem::sortPrescriptionsDesc(Prescription** arr, int n) const {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (Validator::dateToInt(arr[j]->getDate()) <
                Validator::dateToInt(arr[j + 1]->getDate())) {
                Prescription* t = arr[j]; arr[j] = arr[j + 1]; arr[j + 1] = t;
            }
}

// Startup / shutdown
void HospitalSystem::loadAll() {
    FH->loadAll();   // FileHandler handles file I/O and in-memory population
}
void HospitalSystem::saveAll() {
    FH->saveAll();
}

// Login
Patient* HospitalSystem::loginPatient(int id, const string& password) {
    Patient* p = PATS.findByID(id);
    if (p && Validator::stringEqual(p->getPassword(), password)) {
        logSecurityEvent("Patient", to_string(id), "SUCCESS");
        return p;
    }
    logSecurityEvent("Patient", to_string(id), "FAILED");
    return nullptr;
}
Doctor* HospitalSystem::loginDoctor(int id, const string& password) {
    Doctor* d = DOCS.findByID(id);
    if (d && Validator::stringEqual(d->getPassword(), password)) {
        logSecurityEvent("Doctor", to_string(id), "SUCCESS");
        return d;
    }
    logSecurityEvent("Doctor", to_string(id), "FAILED");
    return nullptr;
}
Admin* HospitalSystem::loginAdmin(int id, const string& password) {
    Admin* a = ADMS.findByID(id);
    if (a && Validator::stringEqual(a->getPassword(), password)) {
        logSecurityEvent("Admin", to_string(id), "SUCCESS");
        return a;
    }
    logSecurityEvent("Admin", to_string(id), "FAILED");
    return nullptr;
}

// Patient operations functions
bool HospitalSystem::registerPatient(const string& name, int age, char gender, const string& contact, const string& password, double balance) {
    if (PATS.isFull())
        throw InvalidInputException("Patient list is full.");
    if (!Validator::isValidContact(contact))
        throw InvalidInputException("Invalid contact number.");
    if (!Validator::isValidPassword(password))
        throw InvalidInputException("Password too short (min 6 chars).");

    int newID = FH->getMaxPatientID() + 1;
    Patient p(newID, name, age, gender, contact, password, balance);
    FH->appendPatient(p);   // appendPatient adds to in-memory store AND writes file
    return true;
}
Patient* HospitalSystem::findPatient(int id) const {
    return PATS.findByID(id);
}

// Doctor operation functions
bool HospitalSystem::addDoctor(const string& name, const string& spec, const string& contact, const string& password, float fee) {
    if (DOCS.isFull())
        throw InvalidInputException("Doctor list is full.");
    if (!Validator::isValidContact(contact))
        throw InvalidInputException("Invalid contact number.");
    if (!Validator::isValidPassword(password))
        throw InvalidInputException("Password too short (min 6 chars).");
    if (!Validator::isPositiveFloat(fee))
        throw InvalidInputException("Fee must be a positive value.");

    int newID = FH->getMaxDoctorID() + 1;
    Doctor d(newID, name, spec, contact, password, fee);
    FH->appendDoctor(d);
    return true;
}
bool HospitalSystem::removeDoctor(int doctorID) {
    if (!findDoctor(doctorID)) return false;
    if (doctorHasPending(doctorID)) return false;
    FH->deleteDoctor(doctorID);
    return true;
}
Doctor* HospitalSystem::findDoctor(int id) const {
    return DOCS.findByID(id);
}

// Appointment operation functions
int HospitalSystem::bookAppointment(int patientID, int doctorID, const string& date, const string& slot) {
    if (APPT.isFull())
        throw InvalidInputException("Appointment list is full.");

    Patient* p = findPatient(patientID);
    Doctor* d = findDoctor(doctorID);
    if (!p) throw InvalidInputException("Patient ID not found.");
    if (!d) throw InvalidInputException("Doctor ID not found.");

    // Conflict check via overloaded ==
    Appointment incoming(0, patientID, doctorID, date, slot, "pending");
    for (int i = 0; i < APPT.size(); i++) {
        Appointment* a = APPT.getAt(i);
        if (a && *a == incoming)
            throw SlotUnavailableException(date + " " + slot);
    }

    // Balance check → InsufficientFundsException
    if (p->getBalance() < d->getFee())
        throw InsufficientFundsException(d->getFee(), (float)p->getBalance());

    // Deduct via operator-=
    *p -= d->getFee();

    // Persist new appointment
    int appID = FH->getMaxAppointmentID() + 1;
    Appointment newAppt(appID, patientID, doctorID, date, slot, "pending");
    FH->appendAppointment(newAppt);

    // Persist new bill
    int billID = FH->getMaxBillID() + 1;
    Bill newBill(billID, patientID, appID, d->getFee(), "unpaid", date);
    FH->appendBill(newBill);

    // Persist updated patient balance
    FH->updatePatient(*p);

    return appID;
}
bool HospitalSystem::cancelAppointment(int appointmentID, int patientID) {
    Appointment* a = findAppointment(appointmentID);
    if (!a || a->getPatientID() != patientID
        || !Validator::stringEqual(a->getStatus(), "pending")) {
        return false;
    }

    Doctor* d = findDoctor(a->getDoctorID());
    Patient* p = findPatient(patientID);
    if (!d || !p) return false;

    // Refund via operator+=
    *p += d->getFee();

    a->setStatus("cancelled");
    FH->updateAppointment(*a);

    Bill* b = findBillByAppointment(appointmentID);
    if (b) {
        b->setStatus("cancelled");
        FH->updateBill(*b);
    }

    FH->updatePatient(*p);

    return true;
}
bool HospitalSystem::completeAppointment(int appointmentID, int doctorID) {
    Appointment* a = findAppointment(appointmentID);
    if (!a || a->getDoctorID() != doctorID
        || !Validator::stringEqual(a->getStatus(), "pending")) {
        return false;
    }
    a->setStatus("completed");
    FH->updateAppointment(*a);
    return true;
}
bool HospitalSystem::markNoShow(int appointmentID, int doctorID) {
    Appointment* a = findAppointment(appointmentID);
    if (!a || a->getDoctorID() != doctorID
        || !Validator::stringEqual(a->getStatus(), "pending")) {
        return false;
    }
    a->setStatus("noshow");
    FH->updateAppointment(*a);

    Bill* b = findBillByAppointment(appointmentID);
    if (b) {
        b->setStatus("cancelled");
        FH->updateBill(*b);
    }
    return true;
}
Appointment* HospitalSystem::findAppointment(int id) const {
    return APPT.findByID(id);
}

// Billing operation functions
bool HospitalSystem::payBill(int billID, int patientID) {
    Bill* b = findBill(billID);
    if (!b || b->getPatientID() != patientID
        || !Validator::stringEqual(b->getStatus(), "unpaid")) {
        return false;
    }

    Patient* p = findPatient(patientID);
    if (!p) return false;

    if (p->getBalance() < b->getAmount())
        throw InsufficientFundsException(b->getAmount(), (float)p->getBalance());

    // Deduct via operator-=
    *p -= b->getAmount();
    b->setStatus("paid");

    FH->updateBill(*b);
    FH->updatePatient(*p);

    return true;
}
void HospitalSystem::topUp(int patientID, double amount) {
    Patient* p = findPatient(patientID);
    if (!p) throw InvalidInputException("Patient not found.");
    if (amount <= 0) throw InvalidInputException("Amount must be greater than 0.");

    // Add via operator+=
    *p += amount;
    FH->updatePatient(*p);
}
Bill* HospitalSystem::findBill(int id) const {
    return BILL.findByID(id);
}

// Prescription operation functions
int HospitalSystem::writePrescription(int appointmentID, int doctorID, const string& medicines, const string& notes) {
    Appointment* a = findAppointment(appointmentID);
    if (!a || a->getDoctorID() != doctorID)
        throw InvalidInputException("Invalid appointment ID.");
    if (!Validator::stringEqual(a->getStatus(), "completed"))
        throw InvalidInputException("Appointment is not completed.");
    if (FH->prescriptionExists(appointmentID)) {
        return -1;
    }

    int newID = FH->getMaxPrescID() + 1;
    Prescription pr(newID, appointmentID,
        a->getPatientID(), doctorID,
        getTodayDate(), medicines, notes);
    FH->appendPrescription(pr);
    return newID;
}
Prescription* HospitalSystem::findPrescription(int id) const {
    return PRSC.findByID(id);
}

// Admin operation functions
bool HospitalSystem::dischargePatient(int patientID) {
    Patient* p = findPatient(patientID);
    if (!p) return false;
    if (countUnpaidBills(patientID) > 0) return false;
    if (hasPendingAppointments(patientID)) return false;

    // FileHandler handles archiving to discharged.txt and removing records
    FH->dischargePatient(patientID);

    return true;
}