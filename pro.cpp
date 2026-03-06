// SmartDocComplete.cpp
#include <iostream>
#include <string>
#include <cctype>
#include <chrono>
#include <random>

using namespace std;
using namespace std::chrono;

// ======= CONFIGURABLE LIMITS =======
const int MAX_DOCTORS = 1000; 
const int MAX_APPOINTMENTS = 200;
const int MAX_DAYS = 30;
const int MAX_SLOTS_PER_DAY = 10; 
const int HASH_SIZE = 97; 

// ==================== Utility Classes ====================
class Date {
public:
    int day, month, year;
    Date(int d = 1, int m = 1, int y = 2024) : day(d), month(m), year(y) {}
    string toString() const { return to_string(day) + "/" + to_string(month) + "/" + to_string(year); }
    bool operator==(const Date& o) const { return day == o.day && month == o.month && year == o.year; }
};

class TimeSlot {
public:
    int hour;
    bool isAvailable;
    TimeSlot(int h = 0, bool av = true) : hour(h), isAvailable(av) {}
    string toString() const { return (hour < 10 ? "0" : "") + to_string(hour) + ":00"; }
    bool operator==(const TimeSlot& o) const { return hour == o.hour; }
};

// ==================== Appointment Node ====================
class AppointmentNode {
public:
    int appointmentID;
    int doctorID;
    string doctorName;
    Date date;
    TimeSlot time;
    string type;
    string status;  //cancel of seduled
    AppointmentNode* next;

    AppointmentNode(int aID, int dID, const string& dName, const Date& d, const TimeSlot& t, const string& tp)
        : appointmentID(aID), doctorID(dID), doctorName(dName), date(d), time(t), type(tp), status("scheduled"), next(nullptr) {}
};

// ==================== Patient Node (LinkedList) ====================
class PatientNode {
public:
    int patientID;
    string name;
    int age;
    string phone;
    AppointmentNode* appointmentHead;  // List of this patient's appointments
    int appointmentCount;
    PatientNode* next;

    PatientNode() : patientID(0), age(0), appointmentHead(nullptr), appointmentCount(0), next(nullptr) {}
    
    PatientNode(int id, const string& n, int a, const string& ph) 
        : patientID(id), name(n), age(a), phone(ph), appointmentHead(nullptr), appointmentCount(0), next(nullptr) {}

    void addAppointment(AppointmentNode* node) {
        if (!appointmentHead) appointmentHead = node;
        else {
            AppointmentNode* temp = appointmentHead;
            while (temp->next) temp = temp->next;
            temp->next = node;
        }
        appointmentCount++;
    }

    void displayAppointments() const {
        cout << "Appointments for " << name << " (ID: " << patientID << "):\n";
        if (!appointmentHead) {
            cout << "  (No appointments)\n";
            return;
        }
        AppointmentNode* cur = appointmentHead;
        while (cur) {
            cout << "  ID: " << cur->appointmentID << " | Dr: " << cur->doctorName 
                 << " | " << cur->date.toString() << " " << cur->time.toString() 
                 << " | " << cur->status << "\n";
            cur = cur->next;
        }
    }

    ~PatientNode() {
        AppointmentNode* cur = appointmentHead;
        while (cur) { 
            AppointmentNode* nx = cur->next; 
            delete cur; 
            cur = nx; 
        }
    }
};

// ==================== Doctor Appointment Node (for doctor's schedule) ====================
class DoctorAppointmentNode {
public:
    int appointmentID;
    int patientID;
    string patientName;
    Date date;
    TimeSlot time;
    string type;
    string status;
    DoctorAppointmentNode* next;

    DoctorAppointmentNode(int aID, int pID, const string& pName, const Date& d, const TimeSlot& t, const string& tp)
        : appointmentID(aID), patientID(pID), patientName(pName), date(d), time(t), type(tp), status("scheduled"), next(nullptr) {}
};

// ==================== Doctor ====================
class Doctor {
public:
    int doctorID;
    string name;
    string expertise;
    string city;
    int ranking;
    double fee;

    Date scheduleDates[MAX_DAYS];
    TimeSlot schedule[MAX_DAYS][MAX_SLOTS_PER_DAY];
    int totalDays;

    DoctorAppointmentNode* appointmentHead;
    int appointmentCount;

    Doctor() : doctorID(0), ranking(0), fee(0), totalDays(0), appointmentHead(nullptr), appointmentCount(0) {}

    Doctor(int id, const string& n, const string& exp, const string& c, int r, double f)
        : doctorID(id), name(n), expertise(exp), city(c), ranking(r), fee(f), totalDays(0), appointmentHead(nullptr), appointmentCount(0) {
        initializeSchedule();
    }

    void initializeSchedule() {
        totalDays = MAX_DAYS;
        for (int d = 0; d < totalDays; ++d) {
            scheduleDates[d] = Date(17 + d, 11, 2025);
            for (int s = 0; s < MAX_SLOTS_PER_DAY; ++s) schedule[d][s] = TimeSlot(9 + s, true);
        }
    }

    int findDateIndex(const Date& date) const {
        for (int i = 0; i < totalDays; ++i) if (scheduleDates[i] == date) return i;
        return -1;
    }

    bool isSlotAvailable(const Date& d, const TimeSlot& t) const {
        int idx = findDateIndex(d);
        if (idx == -1) return false;
        for (int s = 0; s < MAX_SLOTS_PER_DAY; ++s)
            if (schedule[idx][s] == t) return schedule[idx][s].isAvailable;
        return false;
    }

    bool bookSlot(const Date& d, const TimeSlot& t) {
        int idx = findDateIndex(d);
        if (idx == -1) return false;
        for (int s = 0; s < MAX_SLOTS_PER_DAY; ++s) {
            if (schedule[idx][s] == t && schedule[idx][s].isAvailable) {
                schedule[idx][s].isAvailable = false;
                return true;
            }
        }
        return false;
    }

    void releaseSlot(const Date& d, const TimeSlot& t) {
        int idx = findDateIndex(d);
        if (idx == -1) return;
        for (int s = 0; s < MAX_SLOTS_PER_DAY; ++s) 
            if (schedule[idx][s] == t) { 
                schedule[idx][s].isAvailable = true; 
                return;
            }
    }

    void addAppointment(DoctorAppointmentNode* node) {
        if (!appointmentHead) appointmentHead = node;
        else {
            DoctorAppointmentNode* temp = appointmentHead;
            while (temp->next) temp = temp->next;
            temp->next = node;
        }
        appointmentCount++;
    }

    DoctorAppointmentNode* findAppointment(int aID) {
        DoctorAppointmentNode* cur = appointmentHead;
        while (cur) { 
            if (cur->appointmentID == aID) return cur; 
            cur = cur->next; 
        }
        return nullptr;
    }

    void displayInfo() const {
        cout << "ID: " << doctorID << " | " << name << " | " << expertise 
             << " | " << city << " | Rank: " << ranking << " | Fee: Rs. " << fee << "\n";
    }

    void displayAvailableSlots(const Date& d) const {
        int idx = findDateIndex(d);
        if (idx == -1) { cout << "No schedule for that date.\n"; return; }
        cout << "Available slots for " << name << " on " << d.toString() << ":\n";
        int count = 0;
        for (int s = 0; s < MAX_SLOTS_PER_DAY; ++s) 
            if (schedule[idx][s].isAvailable) { 
                cout << "  - " << schedule[idx][s].toString() << "\n"; 
                count++; 
            }
        if (count == 0) cout << "  (No slots available)\n";
    }

    ~Doctor() {
        DoctorAppointmentNode* cur = appointmentHead;
        while (cur) { 
            DoctorAppointmentNode* nx = cur->next; 
            delete cur; 
            cur = nx; 
        }
    }
};

// ==================== Hash Node ====================
class HashNode {
public:
    Doctor* doctor;
    HashNode* next;
    HashNode(Doctor* d) : doctor(d), next(nullptr) {}
};

// ==================== AppointmentSystem ====================
class AppointmentSystem {
private:
    Doctor* doctors[MAX_DOCTORS];
    int doctorCount;
    
    // Patient LinkedList
    PatientNode* patientHead;
    int patientCount;

    HashNode* expertiseHash[HASH_SIZE];
    HashNode* nameHash[HASH_SIZE];

    int nextAppointmentID;
    int nextPatientID;

    double setupDurationMs;
    double additionalDurationMs;
    mt19937 rng;

    string lower(const string& s) const {
        string out = s; 
        for (char& c : out) c = (char)tolower(c); 
        return out;
    }
//this hash dunc cause collisions on angrams as cat,tac,act
    int hash(const string& key) const {
        unsigned int h = 0; 
        for (char c : key) h = (h * 31 + (unsigned char)c) % HASH_SIZE; 
        return (int)h;
    }

    void insertHash(HashNode* table[], const string& key, Doctor* d) {
        int idx = hash(lower(key));
        HashNode* n = new HashNode(d);
        n->next = table[idx];
        table[idx] = n;
    }

    int partition(Doctor* arr[], int low, int high) {
        int pivot = arr[high]->ranking;
        int i = low - 1;
        for (int j = low; j < high; ++j) 
            if (arr[j]->ranking > pivot) { 
                i++; 
                swap(arr[i], arr[j]); 
            }
        swap(arr[i + 1], arr[high]);
        return i + 1;
    }

    void quickSort(Doctor* arr[], int low, int high) {
        if (low < high) {
            int pi = partition(arr, low, high);
            quickSort(arr, low, pi - 1);
            quickSort(arr, pi + 1, high);
        }
    }

    void initializeBulkData(int numDoctors, int numPatients) {
        auto t1 = steady_clock::now();
        string expList[5] = { "Cardiologist", "Dermatologist", "Neurologist", "Orthopedic", "Pediatrician" };
        string cityList[5] = { "Karachi", "Lahore", "Islamabad", "Multan", "Faisalabad" };

        // Initialize doctors
        for (int i = 0; i < numDoctors; ++i) {
            string name = "Dr_" + to_string(i + 1);
            string exp = expList[i % 5];
            string city = cityList[i % 5];
            int rank = 5 + (i % 6);
            double fee = 2000 + (i % 5) * 500;
            Doctor* d = new Doctor(i + 1, name, exp, city, rank, fee);
            doctors[doctorCount++] = d;
            insertHash(expertiseHash, exp, d);
            insertHash(nameHash, name, d);
        }

        // Initialize patients with specific assignment pattern
        Date firstDate(17, 11, 2025);
        TimeSlot slot9am(9);
        TimeSlot slot10am(10);

        for (int i = 0; i < numPatients; ++i) {
            PatientNode* p = new PatientNode(nextPatientID++, "Patient_" + to_string(i + 1), 
                                             20 + (i % 50), "0300" + to_string(i));
            
            // Add to linked list
            if (!patientHead) {
                patientHead = p;
            } else {
                PatientNode* temp = patientHead;
                while (temp->next) temp = temp->next;
                temp->next = p;
            }
            patientCount++;

            // Assignment pattern:
            // First 500 patients -> First 500 doctors at 9 AM on 17/11/2025
            // Next 200 patients -> First 200 doctors at 10 AM on 17/11/2025
            Doctor* assignedDoctor = nullptr;
            TimeSlot assignedSlot;

            if (i < 500) {
                // Assign to doctor i at 9 AM
                assignedDoctor = doctors[i];
                assignedSlot = slot9am;
            } else if (i < 700) {
                // Assign to doctor (i-500) at 10 AM
                assignedDoctor = doctors[i - 500];
                assignedSlot = slot10am;
            }

            if (assignedDoctor) {
                assignedDoctor->bookSlot(firstDate, assignedSlot);
                
                // Create appointment for doctor's list
                DoctorAppointmentNode* docAppt = new DoctorAppointmentNode(
                    nextAppointmentID, p->patientID, p->name, firstDate, assignedSlot, "initial"
                );
                assignedDoctor->addAppointment(docAppt);

                // Create appointment for patient's list
                AppointmentNode* patAppt = new AppointmentNode(
                    nextAppointmentID, assignedDoctor->doctorID, assignedDoctor->name, 
                    firstDate, assignedSlot, "initial"
                );
                p->addAppointment(patAppt);
                
                nextAppointmentID++;
            }
        }

        auto t2 = steady_clock::now();
        setupDurationMs = duration<double, milli>(t2 - t1).count();
        additionalDurationMs = 0;
        cout << "Initialized " << numDoctors << " doctors and " << numPatients 
             << " patients in " << setupDurationMs << " ms.\n";
        cout << "Assignment: First 500 patients -> First 500 doctors at 9 AM\n";
        cout << "            Next 200 patients -> First 200 doctors at 10 AM\n";
    }

public:
    AppointmentSystem() : doctorCount(0), patientHead(nullptr), patientCount(0), 
                          nextAppointmentID(1001), nextPatientID(1), 
                          setupDurationMs(0), additionalDurationMs(0) {
        for (int i = 0; i < HASH_SIZE; ++i) { 
            expertiseHash[i] = nullptr; 
            nameHash[i] = nullptr; 
        }
        rng.seed(random_device{}());
        initializeBulkData(500, 700); 
    }

    ~AppointmentSystem() {
        for (int i = 0; i < doctorCount; ++i) delete doctors[i];
        
        // Delete patient linked list
        PatientNode* cur = patientHead;
        while (cur) {
            PatientNode* nx = cur->next;
            delete cur;
            cur = nx;
        }
        
        for (int i = 0; i < HASH_SIZE; ++i) {
            HashNode* cur = expertiseHash[i]; 
            while (cur) { 
                HashNode* nx = cur->next; 
                delete cur; 
                cur = nx; 
            }
            cur = nameHash[i]; 
            while (cur) { 
                HashNode* nx = cur->next; 
                delete cur; 
                cur = nx; 
            }
        }
    }

    // ================= Patient Functions =================
    PatientNode* registerPatient(const string& name, int age, const string& phone) {
        auto t1 = steady_clock::now();
        
        PatientNode* p = new PatientNode(nextPatientID++, name, age, phone);
        
        // Add to linked list
        if (!patientHead) {
            patientHead = p;
        } else {
            PatientNode* temp = patientHead;
            while (temp->next) temp = temp->next;
            temp->next = p;
        }
        patientCount++;
        
        auto t2 = steady_clock::now();
        additionalDurationMs += duration<double, milli>(t2 - t1).count();
        cout << "Registered Patient ID " << p->patientID << " (" << p->name << ")\n";
        return p;
    }

    PatientNode* findPatient(int pid) {
        PatientNode* cur = patientHead;
        while (cur) {
            if (cur->patientID == pid) return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    void displayAllPatients() {
        cout << "\n===== All Patients (Total: " << patientCount << ") =====\n";
        if (!patientHead) {
            cout << "(No patients registered)\n";
            return;
        }
        
        PatientNode* cur = patientHead;
        int count = 1;
        while (cur) {
            cout << count++ << ". ID: " << cur->patientID << " | Name: " << cur->name 
                 << " | Age: " << cur->age << " | Phone: " << cur->phone 
                 << " | Appointments: " << cur->appointmentCount << "\n";
            cur = cur->next;
        }
    }

    void displayRuntime() const {
        double total = setupDurationMs + additionalDurationMs;
        cout << "\n===== Runtime Metrics =====\n";
        cout << "Bulk setup: " << setupDurationMs << " ms\n";
        cout << "Additional operations: " << additionalDurationMs << " ms\n";
        cout << "Total tracked runtime: " << total << " ms\n";
    }

    // ==================== Doctor Search ====================
    void searchByName(const string& namePart) {
        string part = lower(namePart);
        cout << "Search results for '" << namePart << "':\n";
        bool found = false;
        for (int i = 0; i < HASH_SIZE; ++i) {
            HashNode* cur = nameHash[i];
            while (cur) {
                if (lower(cur->doctor->name).find(part) != string::npos) { 
                    cur->doctor->displayInfo(); 
                    found = true; 
                }
                cur = cur->next;
            }
        }
        if (!found) cout << "  (No matching doctors)\n";
    }

    void searchByExpertise(const string& expertise) {
        string key = lower(expertise);
        int idx = hash(key);
        HashNode* cur = expertiseHash[idx];
        Doctor* results[MAX_DOCTORS]; 
        int rc = 0;
        while (cur) { 
            if (lower(cur->doctor->expertise) == key) results[rc++] = cur->doctor; 
            cur = cur->next; 
        }
        if (rc == 0) { cout << "No doctors found.\n"; return; }
        quickSort(results, 0, rc - 1);
        for (int i = 0; i < rc; ++i) results[i]->displayInfo();
    }

    void displayAllDoctors() {
        cout << "All doctors (" << doctorCount << "):\n";
        for (int i = 0; i < doctorCount; ++i) doctors[i]->displayInfo();
    }

    // ==================== Appointment Functions ====================
    bool findAlternativeSlot(Doctor* d, const Date& reqDate, TimeSlot& altTime, Date& altDate, int maxDays = 7) {
        int startIdx = d->findDateIndex(reqDate);
        if (startIdx == -1) return false;
        for (int offset = 0; offset < maxDays; ++offset) {
            int idx = startIdx + offset;
            if (idx >= d->totalDays) break;
            for (int s = 0; s < MAX_SLOTS_PER_DAY; ++s) {
                if (d->schedule[idx][s].isAvailable) {
                    altDate = d->scheduleDates[idx];
                    altTime = d->schedule[idx][s];
                    return true;
                }
            }
        }
        return false;
    }

    // these func is updating scedule of doctor and adding appointment to both doctor and patient lists
     
    void bookAppointment(int patientID, int doctorID, const Date& date, const TimeSlot& time, const string& type) {
        PatientNode* p = findPatient(patientID);
        if (!p) { cout << "Patient not found.\n"; return; }

        Doctor* d = nullptr;
        for (int i = 0; i < doctorCount; ++i) 
            if (doctors[i]->doctorID == doctorID) { 
                d = doctors[i]; 
                break; 
            }
        if (!d) { cout << "Doctor not found.\n"; return; }

        auto t1 = steady_clock::now();

        if (d->isSlotAvailable(date, time)) {
            if (d->bookSlot(date, time)) {
                // Add to doctor's appointment list
                DoctorAppointmentNode* docAppt = new DoctorAppointmentNode(
                    nextAppointmentID, p->patientID, p->name, date, time, type
                );
                d->addAppointment(docAppt);

                // Add to patient's appointment list
                AppointmentNode* patAppt = new AppointmentNode(
                    nextAppointmentID, d->doctorID, d->name, date, time, type
                );
                p->addAppointment(patAppt);

                cout << "Appointment booked! ID: " << nextAppointmentID 
                     << " with " << d->name << " on " << date.toString() 
                     << " at " << time.toString() << "\n";
                nextAppointmentID++;
            }
        } else {
            Date altDate; TimeSlot altTime;
            if (findAlternativeSlot(d, date, altTime, altDate, 7)) {
                cout << "Requested slot not available. Suggested alternative: " 
                     << altDate.toString() << " at " << altTime.toString() << "\n";
                cout << "Book alternative? (y/n): "; 
                char c; cin >> c;
                if (c == 'y' || c == 'Y') {
                    if (d->bookSlot(altDate, altTime)) {
                        DoctorAppointmentNode* docAppt = new DoctorAppointmentNode(
                            nextAppointmentID, p->patientID, p->name, altDate, altTime, type
                        );
                        d->addAppointment(docAppt);

                        AppointmentNode* patAppt = new AppointmentNode(
                            nextAppointmentID, d->doctorID, d->name, altDate, altTime, type
                        );
                        p->addAppointment(patAppt);

                        cout << "Alternative booked! Appointment ID: " << nextAppointmentID << "\n";
                        nextAppointmentID++;
                    }
                } else cout << "No booking made.\n";
            } else cout << "Requested slot not available and no alternative in next 7 days.\n";
        }

        auto t2 = steady_clock::now();
        additionalDurationMs += duration<double, milli>(t2 - t1).count();
    }

    void cancelAppointment(int appointmentID, int patientID) {
        PatientNode* p = findPatient(patientID);
        if (!p) { cout << "Patient not found.\n"; return; }

        for (int i = 0; i < doctorCount; ++i) {
            Doctor* d = doctors[i];
            DoctorAppointmentNode* ap = d->findAppointment(appointmentID);
            if (ap && ap->patientID == patientID && ap->status == "scheduled") {
                d->releaseSlot(ap->date, ap->time);
                ap->status = "cancelled";
                
                // Update patient's appointment status
                AppointmentNode* patAppt = p->appointmentHead;
                while (patAppt) {
                    if (patAppt->appointmentID == appointmentID) {
                        patAppt->status = "cancelled";
                        break;
                    }
                    patAppt = patAppt->next;
                }
                
                cout << "Appointment " << appointmentID << " cancelled.\n";
                return;
            }
        }
        cout << "Appointment not found or cannot be cancelled.\n";
    }

    void rescheduleAppointment(int appointmentID, int patientID, const Date& newDate, const TimeSlot& newTime) {
        PatientNode* p = findPatient(patientID);
        if (!p) { cout << "Patient not found.\n"; return; }

        for (int i = 0; i < doctorCount; ++i) {
            Doctor* d = doctors[i];
            DoctorAppointmentNode* ap = d->findAppointment(appointmentID);
            if (ap && ap->patientID == patientID && ap->status == "scheduled") {
                if (d->isSlotAvailable(newDate, newTime)) {
                    d->releaseSlot(ap->date, ap->time);
                    if (d->bookSlot(newDate, newTime)) {
                        ap->date = newDate; 
                        ap->time = newTime;
                        
                        // Update patient's appointment
                        AppointmentNode* patAppt = p->appointmentHead;
                        while (patAppt) {
                            if (patAppt->appointmentID == appointmentID) {
                                patAppt->date = newDate;
                                patAppt->time = newTime;
                                break;
                            }
                            patAppt = patAppt->next;
                        }
                        
                        cout << "Appointment rescheduled to " << newDate.toString() 
                             << " at " << newTime.toString() << "\n";
                    } else { 
                        d->bookSlot(ap->date, ap->time); 
                        cout << "Failed to book new slot.\n"; 
                    }
                } else { 
                    Date altD; TimeSlot altT; 
                    if (findAlternativeSlot(d, newDate, altT, altD, 7)) 
                        cout << "Suggest: " << altD.toString() << " at " << altT.toString() << "\n"; 
                }
                return;
            }
        }
        cout << "Appointment not found.\n";
    }

    void viewPatientAppointments(int patientID) {
        PatientNode* p = findPatient(patientID);
        if (!p) { cout << "Patient not found.\n"; return; }
        p->displayAppointments();
    }

    void viewDoctorSchedule(int doctorID, const Date& date) {
        Doctor* d = nullptr;
        for (int i = 0; i < doctorCount; ++i) 
            if (doctors[i]->doctorID == doctorID) { 
                d = doctors[i]; 
                break; 
            }
        if (!d) { cout << "Doctor not found.\n"; return; }
        d->displayAvailableSlots(date);
        cout << "Booked slots for " << d->name << " on " << date.toString() << ":\n";
        DoctorAppointmentNode* cur = d->appointmentHead;
        bool any = false;
        while (cur) {
            if (cur->date == date && cur->status == "scheduled") { 
                cout << "  - " << cur->time.toString() << " | Appt ID: " 
                     << cur->appointmentID << " | Patient: " << cur->patientName << "\n"; 
                any = true; 
            }
            cur = cur->next;
        }
        if (!any) cout << "  (No booked appointments)\n";
    }
};

// ==================== Main Menu ====================
void showMenu() {
    cout << "\n========== SmartDoc (Complete) ==========\n";
    cout << "1. Register Patient\n";
    cout << "2. Search Doctor by Name\n";
    cout << "3. Search Doctor by Expertise\n";
    cout << "4. View All Doctors\n";
    cout << "5. View Doctor Schedule (specific date)\n";
    cout << "6. Book Appointment\n";
    cout << "7. Cancel Appointment\n";
    cout << "8. Reschedule Appointment\n";
    cout << "9. View My Appointments\n";
    cout << "10. Display Runtime Metrics\n";
    cout << "11. Display All Patients\n";
    cout << "0. Exit\n";
    cout << "Enter choice: ";
}

int main() {
    AppointmentSystem system;
    int choice; 
    int currentPatientID = 0;

    while (true) {
        showMenu();
        if (!(cin >> choice)) { 
            cin.clear(); 
            cin.ignore(10000, '\n'); 
            cout << "Invalid input. Try again.\n"; 
            continue; 
        }
        cin.ignore();

        if (choice == 0) { cout << "Goodbye!\n"; break; }
        
        else if (choice == 1) { 
            string name, phone; 
            int age; 
            cout << "Enter name: "; 
            getline(cin, name); 
            cout << "Enter age: "; 
            cin >> age; 
            cin.ignore(); 
            cout << "Enter phone: "; 
            getline(cin, phone); 
            PatientNode* p = system.registerPatient(name, age, phone); 
            if (p) currentPatientID = p->patientID; 
        }
        else if (choice == 2) { 
            string namePart; 
            cout << "Enter doctor name or part: "; 
            getline(cin, namePart); 
            system.searchByName(namePart); 
        }
        else if (choice == 3) { 
            string exp; 
            cout << "Enter expertise (exact): "; 
            getline(cin, exp); 
            system.searchByExpertise(exp); 
        }
        else if (choice == 4) system.displayAllDoctors();
        else if (choice == 5) { 
            int docID, d, m, y; 
            cout << "Enter doctor ID: "; 
            cin >> docID; 
            cout << "Enter date (day month year): "; 
            cin >> d >> m >> y; 
            cin.ignore(); 
            system.viewDoctorSchedule(docID, Date(d, m, y)); 
        }
        else if (choice == 6) {
            if (currentPatientID == 0) { 
                cout << "Please register first.\n"; 
                continue; 
            }
            int docID, h, d, m, y; 
            string type; 
            cout << "Enter doctor ID: "; 
            cin >> docID; 
            cout << "Enter date (day month year): "; 
            cin >> d >> m >> y; 
            cout << "Enter hour (9-18): "; 
            cin >> h; 
            cin.ignore(); 
            cout << "Enter type (online/physical): "; 
            getline(cin, type); 
            system.bookAppointment(currentPatientID, docID, Date(d, m, y), TimeSlot(h), type);
        }
        else if (choice == 7) { 
            if (currentPatientID == 0) { 
                cout << "Please register first.\n"; 
                continue; 
            } 
            int apptID; 
            cout << "Enter appointment ID to cancel: "; 
            cin >> apptID; 
            cin.ignore(); 
            system.cancelAppointment(apptID, currentPatientID); 
        }
        else if (choice == 8) { 
            if (currentPatientID == 0) { 
                cout << "Please register first.\n"; 
                continue; 
            } 
            int apptID, d, m, y, h; 
            cout << "Enter appointment ID to reschedule: "; 
            cin >> apptID; 
            cout << "Enter new date (day month year): "; 
            cin >> d >> m >> y; 
            cout << "Enter new hour (9-18): "; 
            cin >> h; 
            cin.ignore(); 
            system.rescheduleAppointment(apptID, currentPatientID, Date(d, m, y), TimeSlot(h)); 
        }
        else if (choice == 9) { 
            if (currentPatientID == 0) { 
                cout << "Please register first.\n"; 
                continue; 
            } 
            system.viewPatientAppointments(currentPatientID); 
        }
        else if (choice == 10) system.displayRuntime();
        else if (choice == 11) system.displayAllPatients();
        else cout << "Invalid choice. Try again.\n";
    }
    return 0;
}