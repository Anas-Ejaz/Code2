#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_ask.H>

class signup {
public:
    int signUpPage(){
        Fl_Window* mainWindow = new Fl_Window(750, 480, "E-Mart Sign Up");

        Fl_PNG_Image* logo = new Fl_PNG_Image("login.png");
        Fl_Box* logoBox = new Fl_Box(175, 5, 120, 60);
        logoBox->image(logo);

        Fl_Box* userLabel = new Fl_Box(50, 30, 100, 30, "Username:");
        Fl_Input* usernameInput = new Fl_Input(160, 30, 250, 30);

        Fl_Box* passLabel = new Fl_Box(50, 80, 100, 30, "Password:");
        Fl_Input* passwordInput = new Fl_Input(160, 80, 250, 30);
        passwordInput->type(FL_SECRET_INPUT);

        Fl_Check_Button* showPasswordCheck = new Fl_Check_Button(160, 115, 150, 25, "Show Password");
        
        Fl_Box* phoneLabel = new Fl_Box(50, 150, 100, 30, "Phone:");
        Fl_Input* phoneInput = new Fl_Input(160, 150, 250, 30);

        Fl_Box* addressLabel = new Fl_Box(50, 200, 100, 30, "Address:");
        Fl_Input* addressInput = new Fl_Input(160, 200, 250, 30);

        Fl_Button* signupButton = new Fl_Button(160, 250, 120, 35, "Sign Up");
        signupButton->callback(validateSignUp);
        
        Fl_Box* messageBox = new Fl_Box(50, 300, 400, 30, "");
        
        mainWindow->end();
        mainWindow->show();
        
        showPasswordCheck->callback({
            passwordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
            Fl::redraw();
        });
        return Fl::run();
    }

    void togglePasswordVisibility(Fl_Widget*, void*, Fl_Input*, Fl_Check_Button*) {
        
    }

};
