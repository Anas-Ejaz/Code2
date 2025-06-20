#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Scroll.H>
#include <FL/fl_ask.H>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm> // For std::remove_if

// For case-insensitive string comparison (Windows specific, or use custom for portable)
#ifdef _WIN32
#include <strings.h> // For strcasecmp on some compilers, or use _stricmp
#define strcasecmp _stricmp
#else
#include <strings.h> // For strcasecmp on Linux/macOS
#endif

using namespace std;

// Global UI Elements
Fl_Input *firstNameInput, *lastNameInput, *usernameInput, *passwordInput, *confirmPasswordInput, *phoneInput, *emailInput, *addressInput;
Fl_Box* messageBox;
Fl_Check_Button* showPasswordCheck;
Fl_Window *mainWindow, *adminWindow, *customerWindow, *productManageWindow, *signInWindow, *buyProductsWindow, *returnProductWindow; // Added new windows
Fl_Multiline_Output* productOutput; // For customer cart display in Buy Products Panel
Fl_Scroll* customerProductScroll; // Global pointer to the scroll area for customer products

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
Fl_Multiline_Output* prodManageOutput; // For admin product list display

// Return Product UI
Fl_Input* returnProductNameInput;
Fl_Box* returnMessageBox;

// Forward declarations
void showCustomerPanel();
void updateCustomerProductDisplay();
void showBuyProductsPanel(); // New panel function
void showReturnProductPanel(); // New panel function
void showSignInPanel(); // Forward declaration for sign in panel
// Note: showAdminPanel and showProductManager forward declarations are now implicit due to reordering

// --- Utility Functions ---

// Callback to toggle password visibility
void togglePasswordVisibility(Fl_Widget*, void*) {
    passwordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
    confirmPasswordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
    Fl::redraw();
}

// Update the customer's cart display
void updateProductOutput() {
    totalPrice = 0;
    char output[1000] = "Products in Cart:\n\n";

    for (const auto& item : cartItems) {
        char line[100];
        sprintf(line, "%s - Rs.%d\n", item.name, item.price);
        strcat(output, line);
        totalPrice += item.price;
    }

    char totalLine[100];
    sprintf(totalLine, "\nTotal: Rs.%d", totalPrice);
    strcat(output, totalLine);

    productOutput->value(output);
}

// Update the admin's product management list
void updateProductList() {
    char buffer[1000] = "Available Products:\n\n";
    for (size_t i = 0; i < allProducts.size(); i++) {
        char line[100];
        sprintf(line, "%zu. %s - Rs.%d\n", i + 1, allProducts[i].name, allProducts[i].price);
        strcat(buffer, line);
    }
    prodManageOutput->value(buffer);
    updateCustomerProductDisplay(); // Update customer display whenever admin products change
}

// --- Admin Panel Callbacks ---

// Callback to add a new product (Admin)
void addProduct(Fl_Widget*, void*) {
    const char* name = prodNameInput->value();
    int price = atoi(prodPriceInput->value());

    if (name[0] == '\0' || price <= 0) {
        fl_alert("Please enter a valid product name and a positive price.");
        return;
    }

    // Check for duplicate product names (case-insensitive for simplicity)
    for (const auto& prod : allProducts) {
        if (strcasecmp(prod.name, name) == 0) {
            fl_alert("Product with this name already exists. Consider changing its price instead.");
            return;
        }
    }

    Product newProduct;
    strcpy(newProduct.name, name);
    newProduct.price = price;
    allProducts.push_back(newProduct);

    prodNameInput->value(""); // Clear input fields
    prodPriceInput->value("");
    updateProductList();
    fl_message("Product added successfully!");
}

// Callback to change an existing product's price (Admin)
void changeProductPrice(Fl_Widget*, void*) {
    int index = atoi(prodNameInput->value()) - 1; // Get index from name input field
    int newPrice = atoi(prodPriceInput->value());

    if (index >= 0 && index < allProducts.size() && newPrice > 0) {
        allProducts[index].price = newPrice;
        fl_message("Product price updated successfully!");
        prodNameInput->value(""); // Clear input fields
        prodPriceInput->value("");
        updateProductList();
    } else {
        fl_alert("Invalid product index or new price. Enter the product number from the list and a positive new price.");
    }
}

// Callback to check total sales (Admin)
void checkSales(Fl_Widget*, void*) {
    char msg[100];
    sprintf(msg, "Total Sales: Rs.%d", totalSales);
    fl_message(msg);
}

// Show the product management window (Admin) - DEFINED HERE FIRST
void showProductManager(Fl_Widget*, void*) {
    if (productManageWindow) { // If window already exists, just show it
        productManageWindow->show();
        updateProductList(); // Ensure list is up to date
        return;
    }
    productManageWindow = new Fl_Window(500, 400, "Manage Products");

    prodNameInput = new Fl_Input(150, 20, 200, 30, "Product Name / Index:");
    prodPriceInput = new Fl_Input(150, 60, 200, 30, "Price:");

    Fl_Button* addBtn = new Fl_Button(100, 100, 120, 30, "Add Product");
    addBtn->callback(addProduct);

    Fl_Button* changeBtn = new Fl_Button(240, 100, 140, 30, "Change Price");
    changeBtn->callback(changeProductPrice);

    prodManageOutput = new Fl_Multiline_Output(20, 150, 460, 200);
    updateProductList(); // Initial display of products

    Fl_Button* backBtn = new Fl_Button(180, 360, 120, 30, "Back");
    backBtn->callback([](Fl_Widget*, void*) {
        productManageWindow->hide();
        adminWindow->show(); // Needs adminWindow to be defined or forward declared
    });

    productManageWindow->end();
    productManageWindow->show();
}


// --- Customer Panel Callbacks & Logic ---

// Callback for adding a product to cart from customer panel
void addProductToCart(Fl_Widget* w, void* data) {
    int productIndex = reinterpret_cast<intptr_t>(data); // Retrieve the index passed as data

    if (productIndex >= 0 && productIndex < allProducts.size()) {
        if (cartItems.size() < MAX_CART_ITEMS) {
            CartItem newItem;
            strcpy(newItem.name, allProducts[productIndex].name);
            newItem.price = allProducts[productIndex].price;
            cartItems.push_back(newItem);
            totalSales += allProducts[productIndex].price; // Accumulate sales
            updateProductOutput();
            fl_message("Product added to cart!");
        } else {
            fl_alert("Your cart is full!");
        }
    }
}

// Callback to delete the last product from the cart
void deleteLastProduct(Fl_Widget*, void*) {
    if (!cartItems.empty()) {
        totalSales -= cartItems.back().price; // Deduct from total sales
        cartItems.pop_back(); // Remove the last item
        updateProductOutput();
        fl_message("Last item removed from cart!");
    } else {
        fl_message("Cart is already empty!");
    }
}

// Callback for the Return Product button in the Return Product panel
void processReturnProduct(Fl_Widget*, void*) {
    const char* productName = returnProductNameInput->value();
    if (strlen(productName) == 0) {
        returnMessageBox->labelcolor(FL_RED);
        returnMessageBox->label("Please enter a product name to return.");
        return;
    }

    bool found = false;
    // Iterate through the cart to find and remove the product
    // Note: Using a manual loop here as std::remove_if requires more complex predicate for char arrays
    for (size_t i = 0; i < cartItems.size(); ++i) {
        if (strcasecmp(cartItems[i].name, productName) == 0) {
            totalSales -= cartItems[i].price; // Deduct from total sales
            cartItems.erase(cartItems.begin() + i); // Remove the item
            found = true;
            returnMessageBox->labelcolor(FL_DARK_GREEN);
            returnMessageBox->label("Product returned successfully!");
            break; // Stop after returning one instance
        }
    }

    if (!found) {
        returnMessageBox->labelcolor(FL_RED);
        returnMessageBox->label("Product not found in cart!");
    }
    returnProductNameInput->value(""); // Clear input
    updateProductOutput(); // Refresh cart display
    returnProductWindow->redraw(); // Redraw the return window to show message
}


// --- Panel Navigation Callbacks ---

// Generic back callback for simple window transitions
void backTo(Fl_Window* currentWindow, Fl_Window* targetWindow) {
    currentWindow->hide();
    targetWindow->show();
}

// --- Window Definitions ---

// Function to update dynamic product buttons in customer's "Buy Products" panel
void updateCustomerProductDisplay() {
    if (!customerProductScroll) return; // Ensure the scroll widget exists

    customerProductScroll->clear(); // Clear existing widgets in the scroll area

    int y_offset = 10; // Starting Y position within the scroll group
    const int button_height = 30;
    const int button_spacing = 10;
    const int button_width = 120; // Example width

    customerProductScroll->begin(); // Begin adding widgets to the scroll group

    for (size_t i = 0; i < allProducts.size(); i++) {
        char buttonLabel[100];
        sprintf(buttonLabel, "%s (Rs.%d)", allProducts[i].name, allProducts[i].price);

        Fl_Button* prodBtn = new Fl_Button(10, y_offset, button_width, button_height, buttonLabel);
        prodBtn->callback(addProductToCart, reinterpret_cast<void*>(i)); // Pass product index
        customerProductScroll->add(prodBtn); // Add button to the scroll area

        y_offset += (button_height + button_spacing);
    }
    customerProductScroll->end(); // End adding widgets to the scroll group
    customerProductScroll->redraw(); // Redraw the scroll area to show new buttons
}

// Show the Buy Products Panel
void showBuyProductsPanel() {
    if (buyProductsWindow) {
        buyProductsWindow->show();
        updateCustomerProductDisplay();
        updateProductOutput(); // Ensure cart is updated on show
        return;
    }

    buyProductsWindow = new Fl_Window(500, 470, "Buy Products"); // Increased height to accommodate all elements

    new Fl_Box(20, 10, 300, 30, "Available Products:");

    customerProductScroll = new Fl_Scroll(20, 50, 450, 180, "Scrollable Products:");
    customerProductScroll->box(FL_THIN_DOWN_BOX);
    customerProductScroll->align(FL_ALIGN_TOP_LEFT);
    customerProductScroll->user_data((void*)"productScroll");
    customerProductScroll->end(); // Important: end the group for the scroll widget

    Fl_Button* deleteLastBtn = new Fl_Button(350, 240, 120, 30, "Delete Last");
    deleteLastBtn->callback(deleteLastProduct);

    productOutput = new Fl_Multiline_Output(20, 280, 450, 100, "Your Cart:");
    productOutput->textsize(14);
    productOutput->labelsize(14);
    productOutput->box(FL_DOWN_BOX);
    updateProductOutput(); // Initial update for cart

    Fl_Button* backBtn = new Fl_Button(200, 430, 100, 30, "Back"); // Adjusted position
    backBtn->callback([](Fl_Widget*, void*) {
        buyProductsWindow->hide(); // Hide current window
        customerWindow->show(); // Show target window
    });

    buyProductsWindow->end();
    buyProductsWindow->show();
    updateCustomerProductDisplay(); // Initial population of product buttons
}

// Show the Return Product Panel
void showReturnProductPanel() {
    if (returnProductWindow) {
        returnProductWindow->show();
        returnMessageBox->label(""); // Clear previous messages
        returnProductNameInput->value(""); // Clear input
        return;
    }

    returnProductWindow = new Fl_Window(400, 200, "Return Product");

    returnProductNameInput = new Fl_Input(150, 40, 200, 30, "Product Name:");

    Fl_Button* returnBtn = new Fl_Button(150, 90, 100, 30, "Return");
    returnBtn->callback(processReturnProduct);

    returnMessageBox = new Fl_Box(20, 140, 360, 30, "");
    returnMessageBox->labelcolor(FL_RED);

    Fl_Button* backBtn = new Fl_Button(150, 170, 100, 30, "Back");
    backBtn->callback([](Fl_Widget*, void*) {
        returnProductWindow->hide(); // Hide current window
        customerWindow->show(); // Show target window
    });

    returnProductWindow->end();
    returnProductWindow->show();
}

// Show the customer panel (hub)
void showCustomerPanel() {
    if (customerWindow) {
        customerWindow->show();
        return;
    }

    customerWindow = new Fl_Window(400, 300, "Customer Panel");

    new Fl_Box(50, 30, 300, 30, "Welcome to the Customer Panel!");

    Fl_Button* buyBtn = new Fl_Button(120, 80, 160, 35, "Buy Products");
    buyBtn->callback([](Fl_Widget*, void*) {
        customerWindow->hide(); // Hide current window
        showBuyProductsPanel(); // Show target window
    });

    Fl_Button* returnBtn = new Fl_Button(120, 130, 160, 35, "Return Product");
    returnBtn->callback([](Fl_Widget*, void*) {
        customerWindow->hide(); // Hide current window
        showReturnProductPanel(); // Show target window
    });

    Fl_Button* backBtn = new Fl_Button(120, 180, 160, 35, "Back to Admin");
    backBtn->callback([](Fl_Widget*, void*) {
        customerWindow->hide(); // Hide current window
        adminWindow->show(); // Show target window
    });

    customerWindow->end();
    customerWindow->show();
}


// Show the admin panel - DEFINED HERE AFTER showProductManager
void showAdminPanel() {
    if (adminWindow) { // If window already exists, just show it
        adminWindow->show();
        return;
    }
    adminWindow = new Fl_Window(400, 300, "Admin Panel");

    new Fl_Box(50, 30, 300, 30, "Welcome to the Admin Panel!");

    Fl_Button* customerBtn = new Fl_Button(120, 80, 160, 35, "Customer Panel");
    customerBtn->callback([](Fl_Widget*, void*) {
        adminWindow->hide(); // Hide current window
        showCustomerPanel(); // Show target window
    });

    Fl_Button* manageBtn = new Fl_Button(120, 130, 160, 35, "Manage Products");
    manageBtn->callback([](Fl_Widget*, void*) {
        adminWindow->hide(); // Hide current window
        showProductManager(nullptr, nullptr); // Call with null args as it's a direct show
    });

    Fl_Button* salesBtn = new Fl_Button(120, 180, 160, 35, "Check Sales");
    salesBtn->callback(checkSales);

    Fl_Button* backToSignInBtn = new Fl_Button(120, 230, 160, 35, "Sign Out");
    backToSignInBtn->callback([](Fl_Widget*, void*) {
        adminWindow->hide(); // Hide current window
        signInWindow->show(); // Show target window
    });


    adminWindow->end();
    adminWindow->show();
}

// Validate Sign-in credentials
void validateSignIn(Fl_Widget*, void*) {
    static Fl_Input* user = (Fl_Input*)signInWindow->child(0);
    static Fl_Input* pass = (Fl_Input*)signInWindow->child(1);

    if (strcmp("admin", user->value()) == 0 && strcmp("admin123", pass->value()) == 0) {
        signInWindow->hide();
        showAdminPanel();
    } else {
        fl_alert("Invalid login!");
    }
}

// Show the sign-in panel
void showSignInPanel() {
    if (signInWindow) {
        signInWindow->show();
        return;
    }
    signInWindow = new Fl_Window(300, 200, "Sign In");

    Fl_Input* user = new Fl_Input(100, 30, 150, 30, "Username:");
    Fl_Input* pass = new Fl_Input(100, 70, 150, 30, "Password:");
    pass->type(FL_SECRET_INPUT);

    Fl_Button* loginBtn = new Fl_Button(100, 120, 100, 30, "Login");
    loginBtn->callback(validateSignIn);

    signInWindow->end();
    signInWindow->show();
}

// Validate Sign-up details
void validateSignUp(Fl_Widget*, void*) {
    const char* firstName = firstNameInput->value();
    const char* lastName = lastNameInput->value();
    const char* username = usernameInput->value();
    const char* password = passwordInput->value();
    const char* confirmPassword = confirmPasswordInput->value();
    const char* phone = phoneInput->value();
    const char* email = emailInput->value();
    const char* address = addressInput->value();

    if (strlen(firstName) == 0 || strlen(lastName) == 0 || strlen(username) == 0 ||
        strlen(password) == 0 || strlen(confirmPassword) == 0 ||
        strlen(phone) == 0 || strlen(email) == 0 || strlen(address) == 0) {
        messageBox->label("All fields are required!");
        return;
    }

    if (strcmp(password, confirmPassword) != 0) {
        messageBox->label("Passwords do not match!");
        return;
    }

    // If validation passes
    messageBox->label("Account created successfully!");
    fl_message("Sign Up successful! Please sign in.");
    mainWindow->hide();
    showSignInPanel();
}

int main() {
    // Initialize some default products
    allProducts.push_back({"Milk", 150});
    allProducts.push_back({"Bread", 100});
    allProducts.push_back({"Rice", 200});

    mainWindow = new Fl_Window(750, 550, "E-Mart Sign Up");

    Fl_PNG_Image* logo = new Fl_PNG_Image("logi.png");
    Fl_Box* logoBox = new Fl_Box(175, 5, 120, 60);
    logoBox->image(logo);
    if (logo->fail()) fl_alert("Image 'logi.png' failed to load! Make sure it's in the same directory.");

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
    signupButton->callback(validateSignUp);

    messageBox = new Fl_Box(50, 450, 400, 30, "");
    messageBox->labelcolor(FL_RED);

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
