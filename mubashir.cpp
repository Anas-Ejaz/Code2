#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <string>
using namespace std;

Fl_Input* usernameInput;
Fl_Input* passwordInput;
Fl_Input* phoneInput;
Fl_Input* emailInput;
Fl_Box* messageBox;
Fl_Check_Button* showPasswordCheck;

void togglePasswordVisibility(Fl_Widget*, void*) {
    if (showPasswordCheck->value()) {
        passwordInput->type(FL_NORMAL_INPUT);
    } else {
        passwordInput->type(FL_SECRET_INPUT);
    }
    Fl::redraw(); // Refresh the UI
}

void validateSignUp(Fl_Widget*, void*) {
    string username = usernameInput->value();
    string password = passwordInput->value();
    string phone = phoneInput->value();
    string email = emailInput->value();

    const string correctUsername = "e-mart";
    const string correctPassword = "12345";
    const string correctPhone = "123456789";
    const string correctEmail = "emart@gmail.com";

    if (username != correctUsername) {
        messageBox->label("Incorrect username!");
    }
    else if (password != correctPassword) {
        messageBox->label("Incorrect password!");
    }
    else if (phone != correctPhone) {
        messageBox->label("Incorrect phone number!");
    }
    else if (email != correctEmail) {
        messageBox->label("Incorrect email!");
    }
    else {
        messageBox->label("Sign Up successful! Welcome to E-Mart!");
    }
}

int main() {
    Fl_Window* window = new Fl_Window(500, 380, "E-Mart Sign Up");

    // Username
    Fl_Box* userLabel = new Fl_Box(50, 30, 100, 30, "Username:");
    usernameInput = new Fl_Input(160, 30, 250, 30);

    // Password
    Fl_Box* passLabel = new Fl_Box(50, 80, 100, 30, "Password:");
    passwordInput = new Fl_Input(160, 80, 250, 30);
    passwordInput->type(FL_SECRET_INPUT);

    // Show password checkbox
    showPasswordCheck = new Fl_Check_Button(160, 115, 150, 25, "Show Password");
    showPasswordCheck->callback(togglePasswordVisibility);

    // Phone number
    Fl_Box* phoneLabel = new Fl_Box(50, 150, 100, 30, "Phone:");
    phoneInput = new Fl_Input(160, 150, 250, 30);

    // Email
    Fl_Box* emailLabel = new Fl_Box(50, 200, 100, 30, "Email:");
    emailInput = new Fl_Input(160, 200, 250, 30);

    // Sign Up button
    Fl_Button* signupButton = new Fl_Button(160, 250, 120, 35, "Sign Up");
    signupButton->callback(validateSignUp);

    // Message box
    messageBox = new Fl_Box(50, 300, 400, 30, "");

    window->end();
    window->show();

    return Fl::run();
}


