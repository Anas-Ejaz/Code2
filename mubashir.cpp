#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_ask.H>
#include <cstring>
#include "ui_sign_up.h"
#include "ui_admin_customer.h"

Fl_Window* mainWindow;
Fl_Input *usernameInput, *passwordInput, *phoneInput, *emailInput;
Fl_Box* messageBox;
Fl_Check_Button* showPasswordCheck;

void togglePasswordVisibility(Fl_Widget*, void*) {
    passwordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
    Fl::redraw();
}

void validateSignUp(Fl_Widget*, void*) {
    const char* username = usernameInput->value();
    const char* password = passwordInput->value();
    const char* phone = phoneInput->value();
    const char* email = emailInput->value();

    if (strlen(username) == 0 || strlen(password) == 0 || strlen(phone) == 0 || strchr(email, '@') == NULL) {
        messageBox->label("Please fill all fields correctly.");
        return;
    }

    if (strcmp(username, "e-mart") != 0) messageBox->label("Incorrect username!");
    else if (strcmp(password, "12345") != 0) messageBox->label("Incorrect password!");
    else if (strcmp(phone, "123456789") != 0) messageBox->label("Incorrect phone!");
    else if (strcmp(email, "emart@gmail.com") != 0) messageBox->label("Incorrect email!");
    else {
        fl_message("Sign Up successful! Welcome to E-Mart!");
        mainWindow->hide();
        showAdminPanel();
    }
}

void createSignUpUI() {
    mainWindow = new Fl_Window(750, 480, "E-Mart Sign Up");

    Fl_PNG_Image* logo = new Fl_PNG_Image("login.png");
    Fl_Box* logoBox = new Fl_Box(160, 5, 120, 60);
    if (!logo->fail()) logoBox->image(logo);
    else fl_alert("Image failed to load!");

    new Fl_Box(50, 30, 100, 30, "Username:");
    usernameInput = new Fl_Input(160, 30, 250, 30);

    new Fl_Box(50, 80, 100, 30, "Password:");
    passwordInput = new Fl_Input(160, 80, 250, 30);
    passwordInput->type(FL_SECRET_INPUT);

    showPasswordCheck = new Fl_Check_Button(160, 115, 150, 25, "Show/Hide Password");
    showPasswordCheck->callback(togglePasswordVisibility);

    new Fl_Box(50, 150, 100, 30, "Phone:");
    phoneInput = new Fl_Input(160, 150, 250, 30);

    new Fl_Box(50, 200, 100, 30, "Email:");
    emailInput = new Fl_Input(160, 200, 250, 30);

    Fl_Button* signupButton = new Fl_Button(160, 250, 120, 35, "Sign Up");
    signupButton->callback(validateSignUp);
    signupButton->color(FL_GREEN);
    signupButton->labelsize(14);

    messageBox = new Fl_Box(50, 300, 400, 30, "");

    mainWindow->end();
    mainWindow->show();
}

// ================== ui_admin_customer.cpp ==================
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Multiline_Output.H>
#include <cstring>
#include "ui_admin_customer.h"

struct Product {
    char name[50];
    int price;
};

const int MAX_PRODUCTS = 100;
Product cart[MAX_PRODUCTS];
int cartCount = 0;
int totalPrice = 0;

Fl_Window* adminWindow;
Fl_Window* customerWindow;
Fl_Multiline_Output* productOutput;

void updateProductOutput() {
    totalPrice = 0;
    char output[1000] = "\xF0\x9F\x9B\x92 Products in Cart:\n\n";

    for (int i = 0; i < cartCount; i++) {
        char line[100];
        sprintf(line, "%s - Rs.%d\n", cart[i].name, cart[i].price);
        strcat(output, line);
        totalPrice += cart[i].price;
    }

    char totalLine[100];
    sprintf(totalLine, "\nTotal: Rs.%d", totalPrice);
    strcat(output, totalLine);

    productOutput->value(output);
}

void addProduct(const char* name, int price) {
    if (cartCount < MAX_PRODUCTS) {
        strcpy(cart[cartCount].name, name);
        cart[cartCount].price = price;
        cartCount++;
        updateProductOutput();
    }
}

void addMilk(Fl_Widget*, void*) { addProduct("Milk", 150); }
void addBread(Fl_Widget*, void*) { addProduct("Bread", 100); }
void addRice(Fl_Widget*, void*) { addProduct("Rice", 200); }

void deleteLastProduct(Fl_Widget*, void*) {
    if (cartCount > 0) {
        cartCount--;
        updateProductOutput();
    }
}

void backToAdmin(Fl_Widget*, void*) {
    customerWindow->hide();
    adminWindow->show();
}

void showCustomerPanel() {
    customerWindow = new Fl_Window(500, 420, "Customer Panel");

    Fl_Box* title = new Fl_Box(20, 10, 300, 30, "\xF0\x9F\x9B\x8D\xEF\xB8\x8F Buy Products:");
    title->labelsize(16);
    title->labelfont(FL_BOLD);

    Fl_Button* milkBtn = new Fl_Button(20, 50, 100, 30, "Milk");
    Fl_Button* breadBtn = new Fl_Button(130, 50, 100, 30, "Bread");
    Fl_Button* riceBtn = new Fl_Button(240, 50, 100, 30, "Rice");
    Fl_Button* deleteBtn = new Fl_Button(350, 50, 120, 30, "Delete Last");

    milkBtn->callback(addMilk);
    breadBtn->callback(addBread);
    riceBtn->callback(addRice);
    deleteBtn->callback(deleteLastProduct);

    productOutput = new Fl_Multiline_Output(20, 100, 450, 230, "Cart:");
    productOutput->textsize(14);
    productOutput->labelsize(14);
    productOutput->box(FL_DOWN_BOX);

    Fl_Button* backBtn = new Fl_Button(200, 350, 100, 30, "Back");
    backBtn->callback(backToAdmin);
    backBtn->color(FL_YELLOW);

    customerWindow->end();
    customerWindow->show();
}

void showAdminPanel() {
    adminWindow = new Fl_Window(400, 250, "Admin Panel");

    Fl_Box* heading = new Fl_Box(50, 30, 300, 30, "Welcome to the Admin Panel!");
    heading->labelsize(16);
    heading->labelfont(FL_BOLD);

    Fl_Button* customerBtn = new Fl_Button(120, 100, 160, 35, "Customer Panel");
    Fl_Button* exitBtn = new Fl_Button(120, 150, 160, 30, "Exit");

    customerBtn->color(FL_BLUE);
    customerBtn->labelcolor(FL_WHITE);
    customerBtn->callback([](Fl_Widget*, void*) {
        adminWindow->hide();
        showCustomerPanel();
    });

    exitBtn->color(FL_RED);
    exitBtn->labelcolor(FL_WHITE);
    exitBtn->callback([](Fl_Widget*, void*) { exit(0); });

    adminWindow->end();
    adminWindow->show();
}



