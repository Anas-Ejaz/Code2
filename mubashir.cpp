 #include <iostream>
#include <string>

using namespace std;

// Function to validate login credentials
bool validateLogin(const string& username, const string& password) {
    const string correctUsername = "e-mart";
    const string correctPassword = "12345";

    if (username != correctUsername && password != correctPassword) {
        cout << "Both username and password are incorrect!" << endl;
        return false;
    }
    else if (username != correctUsername) {
        cout << "Incorrect username!" << endl;
        return false;
    }
    else if (password != correctPassword) {
        cout << "Incorrect password!" << endl;
        return false;
    }
    return true;
}

int main() {
    string username, password;

    cout << "====== E-Mart Login Page ======" << endl;

    // Loop until correct credentials are entered
    while (true) {
        cout << "Enter username: ";
        cin >> username;

        cout << "Enter password: ";
        cin >> password;

        if (validateLogin(username, password)) {
            cout << "\nLogin successful! Welcome to E-Mart!" << endl;
            break;
        } else {
            cout << "Please try again.\n" << endl;
        }
    }

    return 0;
}
