/*Pakize Gurel */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include <regex>

using namespace std;

// Pet class to store and manage pet information
class Pet {
private:
    // Private member variables to store pet details
    string name, breed, medicalRecord, pet_file;
    int age;
    bool vaccinated;

public:
    // Constructor with parameters (default medicalRecord is empty)
    Pet(string n, string b, int a, bool v, string m = "") : name(n), breed(b), age(a), vaccinated(v), medicalRecord(m) {
        // Throw an error if age is negative
        if (a < 0) {
            throw invalid_argument("Age cannot be negative.");
        }
    }
    // Default constructor
    Pet() {}

    // Convert pet details to a CSV-formatted string
    string toCSV() const {
        return name + " " + breed + " " + to_string(age) + " " + (vaccinated ? "Y" : "N") + " " + medicalRecord + "\n";
    }

    // Save all pets to file
    static void saveToFile(const vector<Pet>& pets) {
        ofstream file("pets.txt", ios::trunc); // Open file in truncate mode
        if (!file) {
            throw runtime_error("Error: Unable to open file for writing.");
        }
        // Write each pet to the file
        for (const auto& pet : pets) {
            file << pet.toCSV();
        }
        file.close();
    }

    // Load pets from file and return as vector
    static vector<Pet> loadFromFile() {
        vector<Pet> pets;
        ifstream file("pets.txt");
        if (!file) {
            cerr << "Warning: Unable to open a \"pets.txt\" file for reading. Add your first record to create a file. \n";
            return pets;
        }
        // Read pet data from file
        string name, breed, age_str, vac_str, medicalRecord;
        while (file >> name >> breed >> age_str >> vac_str >> ws && getline(file, medicalRecord)) {
            try {
                int age = stoi(age_str);
                bool vaccinated = (vac_str == "Y" || vac_str == "y");
                pets.emplace_back(name, breed, age, vaccinated, medicalRecord);
            }
            catch (const exception& e) {
                cerr << "Error parsing data for pet: " << name << " - Skipping entry.\n";
            }
        }
        file.close();
        return pets;
    }

    // Display all pets in a readable format
    static void viewPets(const vector<Pet>& pets) {
        if (pets.empty()) {
            cout << "\nNo pets found!\n";
            return;
        }
        for (const auto& pet : pets) {
            cout << "\n============================= "
                << "\nPet Name: " << pet.name
                << "\nBreed: " << pet.breed
                << "\nAge: " << pet.age
                << "\nVaccinated: " << (pet.vaccinated ? "Yes" : "No")
                << "\nMedical Record: " << pet.medicalRecord
                << "\n============================= " << "\n";
        }
    }

    // Add a new pet with user input and save it
    static void addPet(vector<Pet>& pets) {
        string name, breed;
        int age;
        char vac;
        cout << "Add New Pet:\n\n";
        // Get valid name
        while (true) {
            cout << "Enter pet's name: ";
            if (!(cin >> name) || !isValidName(name)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "Invalid input. Name must be a single word.\n";
                continue;
            }
            break;
        }
        // Get valid breed
        while (true) {
            cout << "Enter breed: ";
            if (!(cin >> breed) || !isValidName(breed)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "Invalid input. Breed must be a single word.\n";
                continue;
            }
            break;
        }
        // Get valid age
        while (true) {
            cout << "Enter age: ";
            if (!(cin >> age) || age < 0) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "Invalid input. Age must be a positive integer.\n";
                continue;
            }
            break;
        }
        // Get vaccination status
        cout << "Vaccinated (Y/N): ";
        cin >> vac;
        if (vac != 'Y' && vac != 'y' && vac != 'N' && vac != 'n') {
            cerr << "Invalid input. Defaulting to 'N' (Not Vaccinated).\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            vac = 'N';
        }
        // Add pet to list and save
        try {
            pets.emplace_back(name, breed, age, (vac == 'Y' || vac == 'y'), "none");
            saveToFile(pets);
            cout << "\nPet added successfully.\n";
        }
        catch (const exception& e) {
            cerr << "Error: " << e.what() << "\n";
        }
    }

    // Delete a pet by name
    static void deletePet(vector<Pet>& pets) {
        string pet_name;

        cout << "\nAvailable Pets:";
        viewPets(pets);
        if (pets.empty()) {
            cout << "Please add a pet first!\n";
            return;
        }
        // Get valid pet name
        while (true) {
            cout << "Enter the pet's name to delete pet's details: ";
            if (!(cin >> pet_name) || !isValidName(pet_name)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "Invalid input. Name must be a single word.\n";
                continue;
            }
            break;
        }
        // Search and remove pet
        for (auto& pet : pets) {
            if (pet.name == pet_name) {
                pets.erase(remove_if(pets.begin(), pets.end(), [&](const Pet& pet) { return pet.name == pet_name; }), pets.end());

                // Save updated list
                ofstream file("pets.txt");
                if (!file) {
                    cerr << "Error opening file!" << endl;
                    return;
                }

                for (const auto& pet : pets) {
                    file << pet.toCSV();
                }

                file.close();
                cout << "\nPet deleted successfully!" << endl;
                return;
            }
        }
        cerr << "Pet not found!\n";
    }

    // Add a medical record to a pet
    static void addMedicalRecord(vector<Pet>& pets) {
        string petName;

        cout << "\nAvailable Pets:";
        viewPets(pets);
        if (pets.empty()) {
            cout << "Please add a pet first!\n";
            return;
        }

        // Read full name with spaces
        cout << "Enter the pet's name to add medical record: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, petName);

        // Search for pet and update record
        for (auto& pet : pets) {
            if (pet.name == petName) {
                string newRecord;
                cout << "Enter medical record: ";
                getline(cin, newRecord);

                if (pet.medicalRecord == "none"){
                    pet.medicalRecord = newRecord;
                }
                // Append if record exists, otherwise set it
                if (pet.medicalRecord.empty()) {
                    pet.medicalRecord = newRecord;
                }
                else {
                    pet.medicalRecord += "; " + newRecord;
                }

                // Save changes
                try {
                    saveToFile(pets);
                    cout << "Medical record added successfully!\n";
                }
                catch (const exception& e) {
                    cerr << "Error saving pets: " << e.what() << "\n";
                }
                return;
            }
        }
        cerr << "Pet not found!\n";
    }

    // Edit pet information
    static void editPet(vector<Pet>& pets) {
        string petName;

        cout << "\nAvailable Pets:";
        viewPets(pets);
        if (pets.empty()) {
            cout << "Please add a pet first!\n";
            return;
        }
        // Get valid pet name
        while (true) {
            cout << "Enter the pet's name to update: ";
            if (!(cin >> petName) || !isValidName(petName)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "Invalid input. Name must be a single word.\n";
                continue;
            }
            break;
        }

        // Find and update pet details
        for (auto& pet : pets) {
            if (pet.name == petName) {
                string newBreed;
                int newAge;
                char vaccinated;
                cout << "------------------\nUpdate a Pet:\n\n";

                // Get new breed
                while (true) {
                    cout << "Enter new breed: ";
                    if (!(cin >> newBreed) || !isValidName(newBreed)) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cerr << "Invalid input. Breed must be a single word.\n";
                        continue;
                    }
                    break;
                }
                // Get new age
                while (true) {
                    cout << "Enter new age: ";
                    if (!(cin >> newAge) || newAge < 0) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cerr << "Invalid input. Age must be a positive integer.\n";
                        continue;
                    }
                    break;
                }
                // Get vaccination status
                cout << "Vaccinated (Y/N): ";
                cin >> vaccinated;
                if (vaccinated != 'Y' && vaccinated != 'y' && vaccinated != 'N' && vaccinated != 'n') {
                    cerr << "Invalid input. Defaulting to 'N' (Not Vaccinated).\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    vaccinated = 'N';
                }
                // Update values
                pet.breed = newBreed;
                pet.age = newAge;
                pet.vaccinated = (vaccinated == 'Y' || vaccinated == 'y');
                // Save changes
                try {
                    saveToFile(pets);
                    cout << "\nPet updated successfully!\n";
                }
                catch (const exception& e) {
                    cerr << "Error saving pets: " << e.what() << "\n";
                }
                return;
            }
        }
        cerr << "Pet not found!\n";
    }
    // Validate name (only alphabetic characters, single word)
    static bool isValidName(string name) {
        for (int i = 0; i < name.length(); i++) {
            if (!isalpha(name[i])) {
                return false;
            }
        }
        return true;
    }
    // Getter for name
    string getName() const { return name; }
};


class Owner {
private:
    // Owner's basic details
    string first_name, last_name, phone, email, address;

public:
    // Default constructor
    Owner() {};

    // Parameterized constructor
    Owner(string f, string l, string p, string e, string a) : first_name(f), last_name(l), phone(p), email(e), address(a) {};

    // Save all owners to a file
    static void saveToFile(const vector<Owner>& owners) {
        ofstream file("owners.txt", ios::trunc);  // Truncate file to start fresh
        for (const auto& owner : owners) {
            // Save owner's data in one line
            file << owner.first_name << " " << owner.last_name << " " << owner.phone << " " << owner.email << " " << owner.address << "\n";
        }
        file.close();  // Close file after writing
    }

    // Load owners from file
    static vector<Owner> loadFromFile() {
        vector<Owner> owners;
        ifstream file("owners.txt");
        // Handle case where file doesn't exist
        if (!file) {
            cerr << "Warning: Unable to open a \"owners.txt\" file for reading. Add your first record to create a file.\n";
            return owners;
        }
        // Read owner data line-by-line
        string first_name, last_name, phone, email, address;
        while (file >> first_name >> last_name >> phone >> email >> ws && getline(file, address)) {
            owners.push_back(Owner(first_name, last_name, phone, email, address));
        }
        file.close();
        return owners;
    }

    // Display all owners
    static void displayOwners(const vector<Owner>& owners) {
        if (owners.empty()) {
            cout << "\nNo owners found!" << endl;
            return;
        }
        for (const auto& owner : owners) {
            // Display owner info
            cout << "\n============================="
                << "\nOwner's First Name: " << owner.first_name
                << "\nOwner's Last Name: " << owner.last_name
                << "\nPhone number: " << owner.phone
                << "\nEmail: " << owner.email
                << "\nAddress: " << owner.address
                << "\n============================= " << endl;
        }
    }

    // Add a new owner
    static void addOwner(vector<Owner>& owners) {
        string first_name, last_name, phone, email, address;
        cout << "Add New Owner:\n\n";

        // Get and validate first name
        do {
            cout << "Enter owner's first name: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> first_name;
        } while (!isValidName(first_name) && cout << "Invalid name. Try again!\n");
        // Get and validate last name
        do {
            cout << "Enter owner's last name: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> last_name;
        } while (!isValidName(last_name) && cout << "Invalid name. Try again!\n");
        // Get and validate phone number
        do {
            cout << "Enter owner's phone number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> phone;
        } while (!isValidPhone(phone) && cout << "Invalid phone number. Try again!\n");
        // Get and validate email
        do {
            cout << "Enter owner's email: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> email;
        } while (!isValidEmail(email) && cout << "Invalid email. Try again!\n");
        // Get and validate address
        do {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter owner's address: ";
            getline(cin, address);
        } while (!isValidAddress(address) && cout << "Invalid address. Try again!\n");
        // Add new owner and save to file
        owners.push_back(Owner(first_name, last_name, phone, email, address));
        saveToFile(owners);
        cout << "\nOwner added successfully!" << endl;
    }

    // Update existing owner info
    static void updateOwner(vector<Owner>& owners) {
        string last_name;
        cout << "\nAvailable Owners:";
        displayOwners(owners);
        if (owners.empty()) {
            cout << "Please add an owner first!\n";
            return;
        }
        // Get the last name of owner to be updated
        do {
            cout << "\nEnter owner's last name to update owner's details: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> last_name;
        } while (!isValidName(last_name) && cout << "Invalid name. Try again!\n");

        // Search and update matching owner
        for (auto& owner : owners) {
            if (owner.last_name == last_name) {
                cout << "------------------\nUpdate an Owner:\n\n";
                // Update phone number
                do {
                    cout << "Enter owner's phone number: ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin >> owner.phone;
                } while (!isValidPhone(owner.phone) && cout << "Invalid phone number. Try again!\n");
                // Update email
                do {
                    cout << "Enter owner's email: ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin >> owner.email;
                } while (!isValidEmail(owner.email) && cout << "Invalid email. Try again!\n");

                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                // Update address
                do {
                    cout << "Enter owner's address: ";
                    getline(cin, owner.address);
                } while (!isValidAddress(owner.address) && cout << "Invalid address. Try again!\n");

                saveToFile(owners);
                cout << "\nOwner updated successfully!" << endl;
                return;
            }
        }
        cout << "Owner not found!\n";
    }

    // Delete an owner
    static void deleteOwner(vector<Owner>& owners) {
        string last_name;

        cout << "\nAvailable Owners:";
        displayOwners(owners);
        if (owners.empty()) {
            cout << "Please add an owner first!\n";
            return;
        }
        // Get last name to identify which owner to delete
        do {
            cout << "Enter owner's last name to delete owner's details: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> last_name;
        } while (!isValidName(last_name) && cout << "Invalid name. Try again!\n");
        // Find and remove owner
        for (auto& owner : owners) {
            if (owner.last_name == last_name) {
                owners.erase(remove_if(owners.begin(), owners.end(), [&](const Owner& owner) { return owner.last_name == last_name; }), owners.end());
                // Save updated list
                ofstream file("owners.txt");
                if (!file) {
                    cerr << "Error opening file!" << endl;
                    return;
                }

                for (const auto& owner : owners) {
                    file << owner.first_name << " " << owner.last_name << " "
                        << owner.phone << " " << owner.email << " " << owner.address << "\n";
                }

                file.close();
                cout << "\nOwner deleted successfully!" << endl;
                return;
            }
        }
        cerr << "Owner not found!\n";
    }

    // Validation: ensure name contains only letters
    static bool isValidName(string name) {
        for (int i = 0; i < name.length(); i++) {
            if (!isalpha(name[i])) {
                return false;
            }
        }
        return true;
    }

    // Validation: ensure phone number is 10ñ15 digits
    static bool isValidPhone(string phone) {
        regex phoneRegex(R"(\d{10,15})");
        return regex_match(phone, phoneRegex);
    }

    // Validation: check basic email format
    static bool isValidEmail(string email) {
        regex emailRegex(R"(^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$)");
        return regex_match(email, emailRegex);
    }

    // Validation: address must not be empty
    static bool isValidAddress(string address) {
        if (address.empty()) {
            return false;
        }
        return true;
    }

    // Accessor: get owner's last name
    string getLastName() const { return last_name; }
};


class Appointment {
private:
    // Private member variables for appointment details
    string petName, last_name, date, time, status;

public:
    // Default constructor
    Appointment() {}
    // Constructor with parameters
    Appointment(string p, string l, string d, string t, string s)
        : petName(p), last_name(l), date(d), time(t), status(s) {}

    // Save all appointments to file
    static void saveToFile(const vector<Appointment>& appointments) {
        // Create an output file stream and open "appointments.txt"
        // ios::trunc means it will overwrite the file if it already exists
        ofstream file("appointments.txt", ios::trunc);
        
        // Loop through each appointment in the vector
        for (const auto& app : appointments) {
            file << app.petName << " " << app.last_name << " " << app.date << " " << app.time << " " << app.status << "\n";
        }
        file.close();
    }

    // Load appointments from file
    static vector<Appointment> loadFromFile() {
        // Create an empty vector to store objects
        vector<Appointment> appointments;
        ifstream file("appointments.txt");
        
        // Check if the file is opened or not
        if (!file) {
            cerr << "Warning: Unable to open a \"appointments.txt\" file for reading. Add your first record to create a file. \n";
            return appointments;
        }
        string petName, last_name, date, time, status;
        // Read one line at a time from the file until there is no more data in the file
        while (file >> petName >> last_name >> date >> time >> status) {
            appointments.push_back(Appointment(petName, last_name, date, time, status));
        }
        file.close();
        return appointments;
    }

    // Display all appointments
    static void displayAppointments(const vector<Appointment>& appointments) {
        // Check if the vector is empty
        if (appointments.empty()) {
            cout << "\nNo appointments found!" << endl;
            return;
        }
        // Loop through each appointment in the vector and display it
        for (const auto& app : appointments) {
            cout << "\n============================= " << "\nPet's Name: " << app.petName << "\nOwner's Last Name: " << app.last_name << "\nDate: " << app.date << "\nTime: " << app.time << "\nStatus: " << app.status;
            cout << "\n============================= " << endl;
        }
    }

    // Add new appointment
    static void addAppointment(vector<Appointment>& appointments, vector<Pet>& pets, vector<Owner>& owners) {
        // Declare variables to store input data (Default status is "scheduled")
        string petName, last_name, date, time, status = "scheduled";

        // Show available pets
        cout << "\nAvailable Pets:";
        Pet::viewPets(pets);
        
        // If there are no pets, ask the user to add one first
        if (pets.empty()) {
            cout << "Please add a pet first!\n";
            return;
        }

        // Get pet name from user with validation
        while (true) {
            cout << "Enter the pet's name: ";
            if (!(cin >> petName) || !isValidName(petName)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "Invalid input. Name must be a single word.\n";
                continue;
            }
            break;
        }

        // Check if the entered pet name exists in the list
        bool petExists = any_of(pets.begin(), pets.end(),
            [&petName](const Pet& pet) { return pet.getName() == petName; });
        if (!petExists) {
            cout << "Error: Pet with the name \"" << petName << "\" does not exist.\n";
            return;
        }

        // Show available owners
        cout << "\nAvailable Owners:";
        Owner::displayOwners(owners);
        
        // If there are no owners, ask the user to add one first
        if (owners.empty()) {
            cout << "Please add an owner first!\n";
            return;
        }

        // Get owner's last name with validation
        while (true) {
            cout << "Enter owner's last name: ";
            if (!(cin >> last_name) || !isValidName(last_name)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "Invalid input. Name must be a single word.\n";
                continue;
            }
            break;
        }

        // Check if the entered owner last name exists in the list
        bool ownerExists = any_of(owners.begin(), owners.end(),
            [&last_name](const Owner& owner) { return owner.getLastName() == last_name; });
        if (!ownerExists) {
            cout << "Error: Owner with the name \"" << last_name << "\" does not exist.\n";
            return;
        }
        
        cout << "\nSchedule an Appointment:\n";
        // Ask user to enter the appointment date with validation
        do {
            cout << "Enter date (DD/MM/YYYY): ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> date;
        } while (!isValidDate(date) && cout << "Invalid date. Try again!\n");

        // Ask user to enter the appointment time with validation
        do {
            cout << "Enter time (HH:MM): ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> time;
        } while (!isValidTime(time) && cout << "Invalid time. Try again!\n");

        // Add the new appointment to the appointments list
        appointments.push_back(Appointment(petName, last_name, date, time, status));
        // Save updated appointments to the file
        saveToFile(appointments);
        cout << "\nAppointment added successfully!" << endl;
    }
    
    // Update existing appointment
    static void updateAppointment(vector<Appointment>& appointments) {
        // Declare variables to store input data
        string petName, date, time;

        // Show all existing appointments
        cout << "\nAvailable appointments: ";
        displayAppointments(appointments);
        
        // If no appointments exist, inform the user and exit
        if (appointments.empty()) {
            cout << "Please add an appointment first!\n";
            return;
        }
        
        // Get the pet name to identify which appointment to update
        while (true) {
            cout << "Enter the pet name to update appointment: ";
            if (!(cin >> petName) || !isValidName(petName)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "Invalid input. Name must be a single word.\n";
                continue;
            }
            break;
        }
        // Ask user for the date of the appointment they want to update
        do {
            cout << "Enter date to update appointment (DD/MM/YYYY): ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> date;
        } while (!isValidDate(date) && cout << "Invalid date. Try again!\n");
        
        // Ask for the time of the appointment they want to update
        do {
            cout << "Enter time to update appointment (HH:MM): ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> time;
        } while (!isValidTime(time) && cout << "Invalid time. Try again!\n");

        // Loop through all appointments to find a match
        for (auto& app : appointments) {
            if (app.petName == petName && app.date == date && app.time == time) {
                // If appointment is found – start update process
                cout << "---------------------\n Updating....\n---------------------\n";
                // Ask for new date
                do {
                    cout << "Enter new date (DD/MM/YYYY): ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin >> app.date;
                } while (!isValidDate(app.date) && cout << "Invalid date. Try again!\n");

                // Ask for new time
                do {
                    cout << "Enter new time (HH:MM): ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin >> app.time;
                } while (!isValidTime(app.time) && cout << "Invalid time. Try again!\n");

                // Save updated appointments to the file
                saveToFile(appointments);
                cout << "\nAppointment updated successfully!" << endl;
                return;
            }
        }
        cout << "\nAppointment not found!\n";
    }
    
    // Cancel appointment by updating its status to "cancelled"
    static void cancelAppointment(vector<Appointment>& appointments) {
        // Declare variables to store input data
        string petName, date, time;

        // Show all appointments
        cout << "\nAvailable appointments:";
        displayAppointments(appointments);
        
        // If no appointments exist, ask user to add one first
        if (appointments.empty()) {
            cout << "Please add an appointment first!\n";
            return;
        }
        
        // Get the pet name to identify which appointment to cancel
        while (true) {
            cout << "Enter pet name to cancel appointment: ";
            if (!(cin >> petName) || !isValidName(petName)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "Invalid input. Name must be a single word.\n";
                continue;
            }
            break;
        }
        // Ask user for the date of the appointment they want to cancel
        do {
            cout << "Enter date to cancel appointment (DD/MM/YYYY): ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> date;
        } while (!isValidDate(date) && cout << "Invalid date. Try again!\n");
        
        // Ask user for the time of the appointment they want to cancel
        do {
            cout << "Enter time to cancel appointment (HH:MM): ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> time;
        } while (!isValidTime(time) && cout << "Invalid time. Try again!\n");

        // Loop through all appointments to find a match
        for (auto& app : appointments) {
            if (app.petName == petName && app.date == date && app.time == time) {
                // If appointment is found – change its status to "cancelled"
                app.status = "cancelled";
                cout << "\nAppointment is " << app.status << " successfully\n";
                // Save the updated list back to the file
                saveToFile(appointments);
                return;
            }
        }
        cout << "\nAppointment not found!\n";
    }
    // Complete appointment
    static void completeAppointment(vector<Appointment>& appointments) {
        // Declare variables to store input data
        string petName, date, time;

        // Show all appointments
        cout << "\nAvailable appointments:";
        displayAppointments(appointments);
        
        // If no appointments exist, ask user to add one first
        if (appointments.empty()) {
            cout << "Please add an appointment first!\n";
            return;
        }
        
        // Get the pet name to identify which appointment to complete
        while (true) {
            cout << "Enter the pet name to complete appointment: ";
            if (!(cin >> petName) || !isValidName(petName)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "Invalid input. Name must be a single word.\n";
                continue;
            }
            break;
        }
        
        // Ask user for the date of the appointment they want to complete
        do {
            cout << "Enter date to complete appointment (DD/MM/YYYY): ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> date;
        } while (!isValidDate(date) && cout << "Invalid date. Try again!\n");
        
        // Ask user for the time of the appointment they want to complete
        do {
            cout << "Enter time to complete appointment (HH:MM): ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin >> time;
        } while (!isValidTime(time) && cout << "Invalid time. Try again!\n");

        // Loop through all appointments to find a match
        for (auto& app : appointments) {
            if (app.petName == petName && app.date == date && app.time == time) {
                // If appointment is found – change its status to "completed"
                app.status = "completed";
                cout << "\nAppointment is " << app.status << " successfully\n";
                // Save the updated list back to the file
                saveToFile(appointments);
                return;
            }
        }
        cout << "\nAppointment not found!\n";
    }
    // Show upcoming (scheduled) appointments
    static void displayUpcomingAppointments(const vector<Appointment>& appointments) {
        string status;
        
        // Check if there are any appointments
        if (appointments.empty()) {
            cout << "No appointments found!" << endl;
            return;
        }
        else
            // Loop through all appointments
            for (const auto& app : appointments) {
                if (app.status == "scheduled") {
                    // If the appointment status is "scheduled", display it
                    cout << "\n=============================" << "\nPet's Name: " << app.petName << "\nOwner's Last Name: " << app.last_name << "\nDate: " << app.date << "\nTime: " << app.time << "\nStatus: " << app.status << "\n=============================" << endl;
                }
            }
    }
    // Show all appointments for a specific pet
    static void displayPetAppointments(const vector<Appointment>& appointments, vector<Pet>& pets) {
        string petName;
        
        // If no appointments exist, notify the user
        if (appointments.empty()) {
            cout << "No appointments found!" << endl;
            return;
        }
        // If no pets exist, notify the user
        else if (pets.empty()) {
            cout << "No pets found!" << endl;
            return;
        }
        else
            // Display the list of pets to choose from
            cout << "\nAvailable Pets:";
            Pet::viewPets(pets);

        // Ask user to enter a valid pet name
        while (true) {
            cout << "Enter the pet's name: ";
            if (!(cin >> petName) || !isValidName(petName)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "Invalid input. Name must be a single word.\n";
                continue;
            }
            break;
        }

        // Check if the entered pet name exists in the list
        bool petExists = any_of(pets.begin(), pets.end(),
            [&petName](const Pet& pet) { return pet.getName() == petName; });
        if (!petExists) {
            cout << "Error: Pet with the name \"" << petName << "\" does not exist.\n";
            return;
        }

        // Loop through all appointments and display those matching the pet name
        for (const auto& app : appointments) {
            if (app.petName == petName) {
                cout << "\n=============================" << "\nPet's Name: " << app.petName << "\nOwner's Last Name: " << app.last_name << "\nDate: " << app.date << "\nTime: " << app.time << "\nStatus: " << app.status << "\n=============================" << endl;
            }
        }
    }
    // Validation function to check if the time is in valid HH:MM format
    static bool isValidTime(string time) {
        regex time_pattern(R"((0[0-9]|1[0-9]|2[0-3]):([0-5][0-9]))");
        return regex_match(time, time_pattern);
    }

    // Function to check if a given year is a leap year
    static bool isLeapYear(int year) {
        return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
    }

    // Validation function to check if the date is in valid DD/MM/YYYY format
    static bool isValidDate(string date) {
        // Regular expression for validating the date format (DD/MM/YYYY)
        regex pattern(R"((0[1-9]|[12][0-9]|3[01])/(0[1-9]|1[0-2])/([1-9][0-9]{3}))");
        if (!regex_match(date, pattern)) {
            return false;
        }

        // Extract day, month, and year from the input string
        int day = stoi(date.substr(0, 2));
        int month = stoi(date.substr(3, 2));
        int year = stoi(date.substr(6, 4));

        // Array of days in each month for a common year
        int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        // Adjust the number of days in February for leap years
        if (month == 2 && isLeapYear(year)) {
            daysInMonth[1] = 29;
        }

        // Validate that the day is within the valid range for the given month
        return day <= daysInMonth[month - 1];
    }

    // Validation helper function to check if the name contains only alphabetic characters
    static bool isValidName(string name) {
        for (int i = 0; i < name.length(); i++) {
            if (!isalpha(name[i])) {
                return false;
            }
        }
        return true;
    }
};

void menu(int role) {
    vector<Pet> pets = Pet::loadFromFile();
    vector<Owner> owners = Owner::loadFromFile();
    vector<Appointment> appointments = Appointment::loadFromFile();
    int choice;

    do {
        cout << "\n=============================\nVeterinary Management System\n=============================\n";
        cout << "1. Add records (Pet/Owner)\n"
            << "2. Update records (Pet/Owner)\n"
            << "3. Delete records (Pet/Owner)\n"
            << "4. Add medical records\n"
            << "5. Schedule Appointment\n"
            << "6. Update Appointment(Appointment's records/Cancel appointment/Complete appointment)\n"
            << "7. View (Pets/Owners/All Appointments/Upcoming Appointments/Pet's Appointments)\n"
            << "8. Exit\n\n";
        while (true) {
            cout << "Enter choice: ";
            if (!(cin >> choice) || choice < 1 || choice > 8) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "\nInvalid input. Choice must be a positive integer(1-8).\n";
                continue;
            }
            break;
        }
        cout << "------------------\n";

        if (role == 1 && (choice != 4 && choice != 7 && choice != 8)) {
            cout << "Access Denied! You don't have permission to perform this action.\n";
            continue;
        }

        if (role == 3 && choice == 4) {
            cout << "Access Denied! You don't have permission to perform this action.\n";
            continue;
        }

        switch (choice) {
        case 1:
            int add_choice;
            cout << "Add Records:\n1. Pet\n2. Owner\n3. Go back\n\n";
            while (true) {
                cout << "Enter choice: ";
                if (!(cin >> add_choice) || add_choice < 1 || add_choice > 3) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cerr << "\nInvalid input. Choice must be a positive integer(1-3).\n";
                    continue;
                }
                break;
            }
            cout << "------------------\n";
            switch (add_choice) {
            case 1:
                Pet::addPet(pets);
                continue;
            case 2:
                Owner::addOwner(owners);
                continue;
            case 3:
                cout << "Going back...\n";
                continue;
            default:
                cout << "\nInvalid choice! Please try again.\n";
                continue;
            }
        case 2:
            int update_choice;
            cout << "Update Records:\n1. Pet\n2. Owner\n3. Go back\n\n";
            while (true) {
                cout << "Enter choice: ";
                if (!(cin >> update_choice) || update_choice < 1 || update_choice > 3) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cerr << "\nInvalid input. Choice must be a positive integer(1-3).\n";
                    continue;
                }
                break;
            }
            cout << "------------------\n";
            switch (update_choice) {
            case 1:
                Pet::editPet(pets);
                continue;
            case 2:
                Owner::updateOwner(owners);
                continue;
            case 3:
                cout << "Going back...\n";
                continue;
            default:
                cout << "\nInvalid choice! Please try again.\n";
                continue;
            }
        case 3:
            int delete_choice;
            cout << "Delete Records:\n1. Pet\n2. Owner\n3. Go back\n\n";
            while (true) {
                cout << "Enter choice: ";
                if (!(cin >> delete_choice) || delete_choice < 1 || delete_choice > 3) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cerr << "\nInvalid input. Choice must be a positive integer(1-3).\n";
                    continue;
                }
                break;
            }
            cout << "------------------\n";
            switch (delete_choice) {
            case 1:
                Pet::deletePet(pets);
                continue;
            case 2:
                Owner::deleteOwner(owners);
                continue;
            case 3:
                cout << "Going back...\n";
                continue;
            default:
                cout << "\nInvalid choice! Please try again.\n";
                continue;
            }
        case 4:
            Pet::addMedicalRecord(pets);
            continue;
        case 5:
            Appointment::addAppointment(appointments, pets, owners);
            continue;
        case 6:
            int app_choice;
            cout << "Update:\n1. Appointment's records\n2. Cancel Appointment\n3. Complete Appointment\n4. Go back\n\n";
            while (true) {
                cout << "Enter choice: ";
                if (!(cin >> app_choice) || app_choice < 1 || app_choice > 4) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cerr << "\nInvalid input. Choice must be a positive integer(1-4).\n";
                    continue;
                }
                break;
            }
            cout << "------------------\n";
            switch (app_choice) {
            case 1:
                Appointment::updateAppointment(appointments);
                continue;
            case 2:
                Appointment::cancelAppointment(appointments);
                continue;
            case 3:
                Appointment::completeAppointment(appointments);
                continue;
            case 4:
                cout << "Going back...\n";
                continue;
            default:
                cout << "\nInvalid choice! Please try again.\n";
                continue;
            }
        case 7:
            int view_choice;
            cout << "View:\n1. Pets\n2. Owners\n3. All Appointments\n4. Upcoming Appointments\n5. Pet's Appointments\n6. Go back\n\n";
            while (true) {
                cout << "Enter choice: ";
                if (!(cin >> view_choice) || view_choice < 1 || view_choice > 6) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cerr << "\nInvalid input. Choice must be a positive integer(1-6).\n";
                    continue;
                }
                break;
            }
            cout << "------------------\n";
            switch (view_choice) {
            case 1: Pet::viewPets(pets); continue;
            case 2: Owner::displayOwners(owners); continue;
            case 3: Appointment::displayAppointments(appointments); continue;
            case 4: Appointment::displayUpcomingAppointments(appointments); continue;
            case 5: Appointment::displayPetAppointments(appointments, pets); continue;
            case 6: cout << "Going back...\n"; continue;
            default: cout << "Invalid choice!\n"; continue;
            }
        case 8:
            cout << "Exiting system...\n";
            continue;
        default:
            cout << "\nInvalid choice! Please try again.\n";
            break;
        }
    } while (choice != 8);
}



int main() {
    int role;
    do {
        cout << "------------------------------\nLogin Page - Choose User Role\n------------------------------\n1. Veterinarian\n2. Admin\n3. Staff\n4. Exit\n\n";
        while (true) {
            cout << "Enter role: ";
            if (!(cin >> role) || role < 1 || role > 4) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "\nInvalid input. Role choice must be a positive integer(1-4).\n";
                continue;
            }
            break;
        }
        cout << "------------------\n";
        if (role >= 1 && role <= 3)
            menu(role);
    } while (role != 4);

    return 0;
}
