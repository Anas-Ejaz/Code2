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
#include <cstring>
#include <vector>
#include <algorithm> // For std::max
#include <cstdio>    // For snprintf
#include <string>    // For std::string to manage labels dynamically

// For case-insensitive string comparison (Windows specific, or use custom for portable)
#ifdef _WIN32
#include <strings.h> // For strcasecmp on some compilers, or use _stricmp
#define strcasecmp _stricmp // Map to _stricmp for MSVC/MinGW
#else
#include <strings.h> // For strcasecmp on Linux/macOS
#endif

using namespace std;

// Global UI Elements
// Declare all global window pointers and input/output widgets here.
Fl_Input *firstNameInput, *lastNameInput, *usernameInput, *passwordInput, *confirmPasswordInput, *phoneInput, *emailInput, *addressInput;
Fl_Box* messageBox;
Fl_Check_Button* showPasswordCheck;
Fl_Window *mainWindow, *adminWindow, *customerWindow, *productManageWindow, *adminSignInWindow, *customerSignInWindow, *loginChoiceWindow, *buyProductsWindow, *returnProductWindow; // Added customerSignInWindow and loginChoiceWindow
Fl_Multiline_Output* productOutput; // For customer cart display in Buy Products Panel
Fl_Scroll* customerProductScroll; // Global pointer to the scroll area for customer products
Fl_Group* customerProductScrollContentGroup; // Group to hold dynamic product buttons for scrolling

// Product Management Globals
struct Product {
    char name[50];
    int price;
};
vector<Product> allProducts; // Stores all available products

// Cart Globals
const int MAX_CART_ITEMS = 100;
struct CartItem {
    char name[50];
    int price;
};
vector<CartItem> cartItems; // Using vector for cart items for easier management
int totalPrice = 0;
int totalSales = 0; // Cumulative sales across all customer sessions

// Admin Product Management UI
Fl_Input* prodNameInput;
Fl_Input* prodPriceInput;
Fl_Scroll* adminProductScroll; // Scroll area for admin product list
Fl_Multiline_Output* prodManageOutputContent; // The actual multiline output inside the scroll
Fl_Group* adminProductScrollContentGroup; // Group to hold multiline output for scrolling

// Return Product UI
Fl_Input* returnProductNameInput;
Fl_Box* returnMessageBox;

// Forward declarations of all major panel display functions.
void showCustomerPanel();
void updateCustomerProductDisplay();
void showBuyProductsPanel();
void showReturnProductPanel();
void showAdminSignInPanel(); // Renamed from showSignInPanel
void showCustomerSignInPanel(); // New: for customer specific login
void showLoginChoicePanel();   // New: for choosing between customer and admin login
void showAdminPanel();
void showProductManager(Fl_Widget*, void*);
void showSignUpPanel(); 
void validateSignUp(Fl_Widget*, void*); 

// --- Utility Functions ---

// Callback to toggle password visibility for sign-up fields
void togglePasswordVisibility(Fl_Widget*, void*) {
    passwordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
    confirmPasswordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
    Fl::redraw(); // Redraw to apply the type change immediately
}

// Updates the display of products in the customer's cart and the total price.
void updateProductOutput() {
    totalPrice = 0;
    string output_str = "Products in Cart:\n\n"; // Use std::string for easier concatenation

    for (const auto& item : cartItems) {
        char line[100];
        // Use snprintf for safer buffer handling
        snprintf(line, sizeof(line), "%s - Rs.%d\n", item.name, item.price);
        output_str += line;
        totalPrice += item.price;
    }

    char totalLine[100];
    snprintf(totalLine, sizeof(totalLine), "\nTotal: Rs.%d", totalPrice);
    output_str += totalLine;

    productOutput->value(output_str.c_str());
}

// Updates the list of products displayed in the admin's product management panel.
void updateProductList() {
    string buffer_str = "Available Products:\n\n"; // Use std::string for easier concatenation
    for (size_t i = 0; i < allProducts.size(); i++) {
        char line[150];
        snprintf(line, sizeof(line), "%zu. %s - Rs.%d\n", i + 1, allProducts[i].name, allProducts[i].price);
        buffer_str += line;
    }

    if (prodManageOutputContent && adminProductScroll && adminProductScrollContentGroup) {
        prodManageOutputContent->value(buffer_str.c_str());

        // Calculate height based on actual number of lines and text size
        int num_lines = 0;
        const char* current = buffer_str.c_str();
        while (*current != '\0') {
            if (*current == '\n') {
                num_lines++;
            }
            current++;
        }
        // Ensure at least 1 line for an empty list, plus some padding
        int text_height = max(1, num_lines + 2) * prodManageOutputContent->textsize(); 
        
        // Ensure the content group is at least the height of the scroll area
        // to prevent scrollbar flicker if content is slightly less than scroll area.
        int required_group_height = max(adminProductScroll->h(), text_height);

        // Update the height of the content group, but keep its width consistent
        adminProductScrollContentGroup->size(adminProductScrollContentGroup->w(), required_group_height);
        
        // Update the multiline output's size to fill its parent group's dimensions
        prodManageOutputContent->size(adminProductScrollContentGroup->w(), adminProductScrollContentGroup->h());

        adminProductScrollContentGroup->redraw();
        adminProductScroll->redraw();
        adminProductScroll->scroll_to(0,0); // Reset scroll position to top
    }
    updateCustomerProductDisplay(); // Ensure customer product display is also updated
}

// --- Admin Panel Callbacks ---

void addProduct(Fl_Widget*, void*) {
    const char* name = prodNameInput->value();
    int price = atoi(prodPriceInput->value());

    if (name[0] == '\0' || price <= 0) {
        fl_alert("Please enter a valid product name and a positive price.");
        return;
    }

    for (const auto& prod : allProducts) {
        if (strcasecmp(prod.name, name) == 0) {
            fl_alert("Product with this name already exists. Consider changing its price instead.");
            return;
        }
    }

    Product newProduct;
    strncpy(newProduct.name, name, sizeof(newProduct.name) - 1);
    newProduct.name[sizeof(newProduct.name) - 1] = '\0';
    newProduct.price = price;
    allProducts.push_back(newProduct);

    prodNameInput->value("");
    prodPriceInput->value("");
    updateProductList();
    fl_message("Product added successfully!");
}

void changeProductPrice(Fl_Widget*, void*) {
    const char* productName = prodNameInput->value();
    int newPrice = atoi(prodPriceInput->value());

    if (strlen(productName) == 0 || newPrice <= 0) {
        fl_alert("Please enter a valid product name and a positive new price.");
        return;
    }

    bool found = false;
    for (auto& prod : allProducts) {
        if (strcasecmp(prod.name, productName) == 0) {
            prod.price = newPrice;
            found = true;
            break;
        }
    }

    if (found) {
        fl_message("Product price updated successfully!");
        prodNameInput->value("");
        prodPriceInput->value("");
        updateProductList();
    } else {
        fl_alert("Product not found. Please enter an exact product name from the list.");
    }
}

void checkSales(Fl_Widget*, void*) {
    char msg[100];
    snprintf(msg, sizeof(msg), "Total Sales: Rs.%d", totalSales);
    fl_message(msg);
}

// --- Panel Display Functions (Admin) ---

void showProductManager(Fl_Widget*, void*) {
    if (productManageWindow == nullptr) {
        productManageWindow = new Fl_Window(500, 400, "Manage Products");

        prodNameInput = new Fl_Input(150, 20, 200, 30, "Product Name:");
        prodPriceInput = new Fl_Input(150, 60, 200, 30, "Price:");

        Fl_Button* addBtn = new Fl_Button(100, 100, 120, 30, "Add Product");
        addBtn->color(FL_BLACK); addBtn->labelcolor(FL_WHITE); // Styling
        addBtn->callback(addProduct);

        Fl_Button* changeBtn = new Fl_Button(240, 100, 140, 30, "Change Price");
        changeBtn->color(FL_BLACK); changeBtn->labelcolor(FL_WHITE); // Styling
        changeBtn->callback(changeProductPrice);

        adminProductScroll = new Fl_Scroll(20, 150, 460, 200);
        adminProductScroll->box(FL_THIN_DOWN_BOX);
        adminProductScroll->type(FL_VERTICAL);
        adminProductScroll->begin();
            adminProductScrollContentGroup = new Fl_Group(0, 0, adminProductScroll->w() - Fl::scrollbar_size(), adminProductScroll->h());
            adminProductScrollContentGroup->box(FL_NO_BOX);
            adminProductScrollContentGroup->begin(); // Inner begin
                prodManageOutputContent = new Fl_Multiline_Output(0, 0, adminProductScrollContentGroup->w(), adminProductScrollContentGroup->h());
                prodManageOutputContent->box(FL_NO_BOX);
                prodManageOutputContent->textsize(14);
            adminProductScrollContentGroup->end(); // Inner end
            adminProductScrollContentGroup->resizable(prodManageOutputContent); 
        adminProductScroll->end(); // Outer end
        adminProductScroll->resizable(adminProductScrollContentGroup); 

        Fl_Button* backBtn = new Fl_Button(180, 360, 120, 30, "Back");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE); // Styling
        backBtn->callback([](Fl_Widget*, void*) {
            if (productManageWindow) productManageWindow->hide();
            showAdminPanel();
        });

        productManageWindow->end();
    }
    updateProductList();
    productManageWindow->show();
}

// --- Customer Panel Callbacks & Logic ---

void addProductToCart(Fl_Widget* w, void* data) {
    int productIndex = reinterpret_cast<intptr_t>(data);

    if (productIndex >= 0 && productIndex < allProducts.size()) {
        if (cartItems.size() < MAX_CART_ITEMS) {
            CartItem newItem;
            strncpy(newItem.name, allProducts[productIndex].name, sizeof(newItem.name) - 1);
            newItem.name[sizeof(newItem.name) - 1] = '\0';
            newItem.price = allProducts[productIndex].price;
            cartItems.push_back(newItem);
            totalSales += allProducts[productIndex].price;
            updateProductOutput();
            fl_message("Product added to cart!");
        } else {
            fl_alert("Your cart is full! Please proceed to checkout or remove items.");
        }
    }
}

void deleteLastProduct(Fl_Widget*, void*) {
    if (!cartItems.empty()) {
        totalSales -= cartItems.back().price;
        cartItems.pop_back();
        updateProductOutput();
        fl_message("Last item removed from cart!");
    } else {
        fl_message("Cart is already empty!");
    }
}

void processReturnProduct(Fl_Widget*, void*) {
    const char* productName = returnProductNameInput->value();
    if (strlen(productName) == 0) {
        returnMessageBox->labelcolor(FL_RED);
        returnMessageBox->label("Please enter a product name to return.");
        return;
    }

    bool found = false;
    for (size_t i = 0; i < cartItems.size(); ++i) {
        if (strcasecmp(cartItems[i].name, productName) == 0) {
            totalSales -= cartItems[i].price;
            cartItems.erase(cartItems.begin() + i);
            found = true;
            returnMessageBox->labelcolor(FL_DARK_GREEN);
            returnMessageBox->label("Product returned successfully!");
            break;
        }
    }

    if (!found) {
        returnMessageBox->labelcolor(FL_RED);
        returnMessageBox->label("Product not found in cart!");
    }
    returnProductNameInput->value("");
    updateProductOutput();
    returnProductWindow->redraw();
}


// --- Panel Display Functions (Customer) ---

void updateCustomerProductDisplay() {
    if (!customerProductScroll || !customerProductScrollContentGroup) return;

    // Remove all children before adding new ones
    customerProductScrollContentGroup->clear();
    customerProductScrollContentGroup->begin();

    int y_offset = 10;
    const int button_height = 30;
    const int button_spacing = 10;
    
    // Calculate button width based on the content group's width
    const int button_width = customerProductScrollContentGroup->w() - 20; // 10px padding on each side

    // Ensure the content group's width is set correctly before adding buttons
    customerProductScrollContentGroup->size(customerProductScroll->w() - Fl::scrollbar_size(), customerProductScrollContentGroup->h());


    for (size_t i = 0; i < allProducts.size(); i++) {
        std::string buttonLabel = allProducts[i].name;
        buttonLabel += " (Rs.";
        buttonLabel += std::to_string(allProducts[i].price);
        buttonLabel += ")";

        Fl_Button* prodBtn = new Fl_Button(10, y_offset, button_width, button_height);
        prodBtn->label(buttonLabel.c_str()); // FLTK copies this string internally
        prodBtn->callback(addProductToCart, reinterpret_cast<void*>(i));
        prodBtn->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
        prodBtn->box(FL_UP_BOX);
        prodBtn->color(FL_BLACK); prodBtn->labelcolor(FL_WHITE); // Styling

        y_offset += (button_height + button_spacing);
    }
    customerProductScrollContentGroup->end();

    // Calculate the necessary height for the contained widgets
    int content_height = y_offset + 10; // Add 10px bottom margin
    
    // Ensure content_height is at least the scroll area's visible height
    int min_scroll_height = customerProductScroll->h();
    customerProductScrollContentGroup->size(customerProductScrollContentGroup->w(), max(min_scroll_height, content_height));
    
    // Reset scroll position to top.
    customerProductScroll->scroll_to(0, 0);
    
    customerProductScrollContentGroup->redraw();
    customerProductScroll->redraw();
}


void showBuyProductsPanel() {
    if (buyProductsWindow == nullptr) {
        buyProductsWindow = new Fl_Window(500, 520, "Buy Products");

        new Fl_Box(20, 10, 300, 30, "Available Products:");

        customerProductScroll = new Fl_Scroll(20, 50, 250, 200, "");
        customerProductScroll->box(FL_THIN_DOWN_BOX);
        customerProductScroll->align(FL_ALIGN_TOP_LEFT);
        customerProductScroll->type(FL_VERTICAL);
        customerProductScroll->begin();
            customerProductScrollContentGroup = new Fl_Group(0, 0, customerProductScroll->w() - Fl::scrollbar_size(), customerProductScroll->h());
            customerProductScrollContentGroup->box(FL_NO_BOX);
            customerProductScrollContentGroup->end(); // This closes the group
        customerProductScroll->end();
        customerProductScroll->resizable(customerProductScrollContentGroup); // This is the correct resizable for the scroll's content.

        productOutput = new Fl_Multiline_Output(20, 260, 460, 180, "Products in Cart:");
        productOutput->textsize(14);
        productOutput->labelsize(14);
        productOutput->box(FL_DOWN_BOX);

        Fl_Button* deleteLastBtn = new Fl_Button(350, 450, 120, 30, "Delete Last");
        deleteLastBtn->color(FL_BLACK); deleteLastBtn->labelcolor(FL_WHITE); // Styling
        deleteLastBtn->callback(deleteLastProduct);

        Fl_Button* backBtn = new Fl_Button(200, 480, 100, 30, "Back");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE); // Styling
        backBtn->callback([](Fl_Widget*, void*) {
            if (buyProductsWindow) buyProductsWindow->hide();
            showCustomerPanel();
        });

        buyProductsWindow->end();
    }
    updateCustomerProductDisplay();
    updateProductOutput();
    buyProductsWindow->show();
}

void showReturnProductPanel() {
    if (returnProductWindow == nullptr) {
        returnProductWindow = new Fl_Window(400, 200, "Return Product");

        returnProductNameInput = new Fl_Input(150, 40, 200, 30, "Product Name:");

        Fl_Button* returnBtn = new Fl_Button(150, 90, 100, 30, "Return");
        returnBtn->color(FL_BLACK); returnBtn->labelcolor(FL_WHITE); // Styling
        returnBtn->callback(processReturnProduct);

        returnMessageBox = new Fl_Box(20, 140, 360, 30, "");
        returnMessageBox->labelcolor(FL_RED);

        Fl_Button* backBtn = new Fl_Button(150, 170, 100, 30, "Back");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE); // Styling
        backBtn->callback([](Fl_Widget*, void*) {
            if (returnProductWindow) returnProductWindow->hide();
            showCustomerPanel();
        });

        returnProductWindow->end();
    }
    returnMessageBox->label("");
    returnProductNameInput->value("");
    returnProductWindow->show();
}

void showCustomerPanel() {
    if (customerWindow == nullptr) {
        customerWindow = new Fl_Window(400, 300, "Customer Panel");

        new Fl_Box(50, 30, 300, 30, "Welcome to the Customer Panel!");

        Fl_Button* buyBtn = new Fl_Button(120, 80, 160, 35, "Buy Products");
        buyBtn->color(FL_BLACK); buyBtn->labelcolor(FL_WHITE); // Styling
        buyBtn->callback([](Fl_Widget*, void*) {
            if (customerWindow) customerWindow->hide();
            showBuyProductsPanel();
        });

        Fl_Button* returnBtn = new Fl_Button(120, 130, 160, 35, "Return Product");
        returnBtn->color(FL_BLACK); returnBtn->labelcolor(FL_WHITE); // Styling
        returnBtn->callback([](Fl_Widget*, void*) {
            if (customerWindow) customerWindow->hide();
            showReturnProductPanel();
        });

        Fl_Button* signOutBtn = new Fl_Button(120, 180, 160, 35, "Sign Out"); 
        signOutBtn->color(FL_BLACK); signOutBtn->labelcolor(FL_WHITE); // Styling
        signOutBtn->callback([](Fl_Widget*, void*) {
            if (customerWindow) customerWindow->hide();
            showLoginChoicePanel(); // Go back to the login choice panel on sign out
        });

        customerWindow->end();
    }
    customerWindow->show();
}

void showAdminPanel() {
    if (adminWindow == nullptr) {
        adminWindow = new Fl_Window(400, 300, "Admin Panel");

        new Fl_Box(50, 30, 300, 30, "Welcome to the Admin Panel!");

        // REMOVED: Customer Panel button from Admin Panel
        // Fl_Button* customerBtn = new Fl_Button(120, 80, 160, 35, "Customer Panel");
        // customerBtn->color(FL_BLACK); customerBtn->labelcolor(FL_WHITE); // Styling
        // customerBtn->callback([](Fl_Widget*, void*) {
        //     if (adminWindow) adminWindow->hide();
        //     showCustomerPanel(); 
        // });

        Fl_Button* manageBtn = new Fl_Button(120, 80, 160, 35, "Manage Products"); // Adjusted Y-position
        manageBtn->color(FL_BLACK); manageBtn->labelcolor(FL_WHITE); // Styling
        manageBtn->callback([](Fl_Widget*, void*) {
            if (adminWindow) adminWindow->hide();
            showProductManager(nullptr, nullptr);
        });

        Fl_Button* salesBtn = new Fl_Button(120, 130, 160, 35, "Check Sales"); // Adjusted Y-position
        salesBtn->color(FL_BLACK); salesBtn->labelcolor(FL_WHITE); // Styling
        salesBtn->callback(checkSales);

        Fl_Button* signOutBtn = new Fl_Button(120, 180, 160, 35, "Sign Out"); // Adjusted Y-position
        signOutBtn->color(FL_BLACK); signOutBtn->labelcolor(FL_WHITE); // Styling
        signOutBtn->callback([](Fl_Widget*, void*) {
            if (adminWindow) adminWindow->hide();
            showLoginChoicePanel(); // Go back to the login choice panel on sign out
        });

        adminWindow->end();
    }
    adminWindow->show();
}

// --- NEW PANEL: Login Choice ---
void showLoginChoicePanel() {
    if (loginChoiceWindow == nullptr) {
        loginChoiceWindow = new Fl_Window(350, 200, "Login Choice");
        loginChoiceWindow->color(FL_DARK_CYAN); // Add some styling

        new Fl_Box(50, 30, 250, 30, "Please choose your login type:");

        Fl_Button* customerLoginBtn = new Fl_Button(100, 80, 150, 35, "Customer Login");
        customerLoginBtn->color(FL_DARK_BLUE); customerLoginBtn->labelcolor(FL_WHITE);
        customerLoginBtn->callback([](Fl_Widget*, void*) {
            if (loginChoiceWindow) loginChoiceWindow->hide();
            showCustomerSignInPanel(); // Go to the dedicated customer sign-in
        });

        Fl_Button* adminLoginBtn = new Fl_Button(100, 130, 150, 35, "Admin Login");
        adminLoginBtn->color(FL_DARK_RED); adminLoginBtn->labelcolor(FL_WHITE);
        adminLoginBtn->callback([](Fl_Widget*, void*) {
            if (loginChoiceWindow) loginChoiceWindow->hide();
            showAdminSignInPanel(); // Go to the admin sign-in panel
        });

        loginChoiceWindow->end();
    }
    loginChoiceWindow->show();
}


// --- NEW PANEL: Customer Sign In ---
void validateCustomerSignIn(Fl_Widget* w, void* data) {
    // Assuming the first child is username and second is password for customerSignInWindow
    Fl_Input* user = static_cast<Fl_Input*>(customerSignInWindow->child(0));
    Fl_Input* pass = static_cast<Fl_Input*>(customerSignInWindow->child(1));

    // For demonstration, any non-empty username/password is "valid" for customer
    // In a real app, you'd check against stored user data.
    if (strlen(user->value()) > 0 && strlen(pass->value()) > 0) {
        if (customerSignInWindow) customerSignInWindow->hide();
        showCustomerPanel();
    } else {
        fl_alert("Please enter both username and password.");
    }
    user->value(""); // Clear fields after attempt
    pass->value("");
}

void showCustomerSignInPanel() {
    if (customerSignInWindow == nullptr) {
        customerSignInWindow = new Fl_Window(300, 250, "Customer Sign In");

        Fl_Input* user = new Fl_Input(100, 30, 150, 30, "Username:");
        Fl_Input* pass = new Fl_Input(100, 70, 150, 30, "Password:");
        pass->type(FL_SECRET_INPUT);

        Fl_Button* loginBtn = new Fl_Button(100, 120, 100, 30, "Login");
        loginBtn->color(FL_BLACK); loginBtn->labelcolor(FL_WHITE);
        loginBtn->callback(validateCustomerSignIn);

        Fl_Button* backBtn = new Fl_Button(80, 170, 140, 30, "Back to Choice");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE);
        backBtn->callback([](Fl_Widget*, void*) {
            if (customerSignInWindow) customerSignInWindow->hide();
            showLoginChoicePanel();
        });

        customerSignInWindow->end();
    }
    // Clear fields every time the sign-in panel is shown
    Fl_Input* usernameField = static_cast<Fl_Input*>(customerSignInWindow->child(0));
    if (usernameField) {
        usernameField->value("");
    }

    Fl_Input* passwordField = static_cast<Fl_Input*>(customerSignInWindow->child(1));
    if (passwordField) {
        passwordField->value("");
    }
    customerSignInWindow->show();
}


// --- Admin Sign In (Previously showSignInPanel) ---
void validateAdminSignIn(Fl_Widget* w, void*) { // Renamed callback
    // Assuming the first child is username and second is password for adminSignInWindow
    Fl_Input* user = static_cast<Fl_Input*>(adminSignInWindow->child(0));
    Fl_Input* pass = static_cast<Fl_Input*>(adminSignInWindow->child(1));

    if (strcmp("admin", user->value()) == 0 && strcmp("admin123", pass->value()) == 0) {
        if (adminSignInWindow) adminSignInWindow->hide();
        showAdminPanel();
    } else {
        fl_alert("Invalid admin username or password!");
    }
    user->value(""); // Clear fields after attempt
    pass->value("");
}

void showAdminSignInPanel() { // Renamed from showSignInPanel
    if (adminSignInWindow == nullptr) {
        adminSignInWindow = new Fl_Window(300, 250, "Admin Sign In"); // Increased height to accommodate new button

        Fl_Input* user = new Fl_Input(100, 30, 150, 30, "Username:");
        Fl_Input* pass = new Fl_Input(100, 70, 150, 30, "Password:");
        pass->type(FL_SECRET_INPUT);

        Fl_Button* loginBtn = new Fl_Button(100, 120, 100, 30, "Login");
        loginBtn->color(FL_BLACK); loginBtn->labelcolor(FL_WHITE); // Styling
        loginBtn->callback(validateAdminSignIn); // Changed callback

        // New "Back to Login Choice" button
        Fl_Button* backToChoiceBtn = new Fl_Button(80, 170, 140, 30, "Back to Choice");
        backToChoiceBtn->color(FL_BLACK); backToChoiceBtn->labelcolor(FL_WHITE); // Styling
        backToChoiceBtn->callback([](Fl_Widget*, void*) {
            if (adminSignInWindow) adminSignInWindow->hide();
            showLoginChoicePanel(); // Go back to the initial sign-up window
        });

        adminSignInWindow->end();
    }
    Fl_Input* usernameField = static_cast<Fl_Input*>(adminSignInWindow->child(0));
    if (usernameField) {
        usernameField->value("");
    }

    Fl_Input* passwordField = static_cast<Fl_Input*>(adminSignInWindow->child(1));
    if (passwordField) {
        passwordField->value("");
    }

    adminSignInWindow->show();
}

// DEFINITION FOR THE MISSING VALIDATE SIGN UP FUNCTION
void validateSignUp(Fl_Widget*, void*) {
    const char* first = firstNameInput->value();
    const char* last = lastNameInput->value();
    const char* user = usernameInput->value();
    const char* pass = passwordInput->value();
    const char* confirmPass = confirmPasswordInput->value();
    const char* phone = phoneInput->value();
    const char* email = emailInput->value();
    const char* address = addressInput->value();

    // Basic validation example
    if (strlen(first) == 0 || strlen(last) == 0 || strlen(user) == 0 ||
        strlen(pass) == 0 || strlen(confirmPass) == 0 || strlen(phone) == 0 ||
        strlen(email) == 0 || strlen(address) == 0) {
        messageBox->labelcolor(FL_RED);
        messageBox->label("All fields are required!");
        return;
    }

    if (strcmp(pass, confirmPass) != 0) {
        messageBox->labelcolor(FL_RED);
        messageBox->label("Passwords do not match!");
        passwordInput->value("");
        confirmPasswordInput->value("");
        return;
    }

    // A simple validation for username existence (in a real app, this would check a database)
    // For this example, we prevent "admin" from being registered as a regular user.
    if (strcmp(user, "admin") == 0) { 
        messageBox->labelcolor(FL_RED);
        messageBox->label("Username 'admin' is reserved.");
        return;
    }

    // If validation passes
    messageBox->labelcolor(FL_DARK_GREEN);
    messageBox->label("Sign Up Successful! You can now choose to sign in.");

    // In a real application, you would save this user data (e.g., to a file or database)
    // For now, we'll just clear the fields and switch to the sign-in choice panel after a delay.
    firstNameInput->value("");
    lastNameInput->value("");
    usernameInput->value("");
    passwordInput->value("");
    confirmPasswordInput->value("");
    phoneInput->value("");
    emailInput->value("");
    addressInput->value("");
    showPasswordCheck->value(0); // Uncheck "Show Password"
    togglePasswordVisibility(nullptr, nullptr); // Reset password fields type

    // After successful sign-up, switch to the sign-in choice panel after a short delay
    Fl::add_timeout(1.5, [](void*){
        if (mainWindow) mainWindow->hide();
        showLoginChoicePanel(); // Go to the new login choice panel
    }, nullptr);
}


// Renamed from main window setup to be a callable panel display function
void showSignUpPanel() {
    if (mainWindow == nullptr) { // Only create if it doesn't exist
        mainWindow = new Fl_Window(750, 550, "E-Mart Sign Up");

        // The image files appear to be named 'image_XXXXXX.png' based on your provided context.
        // Assuming 'logi.png' is intended to be one of these or a different logo.
        // If not, replace "logi.png" with the correct image file name.
        Fl_PNG_Image* logo = new Fl_PNG_Image("logi.png"); 
        Fl_Box* logoBox = new Fl_Box(175, 5, 120, 60);
        logoBox->image(logo);
        if (logo->fail()) fl_alert("Image 'logi.png' failed to load! Make sure it's in the same directory as the executable.");

        new Fl_Box(50, 30, 100, 30, "First Name:");
        firstNameInput = new Fl_Input(160, 30, 250, 30);

        new Fl_Box(50, 70, 100, 30, "Last Name:");
        lastNameInput = new Fl_Input(160, 70, 250, 30);

        new Fl_Box(50, 110, 100, 30, "Username:");
        usernameInput = new Fl_Input(160, 110, 250, 30);

        new Fl_Box(50, 150, 100, 30, "Password:");
        passwordInput = new Fl_Input(160, 150, 250, 30);
        passwordInput->type(FL_SECRET_INPUT);

        new Fl_Box(50, 190, 100, 30, "Confirm Password:");
        confirmPasswordInput = new Fl_Input(160, 190, 250, 30);
        confirmPasswordInput->type(FL_SECRET_INPUT);

        showPasswordCheck = new Fl_Check_Button(160, 230, 150, 25, "Show Password");
        showPasswordCheck->callback(togglePasswordVisibility);

        new Fl_Box(50, 270, 100, 30, "Phone:");
        phoneInput = new Fl_Input(160, 270, 250, 30);

        new Fl_Box(50, 310, 100, 30, "Email:");
        emailInput = new Fl_Input(160, 310, 250, 30);

        new Fl_Box(50, 350, 100, 30, "Address:");
        addressInput = new Fl_Input(160, 350, 250, 30);

        Fl_Button* signupButton = new Fl_Button(160, 400, 120, 35, "Sign Up");
        signupButton->color(FL_BLACK); signupButton->labelcolor(FL_WHITE); // Styling
        signupButton->callback(validateSignUp); 

        messageBox = new Fl_Box(50, 450, 400, 30, "");
        messageBox->labelcolor(FL_RED);

        mainWindow->end();
    }
    mainWindow->show();
}


int main() {
    allProducts.push_back({"Milk", 150});
    allProducts.push_back({"Bread", 100});
    allProducts.push_back({"Rice", 200});
    allProducts.push_back({"Apples", 50});
    allProducts.push_back({"Oranges", 70});
    allProducts.push_back({"Bananas", 30});
    allProducts.push_back({"Chocolate", 250});
    allProducts.push_back({"Coffee", 300});
    allProducts.push_back({"Tea", 120});
    allProducts.push_back({"Sugar", 80});
    allProducts.push_back({"Eggs", 120});
    allProducts.push_back({"Flour", 90});
    allProducts.push_back({"Potatoes", 60});
    allProducts.push_back({"Tomatoes", 40});
    allProducts.push_back({"Onions", 35});
    allProducts.push_back({"Chicken", 400});
    allProducts.push_back({"Fish", 350});
    allProducts.push_back({"Cabbage", 30});
    allProducts.push_back({"Carrots", 25});
    allProducts.push_back({"Grapes", 180});
    allProducts.push_back({"Pears", 90});
    allProducts.push_back({"Mangoes", 150}); 
    allProducts.push_back({"Pineapple", 220});
    allProducts.push_back({"Watermelon", 100});
    allProducts.push_back({"Cucumbers", 45});
    allProducts.push_back({"Spinach", 55});


    // Start by showing the sign-up panel
    showSignUpPanel();
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
