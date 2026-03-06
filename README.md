# SmartDoc — Doctor Appointment Management System

> A console-based Doctor Appointment Management System built in C++ using custom data structures — no STL containers.

---

## 📋 Table of Contents
- [About](#about)
- [Features](#features)
- [Data Structures Used](#data-structures-used)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Performance](#performance)
- [Limitations](#limitations)

---

## About

SmartDoc is a console-based appointment scheduling system written in C++, designed to simulate a real-world clinic/hospital booking workflow. Built entirely with custom data structures including linked lists, hash tables, and sorting algorithms — without relying on STL containers like `vector` or `map`.

---

## Features

- **Patient Management** — Register patients and maintain a singly linked list of all records
- **Doctor Directory** — Store up to 1,000 doctors with expertise, city, ranking, and fee info
- **Appointment Booking** — Book, cancel, and reschedule appointments with slot conflict detection
- **Alternative Slot Suggestion** — Automatically suggests the next available slot within 7 days if the requested one is taken
- **Doctor Schedule View** — Displays available and booked slots for any doctor on a given date
- **Search** — Search doctors by name (partial match) or by expertise using a hash table
- **Sorted Results** — Doctors filtered by expertise are sorted by ranking using QuickSort
- **Bulk Data Initialization** — Auto-generates 500 doctors and 700 patients with pre-assigned appointments on startup for testing
- **Runtime Metrics** — Tracks and displays time taken for setup and individual operations in milliseconds

---

## Data Structures Used

| Structure | Usage |
|---|---|
| Singly Linked List | Patient records and appointment histories (patient-side and doctor-side) |
| Hash Table (separate chaining) | Fast doctor lookup by name and expertise |
| 2D Array | Doctor availability schedules across dates and time slots |
| QuickSort | Ranking-based sorting of doctor search results |

---

## Project Structure

```
SmartDocComplete.cpp
│
├── Date                    # Utility class for dates
├── TimeSlot                # Utility class for time slots
├── AppointmentNode         # Linked list node for patient appointments
├── PatientNode             # Linked list node for patient records
├── DoctorAppointmentNode   # Linked list node for doctor schedules
├── Doctor                  # Doctor entity with schedule management
├── HashNode                # Node for hash table chaining
├── AppointmentSystem       # Core system: manages all operations
└── main()                  # Menu-driven interface
```

---

## Getting Started

### Prerequisites
- A C++ compiler supporting C++11 or later (e.g., g++, clang++)

### Compile

```bash
g++ -std=c++11 -o smartdoc SmartDocComplete.cpp
```

### Run

```bash
./smartdoc
```

> On Windows:
> ```bash
> g++ -std=c++11 -o smartdoc.exe SmartDocComplete.cpp
> smartdoc.exe
> ```

---

## Usage

On startup, the system auto-initializes **500 doctors** and **700 patients** with pre-assigned appointments for testing.

```
========== SmartDoc (Complete) ==========
1.  Register Patient
2.  Search Doctor by Name
3.  Search Doctor by Expertise
4.  View All Doctors
5.  View Doctor Schedule (specific date)
6.  Book Appointment
7.  Cancel Appointment
8.  Reschedule Appointment
9.  View My Appointments
10. Display Runtime Metrics
11. Display All Patients
0.  Exit
```

### Example Workflow
1. Run the program
2. Select `1` to register as a new patient
3. Use `3` to search for a doctor by specialty (e.g., `Cardiologist`)
4. Use `5` to view available slots for a doctor on a specific date
5. Use `6` to book an appointment
6. Use `9` to view your appointments

---

## Performance

| Operation | Complexity |
|---|---|
| Patient lookup by ID | O(n) — linked list traversal |
| Doctor search by expertise | O(1) average — hash table |
| Doctor search by name | O(n) — full scan with partial match |
| Appointment booking | O(d × s) — d = days, s = slots per day |
| Sort doctors by ranking | O(n log n) — QuickSort |

---

## Limitations

- Patient lookup is O(n); a hash table for patients would improve performance at scale
- The hash function may cause collisions on anagram strings (e.g., "act", "cat", "tac")
- Schedule dates are hardcoded starting from `17/11/2025`
- No file persistence — all data is lost when the program exits
- Single-user session (one active `currentPatientID` at a time)
