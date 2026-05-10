# MediCore — Hospital Management System

A C++ OOP project built for the Spring 2026 semester. It has a full graphical interface using SFML and manages patients, doctors, appointments, billing, and prescriptions.

---

## What it does

Three types of users can log in — Patient, Doctor, and Admin. Each one gets their own menu.

**Patient** can book appointments, cancel them, pay bills, top up balance, and view prescriptions.

**Doctor** can see today's appointments, mark them complete or no-show, and write prescriptions.

**Admin** can add/remove doctors, add patients, view all records, discharge patients, check unpaid bills, see the security log, and generate a daily report.

All data is saved to `.txt` files and loads back when you restart the program.

---

## Files in the project

```
Hospital Management System1/
├── main.cpp
├── GUI.h / GUI.cpp              ← SFML graphical interface
├── HospitalSystem.h / .cpp      ← core logic
├── FileHandler.h / .cpp         ← all file reading and writing
├── Storage.h                    ← generic template container
├── Person.h / .cpp              ← base class
├── Patient.h / .cpp
├── Doctor.h / .cpp
├── Admin.h / .cpp
├── Appointment.h / .cpp
├── Bill.h / .cpp
├── Prescription.h / .cpp
├── HospitalException.h / .cpp
├── Validator.h / .cpp
├── assets/                      ← fonts, icons, sprites
│   ├── fonts/
|   |── audio/          
│   └── Sprites/
├── data/                        ← all .txt data files live here
│   ├── patients.txt
│   ├── doctors.txt
│   ├── admin.txt
│   ├── appointments.txt
│   ├── bills.txt
│   ├── prescriptions.txt
│   ├── security_log.txt
│   └── discharged.txt
└── External/
    └── SFML/                    ← SFML headers and libs go here
```

---

## Requirements

- Windows (tested on Windows 10/11)
- Visual Studio 2022
- SFML 2.6.2
- C++17 or later

---

## How to compile and run

### Step 1 — Get SFML

Download SFML 2.6.w for Visual C++ from the official site:
`https://www.sfml-dev.org/download.php`

Get the version that matches your Visual Studio (VC++ 17 for VS 2022).

### Step 2 — Put SFML in the right place

Extract the SFML zip. You'll get a folder with `include`, `lib`, and `bin` inside.

Put everything inside the `External/SFML/` folder in the project:

```
External/
└── SFML/
    ├── include/
    ├── lib/
    └── bin/
```

The `.vcxproj` already points to `$(SolutionDir)External\SFML\include` and `$(SolutionDir)External\SFML\lib` so you don't need to change any project settings.

### Step 3 — Copy the DLL files

Copy all the SFML `.dll` files from `SFML/bin/` into the same folder as the `.exe` — that's `x64/Debug/` or `x64/Release/` depending on what you're building.

The project folder already has the DLLs included (sfml-graphics-2.dll, etc.) so this step may already be done.

### Step 4 — Open and build

1. Open `Hospital Management System1.sln` in Visual Studio
2. Set the build to `x64` (check the dropdown at the top — should say x64, not x86)
3. Press `Ctrl+Shift+B` to build
4. Press `F5` or `Ctrl+F5` to run

---

## Default login credentials

These are already in the data files.

| Role    | Name    | Password  |
|---------|---------|-----------|
| Admin   | Ahsan   | ahsan123  |
| Patient | Wassay  | 123456    |
| Doctor  | Fatima Inam | 123456 |
| Doctor  | Ali     | 123456    |

---

## How the data files work

All data is stored in plain `.txt` files inside the `data/` folder. Each file has one record per line, fields separated by commas.

Example — `patients.txt`:
```
1,Wassay,19,M,04523339991,123456,40000
```

Format: `id, name, age, gender, contact, password, balance`

The program reads all files when it starts and saves them automatically when you close it. If a file doesn't exist on first run, it creates it.

---

## Important notes

- The window runs in **fullscreen** at 1920×1080. Press `Escape` to close it.
- The security log (`security_log.txt`) records every login attempt with a timestamp.
- When a patient is discharged, their record moves to `discharged.txt`.
- Slots are fixed: 09:00, 10:00, 11:00, 12:00, 13:00, 14:00, 15:00, 16:00.
- To book an appointment: search a specialization → click a doctor row → enter date → click a slot → click Confirm Booking.

---

## GitHub

Repository link: https://github.com/ahsan-med/Hospital-Management-System
