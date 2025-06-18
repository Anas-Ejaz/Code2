#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_ask.H>

#include <iostream>
#include <cstring> // for strcmp, strcat, sprintf

using namespace std;

// UI elements
Fl_Input *usernameInput, *passwordInput, *phoneInput, *emailInput;
Fl_Box* messageBox;
Fl_Check_Button* showPasswordCheck;
Fl_Window *mainWindow, *adminWindow, *customerWindow;
Fl_Multiline_Output* productOutput;

const int MAX_PRODUCTS = 100;
char cartItems[MAX_PRODUCTS][50];
int cartPrices[MAX_PRODUCTS];
int cartCount = 0;
int totalPrice = 0;


void togglePasswordVisibility(Fl_Widget*, void*) {
    passwordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
    Fl::redraw();
}

// Update cart output
void updateProductOutput() {
    totalPrice = 0;
    char output[1000] = "Products in Cart:\n\n";

    for (int i = 0; i < cartCount; i++) {
        char line[100];
        sprintf(line, "%s - Rs.%d\n", cartItems[i], cartPrices[i]);
        strcat(output, line);
        totalPrice += cartPrices[i];
    }

    char totalLine[100];
    sprintf(totalLine, "\nTotal: Rs.%d", totalPrice);
    strcat(output, totalLine);

    productOutput->value(output);
}


void addMilk(Fl_Widget*, void*) {
    if (cartCount < MAX_PRODUCTS) {
        strcpy(cartItems[cartCount], "Milk");
        cartPrices[cartCount++] = 150;
        updateProductOutput();
    }
}

void addBread(Fl_Widget*, void*) {
    if (cartCount < MAX_PRODUCTS) {
        strcpy(cartItems[cartCount], "Bread");
        cartPrices[cartCount++] = 100;
        updateProductOutput();
    }
}

void addRice(Fl_Widget*, void*) {
    if (cartCount < MAX_PRODUCTS) {
        strcpy(cartItems[cartCount], "Rice");
        cartPrices[cartCount++] = 200;
        updateProductOutput();
    }
}

// Delete last product
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

// Customer panel
void showCustomerPanel() {
    customerWindow = new Fl_Window(500, 420, "Customer Panel");

    new Fl_Box(20, 10, 300, 30, "Buy Products:");

    Fl_Button* milkBtn = new Fl_Button(20, 50, 100, 30, "Milk");
    milkBtn->callback(addMilk);

    Fl_Button* breadBtn = new Fl_Button(130, 50, 100, 30, "Bread");
    breadBtn->callback(addBread);

    Fl_Button* riceBtn = new Fl_Button(240, 50, 100, 30, "Rice");
    riceBtn->callback(addRice);

    Fl_Button* deleteBtn = new Fl_Button(350, 50, 120, 30, "Delete Last");
    deleteBtn->callback(deleteLastProduct);

    productOutput = new Fl_Multiline_Output(20, 100, 450, 230, "Cart:");
    productOutput->textsize(14);
    productOutput->labelsize(14);
    productOutput->box(FL_DOWN_BOX);

    Fl_Button* backBtn = new Fl_Button(200, 350, 100, 30, "Back");
    backBtn->callback(backToAdmin);

    customerWindow->end();
    customerWindow->show();
}

// Admin panel
void showAdminPanel() {
    adminWindow = new Fl_Window(400, 250, "Admin Panel");

    new Fl_Box(50, 30, 300, 30, "Welcome to the Admin Panel!");

    Fl_Button* customerBtn = new Fl_Button(120, 100, 160, 35, "Customer Panel");
    customerBtn->callback([](Fl_Widget*, void*) {
        adminWindow->hide();
        showCustomerPanel();
    });

    Fl_Button* exitBtn = new Fl_Button(120, 150, 160, 30, "Exit");
    exitBtn->callback([](Fl_Widget*, void*) { exit(0); });

    adminWindow->end();
    adminWindow->show();
}


void validateSignUp(Fl_Widget*, void*) {
    const char* username = usernameInput->value();
    const char* password = passwordInput->value();
    const char* phone = phoneInput->value();
    const char* email = emailInput->value();

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


int main() {
    mainWindow = new Fl_Window(750, 480, "E-Mart Sign Up");

    Fl_PNG_Image* logo = new Fl_PNG_Image("login.png");
    Fl_Box* logoBox = new Fl_Box(175, 5, 120, 60);
    logoBox->image(logo);
    if (logo->fail()) fl_alert("Image failed to load!");

    Fl_Box* userLabel = new Fl_Box(50, 30, 100, 30, "Username:");
    usernameInput = new Fl_Input(160, 30, 250, 30);

    Fl_Box* passLabel = new Fl_Box(50, 80, 100, 30, "Password:");
    passwordInput = new Fl_Input(160, 80, 250, 30);
    passwordInput->type(FL_SECRET_INPUT);

    showPasswordCheck = new Fl_Check_Button(160, 115, 150, 25, "Show Password");
    showPasswordCheck->callback(togglePasswordVisibility);

    Fl_Box* phoneLabel = new Fl_Box(50, 150, 100, 30, "Phone:");
    phoneInput = new Fl_Input(160, 150, 250, 30);

    Fl_Box* emailLabel = new Fl_Box(50, 200, 100, 30, "Email:");
    emailInput = new Fl_Input(160, 200, 250, 30);

    Fl_Button* signupButton = new Fl_Button(160, 250, 120, 35, "Sign Up");
    signupButton->callback(validateSignUp);

    messageBox = new Fl_Box(50, 300, 400, 30, "");

    mainWindow->end();
    mainWindow->show();

    return Fl::run();
}




// #include <FL/Fl.H>
// #include <FL/Fl_Window.H>
// #include <FL/Fl_Box.H>

// int main() {
//     Fl_Window window(300, 200, "FLTK Test");
//     Fl_Box box(20, 40, 260, 100, "anas bkl !");
//     window.end();
//     window.show();
//     return Fl::run();
// }   



// #include <iostream>
// #include <string>

// using namespace std;

// const int MAX_USERS = 100;

// struct User {
//     string username;
//     string password;
//     string email;
//     int phoneno;
// };

// User users[MAX_USERS];
// int userCount = 0;


// bool userExists(const string& username) {
//     for (int i = 0; i < userCount; i++) {
//         if (users[i].username == username) {
//             return true;
//         }
//     }
//     return false;
// }

// bool emailExists(const string& email){
//     for(int i = 0; i<userCount; i++) {
//         if(users[i].email == email){
//             return true;
//         }
//     }
//     return false;
// }


// void signup() {
//     if (userCount >= MAX_USERS) {
//         cout << "User limit reached! \n";
//         return;
//     }

//     string username, password,email;
//     int phoneno;
//     cout << "--- Signup Page --- \n";

//     do{
//     cout << "Enter username: ";
//     cin >> username;
    

//     if (userExists(username)) {
//         cout << "Username already exists. Please enter a different username.\n";
//     }

//     } while(userExists(username));
    
//     do{
        
//     cout<<"Enter an email: ";
//     cin>> email;

//     if(emailExists(email)){
//         cout<< "Email already exists. Please Try another."<<endl;
//     }

//     } while(emailExists(email));
        
//     cout << "Enter password: ";
//     cin >> password;

//     cout<<"Enter the phone number to add in this email: ";
//     cin >> phoneno;

//     users[userCount].username = username;
//     users[userCount].password = password;
//     users[userCount].email = email;
//     users[userCount].phoneno = phoneno;
//     userCount++;

//     cout << "Signup successful!\n";
// }

// int main() {
//     char choice;
//     do {
//         signup();

//         cout << "Do you want to add another user? (y/n): "<<endl;
//         cin >> choice;

//     } while (choice == 'y' || choice == 'Y');

//     return 0;
// }
