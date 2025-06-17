#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>

class loginPage {
public:
    int login(){
        Fl_Window* window = new Fl_Window(800, 500, "Login Page");

        Fl_PNG_Image* loginImg = new Fl_PNG_Image("login.png");
        Fl_Box* imgBox = new Fl_Box(0, 0, 400, 500);
        imgBox->image(loginImg);

        window->end();
        window->show();
        return Fl::run();
    }
};


void onClick(Fl_Widget* w, void*) {
    w->label("Clicked hey!");
}

int main() {
    loginPage lp;
    lp.login();
    // Fl_Window* window = new Fl_Window(300, 200, "FLTK Example");
    // Fl_Button* btn = new Fl_Button(100, 80, 100, 30, "Click Me");
    // btn->callback(onClick);
    // window->end();
    // window->show();
    // return Fl::run();
}
