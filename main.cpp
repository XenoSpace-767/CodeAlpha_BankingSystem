#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>

// Class to handle individual transactions
class Transaction {
private:
    std::string type; // "Deposit", "Withdrawal", or "Transfer"
    double amount;
    std::string timestamp;

    std::string getCurrentTime() {
        std::time_t now = std::time(nullptr);
        char buf[80];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    }

public:
    Transaction(std::string t, double amt) : type(t), amount(amt) {
        timestamp = getCurrentTime();
    }

    void displayTransaction() const {
        std::cout << std::left << std::setw(20) << timestamp
                  << std::setw(15) << type
                  << "$" << std::fixed << std::setprecision(2) << amount << "\n";
    }
};

// Class representing a Bank Account
class Account {
private:
    int accountNumber;
    double balance;
    std::vector<Transaction> transactionHistory;

public:
    Account(int accNum, double initialBalance = 0.0)
        : accountNumber(accNum), balance(initialBalance) {
        if (initialBalance > 0) {
            transactionHistory.push_back(Transaction("Initial Deposit", initialBalance));
        }
    }

    int getAccountNumber() const { return accountNumber; }
    double getBalance() const { return balance; }

    void deposit(double amount) {
        if (amount <= 0) {
            std::cout << "[ERROR] Deposit amount must be positive.\n";
            return;
        }
        balance += amount;
        transactionHistory.push_back(Transaction("Deposit", amount));
        std::cout << "[SUCCESS] Deposited $" << std::fixed << std::setprecision(2) << amount << ". New Balance: $" << balance << "\n";
    }

    bool withdraw(double amount) {
        if (amount <= 0) {
            std::cout << "[ERROR] Withdrawal amount must be positive.\n";
            return false;
        }
        if (amount > balance) {
            std::cout << "[ERROR] Insufficient funds! Current Balance: $" << std::fixed << std::setprecision(2) << balance << "\n";
            return false;
        }
        balance -= amount;
        transactionHistory.push_back(Transaction("Withdrawal", amount));
        std::cout << "[SUCCESS] Withdrew $" << std::fixed << std::setprecision(2) << amount << ". New Balance: $" << balance << "\n";
        return true;
    }

    void displayHistory() const {
        std::cout << "\n--- Transaction History for Account #" << accountNumber << " ---\n";
        if (transactionHistory.empty()) {
            std::cout << "No transactions recorded yet.\n";
            return;
        }
        std::cout << std::left << std::setw(20) << "Date & Time"
                  << std::setw(15) << "Type"
                  << "Amount\n";
        std::cout << "--------------------------------------------------\n";
        for (const auto& txn : transactionHistory) {
            txn.displayTransaction();
        }
    }
};

// Class representing a Bank Customer
class Customer {
private:
    int customerID;
    std::string name;
    Account account;

public:
    Customer(int id, std::string customerName, int accNum, double initialBalance)
        : customerID(id), name(customerName), account(accNum, initialBalance) {}

    int getCustomerID() const { return customerID; }
    std::string getName() const { return name; }
    Account& getAccount() { return account; }

    void displayCustomerInfo() const {
        std::cout << "\n===========================================\n";
        std::cout << "             CUSTOMER DETAILS              \n";
        std::cout << "===========================================\n";
        std::cout << "Customer ID    : " << customerID << "\n";
        std::cout << "Customer Name  : " << name << "\n";
        std::cout << "Account Number : " << account.getAccountNumber() << "\n";
        std::cout << "Current Balance: $" << std::fixed << std::setprecision(2) << account.getBalance() << "\n";
        std::cout << "===========================================\n";
    }
};

// Main Banking System Application
class BankingSystem {
private:
    std::vector<Customer> customers;
    int nextCustomerID = 1001;
    int nextAccountNumber = 5001;

    Customer* findCustomer(int accNum) {
        for (auto& customer : customers) {
            if (customer.getAccount().getAccountNumber() == accNum) {
                return &customer;
            }
        }
        return nullptr;
    }

public:
    void createAccount() {
        std::string name;
        double initialDeposit;

        std::cout << "\n--- Create New Bank Account ---\n";
        std::cout << "Enter Customer Full Name: ";
        std::cin.ignore();
        std::getline(std::cin, name);

        std::cout << "Enter Initial Deposit Amount: $";
        while (!(std::cin >> initialDeposit) || initialDeposit < 0) {
            std::cout << "Invalid amount. Enter a positive number: $";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
        }

        customers.push_back(Customer(nextCustomerID++, name, nextAccountNumber++, initialDeposit));
        std::cout << "[SUCCESS] Account successfully created for " << name << "!\n";
        std::cout << "Assigned Account Number: " << (nextAccountNumber - 1) << "\n";
    }

    void performDeposit() {
        int accNum;
        double amount;
        std::cout << "\nEnter Account Number: ";
        std::cin >> accNum;

        Customer* client = findCustomer(accNum);
        if (!client) {
            std::cout << "[ERROR] Account not found.\n";
            return;
        }

        std::cout << "Enter Deposit Amount: $";
        std::cin >> amount;
        client->getAccount().deposit(amount);
    }

    void performWithdrawal() {
        int accNum;
        double amount;
        std::cout << "\nEnter Account Number: ";
        std::cin >> accNum;

        Customer* client = findCustomer(accNum);
        if (!client) {
            std::cout << "[ERROR] Account not found.\n";
            return;
        }

        std::cout << "Enter Withdrawal Amount: $";
        std::cin >> amount;
        client->getAccount().withdraw(amount);
    }

    void performTransfer() {
        int senderAccNum, receiverAccNum;
        double amount;

        std::cout << "\nEnter Sender Account Number: ";
        std::cin >> senderAccNum;
        Customer* sender = findCustomer(senderAccNum);

        if (!sender) {
            std::cout << "[ERROR] Sender account not found.\n";
            return;
        }

        std::cout << "Enter Recipient Account Number: ";
        std::cin >> receiverAccNum;
        Customer* receiver = findCustomer(receiverAccNum);

        if (!receiver) {
            std::cout << "[ERROR] Recipient account not found.\n";
            return;
        }

        if (senderAccNum == receiverAccNum) {
            std::cout << "[ERROR] Cannot transfer money to the same account.\n";
            return;
        }

        std::cout << "Enter Transfer Amount: $";
        std::cin >> amount;

        if (sender->getAccount().withdraw(amount)) {
            receiver->getAccount().deposit(amount);
            std::cout << "[SUCCESS] Transferred $" << std::fixed << std::setprecision(2) << amount
                      << " from Account #" << senderAccNum << " to Account #" << receiverAccNum << "\n";
        }
    }

    void viewAccountDetails() {
        int accNum;
        std::cout << "\nEnter Account Number: ";
        std::cin >> accNum;

        Customer* client = findCustomer(accNum);
        if (!client) {
            std::cout << "[ERROR] Account not found.\n";
            return;
        }

        client->displayCustomerInfo();
        client->getAccount().displayHistory();
    }
};

int main() {
    BankingSystem bank;
    int choice;

    do {
        std::cout << "\n===========================================\n";
        std::cout << "          BANKING MANAGEMENT SYSTEM        \n";
        std::cout << "===========================================\n";
        std::cout << "1. Create Account\n";
        std::cout << "2. Deposit Funds\n";
        std::cout << "3. Withdraw Funds\n";
        std::cout << "4. Transfer Funds\n";
        std::cout << "5. View Account & Transactions\n";
        std::cout << "6. Exit\n";
        std::cout << "Choose an option (1-6): ";

        if (!(std::cin >> choice)) {
            std::cout << "Invalid choice. Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        switch (choice) {
            case 1: bank.createAccount(); break;
            case 2: bank.performDeposit(); break;
            case 3: bank.performWithdrawal(); break;
            case 4: bank.performTransfer(); break;
            case 5: bank.viewAccountDetails(); break;
            case 6: std::cout << "Exiting system. Goodbye!\n"; break;
            default: std::cout << "Invalid option. Select between 1 and 6.\n";
        }
    } while (choice != 6);

    return 0;
}
