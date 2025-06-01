#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <string>
using namespace std;

Fl_Input* usernameInput;
Fl_Input* passwordInput;
Fl_Box* messageBox;

void validateLogin(Fl_Widget*, void*) {
    string username = usernameInput->value();
    string password = passwordInput->value();

    const string correctUsername = "e-mart";
    const string correctPassword = "12345";

    if (username != correctUsername && password != correctPassword) {
        messageBox->label("Both username and password are incorrect!");
    }
    else if (username != correctUsername) {
        messageBox->label("Incorrect username!");
    }
    else if (password != correctPassword) {
        messageBox->label("Incorrect password!");
    }
    else {
        messageBox->label("Login successful! Welcome to E-Mart!");
    }
}

int main() {
    Fl_Window* window = new Fl_Window(400, 250, "E-Mart Login");

    
    Fl_Box* userLabel = new Fl_Box(50, 40, 100, 30, "Username:");
    usernameInput = new Fl_Input(150, 40, 200, 30);

    
    Fl_Box* passLabel = new Fl_Box(50, 90, 100, 30, "Password:");
    passwordInput = new Fl_Input(150, 90, 200, 30);
    passwordInput->type(FL_SECRET_INPUT); 

    
    Fl_Button* loginButton = new Fl_Button(150, 140, 100, 30, "Login");
    loginButton->callback(validateLogin);


    messageBox = new Fl_Box(50, 190, 300, 30, "");

    window->end();
    window->show();

    return Fl::run();
}

