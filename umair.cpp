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
#include <algorithm>
#include <cstdio>
#include <string>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
#include <strings.h>
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

using namespace std;

// --- Global UI Elements Pointers ---
Fl_Input *firstNameInput, *lastNameInput, *usernameInput, *passwordInput, *confirmPasswordInput, *phoneInput, *emailInput, *addressInput;
Fl_Box* messageBox;
Fl_Check_Button* showPasswordCheck;
Fl_Window *mainWindow, *adminWindow, *customerWindow, *adminSignInWindow, *customerSignInWindow, *loginChoiceWindow, *buyProductsWindow, *returnProductWindow;

// Admin Product Management Windows/Inputs
Fl_Window *adminProductManagementWindow, *adminAddProductWindow, *adminUpdateProductWindow, *adminDeleteProductWindow, *adminDisplayProductsWindow;
Fl_Input *addProdNameInput, *addProdIdInput, *addProdPriceInput, *addProdQuantityInput;
Fl_Input *updateProdIdInput, *updateNewProdNameInput, *updateNewProdPriceInput, *updateNewProdQuantityInput;
Fl_Input *deleteProdIdInput;
Fl_Multiline_Output* adminDisplayProductsOutput;
Fl_Scroll* adminProductScroll;
Fl_Group* adminProductScrollContentGroup;

// Customer Management Windows/Inputs
Fl_Window *customerManagementWindow, *customerAddCustomerWindow, *customerUpdateCustomerWindow, *customerDeleteCustomerWindow, *customerDisplayCustomersWindow;
Fl_Input *addCustFirstNameInput, *addCustLastNameInput, *addCustUsernameInput, *addCustPasswordInput, *addCustPhoneInput, *addCustEmailInput, *addCustAddressInput, *addCustIdInput;
Fl_Input *updateCustIdInput, *updateNewCustFirstNameInput, *updateNewCustLastNameInput, *updateNewCustUsernameInput, *updateNewCustPasswordInput, *updateNewCustPhoneInput, *updateNewCustEmailInput, *updateNewCustAddressInput;
Fl_Input *deleteCustIdInput;
Fl_Multiline_Output* customerDisplayCustomersOutput;

// Existing product and cart related UI
Fl_Multiline_Output* productOutput;
Fl_Scroll* customerProductScroll;
Fl_Group* customerProductScrollContentGroup;

// Return Product UI
Fl_Input* returnProductNameInput;
Fl_Box* returnMessageBox;

// --- Data Structures ---
struct Product {
    string id;
    string name;
    int price;
    int quantity;
};
vector<Product> allProducts;

struct Customer {
    string id;
    string firstName;
    string lastName;
    string username;
    string password;
    string phone;
    string email;
    string address;
};
vector<Customer> allCustomers;

struct CartItem {
    string name;
    int price;
};
vector<CartItem> cartItems;

int totalPrice = 0;
int totalSales = 0;

// --- Forward Declarations ---
void showCustomerPanel();
void updateCustomerProductDisplay();
void showBuyProductsPanel();
void showReturnProductPanel();
void showAdminSignInPanel();
void showCustomerSignInPanel();
void showLoginChoicePanel();
void showAdminPanel();
void showSignUpPanel();
void validateSignUp(Fl_Widget*, void*);

// Admin Product Management Declarations
void showAdminProductManagementPanel();
void showAdminAddProductPanel();
void showAdminUpdateProductPanel();
void showAdminDisplayProductsPanel();
void showAdminDeleteProductPanel();
string generateUniqueProductId();
Product* findProductById(const string& id);
void updateAdminDisplayProductsOutput();

// Customer Management Declarations
void showCustomerManagementPanel();
void showCustomerAddCustomerPanel();
void showCustomerUpdateCustomerPanel();
void showCustomerDisplayCustomersPanel();
void showCustomerDeleteCustomerPanel();
string generateUniqueCustomerId();
Customer* findCustomerById(const string& id);
void updateCustomerDisplayOutput();

// --- Utility Functions ---

void togglePasswordVisibility(Fl_Widget*, void*) {
    passwordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
    confirmPasswordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
    Fl::redraw();
}

void updateProductOutput() {
    totalPrice = 0;
    string output_str = "Products in Cart:\n\n";
    for (const auto& item : cartItems) {
        char line[100];
        snprintf(line, sizeof(line), "%s - Rs.%d\n", item.name.c_str(), item.price);
        output_str += line;
        totalPrice += item.price;
    }
    char totalLine[100];
    snprintf(totalLine, sizeof(totalLine), "\nTotal: Rs.%d", totalPrice);
    output_str += totalLine;
    productOutput->value(output_str.c_str());
}

string generateUniqueProductId() {
    static int nextId = 101;
    return "P" + to_string(nextId++);
}

string generateUniqueCustomerId() {
    static int nextId = 1;
    return "C" + to_string(nextId++);
}

Product* findProductById(const string& id) {
    for (auto& prod : allProducts) {
        if (prod.id == id) {
            return &prod;
        }
    }
    return nullptr;
}

Customer* findCustomerById(const string& id) {
    for (auto& cust : allCustomers) {
        if (cust.id == id) {
            return &cust;
        }
    }
    return nullptr;
}

// --- Admin Product Management Callbacks ---

void adminAddProduct(Fl_Widget*, void*) {
    const string name = addProdNameInput->value();
    const string id = addProdIdInput->value();
    int price = atoi(addProdPriceInput->value());
    int quantity = atoi(addProdQuantityInput->value());

    if (name.empty() || id.empty() || price <= 0 || quantity < 0) {
        fl_alert("Please fill all fields correctly (price > 0, quantity >= 0).");
        return;
    }

    if (findProductById(id) != nullptr) {
        fl_alert("Product with this ID already exists. Use Update Product instead.");
        return;
    }

    for (const auto& prod : allProducts) {
        if (strcasecmp(prod.name.c_str(), name.c_str()) == 0) {
            fl_alert("Product with this name already exists. Consider changing its price instead.");
            return;
        }
    }

    allProducts.push_back({id, name, price, quantity});
    fl_message("Product added: ID=%s, Name=%s, Price=%d, Quantity=%d", id.c_str(), name.c_str(), price, quantity);

    addProdIdInput->value(generateUniqueProductId().c_str());
    addProdNameInput->value("");
    addProdPriceInput->value("");
    addProdQuantityInput->value("");
}

void adminUpdateProduct(Fl_Widget*, void*) {
    const string id = updateProdIdInput->value();
    const string newName = updateNewProdNameInput->value();
    int newPrice = atoi(updateNewProdPriceInput->value());
    int newQuantity = atoi(updateNewProdQuantityInput->value());

    Product* prod = findProductById(id);
    if (prod == nullptr) {
        fl_alert("Product with ID '%s' not found.", id.c_str());
        return;
    }

    if (!newName.empty()) prod->name = newName;
    if (newPrice > 0) prod->price = newPrice;
    if (newQuantity >= 0) prod->quantity = newQuantity;

    fl_message("Product ID %s updated successfully!", id.c_str());
    updateProdIdInput->value("");
    updateNewProdNameInput->value("");
    updateNewProdPriceInput->value("");
    updateNewProdQuantityInput->value("");
}

void adminDeleteProduct(Fl_Widget*, void*) {
    const string id = deleteProdIdInput->value();
    
    auto it = remove_if(allProducts.begin(), allProducts.end(), [&](const Product& p){
        return p.id == id;
    });

    if (it != allProducts.end()) {
        allProducts.erase(it, allProducts.end());
        fl_message("Product ID %s deleted successfully.", id.c_str());
    } else {
        fl_alert("Product with ID '%s' not found.", id.c_str());
    }
    deleteProdIdInput->value("");
}

void updateAdminDisplayProductsOutput() {
    if (!adminDisplayProductsOutput || !adminProductScroll || !adminProductScrollContentGroup) return;

    stringstream ss;
    ss << left << setw(10) << "ID" << setw(30) << "Name" << setw(10) << "Price" << setw(10) << "Qty" << "\n";
    ss << string(60, '-') << "\n";

    for (const auto& prod : allProducts) {
        ss << left << setw(10) << prod.id
           << setw(30) << prod.name
           << setw(10) << prod.price
           << setw(10) << prod.quantity << "\n";
    }
    adminDisplayProductsOutput->value(ss.str().c_str());

    int num_lines = count(ss.str().begin(), ss.str().end(), '\n');
    int required_text_height = max(adminDisplayProductsOutput->textsize() * 5, (num_lines + 2) * adminDisplayProductsOutput->textsize());
    adminDisplayProductsOutput->size(adminProductScrollContentGroup->w(), required_text_height);
    int required_group_height = max(adminProductScroll->h(), required_text_height);
    adminProductScrollContentGroup->size(adminProductScrollContentGroup->w(), required_group_height);

    adminProductScrollContentGroup->redraw();
    adminProductScroll->redraw();
    adminProductScroll->scroll_to(0, 0);
}

// --- Customer Management Callbacks ---

void customerAddCustomer(Fl_Widget*, void*) {
    const string id = addCustIdInput->value();
    const string fName = addCustFirstNameInput->value();
    const string lName = addCustLastNameInput->value();
    const string uname = addCustUsernameInput->value();
    const string pass = addCustPasswordInput->value();
    const string phone = addCustPhoneInput->value();
    const string email = addCustEmailInput->value();
    const string address = addCustAddressInput->value();

    if (id.empty() || fName.empty() || lName.empty() || uname.empty() ||
        pass.empty() || phone.empty() || email.empty() || address.empty()) {
        fl_alert("Please fill all customer fields.");
        return;
    }

    if (findCustomerById(id) != nullptr) {
        fl_alert("Customer with this ID already exists.");
        return;
    }

    for (const auto& cust : allCustomers) {
        if (cust.username == uname) {
            fl_alert("Username already exists. Please choose another.");
            return;
        }
    }

    allCustomers.push_back({id, fName, lName, uname, pass, phone, email, address});
    fl_message("Customer added: ID=%s, Username=%s", id.c_str(), uname.c_str());

    addCustIdInput->value(generateUniqueCustomerId().c_str());
    addCustFirstNameInput->value("");
    addCustLastNameInput->value("");
    addCustUsernameInput->value("");
    addCustPasswordInput->value("");
    addCustPhoneInput->value("");
    addCustEmailInput->value("");
    addCustAddressInput->value("");
}

void customerUpdateCustomer(Fl_Widget*, void*) {
    const string id = updateCustIdInput->value();
    Customer* cust = findCustomerById(id);

    if (cust == nullptr) {
        fl_alert("Customer with ID '%s' not found.", id.c_str());
        return;
    }

    const string newFName = updateNewCustFirstNameInput->value();
    const string newLName = updateNewCustLastNameInput->value();
    const string newUname = updateNewCustUsernameInput->value();
    const string newPass = updateNewCustPasswordInput->value();
    const string newPhone = updateNewCustPhoneInput->value();
    const string newEmail = updateNewCustEmailInput->value();
    const string newAddress = updateNewCustAddressInput->value();

    if (!newFName.empty()) cust->firstName = newFName;
    if (!newLName.empty()) cust->lastName = newLName;
    if (!newUname.empty()) {
        bool usernameExists = false;
        for (const auto& existingCust : allCustomers) {
            if (existingCust.id != id && existingCust.username == newUname) {
                usernameExists = true;
                break;
            }
        }
        if (usernameExists) {
            fl_alert("New username '%s' is already taken.", newUname.c_str());
            return;
        }
        cust->username = newUname;
    }
    if (!newPass.empty()) cust->password = newPass;
    if (!newPhone.empty()) cust->phone = newPhone;
    if (!newEmail.empty()) cust->email = newEmail;
    if (!newAddress.empty()) cust->address = newAddress;

    fl_message("Customer ID %s updated successfully!", id.c_str());
    updateCustIdInput->value("");
    updateNewCustFirstNameInput->value("");
    updateNewCustLastNameInput->value("");
    updateNewCustUsernameInput->value("");
    updateNewCustPasswordInput->value("");
    updateNewCustPhoneInput->value("");
    updateNewCustEmailInput->value("");
    updateNewCustAddressInput->value("");
}

void customerDeleteCustomer(Fl_Widget*, void*) {
    const string id = deleteCustIdInput->value();
    
    auto it = remove_if(allCustomers.begin(), allCustomers.end(), [&](const Customer& c){
        return c.id == id;
    });

    if (it != allCustomers.end()) {
        allCustomers.erase(it, allCustomers.end());
        fl_message("Customer ID %s deleted successfully.", id.c_str());
    } else {
        fl_alert("Customer with ID '%s' not found.", id.c_str());
    }
    deleteCustIdInput->value("");
}

void updateCustomerDisplayOutput() {
    if (!customerDisplayCustomersOutput) return;

    stringstream ss;
    ss << left << setw(10) << "ID" << setw(20) << "Username" << setw(20) << "Name" << setw(15) << "Phone" << "\n";
    ss << string(65, '-') << "\n";

    for (const auto& cust : allCustomers) {
        ss << left << setw(10) << cust.id
           << setw(20) << cust.username
           << setw(20) << (cust.firstName + " " + cust.lastName)
           << setw(15) << cust.phone << "\n";
    }
    customerDisplayCustomersOutput->value(ss.str().c_str());

    int num_lines = count(ss.str().begin(), ss.str().end(), '\n');
    int text_height = max(customerDisplayCustomersOutput->textsize() * 5, (num_lines + 3) * customerDisplayCustomersOutput->textsize());
    customerDisplayCustomersOutput->size(customerDisplayCustomersOutput->w(), text_height);
    customerDisplayCustomersOutput->redraw();
    if (customerDisplayCustomersWindow) {
        customerDisplayCustomersWindow->redraw();
    }
}

// --- Admin Panel Callbacks ---

void checkSales(Fl_Widget*, void*) {
    char msg[100];
    snprintf(msg, sizeof(msg), "Total Sales: Rs.%d", totalSales);
    fl_message(msg);
}

void validateAdminSignIn(Fl_Widget* w, void*) {
    Fl_Input* user = static_cast<Fl_Input*>(adminSignInWindow->child(0));
    Fl_Input* pass = static_cast<Fl_Input*>(adminSignInWindow->child(1));

    if (strcmp("admin", user->value()) == 0 && strcmp("admin123", pass->value()) == 0) {
        if (adminSignInWindow) adminSignInWindow->hide();
        showAdminPanel();
    } else {
        fl_alert("Invalid admin username or password!");
    }
    user->value("");
    pass->value("");
}

// --- Customer Panel Callbacks ---

void addProductToCart(Fl_Widget* w, void* data) {
    int productIndex = reinterpret_cast<intptr_t>(data);

    if (productIndex >= 0 && productIndex < allProducts.size()) {
        if (allProducts[productIndex].quantity > 0) {
            CartItem newItem;
            newItem.name = allProducts[productIndex].name;
            newItem.price = allProducts[productIndex].price;
            cartItems.push_back(newItem);
            allProducts[productIndex].quantity--;
            totalSales += allProducts[productIndex].price;
            updateProductOutput();
            updateCustomerProductDisplay();
            fl_message("Product added to cart!");
        } else {
            fl_alert("Product is out of stock!");
        }
    }
}

void deleteLastProduct(Fl_Widget*, void*) {
    if (!cartItems.empty()) {
        const string& removedItemName = cartItems.back().name;
        bool foundInProducts = false;
        for (auto& prod : allProducts) {
            if (prod.name == removedItemName) {
                prod.quantity++;
                foundInProducts = true;
                break;
            }
        }

        totalSales -= cartItems.back().price;
        cartItems.pop_back();
        updateProductOutput();
        if (foundInProducts) {
            updateCustomerProductDisplay();
        }
        fl_message("Last item removed from cart!");
    } else {
        fl_message("Cart is already empty!");
    }
}

void processReturnProduct(Fl_Widget*, void*) {
    const string productName = returnProductNameInput->value();
    if (productName.empty()) {
        returnMessageBox->labelcolor(FL_RED);
        returnMessageBox->label("Please enter a product name to return.");
        return;
    }

    bool found = false;
    for (size_t i = 0; i < cartItems.size(); ++i) {
        if (strcasecmp(cartItems[i].name.c_str(), productName.c_str()) == 0) {
            totalSales -= cartItems[i].price;
            for (auto& prod : allProducts) {
                if (prod.name == cartItems[i].name) {
                    prod.quantity++;
                    break;
                }
            }
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
    updateCustomerProductDisplay();
    returnProductWindow->redraw();
}

// --- Panel Display Functions ---

void showAdminPanel() {
    if (adminWindow == nullptr) {
        adminWindow = new Fl_Window(400, 300, "Admin Panel");
        adminWindow->color(FL_BLACK);

        Fl_Box* welcomeBox = new Fl_Box(50, 30, 300, 30, "Welcome to the Admin Panel!");
        welcomeBox->labelcolor(FL_WHITE);

        Fl_Button* manageProductsBtn = new Fl_Button(120, 80, 160, 35, "Manage Products");
        manageProductsBtn->color(FL_BLACK); manageProductsBtn->labelcolor(FL_WHITE);
        manageProductsBtn->callback([](Fl_Widget*, void*) {
            if (adminWindow) adminWindow->hide();
            showAdminProductManagementPanel();
        });

        Fl_Button* salesBtn = new Fl_Button(120, 130, 160, 35, "Check Sales");
        salesBtn->color(FL_BLACK); salesBtn->labelcolor(FL_WHITE);
        salesBtn->callback(checkSales);

        Fl_Button* signOutBtn = new Fl_Button(120, 180, 160, 35, "Sign Out");
        signOutBtn->color(FL_BLACK); signOutBtn->labelcolor(FL_WHITE);
        signOutBtn->callback([](Fl_Widget*, void*) {
            if (adminWindow) adminWindow->hide();
            showLoginChoicePanel();
        });

        adminWindow->end();
    }
    adminWindow->show();
}

void showAdminProductManagementPanel() {
    if (adminProductManagementWindow == nullptr) {
        adminProductManagementWindow = new Fl_Window(400, 350, "Admin: Manage Products");
        adminProductManagementWindow->color(FL_BLACK);

        Fl_Box* productManageBox = new Fl_Box(50, 30, 300, 30, "Product Management Options");
        productManageBox->labelcolor(FL_WHITE);

        Fl_Button* addBtn = new Fl_Button(120, 80, 160, 35, "Add Product");
        addBtn->color(FL_BLACK); addBtn->labelcolor(FL_WHITE);
        addBtn->callback([](Fl_Widget*, void*) {
            if (adminProductManagementWindow) adminProductManagementWindow->hide();
            showAdminAddProductPanel();
        });

        Fl_Button* updateBtn = new Fl_Button(120, 130, 160, 35, "Update Product");
        updateBtn->color(FL_BLACK); updateBtn->labelcolor(FL_WHITE);
        updateBtn->callback([](Fl_Widget*, void*) {
            if (adminProductManagementWindow) adminProductManagementWindow->hide();
            showAdminUpdateProductPanel();
        });

        Fl_Button* displayBtn = new Fl_Button(120, 180, 160, 35, "Display Products");
        displayBtn->color(FL_BLACK); displayBtn->labelcolor(FL_WHITE);
        displayBtn->callback([](Fl_Widget*, void*) {
            if (adminProductManagementWindow) adminProductManagementWindow->hide();
            showAdminDisplayProductsPanel();
        });

        Fl_Button* deleteBtn = new Fl_Button(120, 230, 160, 35, "Delete Product");
        deleteBtn->color(FL_BLACK); deleteBtn->labelcolor(FL_WHITE);
        deleteBtn->callback([](Fl_Widget*, void*) {
            if (adminProductManagementWindow) adminProductManagementWindow->hide();
            showAdminDeleteProductPanel();
        });

        Fl_Button* backBtn = new Fl_Button(120, 280, 160, 35, "Back to Admin Panel");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE);
        backBtn->callback([](Fl_Widget*, void*) {
            if (adminProductManagementWindow) adminProductManagementWindow->hide();
            showAdminPanel();
        });

        adminProductManagementWindow->end();
    }
    adminProductManagementWindow->show();
}

void showAdminAddProductPanel() {
    if (adminAddProductWindow == nullptr) {
        adminAddProductWindow = new Fl_Window(450, 350, "Admin: Add Product");
        adminAddProductWindow->color(FL_BLACK);

        addProdIdInput = new Fl_Input(150, 40, 250, 30, "Product ID:");
        addProdNameInput = new Fl_Input(150, 80, 250, 30, "Product Name:");
        addProdPriceInput = new Fl_Input(150, 120, 250, 30, "Price (Rs.):");
        addProdQuantityInput = new Fl_Input(150, 160, 250, 30, "Quantity:");

        Fl_Button* saveBtn = new Fl_Button(100, 220, 120, 35, "Save Product");
        saveBtn->color(FL_BLACK); saveBtn->labelcolor(FL_WHITE);
        saveBtn->callback(adminAddProduct);

        Fl_Button* backBtn = new Fl_Button(230, 220, 120, 35, "Back");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE);
        backBtn->callback([](Fl_Widget*, void*) {
            if (adminAddProductWindow) adminAddProductWindow->hide();
            showAdminProductManagementPanel();
        });
        
        adminAddProductWindow->end();
    }
    addProdIdInput->value(generateUniqueProductId().c_str());
    addProdNameInput->value("");
    addProdPriceInput->value("");
    addProdQuantityInput->value("");
    adminAddProductWindow->show();
}

void showAdminUpdateProductPanel() {
    if (adminUpdateProductWindow == nullptr) {
        adminUpdateProductWindow = new Fl_Window(450, 350, "Admin: Update Product");
        adminUpdateProductWindow->color(FL_BLACK);

        updateProdIdInput = new Fl_Input(170, 40, 250, 30, "Product ID to Update:");
        updateNewProdNameInput = new Fl_Input(170, 90, 250, 30, "New Name (optional):");
        updateNewProdPriceInput = new Fl_Input(170, 130, 250, 30, "New Price (optional):");
        updateNewProdQuantityInput = new Fl_Input(170, 170, 250, 30, "New Quantity (optional):");

        Fl_Button* updateBtn = new Fl_Button(100, 230, 120, 35, "Update Product");
        updateBtn->color(FL_BLACK); updateBtn->labelcolor(FL_WHITE);
        updateBtn->callback(adminUpdateProduct);

        Fl_Button* backBtn = new Fl_Button(230, 230, 120, 35, "Back");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE);
        backBtn->callback([](Fl_Widget*, void*) {
            if (adminUpdateProductWindow) adminUpdateProductWindow->hide();
            showAdminProductManagementPanel();
        });
        adminUpdateProductWindow->end();
    }
    updateProdIdInput->value("");
    updateNewProdNameInput->value("");
    updateNewProdPriceInput->value("");
    updateNewProdQuantityInput->value("");
    adminUpdateProductWindow->show();
}

void showAdminDisplayProductsPanel() {
    if (adminDisplayProductsWindow == nullptr) {
        adminDisplayProductsWindow = new Fl_Window(600, 420, "Admin: Display Products");
        adminDisplayProductsWindow->color(FL_BLACK);

        adminProductScroll = new Fl_Scroll(10, 10, 580, 340);
        adminProductScroll->box(FL_THIN_DOWN_BOX);
        adminProductScroll->type(Fl_Scroll::VERTICAL);
        adminProductScroll->begin();
            adminProductScrollContentGroup = new Fl_Group(10, 10, adminProductScroll->w() - Fl::scrollbar_size(), 340);
            adminProductScrollContentGroup->box(FL_NO_BOX);
            adminProductScrollContentGroup->begin();
                adminDisplayProductsOutput = new Fl_Multiline_Output(10, 10, adminProductScrollContentGroup->w(), 340);
                adminDisplayProductsOutput->textsize(14);
                adminDisplayProductsOutput->box(FL_NO_BOX);
                adminDisplayProductsOutput->align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
            adminProductScrollContentGroup->end();
            adminProductScrollContentGroup->resizable(adminDisplayProductsOutput);
        adminProductScroll->end();
        adminProductScroll->resizable(adminProductScrollContentGroup);

        Fl_Button* backBtn = new Fl_Button(250, 360, 100, 35, "Back");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE);
        backBtn->callback([](Fl_Widget*, void*) {
            if (adminDisplayProductsWindow) adminDisplayProductsWindow->hide();
            showAdminProductManagementPanel();
        });
        adminDisplayProductsWindow->end();
    }
    updateAdminDisplayProductsOutput();
    adminDisplayProductsWindow->show();
}

void showAdminDeleteProductPanel() {
    if (adminDeleteProductWindow == nullptr) {
        adminDeleteProductWindow = new Fl_Window(350, 200, "Admin: Delete Product");
        adminDeleteProductWindow->color(FL_BLACK);

        deleteProdIdInput = new Fl_Input(100, 50, 200, 30, "Product ID:");

        Fl_Button* deleteBtn = new Fl_Button(80, 100, 100, 35, "Delete");
        deleteBtn->color(FL_BLACK); deleteBtn->labelcolor(FL_WHITE);
        deleteBtn->callback(adminDeleteProduct);

        Fl_Button* backBtn = new Fl_Button(190, 100, 100, 35, "Back");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE);
        backBtn->callback([](Fl_Widget*, void*) {
            if (adminDeleteProductWindow) adminDeleteProductWindow->hide();
            showAdminProductManagementPanel();
        });
        adminDeleteProductWindow->end();
    }
    deleteProdIdInput->value("");
    adminDeleteProductWindow->show();
}

void showCustomerPanel() {
    if (customerWindow == nullptr) {
        customerWindow = new Fl_Window(400, 350, "Customer Panel");
        customerWindow->color(FL_BLACK);

        Fl_Box* welcomeBox = new Fl_Box(50, 30, 300, 30, "Welcome to the Customer Panel!");
        welcomeBox->labelcolor(FL_WHITE);

        Fl_Button* buyBtn = new Fl_Button(120, 80, 160, 35, "Buy Products");
        buyBtn->color(FL_BLACK); buyBtn->labelcolor(FL_WHITE);
        buyBtn->callback([](Fl_Widget*, void*) {
            if (customerWindow) customerWindow->hide();
            showBuyProductsPanel();
        });

        Fl_Button* returnBtn = new Fl_Button(120, 130, 160, 35, "Return Product");
        returnBtn->color(FL_BLACK); returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback([](Fl_Widget*, void*) {
            if (customerWindow) customerWindow->hide();
            showReturnProductPanel();
        });

        Fl_Button* manageCustomersBtn = new Fl_Button(120, 180, 160, 35, "Manage Customers");
        manageCustomersBtn->color(FL_BLACK); manageCustomersBtn->labelcolor(FL_WHITE);
        manageCustomersBtn->callback([](Fl_Widget*, void*) {
            if (customerWindow) customerWindow->hide();
            showCustomerManagementPanel();
        });

        Fl_Button* signOutBtn = new Fl_Button(120, 230, 160, 35, "Sign Out");
        signOutBtn->color(FL_BLACK); signOutBtn->labelcolor(FL_WHITE);
        signOutBtn->callback([](Fl_Widget*, void*) {
            if (customerWindow) customerWindow->hide();
            showLoginChoicePanel();
        });

        customerWindow->end();
    }
    customerWindow->show();
}

void showCustomerManagementPanel() {
    if (customerManagementWindow == nullptr) {
        customerManagementWindow = new Fl_Window(400, 350, "Customer: Manage Customers");
        customerManagementWindow->color(FL_BLACK);

        Fl_Box* custManageBox = new Fl_Box(50, 30, 300, 30, "Customer Management Options");
        custManageBox->labelcolor(FL_WHITE);

        Fl_Button* addBtn = new Fl_Button(120, 80, 160, 35, "Add Customer");
        addBtn->color(FL_BLACK); addBtn->labelcolor(FL_WHITE);
        addBtn->callback([](Fl_Widget*, void*) {
            if (customerManagementWindow) customerManagementWindow->hide();
            showCustomerAddCustomerPanel();
        });

        Fl_Button* updateBtn = new Fl_Button(120, 130, 160, 35, "Update Customer");
        updateBtn->color(FL_BLACK); updateBtn->labelcolor(FL_WHITE);
        updateBtn->callback([](Fl_Widget*, void*) {
            if (customerManagementWindow) customerManagementWindow->hide();
            showCustomerUpdateCustomerPanel();
        });

        Fl_Button* displayBtn = new Fl_Button(120, 180, 160, 35, "Display Customers");
        displayBtn->color(FL_BLACK); displayBtn->labelcolor(FL_WHITE);
        displayBtn->callback([](Fl_Widget*, void*) {
            if (customerManagementWindow) customerManagementWindow->hide();
            showCustomerDisplayCustomersPanel();
        });

        Fl_Button* deleteBtn = new Fl_Button(120, 230, 160, 35, "Delete Customer");
        deleteBtn->color(FL_BLACK); deleteBtn->labelcolor(FL_WHITE);
        deleteBtn->callback([](Fl_Widget*, void*) {
            if (customerManagementWindow) customerManagementWindow->hide();
            showCustomerDeleteCustomerPanel();
        });

        Fl_Button* backBtn = new Fl_Button(120, 280, 160, 35, "Back to Customer Panel");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE);
        backBtn->callback([](Fl_Widget*, void*) {
            if (customerManagementWindow) customerManagementWindow->hide();
            showCustomerPanel();
        });

        customerManagementWindow->end();
    }
    customerManagementWindow->show();
}

void showCustomerAddCustomerPanel() {
    if (customerAddCustomerWindow == nullptr) {
        customerAddCustomerWindow = new Fl_Window(450, 450, "Customer: Add Customer");
        customerAddCustomerWindow->color(FL_BLACK);

        addCustIdInput = new Fl_Input(150, 40, 250, 30, "Customer ID:");
        addCustFirstNameInput = new Fl_Input(150, 80, 250, 30, "First Name:");
        addCustLastNameInput = new Fl_Input(150, 120, 250, 30, "Last Name:");
        addCustUsernameInput = new Fl_Input(150, 160, 250, 30, "Username:");
        addCustPasswordInput = new Fl_Input(150, 200, 250, 30, "Password:");
        addCustPasswordInput->type(FL_SECRET_INPUT);
        addCustPhoneInput = new Fl_Input(150, 240, 250, 30, "Phone:");
        addCustEmailInput = new Fl_Input(150, 280, 250, 30, "Email:");
        addCustAddressInput = new Fl_Input(150, 320, 250, 30, "Address:");

        Fl_Button* saveBtn = new Fl_Button(100, 380, 120, 35, "Save Customer");
        saveBtn->color(FL_BLACK); saveBtn->labelcolor(FL_WHITE);
        saveBtn->callback(customerAddCustomer);

        Fl_Button* backBtn = new Fl_Button(230, 380, 120, 35, "Back");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE);
        backBtn->callback([](Fl_Widget*, void*) {
            if (customerAddCustomerWindow) customerAddCustomerWindow->hide();
            showCustomerManagementPanel();
        });
        
        customerAddCustomerWindow->end();
    }
    addCustIdInput->value(generateUniqueCustomerId().c_str());
    addCustFirstNameInput->value("");
    addCustLastNameInput->value("");
    addCustUsernameInput->value("");
    addCustPasswordInput->value("");
    addCustPhoneInput->value("");
    addCustEmailInput->value("");
    addCustAddressInput->value("");
    customerAddCustomerWindow->show();
}

void showCustomerUpdateCustomerPanel() {
    if (customerUpdateCustomerWindow == nullptr) {
        customerUpdateCustomerWindow = new Fl_Window(450, 500, "Customer: Update Customer");
        customerUpdateCustomerWindow->color(FL_BLACK);

        updateCustIdInput = new Fl_Input(170, 40, 250, 30, "Customer ID to Update:");
        updateNewCustFirstNameInput = new Fl_Input(170, 90, 250, 30, "New First Name (opt):");
        updateNewCustLastNameInput = new Fl_Input(170, 130, 250, 30, "New Last Name (opt):");
        updateNewCustUsernameInput = new Fl_Input(170, 170, 250, 30, "New Username (opt):");
        updateNewCustPasswordInput = new Fl_Input(170, 210, 250, 30, "New Password (opt):");
        updateNewCustPasswordInput->type(FL_SECRET_INPUT);
        updateNewCustPhoneInput = new Fl_Input(170, 250, 250, 30, "New Phone (opt):");
        updateNewCustEmailInput = new Fl_Input(170, 290, 250, 30, "New Email (opt):");
        updateNewCustAddressInput = new Fl_Input(170, 330, 250, 30, "New Address (opt):");

        Fl_Button* updateBtn = new Fl_Button(100, 390, 120, 35, "Update Customer");
        updateBtn->color(FL_BLACK); updateBtn->labelcolor(FL_WHITE);
        updateBtn->callback(customerUpdateCustomer);

        Fl_Button* backBtn = new Fl_Button(230, 390, 120, 35, "Back");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE);
        backBtn->callback([](Fl_Widget*, void*) {
            if (customerUpdateCustomerWindow) customerUpdateCustomerWindow->hide();
            showCustomerManagementPanel();
        });
        customerUpdateCustomerWindow->end();
    }
    updateCustIdInput->value("");
    updateNewCustFirstNameInput->value("");
    updateNewCustLastNameInput->value("");
    updateNewCustUsernameInput->value("");
    updateNewCustPasswordInput->value("");
    updateNewCustPhoneInput->value("");
    updateNewCustEmailInput->value("");
    updateNewCustAddressInput->value("");
    customerUpdateCustomerWindow->show();
}

void showCustomerDisplayCustomersPanel() {
    if (customerDisplayCustomersWindow == nullptr) {
        customerDisplayCustomersWindow = new Fl_Window(700, 400, "Customer: Display Customers");
        customerDisplayCustomersWindow->color(FL_BLACK);

        customerDisplayCustomersOutput = new Fl_Multiline_Output(10, 10, 680, 340);
        customerDisplayCustomersOutput->textsize(14);
        customerDisplayCustomersOutput->box(FL_THIN_DOWN_BOX);
        customerDisplayCustomersOutput->align(FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        
        Fl_Button* backBtn = new Fl_Button(300, 360, 100, 35, "Back");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE);
        backBtn->callback([](Fl_Widget*, void*) {
            if (customerDisplayCustomersWindow) customerDisplayCustomersWindow->hide();
            showCustomerManagementPanel();
        });
        customerDisplayCustomersWindow->end();
    }
    updateCustomerDisplayOutput();
    customerDisplayCustomersWindow->show();
}

void showCustomerDeleteCustomerPanel() {
    if (customerDeleteCustomerWindow == nullptr) {
        customerDeleteCustomerWindow = new Fl_Window(350, 200, "Customer: Delete Customer");
        customerDeleteCustomerWindow->color(FL_BLACK);

        deleteCustIdInput = new Fl_Input(100, 50, 200, 30, "Customer ID:");

        Fl_Button* deleteBtn = new Fl_Button(80, 100, 100, 35, "Delete");
        deleteBtn->color(FL_BLACK); deleteBtn->labelcolor(FL_WHITE);
        deleteBtn->callback(customerDeleteCustomer);

        Fl_Button* backBtn = new Fl_Button(190, 100, 100, 35, "Back");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE);
        backBtn->callback([](Fl_Widget*, void*) {
            if (customerDeleteCustomerWindow) customerDeleteCustomerWindow->hide();
            showCustomerManagementPanel();
        });
        customerDeleteCustomerWindow->end();
    }
    deleteCustIdInput->value("");
    customerDeleteCustomerWindow->show();
}

void updateCustomerProductDisplay() {
    if (!customerProductScroll || !customerProductScrollContentGroup) return;

    customerProductScrollContentGroup->clear();
    customerProductScrollContentGroup->begin();

    int y_offset = 10;
    const int button_height = 30;
    const int button_spacing = 10;
    const int button_width = customerProductScrollContentGroup->w() - 20;

    customerProductScrollContentGroup->size(customerProductScroll->w() - Fl::scrollbar_size(), customerProductScrollContentGroup->h());

    for (size_t i = 0; i < allProducts.size(); i++) {
        std::string buttonLabel = allProducts[i].name;
        buttonLabel += " (Rs.";
        buttonLabel += std::to_string(allProducts[i].price);
        buttonLabel += ") [Qty: " + std::to_string(allProducts[i].quantity) + "]";

        Fl_Button* prodBtn = new Fl_Button(10, y_offset, button_width, button_height);
        prodBtn->label(buttonLabel.c_str());
        prodBtn->callback(addProductToCart, reinterpret_cast<void*>(i));
        prodBtn->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
        prodBtn->box(FL_UP_BOX);
        prodBtn->color(FL_BLACK); prodBtn->labelcolor(FL_WHITE);

        y_offset += (button_height + button_spacing);
    }
    customerProductScrollContentGroup->end();

    int content_height = y_offset + 10;
    int min_scroll_height = customerProductScroll->h();
    customerProductScrollContentGroup->size(customerProductScrollContentGroup->w(), max(min_scroll_height, content_height));
    
    customerProductScroll->scroll_to(0, 0);
    customerProductScrollContentGroup->redraw();
    customerProductScroll->redraw();
}

void showBuyProductsPanel() {
    if (buyProductsWindow == nullptr) {
        buyProductsWindow = new Fl_Window(500, 520, "Buy Products");
        buyProductsWindow->color(FL_BLACK);

        Fl_Box* productsLabel = new Fl_Box(20, 10, 300, 30, "Available Products:");
        productsLabel->labelcolor(FL_WHITE);

        customerProductScroll = new Fl_Scroll(20, 50, 250, 200, "");
        customerProductScroll->box(FL_THIN_DOWN_BOX);
        customerProductScroll->align(FL_ALIGN_TOP_LEFT);
        customerProductScroll->type(FL_VERTICAL);
        customerProductScroll->begin();
            customerProductScrollContentGroup = new Fl_Group(0, 0, customerProductScroll->w() - Fl::scrollbar_size(), customerProductScroll->h());
            customerProductScrollContentGroup->box(FL_NO_BOX);
            customerProductScrollContentGroup->end();
        customerProductScroll->end();
        customerProductScroll->resizable(customerProductScrollContentGroup);

        productOutput = new Fl_Multiline_Output(20, 260, 460, 180, "Products in Cart:");
        productOutput->textsize(14);
        productOutput->labelsize(14);
        productOutput->box(FL_DOWN_BOX);

        Fl_Button* deleteLastBtn = new Fl_Button(350, 450, 120, 30, "Delete Last");
        deleteLastBtn->color(FL_BLACK); deleteLastBtn->labelcolor(FL_WHITE);
        deleteLastBtn->callback(deleteLastProduct);

        Fl_Button* backBtn = new Fl_Button(200, 480, 100, 30, "Back");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE);
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
        returnProductWindow->color(FL_BLACK);

        returnProductNameInput = new Fl_Input(150, 40, 200, 30, "Product Name:");

        Fl_Button* returnBtn = new Fl_Button(150, 90, 100, 30, "Return");
        returnBtn->color(FL_BLACK); returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback(processReturnProduct);

        returnMessageBox = new Fl_Box(20, 140, 360, 30, "");
        returnMessageBox->labelcolor(FL_RED);

        Fl_Button* backBtn = new Fl_Button(150, 170, 100, 30, "Back");
        backBtn->color(FL_BLACK); backBtn->labelcolor(FL_WHITE);
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

void validateCustomerSignIn(Fl_Widget* w, void* data) {
    Fl_Input* user = static_cast<Fl_Input*>(customerSignInWindow->child(0));
    Fl_Input* pass = static_cast<Fl_Input*>(customerSignInWindow->child(1));

    bool foundCustomer = false;
    for (const auto& cust : allCustomers) {
        if (cust.username == user->value() && cust.password == pass->value()) {
            foundCustomer = true;
            break;
        }
    }

    if (foundCustomer) {
        if (customerSignInWindow) customerSignInWindow->hide();
        showCustomerPanel();
    } else {
        fl_alert("Invalid customer username or password!");
    }
    user->value("");
    pass->value("");
}

void showCustomerSignInPanel() {
    if (customerSignInWindow == nullptr) {
        customerSignInWindow = new Fl_Window(300, 250, "Customer Sign In");
        customerSignInWindow->color(FL_BLACK);

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

void showAdminSignInPanel() {
    if (adminSignInWindow == nullptr) {
        adminSignInWindow = new Fl_Window(300, 250, "Admin Sign In");
        adminSignInWindow->color(FL_BLACK);

        Fl_Input* user = new Fl_Input(100, 30, 150, 30, "Username:");
        Fl_Input* pass = new Fl_Input(100, 70, 150, 30, "Password:");
        pass->type(FL_SECRET_INPUT);

        Fl_Button* loginBtn = new Fl_Button(100, 120, 100, 30, "Login");
        loginBtn->color(FL_BLACK); loginBtn->labelcolor(FL_WHITE);
        loginBtn->callback(validateAdminSignIn);

        Fl_Button* backToChoiceBtn = new Fl_Button(80, 170, 140, 30, "Back to Choice");
        backToChoiceBtn->color(FL_BLACK); backToChoiceBtn->labelcolor(FL_WHITE);
        backToChoiceBtn->callback([](Fl_Widget*, void*) {
            if (adminSignInWindow) adminSignInWindow->hide();
            showLoginChoicePanel();
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

void validateSignUp(Fl_Widget*, void*) {
    const string first = firstNameInput->value();
    const string last = lastNameInput->value();
    const string user = usernameInput->value();
    const string pass = passwordInput->value();
    const string confirmPass = confirmPasswordInput->value();
    const string phone = phoneInput->value();
    const string email = emailInput->value();
    const string address = addressInput->value();

    if (first.empty() || last.empty() || user.empty() ||
        pass.empty() || confirmPass.empty() || phone.empty() ||
        email.empty() || address.empty()) {
        messageBox->labelcolor(FL_RED);
        messageBox->label("All fields are required!");
        return;
    }

    if (pass != confirmPass) {
        messageBox->labelcolor(FL_RED);
        messageBox->label("Passwords do not match!");
        passwordInput->value("");
        confirmPasswordInput->value("");
        return;
    }

    if (user == "admin") {
        messageBox->labelcolor(FL_RED);
        messageBox->label("Username 'admin' is reserved.");
        return;
    }

    for (const auto& cust : allCustomers) {
        if (cust.username == user) {
            messageBox->labelcolor(FL_RED);
            messageBox->label("Username already exists. Please choose another.");
            return;
        }
    }

    allCustomers.push_back({generateUniqueCustomerId(), first, last, user, pass, phone, email, address});

    messageBox->labelcolor(FL_DARK_GREEN);
    messageBox->label("Sign Up Successful! You can now choose to sign in.");

    firstNameInput->value("");
    lastNameInput->value("");
    usernameInput->value("");
    passwordInput->value("");
    confirmPasswordInput->value("");
    phoneInput->value("");
    emailInput->value("");
    addressInput->value("");
    showPasswordCheck->value(0);
    togglePasswordVisibility(nullptr, nullptr);

    Fl::add_timeout(1.5, [](void*){
        if (mainWindow) mainWindow->hide();
        showLoginChoicePanel();
    }, nullptr);
}

void showSignUpPanel() {
    if (mainWindow == nullptr) {
        mainWindow = new Fl_Window(750, 550, "E-Mart Sign Up");
        mainWindow->color(FL_BLACK);

        Fl_PNG_Image* logo = new Fl_PNG_Image("logi.png");
        Fl_Box* logoBox = new Fl_Box(175, 5, 120, 60);
        logoBox->image(logo);
        if (logo->fail()) fl_alert("Image 'logi.png' failed to load! Make sure it's in the same directory as the executable.");

        Fl_Box* firstNameLabel = new Fl_Box(50, 30, 100, 30, "First Name:");
        firstNameLabel->labelcolor(FL_WHITE);
        firstNameInput = new Fl_Input(160, 30, 250, 30);

        Fl_Box* lastNameLabel = new Fl_Box(50, 70, 100, 30, "Last Name:");
        lastNameLabel->labelcolor(FL_WHITE);
        lastNameInput = new Fl_Input(160, 70, 250, 30);

        Fl_Box* usernameLabel = new Fl_Box(50, 110, 100, 30, "Username:");
        usernameLabel->labelcolor(FL_WHITE);
        usernameInput = new Fl_Input(160, 110, 250, 30);

        Fl_Box* passwordLabel = new Fl_Box(50, 150, 100, 30, "Password:");
        passwordLabel->labelcolor(FL_WHITE);
        passwordInput = new Fl_Input(160, 150, 250, 30);
        passwordInput->type(FL_SECRET_INPUT);

        Fl_Box* confirmPasswordLabel = new Fl_Box(50, 190, 100, 30, "Confirm Password:");
        confirmPasswordLabel->labelcolor(FL_WHITE);
        confirmPasswordInput = new Fl_Input(160, 190, 250, 30);
        confirmPasswordInput->type(FL_SECRET_INPUT);

        showPasswordCheck = new Fl_Check_Button(160, 230, 150, 25, "Show Password");
        showPasswordCheck->labelcolor(FL_WHITE);
        showPasswordCheck->callback(togglePasswordVisibility);

        Fl_Box* phoneLabel = new Fl_Box(50, 270, 100, 30, "Phone:");
        phoneLabel->labelcolor(FL_WHITE);
        phoneInput = new Fl_Input(160, 270, 250, 30);

        Fl_Box* emailLabel = new Fl_Box(50, 310, 100, 30, "Email:");
        emailLabel->labelcolor(FL_WHITE);
        emailInput = new Fl_Input(160, 310, 250, 30);

        Fl_Box* addressLabel = new Fl_Box(50, 350, 100, 30, "Address:");
        addressLabel->labelcolor(FL_WHITE);
        addressInput = new Fl_Input(160, 350, 250, 30);

        Fl_Button* signupButton = new Fl_Button(160, 400, 120, 35, "Sign Up");
        signupButton->color(FL_BLACK); signupButton->labelcolor(FL_WHITE);
        signupButton->callback(validateSignUp);

        messageBox = new Fl_Box(50, 450, 400, 30, "");
        messageBox->labelcolor(FL_RED);

        mainWindow->end();
    }
    mainWindow->show();
}

void showLoginChoicePanel() {
    if (loginChoiceWindow == nullptr) {
        loginChoiceWindow = new Fl_Window(350, 250, "Login Choice");
        loginChoiceWindow->color(FL_BLACK);

        Fl_Box* loginChoiceBox = new Fl_Box(50, 30, 250, 30, "Please choose your login type:");
        loginChoiceBox->labelcolor(FL_WHITE);

        Fl_Button* customerLoginBtn = new Fl_Button(100, 80, 150, 35, "Customer Login");
        customerLoginBtn->color(FL_BLACK); customerLoginBtn->labelcolor(FL_WHITE);
        customerLoginBtn->callback([](Fl_Widget*, void*) {
            if (loginChoiceWindow) loginChoiceWindow->hide();
            showCustomerSignInPanel();
        });

        Fl_Button* adminLoginBtn = new Fl_Button(100, 130, 150, 35, "Admin Login");
        adminLoginBtn->color(FL_BLACK); adminLoginBtn->labelcolor(FL_WHITE);
        adminLoginBtn->callback([](Fl_Widget*, void*) {
            if (loginChoiceWindow) loginChoiceWindow->hide();
            showAdminSignInPanel();
        });

        Fl_Button* backToSignUpBtn = new Fl_Button(100, 180, 150, 35, "Back to Sign Up");
        backToSignUpBtn->color(FL_BLACK); backToSignUpBtn->labelcolor(FL_WHITE);
        backToSignUpBtn->callback([](Fl_Widget*, void*) {
            if (loginChoiceWindow) loginChoiceWindow->hide();
            showSignUpPanel();
        });

        loginChoiceWindow->end();
    }
    loginChoiceWindow->show();
}

int main() {
    allProducts.push_back({"P101", "Milk", 150, 100});
    allProducts.push_back({"P102", "Bread", 100, 50});
    allProducts.push_back({"P103", "Rice", 200, 200});
    allProducts.push_back({"P104", "Apples", 50, 75});
    allProducts.push_back({"P105", "Oranges", 70, 60});
    allProducts.push_back({"P106", "Bananas", 30, 120});
    allProducts.push_back({"P107", "Chocolate", 250, 40});
    allProducts.push_back({"P108", "Coffee", 300, 30});
    allProducts.push_back({"P109", "Tea", 120, 80});
    allProducts.push_back({"P110", "Sugar", 80, 150});
    allProducts.push_back({"P111", "Eggs", 120, 90});
    allProducts.push_back({"P112", "Flour", 90, 100});
    allProducts.push_back({"P113", "Potatoes", 60, 200});
    allProducts.push_back({"P114", "Tomatoes", 40, 180});
    allProducts.push_back({"P115", "Onions", 35, 170});
    allProducts.push_back({"P116", "Chicken", 400, 25});
    allProducts.push_back({"P117", "Fish", 350, 20});
    allProducts.push_back({"P118", "Cabbage", 30, 100});
    allProducts.push_back({"P119", "Carrots", 25, 120});
    allProducts.push_back({"P120", "Grapes", 180, 50});
    allProducts.push_back({"P121", "Pears", 90, 70});
    allProducts.push_back({"P122", "Mangoes", 150, 45});
    allProducts.push_back({"P123", "Pineapple", 220, 35});
    allProducts.push_back({"P124", "Watermelon", 100, 20});
    allProducts.push_back({"P125", "Cucumbers", 45, 90});
    allProducts.push_back({"P126", "Spinach", 55, 80});

    allCustomers.push_back({"C1", "John", "Doe", "john.doe", "password123", "123-456-7890", "john@example.com", "123 Main St"});
    allCustomers.push_back({"C2", "Jane", "Smith", "jane.smith", "mypassword", "987-654-3210", "jane@example.com", "456 Oak Ave"});

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
