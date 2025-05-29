#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>

int main() {
    Fl_Window win(300, 200, "FLTK Window");
    Fl_Button btn(100, 80, 100, 30, "lululu lululu");
    win.end();
    win.show();
    return Fl::run();
}


//  #include <iostream>
// #include <string>
// using namespace std;
// bool validateLogin(const string& username, const string& password) {
//     const string correctUsername = "e-mart";
//     const string correctPassword = "12345";
//     if (username != correctUsername && password != correctPassword) {
//         cout << "Both username and password are incorrect!" << endl;
//         return false;
//     }
//     else if (username != correctUsername) {
//         cout << "Incorrect username!" << endl;
//         return false;
//     }
//     else if (password != correctPassword) {
//         cout << "Incorrect password!" << endl;
//         return false;
//     }
//     return true;
// }
// int main() {
//     string username, password;

//     cout << "E-Mart Login Page" << endl;
 
//     while (true) {
//         cout << "Enter username: ";
//         cin >> username;

//         cout << "Enter password: ";
//         cin >> password;

//         if (validateLogin(username, password)) {
//             cout << "\nLogin successful! Welcome to E-Mart!" << endl;
//             break;
//         } else {
//             cout << "Please try again.\n" << endl;
//         }
//     }
//     return 0;
// }
