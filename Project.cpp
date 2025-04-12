#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
using namespace std;

// Item class for inventory items
class Item {
public:
    string name;
    double price;
    int quantity;
    Item(string n, double p, int q) : name(n), price(p), quantity(q) {}
};

// Abstract User class for roles
class User {
protected:
    string role;
public:
    virtual void menu() = 0; // Pure virtual for role-specific menus
    virtual ~User() {}
};

// Supermarket class for shared data and file I/O
class Supermarket {
protected:
    vector<Item> inventory;
    vector<pair<string, double>> expenses;
    double totalSales = 0.0;
    const string invFile = "inventory.csv";
    const string expFile = "expenses.csv";
    const string salesFile = "sales.csv";

    // Save inventory to CSV
    void saveInventory() {
        ofstream file(invFile);
        file << "Name,Price,Quantity\n"; // Header for Excel
        for (const auto& item : inventory) {
            file << "\"" << item.name << "\"," << fixed << setprecision(2) << item.price << "," << item.quantity << "\n";
        }
        file.close();
    }

    // Load inventory from CSV
    void loadInventory() {
        ifstream file(invFile);
        string line;
        getline(file, line); // Skip header
        while (getline(file, line)) {
            string name, priceStr, qtyStr;
            stringstream ss(line);
            getline(ss, name, ',');
            if (name.front() == '\"') name = name.substr(1, name.size() - 2);
            getline(ss, priceStr, ',');
            getline(ss, qtyStr);
            try {
                double price = stod(priceStr);
                int qty = stoi(qtyStr);
                inventory.emplace_back(name, price, qty);
            } catch (...) {}
        }
        file.close();
    }

    // Save expenses to CSV
    void saveExpenses() {
        ofstream file(expFile);
        file << "Description,Amount\n"; // Header for Excel
        for (const auto& exp : expenses) {
            file << "\"" << exp.first << "\"," << fixed << setprecision(2) << exp.second << "\n";
        }
        file.close();
    }

    // Load expenses from CSV
    void loadExpenses() {
        ifstream file(expFile);
        string line;
        getline(file, line); // Skip header
        while (getline(file, line)) {
            string desc, amtStr;
            stringstream ss(line);
            getline(ss, desc, ',');
            if (desc.front() == '\"') desc = desc.substr(1, desc.size() - 2);
            getline(ss, amtStr);
            try {
                double amt = stod(amtStr);
                expenses.emplace_back(desc, amt);
            } catch (...) {}
        }
        file.close();
    }

    // Save sales to CSV
    void saveSales() {
        ofstream file(salesFile);
        file << "TotalSales\n"; // Header for Excel
        file << fixed << setprecision(2) << totalSales << "\n";
        file.close();
    }

    // Load sales from CSV
    void loadSales() {
        ifstream file(salesFile);
        string line;
        getline(file, line); // Skip header
        if (getline(file, line)) {
            try {
                totalSales = stod(line);
            } catch (...) {}
        }
        file.close();
    }

public:
    Supermarket() { loadInventory(); loadExpenses(); loadSales(); } // Load all data on start
};

// Owner class with full access
class Owner : public User, public Supermarket {
public:
    Owner() { role = "Owner"; }

    void menu() override {
        int choice;
        while (true) {
            cout << "\nOwner Menu\n1. Add Item\n2. View Inventory\n3. Add Expense\n4. View Expenses\n5. View Total Sales\n6. Exit\nChoice: ";
            cin >> choice;
            cin.ignore();
            if (choice == 6) break;
            if (choice == 1) { // Add item to inventory
                string name;
                double price;
                int qty;
                cout << "Enter item name: ";
                getline(cin, name);
                cout << "Enter price: ";
                cin >> price;
                cout << "Enter quantity: ";
                cin >> qty;
                if (price >= 0 && qty >= 0) {
                    inventory.emplace_back(name, price, qty);
                    saveInventory();
                    cout << "Item added.\n";
                } else {
                    cout << "Invalid price or quantity.\n";
                }
            }
            else if (choice == 2) { viewInventory(); } // Show inventory
            else if (choice == 3) { // Add expense
                string desc;
                double amt;
                cout << "Enter expense description: ";
                getline(cin, desc);
                cout << "Enter amount: ";
                cin >> amt;
                if (amt >= 0) {
                    expenses.emplace_back(desc, amt);
                    saveExpenses();
                    cout << "Expense added.\n";
                } else {
                    cout << "Invalid amount.\n";
                }
            }
            else if (choice == 4) { viewExpenses(); } // Show expenses
            else if (choice == 5) { // Show sales
                cout << "Total Sales: $" << fixed << setprecision(2) << totalSales << "\n";
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
    }

private:
    // Display inventory
    void viewInventory() {
        if (inventory.empty()) {
            cout << "No items in inventory.\n";
            return;
        }
        cout << "\nInventory:\n";
        for (size_t i = 0; i < inventory.size(); ++i) {
            cout << i + 1 << ". " << inventory[i].name << ": $"<< fixed << setprecision(2) << inventory[i].price << ", Qty: " << inventory[i].quantity << "\n";
        }
    }

    // Display expenses
    void viewExpenses() {
        if (expenses.empty()) {
            cout << "No expenses recorded.\n";
            return;
        }
        cout << "\nExpenses:\n";
        double total = 0;
        for (size_t i = 0; i < expenses.size(); ++i) {
            cout << i + 1 << ". " << expenses[i].first << ": $" << fixed << setprecision(2) << expenses[i].second << "\n";
            total += expenses[i].second;
        }
        cout << "Total Expenses: $" << fixed << setprecision(2) << total << "\n";
    }
};

// Worker class for sales and inventory viewing
class Worker : public User, public Supermarket {
public:
    Worker() { role = "Worker"; }

    void menu() override {
        int choice;
        while (true) {
            cout << "\nWorker Menu\n1. Sell Item\n2. View Inventory\n3. Exit\nChoice: ";
            cin >> choice;
            cin.ignore();
            if (choice == 3) break;
            if (choice == 1) { // Sell item
                viewInventory();
                int index;
                cout << "Enter item number to sell: ";
                cin >> index;
                if (index < 1 || index > static_cast<int>(inventory.size())) {
                    cout << "Invalid item number.\n";
                    continue;
                }
                cout << "Enter quantity: ";
                int qty;
                cin >> qty;
                if (qty <= 0 || qty > inventory[index - 1].quantity) {
                    cout << "Invalid quantity.\n";
                    continue;
                }
                inventory[index - 1].quantity -= qty;
                double sale = qty * inventory[index - 1].price;
                totalSales += sale;
                saveInventory();
                saveSales();
                cout << "Sold " << qty << " of " << inventory[index - 1].name << " for $" << fixed << setprecision(2) << sale << "\n";
            }
            else if (choice == 2) { viewInventory(); } // Show inventory
            else {
                cout << "Invalid choice.\n";
            }
        }
    }

private:
    // Display inventory
    void viewInventory() {
        if (inventory.empty()) {
            cout << "No items in inventory.\n";
            return;
        }
        cout << "\nInventory:\n";
        for (size_t i = 0; i < inventory.size(); ++i) {
            cout << i + 1 << ". " << inventory[i].name << ": $"<< fixed << setprecision(2) << inventory[i].price << ", Qty: " << inventory[i].quantity << "\n";
        }
    }
};

// Customer class for viewing items
class Customer : public User, public Supermarket {
public:
    Customer() { role = "Customer"; }

    void menu() override {
        int choice;
        while (true) {
            cout << "\nCustomer Menu\n1. View Items\n2. Exit\nChoice: ";
            cin >> choice;
            if (choice == 2) break;
            if (choice == 1) { // Show available items
                if (inventory.empty()) {
                    cout << "No items available.\n";
                    continue;
                }
                cout << "\nAvailable Items:\n";
                for (size_t i = 0; i < inventory.size(); ++i) {
                    if (inventory[i].quantity > 0) {
                        cout << i + 1 << ". " << inventory[i].name << ": $"<< fixed << setprecision(2) << inventory[i].price << "\n";
                    }
                }
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
    }
};

// Main function 
int main() {
    int role;
    cout << "Supermarket System\n1. Owner\n2. Worker\n3. Customer\nChoose role: ";
    cin >> role;

    User* user = nullptr;
    if (role == 1) user = new Owner();
    else if (role == 2) user = new Worker();
    else if (role == 3) user = new Customer();
    else {
        cout << "Invalid role.\n";
        return 1;
    }

    user->menu(); 
    delete user; // Clean prev.
    return 0;
}