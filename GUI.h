// GUI.h — MediCore Hospital Management System SFML GUI
#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <string>
using namespace std;

namespace WND {
    const float W = 1920.f;
    const float H = 1080.f;
}
namespace Theme {
    const sf::Color BG = sf::Color(10, 12, 18);              // near-black background
    const sf::Color HEADER_BG = sf::Color(8, 24, 58);        // very dark navy header
    const sf::Color TILE_BG = sf::Color(16, 22, 36);         // dark card surface
    const sf::Color TILE_HOVER = sf::Color(20, 40, 80);      // blue-tinted hover
    const sf::Color TILE_BORDER = sf::Color(30, 70, 140);    // medium blue border
    const sf::Color BTN_PRIMARY = sf::Color(0, 100, 220);    // vivid blue
    const sf::Color BTN_HOVER = sf::Color(0, 130, 255);      // brighter blue on hover
    const sf::Color BTN_TEXT = sf::Color(230, 240, 255);      // near-white text
    const sf::Color TEXT_DARK = sf::Color(200, 215, 240);     // light blue-white text
    const sf::Color TEXT_GRAY = sf::Color(120, 140, 175);     // muted blue-gray
    const sf::Color SUCCESS = sf::Color(40, 180, 100);        // green (kept distinct)
    const sf::Color DANGER = sf::Color(240, 60, 60);          // red (kept distinct)
    const sf::Color INPUT_BG = sf::Color(14, 20, 34);        // dark input field
    const sf::Color INPUT_BORDER = sf::Color(30, 60, 120);   // blue input border
    const sf::Color INPUT_FOCUS = sf::Color(0, 120, 255);    // bright blue focus ring
    const sf::Color DIVIDER = sf::Color(25, 40, 75);         // subtle blue divider
    const sf::Color ROW_HOVER = sf::Color(18, 38, 72);       // row hover — dark blue
    const sf::Color ROW_SELECT = sf::Color(0, 70, 160);      // row select — strong blue
    const sf::Color TITLE_TEXT = sf::Color(70, 160, 255);     // bright blue for titles
}
namespace Assets {
    const string LOGO = "assets/Sprites/logo.png";
    const string BG = "assets/Sprites/bg.png";
    const string FONT_REGULAR = "assets/fonts/Roboto-Black.ttf";
    const string FONT_BOLD = "assets/fonts/Roboto-Bold.ttf";
    const string AVATAR_PATIENT = "assets/Sprites/avatar_patient.png";
    const string AVATAR_DOCTOR = "assets/Sprites/avatar_doctor.jpg";
    const string AVATAR_ADMIN = "assets/Sprites/avatar_admin.jpg";
    const string ICON_APPT = "assets/Sprites/icon_appointment.png";
    const string ICON_PATIENT = "assets/Sprites/icon_patient.jpg";
    const string ICON_DOCTOR = "assets/Sprites/icon_doctor.png";
    const string ICON_BILLING = "assets/Sprites/icon_billing.png";
    const string ICON_PRESC = "assets/Sprites/icon_prescription.png";
    const string ICON_ADMIN = "assets/Sprites/icon_admin.png";
    const string ICON_REPORTS = "assets/Sprites/icon_reports.jpg";
    const string ICON_DISCHARGE = "assets/Sprites/icon_discharge.png";
    const string ICON_SECURITY = "assets/Sprites/icon_security.png";
    const string ICON_CANCEL = "assets/Sprites/icon_cancel.png";
    const string ICON_RECORDS = "assets/Sprites/icon_records.png";
    const string ICON_TOPUP = "assets/Sprites/icon_topup.jpg";
}

class UIButton; class UIInputBox; class UITile;
class UITable;  class UIMessageBox; class Screen; class AssetManager;

// Asset Manager
class AssetManager {
public:
    AssetManager();
    ~AssetManager() {}
    sf::Texture& getTexture(const string& path);
    sf::Font& getFont(bool bold = false);
private:
    static const int MAX_TEXTURES = 32;
    string m_keys[MAX_TEXTURES];
    sf::Texture m_textures[MAX_TEXTURES];
    int m_count;
    sf::Font m_fontRegular;
    sf::Font m_fontBold;
    int findKey(const string& path) const;
};

// UI Button
class UIButton {
public:
    UIButton();
    void init(float x, float y, float w, float h, const string& label, AssetManager& assets, bool primary = true);
    void initWithIcon(float x, float y, float w, float h, const string& label, const string& iconPath, AssetManager& assets);
    // pass mapped mouse position for correct hit detection
    void handleEvent(const sf::Event& e, const sf::RenderWindow& win, sf::Vector2f mousePos);
    void draw(sf::RenderWindow& win);
    bool isClicked() const;
    void resetClick();
    void setLabel(const string& label);
    void setEnabled(bool enabled);
    void setPosition(float x, float y);
    sf::FloatRect getBounds() const { return m_rect.getGlobalBounds(); }
private:
    sf::RectangleShape m_rect;
    sf::Text m_text;
    sf::Sprite m_icon;
    sf::Texture* m_iconTex;
    bool m_hasIcon, m_hovered, m_clicked, m_enabled, m_primary;
    void updateVisual();
};

// UI Input Box
class UIInputBox {
public:
    UIInputBox();
    void init(float x, float y, float w, float h, const string& placeholder, AssetManager& assets, bool isPassword = false);
    void handleEvent(const sf::Event& e, const sf::RenderWindow& win, sf::Vector2f mousePos);
    void draw(sf::RenderWindow& win);
    string getText() const;
    void clear();
    void setFocused(bool f);
    bool isFocused() const;
    void setReadOnly(bool ro);
private:
    sf::RectangleShape m_rect;
    sf::Text m_text, m_placeholder;
    string m_buffer;
    bool m_focused, m_isPassword, m_readOnly;
    sf::Clock m_cursorClock;
    void updateText();
};

// UI Tile
class UITile {
public:
    UITile();
    void init(float x, float y, float tileSize, const string& label, const string& iconPath, AssetManager& assets);
    void handleEvent(const sf::Event& e, const sf::RenderWindow& win, sf::Vector2f mousePos);
    void draw(sf::RenderWindow& win);
    bool isClicked() const;
    void resetClick();
private:
    sf::RectangleShape m_card, m_shadow;
    sf::Sprite m_icon;
    sf::Texture* m_iconTex;
    sf::Text m_label;
    bool m_hovered, m_clicked;
};

// UI Table
class UITable {
public:
    UITable();
    ~UITable() {}
    void init(float x, float y, float w, float h, const string* headers, int colCount, AssetManager& assets);
    void addRow(const string* cells);
    void clearRows();
    //  pass mapped mouse position
    void handleEvent(const sf::Event& e, sf::Vector2f mousePos);
    void draw(sf::RenderWindow& win);

    // row selection
    int  getSelectedRow() const { return m_selectedRow; }
    void clearSelection() { m_selectedRow = -1; }
    // get cell value of selected row
    string getSelectedCell(int col) const;

private:
    static const int MAX_COLS = 8;
    static const int MAX_ROWS = 200;
    float m_x, m_y, m_w, m_h;
    int m_cols;
    float m_colW[MAX_COLS];
    string m_headers[MAX_COLS];
    string m_data[MAX_ROWS][MAX_COLS];
    int m_rowCount;
    int m_selectedRow;   
    int m_hoveredRow;    
    float m_scrollOffset;
    sf::Font* m_font;
    sf::RectangleShape m_bg, m_headerBar;
};

// UI Message Box
class UIMessageBox {
public:
    UIMessageBox();
    void init(AssetManager& assets);
    void show(const string& title, const string& body, bool hasCancel = false);
    bool isVisible() const;
    void handleEvent(const sf::Event& e, const sf::RenderWindow& win, sf::Vector2f mousePos);
    void draw(sf::RenderWindow& win);
    bool confirmedOK() const;
    bool confirmedCancel() const;
    void reset();
private:
    sf::RectangleShape m_overlay, m_panel;
    sf::Text m_title, m_body;
    UIButton m_btnOK, m_btnCancel;
    bool m_visible, m_ok, m_cancel, m_hasCancel;
};

// Screen IDs
enum class ScreenID {
    SPLASH, LOGIN,
    PATIENT_DASHBOARD, PATIENT_BOOK_APPT, PATIENT_MY_APPTS,
    PATIENT_RECORDS, PATIENT_BILLS, PATIENT_PAY_BILL, PATIENT_TOPUP,
    DOCTOR_DASHBOARD, DOCTOR_TODAY_APPTS, DOCTOR_MARK_COMPLETE,
    DOCTOR_WRITE_PRESC, DOCTOR_PATIENT_HISTORY,
    ADMIN_DASHBOARD, ADMIN_ADD_DOCTOR, ADMIN_ADD_PATIENT,
    ADMIN_REMOVE_DOCTOR, ADMIN_ALL_PATIENTS, ADMIN_ALL_DOCTORS,
    ADMIN_ALL_APPTS, ADMIN_UNPAID_BILLS, ADMIN_DISCHARGE,
    ADMIN_SECURITY_LOG, ADMIN_DAILY_REPORT, EXIT
};

// GUI Context
struct GUIContext {
    sf::RenderWindow* window;
    AssetManager* assets;
    ScreenID nextScreen;
    int loggedInID;
    int role;
    string loggedInName;
    float patientBalance;
    int selectedID;
    // store mapped mouse position so all screens use same coords
    sf::Vector2f mousePos;
};

// Abstract Screen class
class Screen {
public:
    virtual ~Screen() {}
    virtual void onEnter(GUIContext& ctx) = 0;
    virtual void handleEvent(const sf::Event& e, GUIContext& ctx) = 0;
    virtual void update(float dt, GUIContext& ctx) = 0;
    virtual void draw(sf::RenderWindow& win, GUIContext& ctx) = 0;
};

// compute mapped mouse pos, fixes click offset
// defined in GUI.cpp, used by every screen
sf::Vector2f getMappedMousePos(const sf::RenderWindow& win);

// Splash Screen
class SplashScreen : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    sf::Sprite m_logo;
    sf::Text m_title, m_subtitle;
    float m_timer, m_alpha;
};

// Login Screen
class LoginScreen : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UITile m_rolePatient, m_roleDoctor, m_roleAdmin;
    UIInputBox m_inputName, m_inputPass;
    UIButton m_btnLogin, m_btnBackRole;
    UIMessageBox m_msgBox;
    sf::Text m_header, m_labelRole, m_attemptsText;
    int m_selectedRole, m_failedAttempts;
};

// Patient Dashboard
class PatientDashboard : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    static const int TILE_COUNT = 7;
    UITile m_tiles[TILE_COUNT];
    UIButton m_btnLogout;
    sf::Text m_welcome, m_balanceText;
};

// Patient Book Appointment, table row selection
class PatientBookAppt : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UIInputBox m_inputSpec;
    UIButton m_btnSearch;
    UITable m_tableDoc;    // click row, selects doctor
    UIInputBox m_inputDate;
    UIButton m_btnLoadSlots; // explicit "Show Slots" button
    UITable m_tableSlots;  // click row,  selects slot
    UIButton m_btnBook;
    UIButton m_btnBack;
    UIMessageBox m_msgBox;
    sf::Text m_title, m_statusText;
    sf::Text m_lblSelectedDoc, m_lblSelectedSlot; // NEW feedback labels
    int m_selectedDocID;
    string m_selectedDate, m_selectedSlot;
    void loadDoctors(const string& spec);
    void loadSlots(int docID, const string& date);
};

// Patients my apppointments
class PatientMyAppts : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UITable m_table;
    UIInputBox m_inputID;
    UIButton m_btnCancel, m_btnBack;
    UIMessageBox m_msgBox;
    sf::Text m_title;
    void refreshTable(GUIContext& ctx);
};

// Patient Bills
class PatientBills : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UITable m_table;
    UIInputBox m_inputBillID;
    UIButton m_btnPay, m_btnTopUp, m_btnBack;
    UIMessageBox m_msgBox;
    sf::Text m_title, m_totalText;
};

// Patient Records
class PatientRecords : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UITable m_table;
    UIButton m_btnBack;
    sf::Text m_title;
};

// Patient Top Up
class PatientTopUp : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UIInputBox m_inputAmount;
    UIButton m_btnAdd, m_btnBack;
    UIMessageBox m_msgBox;
    sf::Text m_title, m_currentBalance;
};

// Doctor Dashboard
class DoctorDashboard : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    static const int TILE_COUNT = 5;
    UITile m_tiles[TILE_COUNT];
    UIButton m_btnLogout;
    sf::Text m_welcome;
};

// Doctor Todays Appointments
class DoctorTodayAppts : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UITable m_table;
    UIInputBox m_inputID;
    UIButton m_btnComplete, m_btnNoShow, m_btnBack;
    UIMessageBox m_msgBox;
    sf::Text m_title;
};

// Doctor Write Prescription
class DoctorWritePresc : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UIInputBox m_inputApptID, m_inputMeds, m_inputNotes;
    UIButton m_btnSave, m_btnBack;
    UIMessageBox m_msgBox;
    sf::Text m_title;
};

// Doctor Patient History
class DoctorPatientHistory : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UIInputBox m_inputPatientID;
    UIButton m_btnSearch, m_btnBack;
    UITable m_table;
    UIMessageBox m_msgBox;
    sf::Text m_title;
};

// Admin Dashboard
class AdminDashboard : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    static const int TILE_COUNT = 10;
    UITile m_tiles[TILE_COUNT];
    UIButton m_btnLogout;
    sf::Text m_welcome;
};

// Admin Add Doctor
class AdminAddDoctor : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UIInputBox m_inputName, m_inputSpec, m_inputContact, m_inputPass, m_inputFee;
    UIButton m_btnAdd, m_btnBack;
    UIMessageBox m_msgBox;
    sf::Text m_title;
    bool m_addedOK; //flag to navigate back after OK
};

// Admin Add Patient
class AdminAddPatient : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UIInputBox m_inputName, m_inputAge, m_inputGender, m_inputContact, m_inputPass, m_inputBalance;
    UIButton  m_btnAdd, m_btnBack;
    UIMessageBox m_msgBox;
    sf::Text m_title;
    bool m_addedOK; 
};

// Admin Remove Doctor
class AdminRemoveDoctor : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UITable m_table;
    UIInputBox m_inputID;
    UIButton m_btnRemove, m_btnBack;
    UIMessageBox m_msgBox;
    sf::Text m_title;
};

// Admin View Patients
class AdminViewPatients : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UITable m_table;
    UIButton m_btnBack;
    sf::Text m_title;
};

// Admin View Doctor
class AdminViewDoctors : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UITable m_table;
    UIButton m_btnBack;
    sf::Text m_title;
};

// Admin All Appointments
class AdminAllAppts : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UITable m_table;
    UIButton m_btnBack;
    sf::Text m_title;
};

// Admin Unpaid Bills
class AdminUnpaidBills : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UITable m_table;
    UIButton m_btnBack;
    sf::Text m_title;
};

// Admin Discharge
class AdminDischarge : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UITable m_table;
    UIInputBox m_inputID;
    UIButton m_btnDischarge, m_btnBack;
    UIMessageBox m_msgBox;
    sf::Text m_title;
};

// Admin Security Log
class AdminSecurityLog : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UITable m_table;
    UIButton m_btnBack;
    sf::Text m_title;
};

// Admin Daily Report
class AdminDailyReport : public Screen {
public:
    void onEnter(GUIContext& ctx) override;
    void handleEvent(const sf::Event& e, GUIContext& ctx) override;
    void update(float dt, GUIContext& ctx) override;
    void draw(sf::RenderWindow& win, GUIContext& ctx) override;
private:
    UITable m_tableSummary, m_tableOutstanding, m_tableDocSummary;
    UIButton m_btnBack;
    sf::Text m_title, m_revenueText;
};

// GUI App
class GUIApp {
public:
    GUIApp();
    ~GUIApp() {}
    void run();
private:
    void nitScreens();
    void switchScreen(ScreenID id);
    Screen* screenFor(ScreenID id);

    sf::RenderWindow m_window;
    AssetManager m_assets;
    GUIContext m_ctx;

    SplashScreen m_splash;
    LoginScreen m_login;
    PatientDashboard m_patDash;
    PatientBookAppt m_patBook;
    PatientMyAppts m_patAppts;
    PatientRecords m_patRecords;
    PatientBills m_patBills;
    PatientTopUp m_patTopUp;
    DoctorDashboard m_docDash;
    DoctorTodayAppts m_docToday;
    DoctorWritePresc m_docPresc;
    DoctorPatientHistory m_docHistory;
    AdminDashboard m_adminDash;
    AdminAddDoctor m_adminAddDoc;
    AdminAddPatient m_adminAddPat;
    AdminRemoveDoctor m_adminRemDoc;
    AdminViewPatients m_adminPats;
    AdminViewDoctors m_adminDocs;
    AdminAllAppts m_adminAppts;
    AdminUnpaidBills m_adminBills;
    AdminDischarge m_adminDischarge;
    AdminSecurityLog m_adminLog;
    AdminDailyReport m_adminReport;

    Screen* m_current;
    ScreenID m_currentID;
};