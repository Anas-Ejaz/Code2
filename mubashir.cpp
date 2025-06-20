#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Group.H> // Added for better scroll content management
#include <FL/fl_ask.H>
#include <iostream>
#include <cstring>
#include <vector>
#include <algorithm> // For std::remove_if
#include <cstdio>    // For snprintf

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
Fl_Window *mainWindow, *adminWindow, *customerWindow, *productManageWindow, *signInWindow, *buyProductsWindow, *returnProductWindow;
Fl_Multiline_Output* productOutput; // For customer cart display in Buy Products Panel
Fl_Scroll* customerProductScroll; // Global pointer to the scroll area for customer products
Fl_Group* customerProductScrollContentGroup; // New: Group to hold dynamic product buttons for scrolling

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
Fl_Group* adminProductScrollContentGroup; // New: Group to hold multiline output for scrolling

// Return Product UI
Fl_Input* returnProductNameInput;
Fl_Box* returnMessageBox;

// Forward declarations of all major panel display functions.
// This is essential to allow functions to call each other, especially in callbacks.
void showCustomerPanel();
void updateCustomerProductDisplay();
void showBuyProductsPanel();
void showReturnProductPanel();
void showSignInPanel();
void showAdminPanel();
void showProductManager(Fl_Widget*, void*); // Keep forward declaration for function pointer type


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
    char output[1000] = "Products in Cart:\n\n";

    for (const auto& item : cartItems) {
        char line[100];
        // Use snprintf for safer buffer handling
        snprintf(line, sizeof(line), "%s - Rs.%d\n", item.name, item.price);
        strcat(output, line);
        totalPrice += item.price;
    }

    char totalLine[100];
    snprintf(totalLine, sizeof(totalLine), "\nTotal: Rs.%d", totalPrice);
    strcat(output, totalLine);

    productOutput->value(output);
}

// Updates the list of products displayed in the admin's product management panel.
void updateProductList() {
    char buffer[4000] = "Available Products:\n\n"; // Increased buffer size to accommodate more products
    for (size_t i = 0; i < allProducts.size(); i++) {
        char line[150]; // Increased line buffer
        snprintf(line, sizeof(line), "%zu. %s - Rs.%d\n", i + 1, allProducts[i].name, allProducts[i].price);
        strcat(buffer, line);
    }
    if (prodManageOutputContent) { // Ensure the widget exists before updating
        prodManageOutputContent->value(buffer);
        if (adminProductScroll) {
            // Update the size of the content group to match the text height
            // This is key for the scrollbar to appear dynamically
            int text_height = prodManageOutputContent->textsize() * (prodManageOutputContent->value() ? (int)strlen(prodManageOutputContent->value()) / (prodManageOutputContent->w() / prodManageOutputContent->textsize() * 0.6) + 2 : 1) ; // Rough estimate
            if (prodManageOutputContent->value() && strlen(prodManageOutputContent->value()) > 0) {
                 int num_lines = 0;
                 for (int k = 0; buffer[k] != '\0'; ++k) {
                     if (buffer[k] == '\n') {
                         num_lines++;
                     }
                 }
                 // Adjust height to at least fit 1 line, plus extra for potential last line
                 text_height = (num_lines + 2) * prodManageOutputContent->textsize();
            } else {
                text_height = prodManageOutputContent->textsize() * 2; // Default for empty
            }

            prodManageOutputContent->size(prodManageOutputContent->w(), max(adminProductScroll->h() - 20, text_height)); // Ensure content is at least scroll height or fits text

            adminProductScrollContentGroup->redraw(); // Redraw the group holding content
            adminProductScroll->redraw(); // Redraw the scroll
            adminProductScroll->scroll_to(0,0); // Reset scroll position to top
        }
    }
    updateCustomerProductDisplay(); // Ensure customer product display is also updated
}

// --- Admin Panel Callbacks ---

// Callback to add a new product to the global product list.
void addProduct(Fl_Widget*, void*) {
    const char* name = prodNameInput->value();
    int price = atoi(prodPriceInput->value());

    if (name[0] == '\0' || price <= 0) {
        fl_alert("Please enter a valid product name and a positive price.");
        return;
    }

    // Check for duplicate product names (case-insensitive)
    for (const auto& prod : allProducts) {
        if (strcasecmp(prod.name, name) == 0) {
            fl_alert("Product with this name already exists. Consider changing its price instead.");
            return;
        }
    }

    Product newProduct;
    // Safely copy string
    strncpy(newProduct.name, name, sizeof(newProduct.name) - 1);
    newProduct.name[sizeof(newProduct.name) - 1] = '\0'; // Ensure null termination
    newProduct.price = price;
    allProducts.push_back(newProduct);

    prodNameInput->value(""); // Clear input fields after adding
    prodPriceInput->value("");
    updateProductList(); // Refresh the product display
    fl_message("Product added successfully!");
}

// Callback to change the price of an existing product by name.
void changeProductPrice(Fl_Widget*, void*) {
    const char* productName = prodNameInput->value();
    int newPrice = atoi(prodPriceInput->value());

    if (strlen(productName) == 0 || newPrice <= 0) {
        fl_alert("Please enter a valid product name and a positive new price.");
        return;
    }

    bool found = false;
    for (auto& prod : allProducts) { // Use auto& to allow modification
        if (strcasecmp(prod.name, productName) == 0) {
            prod.price = newPrice;
            found = true;
            break; // Found and updated, exit loop
        }
    }

    if (found) {
        fl_message("Product price updated successfully!");
        prodNameInput->value(""); // Clear input fields
        prodPriceInput->value("");
        updateProductList(); // Refresh the product display
    } else {
        fl_alert("Product not found. Please enter an exact product name from the list.");
    }
}

// Callback to display the total sales accumulated.
void checkSales(Fl_Widget*, void*) {
    char msg[100];
    snprintf(msg, sizeof(msg), "Total Sales: Rs.%d", totalSales);
    fl_message(msg);
}

// --- Panel Display Functions (Admin) ---

// Displays or shows the Product Management window for admin users.
void showProductManager(Fl_Widget*, void*) {
    if (productManageWindow == nullptr) { // Create window only if it doesn't exist
        productManageWindow = new Fl_Window(500, 400, "Manage Products");

        prodNameInput = new Fl_Input(150, 20, 200, 30, "Product Name:"); // Changed label
        prodPriceInput = new Fl_Input(150, 60, 200, 30, "Price:");

        Fl_Button* addBtn = new Fl_Button(100, 100, 120, 30, "Add Product");
        addBtn->callback(addProduct);

        Fl_Button* changeBtn = new Fl_Button(240, 100, 140, 30, "Change Price");
        changeBtn->callback(changeProductPrice);

        // Embed prodManageOutputContent within an Fl_Scroll, inside a content group
        adminProductScroll = new Fl_Scroll(20, 150, 460, 200);
        adminProductScroll->box(FL_THIN_DOWN_BOX); // Consistent box type
        adminProductScroll->type(FL_VERTICAL); // Only show vertical scrollbar when needed
        adminProductScroll->begin();
            // Create a group to hold the multiline output, this group will be resized
            adminProductScrollContentGroup = new Fl_Group(0, 0, adminProductScroll->w() - adminProductScroll->scrollbar_size(), adminProductScroll->h());
            adminProductScrollContentGroup->begin();
                // prodManageOutputContent is placed relative to its parent group (0,0)
                prodManageOutputContent = new Fl_Multiline_Output(0, 0, adminProductScrollContentGroup->w(), adminProductScrollContentGroup->h());
                prodManageOutputContent->box(FL_NO_BOX); // No extra box around content
            adminProductScrollContentGroup->end();
            adminProductScrollContentGroup->resizable(prodManageOutputContent); // Make the multiline output the resizable child of the group
        adminProductScroll->end();
        adminProductScroll->resizable(adminProductScrollContentGroup); // Make the content group resizable inside the scroll

        // Back button for Manage Products panel
        Fl_Button* backBtn = new Fl_Button(180, 360, 120, 30, "Back");
        backBtn->callback([](Fl_Widget*, void*) {
            if (productManageWindow) productManageWindow->hide(); // Hide current window safely
            showAdminPanel(); // Call the function to show Admin Panel
        });

        productManageWindow->end();
    }
    updateProductList(); // Always update list when showing
    productManageWindow->show();
}

// --- Customer Panel Callbacks & Logic ---

// Callback for adding a product to the customer's cart.
void addProductToCart(Fl_Widget* w, void* data) {
    int productIndex = reinterpret_cast<intptr_t>(data); // Get product index passed as data

    if (productIndex >= 0 && productIndex < allProducts.size()) {
        if (cartItems.size() < MAX_CART_ITEMS) {
            CartItem newItem;
            strncpy(newItem.name, allProducts[productIndex].name, sizeof(newItem.name) - 1);
            newItem.name[sizeof(newItem.name) - 1] = '\0'; // Ensure null termination
            newItem.price = allProducts[productIndex].price;
            cartItems.push_back(newItem);
            totalSales += allProducts[productIndex].price; // Accumulate sales
            updateProductOutput(); // Refresh cart display
            fl_message("Product added to cart!");
        } else {
            fl_alert("Your cart is full! Please proceed to checkout or remove items.");
        }
    }
}

// Callback to remove the last added product from the cart.
void deleteLastProduct(Fl_Widget*, void*) {
    if (!cartItems.empty()) {
        totalSales -= cartItems.back().price; // Deduct from total sales
        cartItems.pop_back(); // Remove the last item
        updateProductOutput(); // Refresh cart display
        fl_message("Last item removed from cart!");
    } else {
        fl_message("Cart is already empty!");
    }
}

// Callback for processing a product return in the Return Product panel.
void processReturnProduct(Fl_Widget*, void*) {
    const char* productName = returnProductNameInput->value();
    if (strlen(productName) == 0) {
        returnMessageBox->labelcolor(FL_RED);
        returnMessageBox->label("Please enter a product name to return.");
        return;
    }

    bool found = false;
    // Iterate through the cart to find and remove the product (first instance found)
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
    returnProductNameInput->value(""); // Clear input field
    updateProductOutput(); // Refresh cart display (important for consistency)
    returnProductWindow->redraw(); // Redraw the return window to show message updates
}


// --- Panel Display Functions (Customer) ---

// Dynamically creates and updates product buttons in the customer's "Buy Products" scroll area.
void updateCustomerProductDisplay() {
    if (!customerProductScroll || !customerProductScrollContentGroup) return;

    // Clear existing children from the content group
    customerProductScrollContentGroup->clear();
    customerProductScrollContentGroup->begin();

    int y_offset = 10; // Starting Y position for the first button
    const int button_height = 30;
    const int button_spacing = 10;
    const int button_width = customerProductScrollContentGroup->w() - 20; // Adjusted width to fit within scroll group

    for (size_t i = 0; i < allProducts.size(); i++) {
        char* buttonLabel = (char*)malloc(100); // Dynamically allocate for label
        if (!buttonLabel) continue; // Handle malloc failure

        snprintf(buttonLabel, 100, "%s (Rs.%d)", allProducts[i].name, allProducts[i].price);

        // Create button positioned relative to the scroll content group (0,0)
        Fl_Button* prodBtn = new Fl_Button(10, y_offset, button_width, button_height);
        prodBtn->label(buttonLabel); // Set label
        prodBtn->callback(addProductToCart, reinterpret_cast<void*>(i)); // Pass product index to callback
        prodBtn->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP); // Align text left, inside, allow wrapping
        prodBtn->box(FL_UP_BOX); // Give buttons a visible box type

        // Automatically added to customerProductScrollContentGroup by begin()/end()
        // No explicit `add()` needed here for the group.

        y_offset += (button_height + button_spacing); // Move down for the next button
    }
    customerProductScrollContentGroup->end(); // End adding widgets to the content group.

    // Calculate the necessary height for the contained widgets
    int content_height = y_offset; // Total height occupied by buttons + spacing

    // Resize the content group to encompass all its children.
    // This makes the scrollbar aware of the total scrollable area.
    customerProductScrollContentGroup->size(customerProductScrollContentGroup->w(), max(customerProductScroll->h(), content_height));
    
    // Reset scroll position to top.
    customerProductScroll->scroll_to(0, 0);
    
    // Ensure the scroll area redraws to reflect new buttons and scrollbar state
    customerProductScrollContentGroup->redraw();
    customerProductScroll->redraw();
}


// Displays or shows the "Buy Products" window for customers.
void showBuyProductsPanel() {
    if (buyProductsWindow == nullptr) { // Create window only if it doesn't exist
        buyProductsWindow = new Fl_Window(500, 520, "Buy Products"); // Increased window height

        new Fl_Box(20, 10, 300, 30, "Available Products:");

        // customerProductScroll: Removed the redundant label argument.
        // Adjusted width to provide more space and height for more products.
        customerProductScroll = new Fl_Scroll(20, 50, 250, 200, ""); // X, Y, W, H. Removed label.
        customerProductScroll->box(FL_THIN_DOWN_BOX); // Set consistent box type here
        customerProductScroll->align(FL_ALIGN_TOP_LEFT);
        customerProductScroll->type(FL_VERTICAL); // Ensure vertical scrollbar is managed automatically
        customerProductScroll->begin();
            // Create a group to hold the dynamically added product buttons
            customerProductScrollContentGroup = new Fl_Group(0, 0, customerProductScroll->w() - customerProductScroll->scrollbar_size(), customerProductScroll->h());
            customerProductScrollContentGroup->box(FL_NO_BOX); // No visible box for the content group
            customerProductScrollContentGroup->end();
            customerProductScrollContentGroup->resizable(customerProductScrollContentGroup); // Make the content group resizable itself
        customerProductScroll->end();
        customerProductScroll->resizable(customerProductScrollContentGroup); // Make the content group the resizable child of the scroll

        // Moved productOutput (cart) significantly higher and adjusted size
        productOutput = new Fl_Multiline_Output(20, 260, 460, 180, "Products in Cart:"); // Moved up, increased height and width
        productOutput->textsize(14);
        productOutput->labelsize(14);
        productOutput->box(FL_DOWN_BOX);


        Fl_Button* deleteLastBtn = new Fl_Button(350, 450, 120, 30, "Delete Last"); // Adjusted position
        deleteLastBtn->callback(deleteLastProduct);

        Fl_Button* backBtn = new Fl_Button(200, 480, 100, 30, "Back"); // Adjusted position
        backBtn->callback([](Fl_Widget*, void*) {
            if (buyProductsWindow) buyProductsWindow->hide(); // Safely hide current window
            showCustomerPanel(); // Call the function to show Customer Panel
        });

        buyProductsWindow->end();
    }
    updateCustomerProductDisplay(); // Always update product buttons on show
    updateProductOutput(); // Always update cart display on show
    buyProductsWindow->show();
}

// Displays or shows the "Return Product" window for customers.
void showReturnProductPanel() {
    if (returnProductWindow == nullptr) { // Create window only if it doesn't exist
        returnProductWindow = new Fl_Window(400, 200, "Return Product");

        returnProductNameInput = new Fl_Input(150, 40, 200, 30, "Product Name:");

        Fl_Button* returnBtn = new Fl_Button(150, 90, 100, 30, "Return");
        returnBtn->callback(processReturnProduct);

        returnMessageBox = new Fl_Box(20, 140, 360, 30, "");
        returnMessageBox->labelcolor(FL_RED);

        Fl_Button* backBtn = new Fl_Button(150, 170, 100, 30, "Back");
        backBtn->callback([](Fl_Widget*, void*) {
            if (returnProductWindow) returnProductWindow->hide(); // Safely hide current window
            showCustomerPanel(); // Call the function to show Customer Panel
        });

        returnProductWindow->end();
    }
    // Clear message and input when showing the panel
    returnMessageBox->label("");
    returnProductNameInput->value("");
    returnProductWindow->show();
}

// Displays or shows the main Customer Panel.
void showCustomerPanel() {
    if (customerWindow == nullptr) { // Create window only if it doesn't exist
        customerWindow = new Fl_Window(400, 300, "Customer Panel");

        new Fl_Box(50, 30, 300, 30, "Welcome to the Customer Panel!");

        Fl_Button* buyBtn = new Fl_Button(120, 80, 160, 35, "Buy Products");
        buyBtn->callback([](Fl_Widget*, void*) {
            if (customerWindow) customerWindow->hide();
            showBuyProductsPanel(); // Call the function to show Buy Products Panel
        });

        Fl_Button* returnBtn = new Fl_Button(120, 130, 160, 35, "Return Product");
        returnBtn->callback([](Fl_Widget*, void*) {
            if (customerWindow) customerWindow->hide();
            showReturnProductPanel(); // Call the function to show Return Product Panel
        });

        Fl_Button* backBtn = new Fl_Button(120, 180, 160, 35, "Back to Admin");
        backBtn->callback([](Fl_Widget*, void*) {
            if (customerWindow) customerWindow->hide();
            showAdminPanel(); // Call the function to show Admin Panel
        });

        customerWindow->end();
    }
    customerWindow->show();
}


// Displays or shows the main Admin Panel.
void showAdminPanel() {
    if (adminWindow == nullptr) { // Create window only if it doesn't exist
        adminWindow = new Fl_Window(400, 300, "Admin Panel");

        new Fl_Box(50, 30, 300, 30, "Welcome to the Admin Panel!");

        Fl_Button* customerBtn = new Fl_Button(120, 80, 160, 35, "Customer Panel");
        customerBtn->callback([](Fl_Widget*, void*) {
            if (adminWindow) adminWindow->hide();
            showCustomerPanel(); // Call the function to show Customer Panel
        });

        Fl_Button* manageBtn = new Fl_Button(120, 130, 160, 35, "Manage Products");
        manageBtn->callback([](Fl_Widget*, void*) {
            if (adminWindow) adminWindow->hide();
            showProductManager(nullptr, nullptr); // Call with null args as it's a direct show
        });

        Fl_Button* salesBtn = new Fl_Button(120, 180, 160, 35, "Check Sales");
        salesBtn->callback(checkSales);

        Fl_Button* backToSignInBtn = new Fl_Button(120, 230, 160, 35, "Sign Out");
        backToSignInBtn->callback([](Fl_Widget*, void*) {
            if (adminWindow) adminWindow->hide();
            showSignInPanel(); // Call the function to show Sign In Panel
        });

        adminWindow->end();
    }
    adminWindow->show();
}

// Validates sign-in credentials for the admin.
void validateSignIn(Fl_Widget*, void*) {
    // These pointers should be valid as signInWindow would have been created
    // before this callback is ever triggered by a user.
    Fl_Input* user = static_cast<Fl_Input*>(signInWindow->child(0));
    Fl_Input* pass = static_cast<Fl_Input*>(signInWindow->child(1));

    if (strcmp("admin", user->value()) == 0 && strcmp("admin123", pass->value()) == 0) {
        if (signInWindow) signInWindow->hide();
        showAdminPanel(); // Navigate to the admin panel
    } else {
        fl_alert("Invalid username or password!");
    }
}

// Displays or shows the Sign-In window.
void showSignInPanel() {
    if (signInWindow == nullptr) { // Create window only if it doesn't exist
        signInWindow = new Fl_Window(300, 200, "Sign In");

        Fl_Input* user = new Fl_Input(100, 30, 150, 30, "Username:");
        Fl_Input* pass = new Fl_Input(100, 70, 150, 30, "Password:");
        pass->type(FL_SECRET_INPUT);

        Fl_Button* loginBtn = new Fl_Button(100, 120, 100, 30, "Login");
        loginBtn->callback(validateSignIn);

        signInWindow->end();
    }
    // Clear inputs when showing the panel.
    // Cast to temporary variable before checking for nullptr.
    Fl_Input* usernameField = static_cast<Fl_Input*>(signInWindow->child(0));
    if (usernameField) {
        usernameField->value(""); // Username
    }

    Fl_Input* passwordField = static_cast<Fl_Input*>(signInWindow->child(1));
    if (passwordField) {
        passwordField->value(""); // Password
    }

    signInWindow->show();
}

// Validates sign-up details and creates a "user" (currently just a message).
void validateSignUp(Fl_Widget*, void*) {
    const char* firstName = firstNameInput->value();
    const char* lastName = lastNameInput->value();
    const char* username = usernameInput->value();
    const char* password = passwordInput->value();
    const char* confirmPassword = confirmPasswordInput->value();
    const char* phone = phoneInput->value();
    const char* email = emailInput->value();
    const char* address = addressInput->value();

    // Basic validation for empty fields
    if (strlen(firstName) == 0 || strlen(lastName) == 0 || strlen(username) == 0 ||
        strlen(password) == 0 || strlen(confirmPassword) == 0 ||
        strlen(phone) == 0 || strlen(email) == 0 || strlen(address) == 0) {
        messageBox->label("All fields are required!");
        return;
    }

    // Password match validation
    if (strcmp(password, confirmPassword) != 0) {
        messageBox->label("Passwords do not match!");
        return;
    }

    // If all validation passes
    messageBox->label("Account created successfully!");
    fl_message("Sign Up successful! Please sign in.");
    if (mainWindow) mainWindow->hide(); // Hide the sign-up window
    showSignInPanel(); // Navigate to the sign-in panel
}

// Main entry point of the application.
int main() {
    // Initialize some default products for demonstration.
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
    allProducts.push_back({"Mangoes", 150}); // Added more products to test scrolling
    allProducts.push_back({"Pineapple", 220});
    allProducts.push_back({"Watermelon", 100});
    allProducts.push_back({"Cucumbers", 45});
    allProducts.push_back({"Spinach", 55});


    // Setup the main registration/sign-up window.
    mainWindow = new Fl_Window(750, 550, "E-Mart Sign Up");

    // Load and display logo (ensure "logi.png" is in the same directory).
    Fl_PNG_Image* logo = new Fl_PNG_Image("logi.png");
    Fl_Box* logoBox = new Fl_Box(175, 5, 120, 60);
    logoBox->image(logo);
    if (logo->fail()) fl_alert("Image 'logi.png' failed to load! Make sure it's in the same directory as the executable.");

    // Input fields for sign-up form.
    new Fl_Box(50, 30, 100, 30, "First Name:");
    firstNameInput = new Fl_Input(160, 30, 250, 30);

    new Fl_Box(50, 70, 100, 30, "Last Name:");
    lastNameInput = new Fl_Input(160, 70, 250, 30);

    new Fl_Box(50, 110, 100, 30, "Username:");
    usernameInput = new Fl_Input(160, 110, 250, 30);

    new Fl_Box(50, 150, 100, 30, "Password:");
    passwordInput = new Fl_Input(160, 150, 250, 30);
    passwordInput->type(FL_SECRET_INPUT); // Mask password input

    new Fl_Box(50, 190, 100, 30, "Confirm Password:");
    confirmPasswordInput = new Fl_Input(160, 190, 250, 30);
    confirmPasswordInput->type(FL_SECRET_INPUT); // Mask confirm password input

    // Checkbox to toggle password visibility.
    showPasswordCheck = new Fl_Check_Button(160, 230, 150, 25, "Show Password");
    showPasswordCheck->callback(togglePasswordVisibility);

    new Fl_Box(50, 270, 100, 30, "Phone:");
    phoneInput = new Fl_Input(160, 270, 250, 30);

    new Fl_Box(50, 310, 100, 30, "Email:");
    emailInput = new Fl_Input(160, 310, 250, 30);

    new Fl_Box(50, 350, 100, 30, "Address:");
    addressInput = new Fl_Input(160, 350, 250, 30);

    // Sign Up button.
    Fl_Button* signupButton = new Fl_Button(160, 400, 120, 35, "Sign Up");
    signupButton->callback(validateSignUp);

    // Message box for validation feedback.
    messageBox = new Fl_Box(50, 450, 400, 30, "");
    messageBox->labelcolor(FL_RED);

    mainWindow->end(); // End the main window's widget definition
    mainWindow->show(); // Show the main window
    return Fl::run(); // Start the FLTK event loop
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





