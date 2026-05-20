
#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <ctime>
#include <iomanip>

using namespace std;

// ================= ABSTRACT CLASS =================
class Vehicle {
protected:
    string plateNumber;
    string ownerName;
    int entryHour;

public:
    Vehicle(string plate, string owner, int hour)
        : plateNumber(plate), ownerName(owner), entryHour(hour) {}

    virtual double calculateFee(int exitHour) = 0;
    virtual string getVehicleType() = 0;
    virtual string getRequiredSlot() = 0;

    string getPlateNumber() {
        return plateNumber;
    }

    string getOwnerName() {
        return ownerName;
    }

    int getEntryHour() {
        return entryHour;
    }

    virtual ~Vehicle() {}
};

// ================= CAR CLASS =================
class Car : public Vehicle {
public:
    Car(string plate, string owner, int hour)
        : Vehicle(plate, owner, hour) {}

    double calculateFee(int exitHour) override {
        int duration = exitHour - entryHour;

        if(duration <= 0)
            duration = 1;

        double rate = 100;

        // Peak hour pricing
        if(exitHour >= 18)
            rate *= 1.5;

        return duration * rate;
    }

    string getVehicleType() override {
        return "Car";
    }

    string getRequiredSlot() override {
        return "Medium";
    }
};

// ================= BIKE CLASS =================
class Bike : public Vehicle {
public:
    Bike(string plate, string owner, int hour)
        : Vehicle(plate, owner, hour) {}

    double calculateFee(int exitHour) override {
        int duration = exitHour - entryHour;

        if(duration <= 0)
            duration = 1;

        double rate = 50;

        if(exitHour >= 18)
            rate *= 1.2;

        return duration * rate;
    }

    string getVehicleType() override {
        return "Bike";
    }

    string getRequiredSlot() override {
        return "Small";
    }
};

// ================= TRUCK CLASS =================
class Truck : public Vehicle {
public:
    Truck(string plate, string owner, int hour)
        : Vehicle(plate, owner, hour) {}

    double calculateFee(int exitHour) override {
        int duration = exitHour - entryHour;

        if(duration <= 0)
            duration = 1;

        double rate = 200;

        if(exitHour >= 18)
            rate *= 2;

        return duration * rate;
    }

    string getVehicleType() override {
        return "Truck";
    }

    string getRequiredSlot() override {
        return "Large";
    }
};

// ================= PARKING SLOT =================
class ParkingSlot {
private:
    int slotID;
    string slotType;
    bool occupied;

public:
    ParkingSlot(int id, string type)
        : slotID(id), slotType(type), occupied(false) {}

    bool isOccupied() {
        return occupied;
    }

    void occupySlot() {
        occupied = true;
    }

    void freeSlot() {
        occupied = false;
    }

    string getSlotType() {
        return slotType;
    }

    int getSlotID() {
        return slotID;
    }
};

// ================= RESERVATION =================
class Reservation {
private:
    string reservationID;
    string plateNumber;

public:
    Reservation(string id, string plate)
        : reservationID(id), plateNumber(plate) {}

    string getReservationID() {
        return reservationID;
    }

    string getPlateNumber() {
        return plateNumber;
    }
};

// ================= PARKING LOT =================
class ParkingLot {
private:
    vector<ParkingSlot> slots;
    vector<Vehicle*> parkedVehicles;
    vector<Reservation> reservations;
    queue<Vehicle*> waitingQueue;

    double totalRevenue;

public:
    ParkingLot() {
        totalRevenue = 0;

        // Small slots
        for(int i = 1; i <= 3; i++) {
            slots.push_back(ParkingSlot(i, "Small"));
        }

        // Medium slots
        for(int i = 4; i <= 7; i++) {
            slots.push_back(ParkingSlot(i, "Medium"));
        }

        // Large slots
        for(int i = 8; i <= 10; i++) {
            slots.push_back(ParkingSlot(i, "Large"));
        }
    }

    // ================= ADD VEHICLE =================
    void parkVehicle(Vehicle* vehicle) {
        for(auto &slot : slots) {
            if(!slot.isOccupied() &&
               slot.getSlotType() == vehicle->getRequiredSlot()) {

                slot.occupySlot();
                parkedVehicles.push_back(vehicle);

                cout << "\nVehicle Parked Successfully!\n";
                cout << "Vehicle: " << vehicle->getVehicleType() << endl;
                cout << "Plate Number: " << vehicle->getPlateNumber() << endl;
                cout << "Allocated Slot: " << slot.getSlotID() << endl;

                generateTicket(vehicle, slot.getSlotID());
                return;
            }
        }

        cout << "\nParking Full For "
             << vehicle->getVehicleType()
             << "!\n";

        cout << "Vehicle Added To Waiting Queue.\n";

        waitingQueue.push(vehicle);
    }

    // ================= REMOVE VEHICLE =================
    void exitVehicle(string plate, int exitHour) {
        for(size_t i = 0; i < parkedVehicles.size(); i++) {
            if(parkedVehicles[i]->getPlateNumber() == plate) {

                Vehicle* vehicle = parkedVehicles[i];

                double fee = vehicle->calculateFee(exitHour);
                totalRevenue += fee;

                cout << "\n===== BILL =====\n";
                cout << "Vehicle Type: "
                     << vehicle->getVehicleType() << endl;
                cout << "Plate Number: "
                     << vehicle->getPlateNumber() << endl;
                cout << "Total Fee: KES "
                     << fee << endl;

                saveTransaction(vehicle, fee);

                parkedVehicles.erase(parkedVehicles.begin() + i);

                freeMatchingSlot(vehicle->getRequiredSlot());

                processWaitingQueue();

                delete vehicle;

                return;
            }
        }

        cout << "Vehicle Not Found!\n";
    }

    // ================= FREE SLOT =================
    void freeMatchingSlot(string type) {
        for(auto &slot : slots) {
            if(slot.getSlotType() == type && slot.isOccupied()) {
                slot.freeSlot();
                return;
            }
        }
    }

    // ================= PROCESS WAITING QUEUE =================
    void processWaitingQueue() {
        if(waitingQueue.empty())
            return;

        Vehicle* nextVehicle = waitingQueue.front();

        for(auto &slot : slots) {
            if(!slot.isOccupied() &&
               slot.getSlotType() == nextVehicle->getRequiredSlot()) {

                waitingQueue.pop();

                slot.occupySlot();
                parkedVehicles.push_back(nextVehicle);

                cout << "\nWaiting Vehicle Assigned Automatically!\n";
                cout << "Vehicle: "
                     << nextVehicle->getVehicleType() << endl;

                return;
            }
        }
    }

    // ================= RESERVATION =================
    void createReservation(string plate) {
        string reservationID = "RES" + to_string(rand() % 10000);

        reservations.push_back(
            Reservation(reservationID, plate)
        );

        cout << "\nReservation Successful!\n";
        cout << "Reservation ID: "
             << reservationID << endl;
    }

    // ================= DISPLAY STATUS =================
    void displayStatus() {
        cout << "\n===== PARKING STATUS =====\n";

        int available = 0;
        int occupied = 0;

        for(auto &slot : slots) {
            if(slot.isOccupied())
                occupied++;
            else
                available++;
        }

        cout << "Available Slots: " << available << endl;
        cout << "Occupied Slots: " << occupied << endl;
        cout << "Vehicles In Queue: "
             << waitingQueue.size() << endl;

        cout << "Total Revenue: KES "
             << totalRevenue << endl;
    }

    // ================= TICKET =================
    void generateTicket(Vehicle* vehicle, int slotID) {
        cout << "\n===== SMART PARKING TICKET =====\n";
        cout << "Owner: "
             << vehicle->getOwnerName() << endl;
        cout << "Vehicle: "
             << vehicle->getVehicleType() << endl;
        cout << "Plate: "
             << vehicle->getPlateNumber() << endl;
        cout << "Slot Number: "
             << slotID << endl;
        cout << "Entry Hour: "
             << vehicle->getEntryHour() << ":00" << endl;
        cout << "===============================\n";
    }

    // ================= SAVE TRANSACTION =================
    void saveTransaction(Vehicle* vehicle, double fee) {
        ofstream file("transactions.txt", ios::app);

        file << vehicle->getVehicleType() << " | "
             << vehicle->getPlateNumber() << " | "
             << fee << endl;

        file.close();
    }

    // ================= DESTRUCTOR =================
    ~ParkingLot() {
        for(auto vehicle : parkedVehicles) {
            delete vehicle;
        }
    }
};

// ================= MAIN =================
int main() {
    srand(time(0));

    ParkingLot parkingLot;

    int choice;

    do {
        cout << "\n========== SMART PARKING SYSTEM ==========";
        cout << "\n1. Park Car";
        cout << "\n2. Park Bike";
        cout << "\n3. Park Truck";
        cout << "\n4. Exit Vehicle";
        cout << "\n5. Create Reservation";
        cout << "\n6. View Parking Status";
        cout << "\n0. Exit";
        cout << "\nEnter Choice: ";
        cin >> choice;

        if(choice >= 1 && choice <= 3) {
            string plate, owner;
            int entryHour;

            cout << "Enter Plate Number: ";
            cin >> plate;

            cout << "Enter Owner Name: ";
            cin >> owner;

            cout << "Enter Entry Hour (0-23): ";
            cin >> entryHour;

            Vehicle* vehicle = nullptr;

            if(choice == 1)
                vehicle = new Car(plate, owner, entryHour);

            else if(choice == 2)
                vehicle = new Bike(plate, owner, entryHour);

            else
                vehicle = new Truck(plate, owner, entryHour);

            parkingLot.parkVehicle(vehicle);
        }

        else if(choice == 4) {
            string plate;
            int exitHour;

            cout << "Enter Plate Number: ";
            cin >> plate;

            cout << "Enter Exit Hour (0-23): ";
            cin >> exitHour;

            parkingLot.exitVehicle(plate, exitHour);
        }

        else if(choice == 5) {
            string plate;

            cout << "Enter Plate Number: ";
            cin >> plate;

            parkingLot.createReservation(plate);
        }

        else if(choice == 6) {
            parkingLot.displayStatus();
        }

    } while(choice != 0);

    cout << "\nSystem Closed Successfully!\n";

    return 0;
}
