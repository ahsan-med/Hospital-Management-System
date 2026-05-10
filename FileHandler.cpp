#define _CRT_SECURE_NO_WARNINGS
#include "GUI.h"          // for UITable (used in loadSecurityLog)
#include "FileHandler.h"
#include "HospitalException.h"
#include <fstream>
#include <sstream>
#include <ctime>
using namespace std;

//Singleton instance
FileHandler* FileHandler::s_instance = nullptr;

FileHandler* FileHandler::getInstance() {
    if (!s_instance) s_instance = new FileHandler();
    return s_instance;
}
FileHandler::FileHandler() : m_loaded(false) {
    createFilesIfMissing();
}

//Local Time Management
string FileHandler::getTimestamp() const {
    time_t t = time(nullptr);
    char buf[32];
    strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", localtime(&t));
    return string(buf);
}

//write file functions 
void FileHandler::writePatientLine(ofstream& out, const Patient& p) {
    out << p.getID() << "," << p.getName() << "," << p.getAge() << "," << p.getGender() << "," << p.getContact() << "," << p.getPassword() << "," << p.getBalance() << "\n";
}
void FileHandler::writeDoctorLine(ofstream& out, const Doctor& d) {
    out << d.getID() << "," << d.getName() << "," << d.getSpecialization() << "," << d.getContact() << "," << d.getPassword() << "," << d.getFee() << "\n";
}
void FileHandler::writeAdminLine(ofstream& out, const Admin& a) {
    out << a.getID() << "," << a.getName() << "," << a.getPassword() << "\n";
}
void FileHandler::writeAppointmentLine(ofstream& out, const Appointment& a) {
    out << a.getAppointmentID() << "," << a.getPatientID() << "," << a.getDoctorID() << "," << a.getDate() << "," << a.getTimeSlot() << "," << a.getStatus() << "\n";
}
void FileHandler::writeBillLine(ofstream& out, const Bill& b) {
    out << b.getBillID() << "," << b.getPatientID() << "," << b.getAppointmentID() << "," << b.getAmount() << "," << b.getStatus() << "," << b.getDate() << "\n";
}
void FileHandler::writePrescriptionLine(ofstream& out, const Prescription& p) {
    out << p.getPrescriptionID() << "," << p.getAppointmentID() << "," << p.getPatientID() << "," << p.getDoctorID() << "," << p.getDate() << "," << p.getMedicines() << "," << p.getNotes() << "\n";
}

// Loading from files
void FileHandler::loadAll() {
    if (m_loaded) return;

    string line;

    // patients.txt  == patient_id,name,age,gender,contact,password,balance
    ifstream fp("data/patients.txt");
    while (fp.is_open() && getline(fp, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string tok, name, contact, pass;
        int id, age; char gender; double bal;
        getline(ss, tok, ','); id = stoi(tok);
        getline(ss, name, ',');
        getline(ss, tok, ','); age = stoi(tok);
        getline(ss, tok, ','); gender = tok[0];
        getline(ss, contact, ',');
        getline(ss, pass, ',');
        getline(ss, tok);       bal = stod(tok);
        m_patients.add(new Patient(id, name, age, gender, contact, pass, bal));
    }

    // doctors.txt  == doctor_id,name,specialization,contact,password,fee
    ifstream fd("data/doctors.txt");
    while (fd.is_open() && getline(fd, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string tok, name, spec, contact, pass;
        int id; float fee;
        getline(ss, tok, ','); id = stoi(tok);
        getline(ss, name, ',');
        getline(ss, spec, ',');
        getline(ss, contact, ',');
        getline(ss, pass, ',');
        getline(ss, tok);       fee = stof(tok);
        m_doctors.add(new Doctor(id, name, spec, contact, pass, fee));
    }

    // admin.txt   == admin_id,name,password
    ifstream fa("data/admin.txt");
    while (fa.is_open() && getline(fa, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string tok, name, pass;
        int id;
        getline(ss, tok, ','); id = stoi(tok);
        getline(ss, name, ',');
        getline(ss, pass);
        m_admins.add(new Admin(id, name, pass));
    }

    // appointments.txt == appt_id,pat_id,doc_id,date,slot,status
    ifstream fap("data/appointments.txt");
    while (fap.is_open() && getline(fap, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string tok, date, slot, status;
        int apptID, patID, docID;
        getline(ss, tok, ','); apptID = stoi(tok);
        getline(ss, tok, ','); patID = stoi(tok);
        getline(ss, tok, ','); docID = stoi(tok);
        getline(ss, date, ',');
        getline(ss, slot, ',');
        getline(ss, status);
        m_appointments.add(new Appointment(apptID, patID, docID, date, slot, status));
    }

    // bills.txt   == bill_id,pat_id,appt_id,amount,status,date
    ifstream fb("data/bills.txt");
    while (fb.is_open() && getline(fb, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string tok, status, date;
        int billID, patID, apptID; float amount;
        getline(ss, tok, ','); billID = stoi(tok);
        getline(ss, tok, ','); patID = stoi(tok);
        getline(ss, tok, ','); apptID = stoi(tok);
        getline(ss, tok, ','); amount = stof(tok);
        getline(ss, status, ',');
        getline(ss, date);
        m_bills.add(new Bill(billID, patID, apptID, amount, status, date));
    }

    // prescriptions.txt == presc_id,appt_id,pat_id,doc_id,date,medicines,notes
    ifstream fpr("data/prescriptions.txt");
    while (fpr.is_open() && getline(fpr, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string tok, date, meds, notes;
        int presID, apptID, patID, docID;
        getline(ss, tok, ','); presID = stoi(tok);
        getline(ss, tok, ','); apptID = stoi(tok);
        getline(ss, tok, ','); patID = stoi(tok);
        getline(ss, tok, ','); docID = stoi(tok);
        getline(ss, date, ',');
        getline(ss, meds, ',');
        getline(ss, notes);
        m_prescriptions.add(new Prescription(presID, apptID, patID, docID, date, meds, notes));
    }

    m_loaded = true;
}

// Save all finctions
void FileHandler::saveAll() {
    ofstream fp("data/patients.txt");
    for (int i = 0; i < m_patients.size(); i++) {
        Patient* p = m_patients.getAt(i);
        if (p) writePatientLine(fp, *p);
    }

    ofstream fd("data/doctors.txt");
    for (int i = 0; i < m_doctors.size(); i++) {
        Doctor* d = m_doctors.getAt(i);
        if (d) writeDoctorLine(fd, *d);
    }

    ofstream fa("data/admin.txt");
    for (int i = 0; i < m_admins.size(); i++) {
        Admin* a = m_admins.getAt(i);
        if (a) writeAdminLine(fa, *a);
    }

    ofstream fap("data/appointments.txt");
    for (int i = 0; i < m_appointments.size(); i++) {
        Appointment* a = m_appointments.getAt(i);
        if (a) writeAppointmentLine(fap, *a);
    }

    ofstream fb("data/bills.txt");
    for (int i = 0; i < m_bills.size(); i++) {
        Bill* b = m_bills.getAt(i);
        if (b) writeBillLine(fb, *b);
    }

    ofstream fpr("data/prescriptions.txt");
    for (int i = 0; i < m_prescriptions.size(); i++) {
        Prescription* p = m_prescriptions.getAt(i);
        if (p) writePrescriptionLine(fpr, *p);
    }
}

// Storage Templates accessors
Storage<Patient>& FileHandler::loadPatients() { return m_patients; }
Storage<Doctor>& FileHandler::loadDoctors() { return m_doctors; }
Storage<Admin>& FileHandler::loadAdmin() { return m_admins; }
Storage<Appointment>& FileHandler::loadAppointments() { return m_appointments; }
Storage<Bill>& FileHandler::loadBills() { return m_bills; }
Storage<Prescription>& FileHandler::loadPrescriptions() { return m_prescriptions; }

//Appending files 
void FileHandler::appendPatient(const Patient& p) {
    m_patients.add(new Patient(p));
    ofstream f("data/patients.txt", ios::app);
    writePatientLine(f, p);
}
void FileHandler::appendDoctor(const Doctor& d) {
    m_doctors.add(new Doctor(d));
    ofstream f("data/doctors.txt", ios::app);
    writeDoctorLine(f, d);
}
void FileHandler::appendAppointment(const Appointment& a) {
    m_appointments.add(new Appointment(a));
    ofstream f("data/appointments.txt", ios::app);
    writeAppointmentLine(f, a);
}
void FileHandler::appendBill(const Bill& b) {
    m_bills.add(new Bill(b));
    ofstream f("data/bills.txt", ios::app);
    writeBillLine(f, b);
}
void FileHandler::appendPrescription(const Prescription& p) {
    m_prescriptions.add(new Prescription(p));
    ofstream f("data/prescriptions.txt", ios::app);
    writePrescriptionLine(f, p);
}

// Update files
void FileHandler::updatePatient(const Patient& p) {
    Patient* existing = m_patients.findByID(p.getID());
    if (existing) *existing = p;
    ofstream f("data/patients.txt");
    for (int i = 0; i < m_patients.size(); i++) {
        Patient* pt = m_patients.getAt(i);
        if (pt) writePatientLine(f, *pt);
    }
}
void FileHandler::updateDoctor(const Doctor& d) {
    Doctor* existing = m_doctors.findByID(d.getID());
    if (existing) *existing = d;
    ofstream f("data/doctors.txt");
    for (int i = 0; i < m_doctors.size(); i++) {
        Doctor* dr = m_doctors.getAt(i);
        if (dr) writeDoctorLine(f, *dr);
    }
}
void FileHandler::updateAppointment(const Appointment& a) {
    Appointment* existing = m_appointments.findByID(a.getAppointmentID());
    if (existing) *existing = a;
    ofstream f("data/appointments.txt");
    for (int i = 0; i < m_appointments.size(); i++) {
        Appointment* ap = m_appointments.getAt(i);
        if (ap) writeAppointmentLine(f, *ap);
    }
}
void FileHandler::updateBill(const Bill& b) {
    Bill* existing = m_bills.findByID(b.getBillID());
    if (existing) *existing = b;
    ofstream f("data/bills.txt");
    for (int i = 0; i < m_bills.size(); i++) {
        Bill* bi = m_bills.getAt(i);
        if (bi) writeBillLine(f, *bi);
    }
}

//Deleting Doctor
void FileHandler::deleteDoctor(int id) {
    m_doctors.removeByID(id);
    ofstream f("data/doctors.txt");
    for (int i = 0; i < m_doctors.size(); i++) {
        Doctor* d = m_doctors.getAt(i);
        if (d) writeDoctorLine(f, *d);
    }
}

// Max ID helpers
int FileHandler::getMaxPatientID() const { return m_patients.maxID(); }
int FileHandler::getMaxDoctorID() const { return m_doctors.maxID(); }
int FileHandler::getMaxAppointmentID() const { return m_appointments.maxID(); }
int FileHandler::getMaxBillID() const { return m_bills.maxID(); }
int FileHandler::getMaxPrescID() const { return m_prescriptions.maxID(); }

//Business logic helpers
bool FileHandler::prescriptionExists(int appointmentID) const {
    for (int i = 0; i < m_prescriptions.size(); i++) {
        Prescription* p = m_prescriptions.getAt(i);
        if (p && p->getAppointmentID() == appointmentID) return true;
    }
    return false;
}
bool FileHandler::hasUnpaidBills(int patientID) const {
    for (int i = 0; i < m_bills.size(); i++) {
        Bill* b = m_bills.getAt(i);
        if (b && b->getPatientID() == patientID && b->getStatus() == "unpaid")
            return true;
    }
    return false;
}
bool FileHandler::hasPendingAppts(int patientID) const {
    for (int i = 0; i < m_appointments.size(); i++) {
        Appointment* a = m_appointments.getAt(i);
        if (a && a->getPatientID() == patientID && a->getStatus() == "pending")
            return true;
    }
    return false;
}
void FileHandler::dischargePatient(int patientID) {
    Patient* p = m_patients.findByID(patientID);
    if (!p) return;

    // Archive patient row to discharged.txt
    ofstream dis("data/discharged.txt", ios::app);
    writePatientLine(dis, *p);

    // Remove patient
    m_patients.removeByID(patientID);
    ofstream fp("data/patients.txt");
    for (int i = 0; i < m_patients.size(); i++) {
        Patient* pt = m_patients.getAt(i);
        if (pt) writePatientLine(fp, *pt);
    }

    // Remove appointments for this patient
    for (int i = m_appointments.size() - 1; i >= 0; i--) {
        Appointment* a = m_appointments.getAt(i);
        if (a && a->getPatientID() == patientID)
            m_appointments.removeByID(a->getAppointmentID());
    }
    ofstream fap("data/appointments.txt");
    for (int i = 0; i < m_appointments.size(); i++) {
        Appointment* a = m_appointments.getAt(i);
        if (a) writeAppointmentLine(fap, *a);
    }

    // Remove bills for this patient
    for (int i = m_bills.size() - 1; i >= 0; i--) {
        Bill* b = m_bills.getAt(i);
        if (b && b->getPatientID() == patientID)
            m_bills.removeByID(b->getBillID());
    }
    ofstream fb("data/bills.txt");
    for (int i = 0; i < m_bills.size(); i++) {
        Bill* b = m_bills.getAt(i);
        if (b) writeBillLine(fb, *b);
    }

    // Remove prescriptions for this patient
    for (int i = m_prescriptions.size() - 1; i >= 0; i--) {
        Prescription* pr = m_prescriptions.getAt(i);
        if (pr && pr->getPatientID() == patientID)
            m_prescriptions.removeByID(pr->getPrescriptionID());
    }
    ofstream fpr("data/prescriptions.txt");
    for (int i = 0; i < m_prescriptions.size(); i++) {
        Prescription* pr = m_prescriptions.getAt(i);
        if (pr) writePrescriptionLine(fpr, *pr);
    }
}

//Security log
void FileHandler::logSecurityEvent(const string& role, const string& enteredID, const string& result) {
    ofstream f("data/security_log.txt", ios::app);
    if (!f.is_open()) return;
    f << getTimestamp() << "," << role << "," << enteredID << "," << result << "\n";
}
void FileHandler::loadSecurityLog(UITable& table) {
    ifstream f("data/security_log.txt");
    if (!f.is_open()) return;
    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string ts, role, eid, result;
        getline(ss, ts, ',');
        getline(ss, role, ',');
        getline(ss, eid, ',');
        getline(ss, result);
        string row[] = { ts, role, eid, result };
        table.addRow(row);
    }
}

//File creation for first time usage
void FileHandler::createFilesIfMissing() {
    const string files[] = {
        "data/patients.txt", "data/doctors.txt", "data/admin.txt",
        "data/appointments.txt", "data/bills.txt", "data/prescriptions.txt",
        "data/security_log.txt", "data/discharged.txt"
    };
    for (const string& fname : files) {
        ifstream check(fname);
        if (!check.is_open()) {
            ofstream create(fname);
        }
    }
}