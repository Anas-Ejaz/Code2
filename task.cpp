#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Group.H>
#include <FL/fl_ask.H>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdio>

#ifdef _WIN32
#include <strings.h>
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

using namespace std;

// Structs
struct Product { char name[50]; int price; };
struct CartItem { char name[50]; int price; };

// Globals
Fl_Window *mainWindow, *signInWindow, *adminWindow, *customerWindow;
Fl_Input *firstNameInput, *lastNameInput, *usernameInput, *passwordInput, *confirmPasswordInput, *phoneInput, *emailInput, *addressInput;
Fl_Check_Button* showPasswordCheck;
Fl_Box* messageBox;
Fl_Input *prodNameInput, *prodPriceInput, *returnProductNameInput;
Fl_Box* returnMessageBox;
Fl_Scroll *adminProductScroll, *customerProductScroll;
Fl_Group *adminProductScrollContentGroup, *customerProductScrollContentGroup;
Fl_Multiline_Output *prodManageOutputContent, *productOutput;

vector<Product> allProducts;
vector<CartItem> cartItems;
int totalPrice = 0;
int totalSales = 0;
const int MAX_CART_ITEMS = 100;

// Forward Declarations
void showAdminPanel(); void showCustomerPanel(); void updateProductOutput();
void updateCustomerProductDisplay(); void updateProductList();

void togglePasswordVisibility(Fl_Widget*, void*) {
    passwordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
    confirmPasswordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
    Fl::redraw();
}

void updateProductOutput() {
    totalPrice = 0;
    char output[1000] = "Products in Cart:\n\n";
    for (const auto& item : cartItems) {
        char line[100]; snprintf(line, sizeof(line), "%s - Rs.%d\n", item.name, item.price);
        strcat(output, line); totalPrice += item.price;
    }
    char totalLine[100]; snprintf(totalLine, sizeof(totalLine), "\nTotal: Rs.%d", totalPrice);
    strcat(output, totalLine);
    if (productOutput) productOutput->value(output);
}

void updateProductList() {
    char buffer[4000] = "Available Products:\n\n";
    for (size_t i = 0; i < allProducts.size(); i++) {
        char line[150]; snprintf(line, sizeof(line), "%zu. %s - Rs.%d\n", i+1, allProducts[i].name, allProducts[i].price);
        strcat(buffer, line);
    }
    if (prodManageOutputContent) {
        prodManageOutputContent->value(buffer);
        int num_lines = 0;
        for (int k = 0; buffer[k] != '\0'; ++k) if (buffer[k] == '\n') num_lines++;
        int height = (num_lines+2) * prodManageOutputContent->textsize();
        prodManageOutputContent->size(prodManageOutputContent->w(), height);
        adminProductScrollContentGroup->size(adminProductScrollContentGroup->w(), height);
        adminProductScroll->scroll_to(0, 0);
        adminProductScroll->redraw();
        adminProductScrollContentGroup->redraw();
    }
    updateCustomerProductDisplay();
}

void addProductToCart(Fl_Widget*, void* data) {
    int index = reinterpret_cast<intptr_t>(data);
    if (index >= 0 && index < allProducts.size() && cartItems.size() < MAX_CART_ITEMS) {
        CartItem item; strncpy(item.name, allProducts[index].name, 49); item.name[49] = '\0';
        item.price = allProducts[index].price;
        cartItems.push_back(item); totalSales += item.price;
        updateProductOutput(); fl_message("Added to cart");
    }
}

void deleteLastProduct(Fl_Widget*, void*) {
    if (!cartItems.empty()) {
        totalSales -= cartItems.back().price; cartItems.pop_back();
        updateProductOutput();
    }
}

void checkoutCart(Fl_Widget*, void*) {
    if (cartItems.empty()) { fl_alert("Cart empty!"); return; }
    char msg[200]; snprintf(msg, sizeof(msg), "Thank you! Total: Rs.%d", totalPrice);
    fl_message(msg); cartItems.clear(); totalPrice = 0; updateProductOutput();
}

void updateCustomerProductDisplay() {
    if (!customerProductScrollContentGroup || !customerProductScroll) return;
    customerProductScrollContentGroup->clear();
    customerProductScrollContentGroup->begin();
    int y = 10;
    for (size_t i = 0; i < allProducts.size(); i++) {
        char label[100]; snprintf(label, 100, "%s (Rs.%d)", allProducts[i].name, allProducts[i].price);
        Fl_Button* b = new Fl_Button(10, y, customerProductScroll->w() - 40, 30);
        b->label(label); b->callback(addProductToCart, reinterpret_cast<void*>(i));
        b->box(FL_UP_BOX); y += 40;
    }
    customerProductScrollContentGroup->end();
    customerProductScrollContentGroup->size(customerProductScrollContentGroup->w(), max(y, customerProductScroll->h()));
    customerProductScroll->scroll_to(0, 0);
    customerProductScroll->redraw();
}

void showAdminPanel() {
    adminWindow = new Fl_Window(500, 300, "Admin Panel");
    Fl_Box* label = new Fl_Box(20, 40, 460, 40, "Admin Panel Loaded Successfully");
    label->labelfont(FL_BOLD); label->labelsize(16); label->align(FL_ALIGN_CENTER);
    adminWindow->end(); adminWindow->show();
}

void showCustomerPanel() {
    customerWindow = new Fl_Window(500, 300, "Customer Panel");
    Fl_Box* label = new Fl_Box(20, 40, 460, 40, "Customer Panel Loaded Successfully");
    label->labelfont(FL_BOLD); label->labelsize(16); label->align(FL_ALIGN_CENTER);
    customerWindow->end(); customerWindow->show();
}

int main() {
    allProducts.push_back({"Milk", 150});
    allProducts.push_back({"Bread", 100});
    allProducts.push_back({"Rice", 200});
    allProducts.push_back({"Apples", 50});
    allProducts.push_back({"Oranges", 70});

    signInWindow = new Fl_Window(300, 200, "Sign In");
    Fl_Input* user = new Fl_Input(100, 30, 150, 30, "Username:");
    Fl_Input* pass = new Fl_Input(100, 70, 150, 30, "Password:");
    pass->type(FL_SECRET_INPUT);
    Fl_Button* loginBtn = new Fl_Button(100, 120, 100, 30, "Login");
    loginBtn->callback([](Fl_Widget*, void*) {
        if (strcmp(user->value(), "admin") == 0 && strcmp(pass->value(), "admin123") == 0) {
            signInWindow->hide(); showAdminPanel();
        } else fl_alert("Invalid credentials");
    });
    signInWindow->end();
    signInWindow->show();
    return Fl::run();
}
