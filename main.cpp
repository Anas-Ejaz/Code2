#include <string>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Scroll.H>

using namespace std;

class signup;
class login;
class admin;
class product;
class employee;
class linkedList;


class linkedList{
    public:
    struct productNode {
        int productId;
        string productName;
        float productPrice;
        int productQuantity;
        productNode* next;
    };

    struct employeeNode {
        int employeeId;
        string employeeName;
        string employeePosition;
        float employeeSalary;
        employeeNode* next;
    };

    productNode* createProductNode(int id, string name, float price, int quantity) {
        productNode* newNode = new productNode;
        newNode->productId = id;
        newNode->productName = name;
        newNode->productPrice = price;
        newNode->productQuantity = quantity;
        newNode->next = nullptr;
        return newNode;
    }

    void insertProduct(productNode*& head, int id, string name, float price, int quantity) {
        productNode* newNode = createProductNode(id, name, price, quantity);
        if (head == nullptr) {
            head = newNode;
        } else {
            productNode* temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    void deleteProduct(productNode*& head, int id) {
        if (head == nullptr) return; // List is empty
        if (head->productId == id) {
            productNode* temp = head;
            head = head->next;
            delete temp;
            return;
        }
        productNode* current = head;
        while (current->next != nullptr && current->next->productId != id) {
            current = current->next;
        }
        if (current->next != nullptr) {
            productNode* temp = current->next;
            current->next = current->next->next;
            delete temp;
        }
    }

    string displayProducts(productNode* head);


    employeeNode* createEmployeeNode(employeeNode*& head, int id, string name, string position, float salary) {
        employeeNode* empNode = new employeeNode;
        empNode->employeeId = id;
        empNode->employeeName = name;
        empNode->employeePosition = position;
        empNode->employeeSalary = salary;
        empNode->next = nullptr;
        return empNode;
    }

    void insertEmployee(employeeNode*& head, int id, string name, string position, float salary) {
        employeeNode* newNode = createEmployeeNode(head, id, name, position, salary);

        if (head == nullptr) {
            head = newNode;
        } else {
            employeeNode* temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    void deleteEmployee(employeeNode*& head, int id) {
        if (head == nullptr) return; // List is empty
        if (head->employeeId == id) {
            employeeNode* temp = head;
            head = head->next;
            delete temp;
            return;
        }
        employeeNode* current = head;
        while (current->next != nullptr && current->next->employeeId != id) {
            current = current->next;
        }
        if (current->next != nullptr) {
            employeeNode* temp = current->next;
            current->next = current->next->next;
            delete temp;
        }
    }

    string displayEmployees(employeeNode* head); 

};

class employee{
public:
    int employeePage(){
        if(Fl::first_window()) Fl::first_window()->hide();

        Fl_Window* employeeWindow = new Fl_Window(900, 600, "E-Mart Employee Management");
        employeeWindow->color(FL_BLACK);

        Fl_Box* heading = new Fl_Box(0, 40, employeeWindow->w(), 50, "Employee Management");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->align(FL_ALIGN_CENTER);

        Fl_Button* addEmployeeBtn = new Fl_Button(350, 150, 200, 50, "Add Employee");
        addEmployeeBtn->align(FL_ALIGN_CENTER);
        addEmployeeBtn->color(FL_BLUE);
        addEmployeeBtn->labelcolor(FL_WHITE);
        addEmployeeBtn->callback([](Fl_Widget*, void* data) {
            ((employee*)data)->addEmployee();
        });

        Fl_Button* viewEmployeeBtn = new Fl_Button(350, 250, 200, 50, "View Employees");
        viewEmployeeBtn->align(FL_ALIGN_CENTER);    
        viewEmployeeBtn->color(FL_BLUE);
        viewEmployeeBtn->labelcolor(FL_WHITE);
        viewEmployeeBtn->callback([](Fl_Widget*, void* data) {
            ((employee*)data)->viewEmployee();
        }, this);

        Fl_Button* deleteEmployeeBtn = new Fl_Button(350, 350, 200, 50, "Delete Employee");
        deleteEmployeeBtn->align(FL_ALIGN_CENTER);
        deleteEmployeeBtn->color(FL_BLUE);
        deleteEmployeeBtn->labelcolor(FL_WHITE);   
        deleteEmployeeBtn->callback([](Fl_Widget*, void* data) {
            // Functionality to delete an employee
        }, this);

        Fl_Button* returnBtn = new Fl_Button(390, employeeWindow->h() - 150, 120, 35, "Return");
        returnBtn->align(FL_ALIGN_CENTER);
        returnBtn->color(FL_BLUE);
        returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback([](Fl_Widget*, void* data) {
           ((employee*)data)->goAdminPanel();
        }, this);

        employeeWindow->end();
        employeeWindow->show();

        return Fl::run();
    }

    int addEmployee(){
        if(Fl::first_window()) Fl::first_window()->hide();

        Fl_Window* addEmployeeWindow = new Fl_Window(900, 600, "E-Mart Employee Management");
        addEmployeeWindow->color(FL_BLACK);

        Fl_Box* heading = new Fl_Box(0, 40, addEmployeeWindow->w(), 50, "Add Employee");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->align(FL_ALIGN_CENTER);

        Fl_Box* employeeIdLabel = new Fl_Box(300, 150, 80, 30, "Employee ID:");
        employeeIdLabel->labelcolor(FL_WHITE);
        Fl_Input* employeeIdInput = new Fl_Input(400, 150, 200, 30);
        employeeIdInput->type(FL_NORMAL_INPUT);

        Fl_Box* employeeNameLabel = new Fl_Box(300, 220, 80, 30, "Employee Name:");
        employeeNameLabel->labelcolor(FL_WHITE);
        Fl_Input* employeeNameInput = new Fl_Input(400, 220, 200, 30);
        employeeNameInput->type(FL_NORMAL_INPUT);

        Fl_Box* employeePositionLabel = new Fl_Box(300, 290, 80, 30, "Designation:");
        employeePositionLabel->labelcolor(FL_WHITE);
        Fl_Input* employeePositionInput = new Fl_Input(400, 290, 200, 30);
        employeePositionInput->type(FL_NORMAL_INPUT);

        Fl_Box* employeeSalaryLabel = new Fl_Box(300, 360, 80, 30, "Salary:");
        employeeSalaryLabel->labelcolor(FL_WHITE);
        Fl_Input* employeeSalaryInput = new Fl_Input(400, 360, 200, 30);
        employeeSalaryInput->type(FL_NORMAL_INPUT);

        Fl_Button* submitBtn = new Fl_Button(250, 450, 200, 50, "Submit");
        submitBtn->color(FL_BLUE);
        submitBtn->labelcolor(FL_WHITE);
        submitBtn->callback([](Fl_Widget*, void* data) {
            fl_message("Employee added successfully!");
            ((employee*)data)->employeePage();
        }, this);

        Fl_Button* returnBtn = new Fl_Button(500, 450, 200, 50, "Return");
        returnBtn->color(FL_BLUE);
        returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback([](Fl_Widget*, void* data) {
            ((employee*)data)->employeePage();
        }, this);

        addEmployeeWindow->end();
        addEmployeeWindow->show();  
        return Fl::run();
    }

    int viewEmployee(){
        if(Fl::first_window()) Fl::first_window()->hide();

        Fl_Window* viewEmployeeWindow = new Fl_Window(900, 600, "E-Mart Employee Management");
        viewEmployeeWindow->color(FL_BLACK);

        Fl_Box* heading = new Fl_Box(0, 40, viewEmployeeWindow->w(), 50, "View Employees");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->align(FL_ALIGN_CENTER);

        Fl_Scroll* scroll = new Fl_Scroll(50, 100, viewEmployeeWindow->w() - 100, viewEmployeeWindow->h() - 200);
        scroll->color(FL_BLACK);

        linkedList l;
        linkedList::employeeNode* employeeHead = nullptr;
        string employeeList = l.displayEmployees(employeeHead);
        Fl_Multiline_Output* output = new Fl_Multiline_Output(60, 110, scroll->w() - 20, scroll->h() - 20);
        output->color(FL_WHITE);
        output->textcolor(FL_BLACK);
        output->value(employeeList.c_str());
        scroll->add(output);

        scroll->end();

        Fl_Button* returnBtn = new Fl_Button(390, viewEmployeeWindow->h() - 100, 120, 35, "Return");
        returnBtn->align(FL_ALIGN_CENTER);
        returnBtn->color(FL_BLUE);
        returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback([](Fl_Widget*, void* data) {
           ((employee*)data)->employeePage();
        }, this);

        viewEmployeeWindow->end();
        viewEmployeeWindow->show();

        return Fl::run();
    }

    void goAdminPanel(); 
};


class product{
public:
    int productPage(){
        if(Fl::first_window()) Fl::first_window()->hide();

        Fl_Window* productWindow = new Fl_Window(900, 600, "E-Mart Product Management");
        productWindow->color(FL_BLACK);

        Fl_Box* heading = new Fl_Box(0, 40, productWindow->w(), 50, "Product Management");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->align(FL_ALIGN_CENTER);

        Fl_Button* addProductBtn = new Fl_Button(350, 150, 200, 50, "Add Product");
        addProductBtn->align(FL_ALIGN_CENTER);
        addProductBtn->color(FL_BLUE);
        addProductBtn->labelcolor(FL_WHITE);
        addProductBtn->callback([](Fl_Widget*, void* d) {
            ((product*)d)->addProduct();
        }, this);

        Fl_Button* viewProductBtn = new Fl_Button(350, 250, 200, 50, "View Products");
        viewProductBtn->align(FL_ALIGN_CENTER);
        viewProductBtn->color(FL_BLUE);
        viewProductBtn->labelcolor(FL_WHITE);
        viewProductBtn->callback([](Fl_Widget*, void* data) {
            ((product*)data)->viewProduct();
        }, this);

        Fl_Button* deleteProductBtn = new Fl_Button(350, 350, 200, 50, "Delete Product");
        deleteProductBtn->align(FL_ALIGN_CENTER);
        deleteProductBtn->color(FL_BLUE);
        deleteProductBtn->labelcolor(FL_WHITE);   
        deleteProductBtn->callback([](Fl_Widget*, void* data) {
            // Functionality to delete a product
        }, this);

        Fl_Button* returnBtn = new Fl_Button(390, productWindow->h() - 150, 120, 35, "Return");
        returnBtn->align(FL_ALIGN_CENTER);
        returnBtn->color(FL_BLUE);
        returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback([](Fl_Widget*, void* data) {
           ((product*)data)->goAdminPanel();
        }, this);

        productWindow->end();
        productWindow->show();

        return Fl::run();
    }

    int addProduct(){
        if(Fl::first_window()) Fl::first_window()->hide();

        Fl_Window* addProductWindow = new Fl_Window(900, 600, "E-Mart Product Management");
        addProductWindow->color(FL_BLACK);
        Fl_Box* heading = new Fl_Box(0, 40, addProductWindow->w(), 50, "Add Product");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->align(FL_ALIGN_CENTER);

        Fl_Box* productIdLabel = new Fl_Box(300, 150, 80, 30, "Product ID:");
        productIdLabel->labelcolor(FL_WHITE);
        Fl_Input* productIdInput = new Fl_Input(400, 150, 200, 30);
        productIdInput->type(FL_NORMAL_INPUT);

        Fl_Box* productNameLabel = new Fl_Box(300, 220, 80, 30, "Product Name:");
        productNameLabel->labelcolor(FL_WHITE);
        Fl_Input* productNameInput = new Fl_Input(400, 220, 200, 30);
        productNameInput->type(FL_NORMAL_INPUT);

        Fl_Box* productPriceLabel = new Fl_Box(300, 290, 80, 30, "Product Price:");
        productPriceLabel->labelcolor(FL_WHITE);
        Fl_Input* productPriceInput = new Fl_Input(400, 290, 200, 30);
        productPriceInput->type(FL_NORMAL_INPUT);

        Fl_Box* productQuantityLabel = new Fl_Box(300, 360, 80, 30, "Quantity:");
        productQuantityLabel->labelcolor(FL_WHITE);
        Fl_Input* productQuantityInput = new Fl_Input(400, 360, 200, 30);
        productQuantityInput->type(FL_NORMAL_INPUT);

        Fl_Button* submitBtn = new Fl_Button(250, 450, 200, 50, "Submit");
        submitBtn->color(FL_BLUE);
        submitBtn->labelcolor(FL_WHITE);
        submitBtn->callback([](Fl_Widget*, void* data) {
            int id = stoi(((Fl_Input*)data)->value());
            string name = ((Fl_Input*)data)->value();
            float price = stof(((Fl_Input*)data)->value());
            int quantity = stoi(((Fl_Input*)data)->value());

            if (id < 0 || price < 0 || quantity < 0) {
                fl_alert("Invalid input! Please enter valid values.");
                return;
            }else if (name.empty()) {
                fl_alert("Product name cannot be empty!");
                return;
            }

            ((product*)data)->addProduct(id, name, price, quantity);
            ((product*)data)->productPage();
        }, this);

        Fl_Button* returnBtn = new Fl_Button(500, 450, 200, 50, "Return");
        returnBtn->color(FL_BLUE);
        returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback([](Fl_Widget*, void* data) {
            ((product*)data)->productPage();
        }, this);

        addProductWindow->end();
        addProductWindow->show();
        return Fl::run();
    }

    int viewProduct(){
        if(Fl::first_window()) Fl::first_window()->hide();

        Fl_Window* viewProductWindow = new Fl_Window(900, 600, "E-Mart Product Management");
        viewProductWindow->color(FL_BLACK);

        Fl_Box* heading = new Fl_Box(0, 40, viewProductWindow->w(), 50, "View Products");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->align(FL_ALIGN_CENTER);

        Fl_Scroll* scroll = new Fl_Scroll(50, 100, viewProductWindow->w() - 100, viewProductWindow->h() - 200);
        scroll->color(FL_BLACK);

        linkedList l;
        linkedList::productNode* productHead = nullptr;
        string productList = l.displayProducts(productHead);

        Fl_Multiline_Output* output = new Fl_Multiline_Output(60, 110, scroll->w() - 20, scroll->h() - 20);
        output->color(FL_WHITE);
        output->textcolor(FL_BLACK);
        output->value(productList.c_str());
        scroll->add(output);

        scroll->end();

        Fl_Button* returnBtn = new Fl_Button(390, viewProductWindow->h() - 100, 120, 35, "Return");
        returnBtn->align(FL_ALIGN_CENTER);
        returnBtn->color(FL_BLUE);
        returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback([](Fl_Widget*, void* data) {
           ((product*)data)->productPage();
        }, this);

        viewProductWindow->end();
        viewProductWindow->show();

        return Fl::run();
    }


    void goAdminPanel();
    void addProduct(int id, string name, float price, int quantity) {
        linkedList l;
        linkedList::productNode* productHead = nullptr;
        l.insertProduct(productHead, id, name, price, quantity);
        fl_message("Product added successfully!");

    }
};


class admin{
public:
    int adminPage(){
        Fl_Window* adminWindow = new Fl_Window(900, 600, "E-Mart Admin Panel");
        adminWindow->color(FL_BLACK);

        Fl_Box* heading = new Fl_Box(0, 40, adminWindow->w(), 50, "Admin Panel");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->align(FL_ALIGN_CENTER);

        // Buttons for different admin functionalities
        Fl_Button* manageProductsBtn = new Fl_Button(350, 150, 200, 50, "Manage Products");
        manageProductsBtn->align(FL_ALIGN_CENTER);
        manageProductsBtn->color(FL_BLUE);
        manageProductsBtn->labelcolor(FL_WHITE);
        manageProductsBtn->callback([](Fl_Widget*, void* d) {
            ((admin*)d)->goProductPage();
        }, this);

        Fl_Button* manageCustomersBtn = new Fl_Button(350, 250, 200, 50, "Manage Employees");
        manageCustomersBtn->align(FL_ALIGN_CENTER);
        manageCustomersBtn->color(FL_BLUE);
        manageCustomersBtn->labelcolor(FL_WHITE);
        manageCustomersBtn->callback([](Fl_Widget*, void* d) {
            ((admin*)d)->goEmployeePage();
        }, this);

        Fl_Button* transactionBtn = new Fl_Button(350, 350, 200, 50, "Transactions");
        transactionBtn->align(FL_ALIGN_CENTER);
        transactionBtn->color(FL_BLUE);
        transactionBtn->labelcolor(FL_WHITE);   
        transactionBtn->callback([](Fl_Widget*, void*) {
            // Functionality to view reports
        });

        Fl_Button* logoutBtn = new Fl_Button(390, adminWindow->h() - 150, 120, 35, "Logout");
        logoutBtn->align(FL_ALIGN_CENTER);
        logoutBtn->color(FL_BLUE);
        logoutBtn->labelcolor(FL_WHITE);
        logoutBtn->callback([](Fl_Widget*, void* data) {
           ((admin*)data)->gologinPage();
        }, this);

        adminWindow->end();
        adminWindow->show();

        return Fl::run();
    }

    void gologinPage();
    void goProductPage();
    void goEmployeePage();
};


class signup {
public:
    Fl_Input* passwordInput;
    Fl_Input* confirmPasswordInput;
    Fl_Check_Button* showPasswordCheck;
    Fl_Input* usernameInput;
    Fl_Input* phoneInput;
    Fl_Input* emailInput;
    Fl_Input* addressInput;

    int signUpPage(){
        Fl_Window* mainWindow = new Fl_Window(900, 600, "E-Mart Sign Up");
        mainWindow->color(FL_BLACK);

        Fl_Box* heading = new Fl_Box(0, 40, mainWindow->w(), 50, "E Mart SignUp");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->align(FL_ALIGN_CENTER);

        int elementWidth = 250;
        int elementHeight = 30;
        int labelWidth = 150;
        int labelHeight = 30;
        int spacing = 50;
        int centerX = mainWindow->w() / 2 - elementWidth / 2;
        int startY = 120;

        Fl_Box* usernameLabel = new Fl_Box(centerX - labelWidth - 10, startY, labelWidth, labelHeight, "Username:");
        usernameLabel->labelcolor(FL_WHITE);
        usernameLabel->align(FL_ALIGN_CENTER);
        Fl_Input* usernameInput = new Fl_Input(centerX, startY, elementWidth, elementHeight);

        Fl_Box* phoneLabel = new Fl_Box(centerX - labelWidth - 10, startY + spacing, labelWidth, labelHeight, "Phone:");
        phoneLabel->labelcolor(FL_WHITE);
        phoneLabel->align(FL_ALIGN_CENTER);
        Fl_Input* phoneInput = new Fl_Input(centerX, startY + spacing, elementWidth, elementHeight);

        Fl_Box* emailLabel = new Fl_Box(centerX - labelWidth - 10, startY + 2 * spacing, labelWidth, labelHeight, "Email:");
        emailLabel->labelcolor(FL_WHITE);
        emailLabel->align(FL_ALIGN_CENTER);
        Fl_Input* emailInput = new Fl_Input(centerX, startY + 2 * spacing, elementWidth, elementHeight);

        Fl_Box* addressLabel = new Fl_Box(centerX - labelWidth - 10, startY + 3 * spacing, labelWidth, labelHeight, "Address:");
        addressLabel->labelcolor(FL_WHITE);
        addressLabel->align(FL_ALIGN_CENTER);
        Fl_Input* addressInput = new Fl_Input(centerX, startY + 3 * spacing, elementWidth, elementHeight);

        Fl_Box* passwordLabel = new Fl_Box(centerX - labelWidth - 10, startY + 4 * spacing, labelWidth, labelHeight, "Password:");
        passwordLabel->labelcolor(FL_WHITE);
        passwordLabel->align(FL_ALIGN_CENTER);
        Fl_Input* passwordInput = new Fl_Input(centerX, startY + 4 * spacing, elementWidth, elementHeight);
        passwordInput->type(FL_SECRET_INPUT);

        Fl_Box* confirmPasswordLabel = new Fl_Box(centerX - labelWidth - 10, startY + 5 * spacing, labelWidth, labelHeight, "Confirm Password:");
        confirmPasswordLabel->labelcolor(FL_WHITE);
        confirmPasswordLabel->align(FL_ALIGN_CENTER);
        Fl_Input* confirmPasswordInput = new Fl_Input(centerX, startY + 5 * spacing, elementWidth, elementHeight);
        confirmPasswordInput->type(FL_SECRET_INPUT);

        Fl_Check_Button* showPasswordCheck = new Fl_Check_Button(centerX, startY + 6 * spacing, 150, 25, "Show Password");
        showPasswordCheck->labelcolor(FL_WHITE);
        showPasswordCheck->callback([](Fl_Widget*, void* data) {
            ((signup*)data)->togglePassword(nullptr, data);
        }, this);

        Fl_Button* signupButton = new Fl_Button(centerX - 40, startY + 7 * spacing, 120, 35, "Sign Up");
        signupButton->color(FL_BLUE);
        signupButton->labelcolor(FL_WHITE);
        signupButton->callback([](Fl_Widget*, void* data) {
            ((signup*)data)->validateSignUp(nullptr, data);
        }, this);

        Fl_Button* returnBtn = new Fl_Button(centerX + 110, startY + 7 * spacing, 120, 35, "Return");
        returnBtn->color(FL_BLUE);
        returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback([](Fl_Widget*, void* data) {
            ((signup*)data)->gologinPage();
        }, this);

        mainWindow->end();
        mainWindow->show();

        return Fl::run();

    }

    void gologinPage();

    void validateSignUp(Fl_Widget*, void*){

        string user = usernameInput->value();
        string pass = passwordInput->value();
        string confirmPass = confirmPasswordInput->value();
        string phone = phoneInput->value();
        string email = emailInput->value();
        string address = addressInput->value();

        // 1. Check for empty fields
        if (user.empty() || pass.empty() || confirmPass.empty() ||
            phone.empty() || email.empty() || address.empty()) {
            fl_alert("All fields are required!");
            return;
        }

        // 2. Check if passwords match
        if (pass != confirmPass) {
            fl_alert("Passwords do not match!");
            passwordInput->value("");
            confirmPasswordInput->value("");
            return;
        }

        // 3. Check if username is reserved
        if (user == "admin") {
            fl_alert("Username 'admin' is reserved.");
            return;
        }


        // 5. Success
        fl_message("Sign Up Successful! You can now choose to sign in.");

        // 6. Clear input fields
        usernameInput->value("");
        passwordInput->value("");
        confirmPasswordInput->value("");
        phoneInput->value("");
        emailInput->value("");
        addressInput->value("");
        showPasswordCheck->value(0);
        togglePassword(nullptr, nullptr); // not togglePasswordVisibility

        Fl::add_timeout(1.5, [](void*){
            Fl::delete_widget(Fl::first_window());
        }, nullptr);    
    }
        
    void togglePassword(Fl_Widget*, void*) {
        passwordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
        confirmPasswordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
        Fl::redraw();
    }

    
};



class login{
    public:
    Fl_Input* usernameInput;
    Fl_Input* passwordInput;
    Fl_Check_Button* showPasswordCheck;
    Fl_Window* loginWindow;

    int loginPage(){
        loginWindow = new Fl_Window(900, 600, "E-Mart Login");
        loginWindow->color(FL_BLACK);

        // Heading
        Fl_Box* heading = new Fl_Box(0, 40, loginWindow->w(), 50, "E Mart Login");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->labelfont(FL_BOLD);
        heading->align(FL_ALIGN_CENTER);

        // Calculate common dimensions for alignment
        int elementWidth = 250;
        int elementHeight = 30;
        int labelWidth = 150;
        int labelHeight = 30;
        int spacing = 50;
        int centerX = loginWindow->w() / 2 - elementWidth / 2; // Horizontal centering
        int startY = loginWindow->h() / 3; // Start Y position for elements

        // Username
        Fl_Box* usernameLabel = new Fl_Box(centerX - labelWidth - 10, startY, labelWidth, labelHeight, "Username:");
        usernameLabel->labelcolor(FL_WHITE);
        usernameLabel->align(FL_ALIGN_CENTER);
        Fl_Input* usernameInput = new Fl_Input(centerX, startY, elementWidth, elementHeight);

        // Password
        Fl_Box* passwordLabel = new Fl_Box(centerX - labelWidth - 10, startY + spacing, labelWidth, labelHeight, "Password:");
        passwordLabel->labelcolor(FL_WHITE);
        passwordLabel->align(FL_ALIGN_CENTER);
        Fl_Input* passwordInput = new Fl_Input(centerX, startY + spacing, elementWidth, elementHeight);
        passwordInput->type(FL_SECRET_INPUT);

        // Show Password Check
        Fl_Check_Button* showPasswordCheck = new Fl_Check_Button(centerX, startY + 2 * spacing, elementWidth, elementHeight, "Show Password");
        showPasswordCheck->labelcolor(FL_WHITE);
        showPasswordCheck->callback([](Fl_Widget*, void* data) {
            ((login*)data)->togglePassword();
        }, this);

        // Login Button
        Fl_Button* loginButton = new Fl_Button(centerX - 20, startY + 3 * spacing, 120, 35, "Login");
        loginButton->color(FL_BLUE);
        loginButton->labelcolor(FL_WHITE);
        loginButton->callback([](Fl_Widget*, void* data) {
            ((login*)data)->verifyUser();
        }, this);

        // Sign Up Button
        Fl_Button* signupButton = new Fl_Button(centerX + 140, startY + 3 * spacing, 120, 35, "Sign Up");
        signupButton->color(FL_BLUE);
        signupButton->labelcolor(FL_WHITE);
        signupButton->callback([](Fl_Widget*, void* data) {
            ((login*)data)->gosignUpPage();
        }, this);

        loginWindow->end();
        loginWindow->show();

        return Fl::run();
    }

    void verifyUser();


    void gosignUpPage();


    void togglePassword() {
        passwordInput->type(showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
        Fl::redraw();
    }
};


void signup::gologinPage() {
        if (Fl::first_window()) Fl::first_window()->hide();

        login* log = new login();
        log->loginPage();
    }

void login::gosignUpPage() {
        if (loginWindow) loginWindow->hide();
        
        signup* s = new signup();
        s->signUpPage();
    }

void login::verifyUser() {
        if (Fl::first_window()) Fl::first_window()->hide();
        
        admin* a = new admin();
        a->adminPage();
    }

void admin::gologinPage() {
        if (Fl::first_window()) Fl::first_window()->hide();

        login* log = new login();
        log->loginPage();
    }

void admin::goProductPage() {
        if (Fl::first_window()) Fl::first_window()->hide();

        product* p = new product();
        p->productPage();
    }  

void admin::goEmployeePage() {
        if (Fl::first_window()) Fl::first_window()->hide();

        employee* e = new employee();
        e->employeePage();
    }

void product::goAdminPanel() {
        if (Fl::first_window()) Fl::first_window()->hide();

        admin* a = new admin();
        a->adminPage();
    }

void employee::goAdminPanel() {
        if (Fl::first_window()) Fl::first_window()->hide();

        admin* a = new admin();
        a->adminPage();
    }

 
string linkedList::displayProducts(productNode* head) {
        if (head == nullptr) {
            return "No products available.";
        }

        string result = "ID\tName\t\tPrice\tQuantity\n";
        productNode* temp = head;
        while (temp != nullptr) {
            result += to_string(temp->productId) + "\t" +
                    temp->productName + "\t\t" +
                    to_string(temp->productPrice) + "\t" +
                    to_string(temp->productQuantity) + "\n";
            temp = temp->next;
        }
        return result;
    }    
string linkedList::displayEmployees(employeeNode* head) {
        if (head == nullptr) {
            return "No employees available.";
        }

        string result = "ID\tName\t\tPosition\tSalary\n";
        employeeNode* temp = head;
        while (temp != nullptr) {
            result += to_string(temp->employeeId) + "\t" +
                    temp->employeeName + "\t\t" +
                    temp->employeePosition + "\t" +
                    to_string(temp->employeeSalary) + "\n";
            temp = temp->next;
        }
        return result;
    }



int main() {
    login log;
    log.loginPage();
    return Fl::run();
}   