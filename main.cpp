#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_ask.H>

using namespace std;

class login;


class signup {
public:
    Fl_Input* passwordInput;
    Fl_Input* confirmPasswordInput;
    Fl_Check_Button* showPasswordCheck;
    Fl_Input* usernameInput;
    Fl_Input* phoneInput;
    Fl_Input* emailInput;
    Fl_Input* addressInput;

    int signUpPage(){
        Fl_Window* mainWindow = new Fl_Window(900, 600, "E-Mart Sign Up");
        mainWindow->color(FL_BLACK);

        Fl_Box* heading = new Fl_Box(0, 40, mainWindow->w(), 50, "Sign Up");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->align(FL_ALIGN_CENTER);

        int elementWidth = 250;
        int elementHeight = 30;
        int labelWidth = 150;
        int labelHeight = 30;
        int spacing = 50;
        int centerX = mainWindow->w() / 2 - elementWidth / 2;
        int startY = 120;

        Fl_Box* usernameLabel = new Fl_Box(centerX - labelWidth - 10, startY, labelWidth, labelHeight, "Username:");
        usernameLabel->labelcolor(FL_WHITE);
        usernameLabel->align(FL_ALIGN_CENTER);
        Fl_Input* usernameInput = new Fl_Input(centerX, startY, elementWidth, elementHeight);

        Fl_Box* phoneLabel = new Fl_Box(centerX - labelWidth - 10, startY + spacing, labelWidth, labelHeight, "Phone:");
        phoneLabel->labelcolor(FL_WHITE);
        phoneLabel->align(FL_ALIGN_CENTER);
        Fl_Input* phoneInput = new Fl_Input(centerX, startY + spacing, elementWidth, elementHeight);

        Fl_Box* emailLabel = new Fl_Box(centerX - labelWidth - 10, startY + 2 * spacing, labelWidth, labelHeight, "Email:");
        emailLabel->labelcolor(FL_WHITE);
        emailLabel->align(FL_ALIGN_CENTER);
        Fl_Input* emailInput = new Fl_Input(centerX, startY + 2 * spacing, elementWidth, elementHeight);

        Fl_Box* addressLabel = new Fl_Box(centerX - labelWidth - 10, startY + 3 * spacing, labelWidth, labelHeight, "Address:");
        addressLabel->labelcolor(FL_WHITE);
        addressLabel->align(FL_ALIGN_CENTER);
        Fl_Input* addressInput = new Fl_Input(centerX, startY + 3 * spacing, elementWidth, elementHeight);

        Fl_Box* passwordLabel = new Fl_Box(centerX - labelWidth - 10, startY + 4 * spacing, labelWidth, labelHeight, "Password:");
        passwordLabel->labelcolor(FL_WHITE);
        passwordLabel->align(FL_ALIGN_CENTER);
        Fl_Input* passwordInput = new Fl_Input(centerX, startY + 4 * spacing, elementWidth, elementHeight);
        passwordInput->type(FL_SECRET_INPUT);

        Fl_Box* confirmPasswordLabel = new Fl_Box(centerX - labelWidth - 10, startY + 5 * spacing, labelWidth, labelHeight, "Confirm Password:");
        confirmPasswordLabel->labelcolor(FL_WHITE);
        confirmPasswordLabel->align(FL_ALIGN_CENTER);
        Fl_Input* confirmPasswordInput = new Fl_Input(centerX, startY + 5 * spacing, elementWidth, elementHeight);
        confirmPasswordInput->type(FL_SECRET_INPUT);

        Fl_Check_Button* showPasswordCheck = new Fl_Check_Button(centerX, startY + 6 * spacing, 150, 25, "Show Password");
        showPasswordCheck->labelcolor(FL_WHITE);
        showPasswordCheck->callback([](Fl_Widget*, void* data) {
            ((signup*)data)->togglePassword(nullptr, data);
        }, this);

        Fl_Button* signupButton = new Fl_Button(centerX - 40, startY + 7 * spacing, 120, 35, "Sign Up");
        signupButton->color(FL_BLUE);
        signupButton->labelcolor(FL_WHITE);
        signupButton->callback([](Fl_Widget*, void* data) {
            ((signup*)data)->validateSignUp(nullptr, data);
        }, this);

        Fl_Button* returnBtn = new Fl_Button(centerX + 110, startY + 7 * spacing, 120, 35, "Return");
        returnBtn->color(FL_BLUE);
        returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback([](Fl_Widget*, void* data) {
            ((signup*)data)->gologinPage();
        }, this);

        mainWindow->end();
        mainWindow->show();

        return Fl::run();

    }

    void gologinPage();

    void validateSignUp(Fl_Widget*, void*){

        string user = usernameInput->value();
        string pass = passwordInput->value();
        string confirmPass = confirmPasswordInput->value();
        string phone = phoneInput->value();
        string email = emailInput->value();
        string address = addressInput->value();

        // 1. Check for empty fields
        if (user.empty() || pass.empty() || confirmPass.empty() ||
            phone.empty() || email.empty() || address.empty()) {
            fl_alert("All fields are required!");
            return;
        }

        // 2. Check if passwords match
        if (pass != confirmPass) {
            fl_alert("Passwords do not match!");
            passwordInput->value("");
            confirmPasswordInput->value("");
            return;
        }

        // 3. Check if username is reserved
        if (user == "admin") {
            fl_alert("Username 'admin' is reserved.");
            return;
        }


        // 5. Success
        fl_message("Sign Up Successful! You can now choose to sign in.");

        // 6. Clear input fields
        usernameInput->value("");
        passwordInput->value("");
        confirmPasswordInput->value("");
        phoneInput->value("");
        emailInput->value("");
        addressInput->value("");
        showPasswordCheck->value(0);
        togglePassword(nullptr, nullptr); // not togglePasswordVisibility

        Fl::add_timeout(1.5, [](void*){
            Fl::delete_widget(Fl::first_window());
        }, nullptr);    
    }
        
    void togglePassword(Fl_Widget*, void*) {
        passwordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
        confirmPasswordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
        Fl::redraw();
    }

    
};



class login{
    public:
    Fl_Input* usernameInput;
    Fl_Input* passwordInput;
    Fl_Check_Button* showPasswordCheck;
    Fl_Window* loginWindow;

    int loginPage(){
        loginWindow = new Fl_Window(900, 600, "E-Mart Login");
        loginWindow->color(FL_BLACK);

        // Heading
        Fl_Box* heading = new Fl_Box(0, 40, loginWindow->w(), 50, "Login");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->labelfont(FL_BOLD);
        heading->align(FL_ALIGN_CENTER);

        // Calculate common dimensions for alignment
        int elementWidth = 250;
        int elementHeight = 30;
        int labelWidth = 150;
        int labelHeight = 30;
        int spacing = 50;
        int centerX = loginWindow->w() / 2 - elementWidth / 2; // Horizontal centering
        int startY = loginWindow->h() / 3; // Start Y position for elements

        // Username
        Fl_Box* usernameLabel = new Fl_Box(centerX - labelWidth - 10, startY, labelWidth, labelHeight, "Username:");
        usernameLabel->labelcolor(FL_WHITE);
        usernameLabel->align(FL_ALIGN_CENTER);
        Fl_Input* usernameInput = new Fl_Input(centerX, startY, elementWidth, elementHeight);

        // Password
        Fl_Box* passwordLabel = new Fl_Box(centerX - labelWidth - 10, startY + spacing, labelWidth, labelHeight, "Password:");
        passwordLabel->labelcolor(FL_WHITE);
        passwordLabel->align(FL_ALIGN_CENTER);
        Fl_Input* passwordInput = new Fl_Input(centerX, startY + spacing, elementWidth, elementHeight);
        passwordInput->type(FL_SECRET_INPUT);

        // Show Password Check
        Fl_Check_Button* showPasswordCheck = new Fl_Check_Button(centerX, startY + 2 * spacing, elementWidth, elementHeight, "Show Password");
        showPasswordCheck->labelcolor(FL_WHITE);
        showPasswordCheck->callback([](Fl_Widget*, void* data) {
            ((login*)data)->togglePassword();
        }, this);

        // Login Button
        Fl_Button* loginButton = new Fl_Button(centerX - 20, startY + 3 * spacing, 120, 35, "Login");
        loginButton->color(FL_BLUE);
        loginButton->labelcolor(FL_WHITE);
        loginButton->callback([](Fl_Widget*, void* data) {
            ((login*)data)->validateLogin();
        }, this);

        // Sign Up Button
        Fl_Button* signupButton = new Fl_Button(centerX + 140, startY + 3 * spacing, 120, 35, "Sign Up");
        signupButton->color(FL_BLUE);
        signupButton->labelcolor(FL_WHITE);
        signupButton->callback([](Fl_Widget*, void* data) {
            ((login*)data)->gosignUpPage();
        }, this);

        loginWindow->end();
        loginWindow->show();

        return Fl::run();
    }

    void gosignUpPage() {
        if (loginWindow) loginWindow->hide();
        
        signup* s = new signup();
        s->signUpPage();
    }

    void validateLogin() {
        string user = usernameInput->value();
        string pass = passwordInput->value();

        // 1. Check for empty fields
        if (user.empty() || pass.empty()) {
            fl_alert("Username and Password cannot be empty!");
            return;
        }

        // 2. Check if username is reserved
        if (user == "admin") {
            fl_alert("Username 'admin' is reserved.");
            return;
        }

        // 3. Simulate successful login
        fl_message("Login Successful! Welcome, %s!", user.c_str());

        // 4. Clear input fields
        usernameInput->value("");
        passwordInput->value("");
        showPasswordCheck->value(0);
        togglePassword(); // not togglePasswordVisibility

        Fl::add_timeout(1.5, [](void*){
            Fl::delete_widget(Fl::first_window());
        }, nullptr);
    }   

    void togglePassword() {
        passwordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
        Fl::redraw();
    }
};


void signup::gologinPage() {
        if (Fl::first_window()) Fl::first_window()->hide();

        login* log = new login();  // use pointer
        log->loginPage();
    }

int main() {
    login log;
    log.loginPage();
    return Fl::run();
}   