#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>

void onClick(Fl_Widget* w, void*) {
    w->label("Clicked!");
}

int main() {
    Fl_Window* window = new Fl_Window(300, 200, "FLTK Example");
    Fl_Button* btn = new Fl_Button(100, 80, 100, 30, "Click Me");
    btn->callback(onClick);
    window->end();
    window->show();
    return Fl::run();
}