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
#include <stdexcept>
#include <fstream>
#include <iomanip>

using namespace std; 

class signup;
class login;
class admin;
class product;
class employee;
class customer;
class transaction;
class queue;
class linkedList;


class queue{

public:
    struct cartNode {
        int productId;
        int quantity;
        float price;
        string username;
        string password;
        string address;
        int phone;
        int totalBill;
        cartNode* next;
    };

    cartNode* front;
    cartNode* rear;

    queue() {
        front = rear = nullptr;
    }

    cartNode* createCartNode(int productId, int quantity, float price, string username, string password, string address, int phone, int totalBill) {
        cartNode* newNode = new cartNode;
        newNode->productId = productId;
        newNode->quantity = quantity;
        newNode->price = price;
        newNode->username = username;
        newNode->password = password;
        newNode->address = address;
        newNode->phone = phone;
        newNode->totalBill = totalBill;
        newNode->next = nullptr;
        return newNode;
    }

    void enqueue(int productId, int quantity, float price, string username, string password, string address, int phone, int totalBill) {
        cartNode* newNode = createCartNode(productId, quantity, price, username, password, address, phone, totalBill);
        if (rear == nullptr) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
    }

    bool isEmpty() {
        return front == nullptr;
    }

    void loadCartFromFile(queue& queue) {
        ifstream file("cart.txt");
        if (!file.is_open()) {
            fl_alert("Failed to open cart.txt");
            return;
        }

        int productId, quantity, phone, totalBill;
        float price;
        string username, password, address;

        while (file >> productId >> quantity >> price >> username >> password >> phone >> address >> totalBill) {
            queue.enqueue(productId, quantity, price, username, password, address, phone, totalBill);
        }
        file.close();
    }

    string displayGroupedCart(queue& q) {
        stringstream ss;
        queue::cartNode* outer = q.front;

        while (outer != nullptr) {
            bool alreadyShown = false;
            queue::cartNode* checker = q.front;
            while (checker != outer) {
                if (checker->username == outer->username &&
                    checker->password == outer->password &&
                    checker->phone == outer->phone &&
                    checker->address == outer->address) {
                    alreadyShown = true;
                    break;
                }
                checker = checker->next;
            }

            if (!alreadyShown) {
                ss << "\nCustomer Name: " << outer->username
                << "\tPhone: " << outer->phone
                << "\tAddress: " << outer->address << "\n";
                
                ss << left << setw(12) << "Product ID"
                << setw(12) << "Quantity"
                << setw(12) << "Price"
                << setw(12) << "Total" << "\n";
                ss << string(50, '-') << "\n";

                int grandTotal = 0;

                queue::cartNode* inner = q.front;
                while (inner != nullptr) {
                    if (inner->username == outer->username &&
                        inner->password == outer->password &&
                        inner->phone == outer->phone &&
                        inner->address == outer->address) {
                        
                        int itemTotal = inner->quantity * inner->price;
                        grandTotal += itemTotal;

                        ss << left
                        << setw(12) << inner->productId
                        << setw(12) << inner->quantity
                        << setw(12) << fixed << setprecision(2) << inner->price
                        << setw(12) << itemTotal << "\n";
                    }
                    inner = inner->next;
                }

                ss << string(50, '-') << "\n";
                ss << "Total Bill: " << grandTotal << "\n";
                ss << string(50, '=') << "\n";
            }

            outer = outer->next;
        }

        return ss.str();
    }
};

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

    struct customerNode {
        string username;
        int phone;
        string address;
        string password;
        customerNode* next;
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

    bool exists(productNode* head, int id) {
        productNode* temp = head;
        while (temp != nullptr) {
            if (temp->productId == id)
                return true;
            temp = temp->next;
        }
        return false;
    }

    void clearProduct(productNode*& head) {
        while (head != nullptr) {
            productNode* temp = head;
            head = head->next;
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
        if (head == nullptr) return;
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

    bool exists(employeeNode* head, int id) {
        employeeNode* temp = head;
        while (temp != nullptr) {
            if (temp->employeeId == id)
                return true;
            temp = temp->next;
        }
        return false;
    }

    string displayEmployees(employeeNode* head); 



    customerNode* createCustomerNode(string username, int phone, string address, string password) {
        customerNode* newNode = new customerNode;
        newNode->username = username;
        newNode->phone = phone;
        newNode->address = address;
        newNode->password = password;
        newNode->next = nullptr;
        return newNode;
    }

    void insertCustomer(customerNode*& head, string username, int phone, string address, string password) {
        customerNode* newNode = createCustomerNode(username, phone, address, password);
        if (head == nullptr) {
            head = newNode;
        } else {
            customerNode* temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    void deleteCustomer(customerNode*& head, string username) {
        if (head == nullptr) return; // List is empty
        if (head->username == username) {
            customerNode* temp = head;
            head = head->next;
            delete temp;
            return;
        }
        customerNode* current = head;
        while (current->next != nullptr && current->next->username != username) {
            current = current->next;
        }
        if (current->next != nullptr) {
            customerNode* temp = current->next;
            current->next = current->next->next;
            delete temp;
        }
    }

    bool existsCustomer(customerNode* head, string username, string password) {
        customerNode* temp = head;
        while (temp != nullptr) {
            if (temp->username == username && temp->password == password)
                return true;
            temp = temp->next;
        }
        return false;
    }

    string displayCustomers(customerNode* head);

};


class customer{
public:
    linkedList l;
    linkedList::customerNode* customerHead = nullptr;
    linkedList::productNode* productHead = nullptr;
    queue::cartNode* cartHead = nullptr;

    Fl_Input* productIdInput;
    Fl_Input* quantityInput;

    string user, pass, address;
    int phone;
    
    int customerPanel(string username, string password, int phone, string address){
        if(Fl::first_window()) Fl::first_window()->hide();

        this->user = username;
        this->pass = password;
        this->phone = phone;
        this->address = address;

        Fl_Window* customerWindow = new Fl_Window(900, 600, "E-Mart Customer Panel");
        customerWindow->color(FL_BLACK);

        Fl_Box* heading = new Fl_Box(0, 40, customerWindow->w(), 50, "Customer Panel");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->align(FL_ALIGN_CENTER);

        Fl_Box* welcomeMessage = new Fl_Box(0, 100, customerWindow->w(), 30, ("Welcome, " + username).c_str());
        welcomeMessage->labelsize(20);
        welcomeMessage->labelcolor(FL_WHITE);
        welcomeMessage->align(FL_ALIGN_CENTER);

        Fl_Scroll * scroll = new Fl_Scroll(50, 150, customerWindow->w() - 400, customerWindow->h() - 250);
        scroll->color(FL_BLACK);
        string product = l.displayProducts(productHead);
        Fl_Multiline_Output* output = new Fl_Multiline_Output(60, 160, scroll->w() - 20, scroll->h() - 20);
        output->color(FL_WHITE);
        output->textcolor(FL_BLACK);
        output->value(product.c_str());
        scroll->add(output);
        scroll->end();

        Fl_Box* productIdLabel = new Fl_Box(600, customerWindow->h() - 420, 80, 30, "Product ID:");
        productIdLabel->labelcolor(FL_WHITE);

        productIdInput = new Fl_Input(700, customerWindow->h() - 420, 150, 30);

        Fl_Box* quantityLabel = new Fl_Box(600, customerWindow->h() - 380, 80, 30, "Quantity:");
        quantityLabel->labelcolor(FL_WHITE);
        quantityInput = new Fl_Input(700, customerWindow->h() - 380, 150, 30);

        Fl_Button* addToCartBtn = new Fl_Button(650, customerWindow->h() - 280, 150, 30, "Add to Cart");
        addToCartBtn->color(FL_BLUE);
        addToCartBtn->labelcolor(FL_WHITE);

        addToCartBtn->callback([](Fl_Widget*, void* data) {
            customer* c = (customer*)data;
            string idStr = c->productIdInput->value();
            string quantityStr = c->quantityInput->value();

            if (idStr.empty()) {
                fl_alert("Please enter a product ID.");
                return;
            }

            if (quantityStr.empty()) {
                fl_alert("Please enter a quantity.");
                return;
            }

            int id;
            int quantity;

            try{
                id = stoi(idStr);
            } catch (const invalid_argument&) {
                fl_alert("Invalid ID! Please enter valid numbers.");
                return;
            }

            try {
                quantity = stoi(quantityStr);
            } catch (const invalid_argument&) {
                fl_alert("Invalid Quantity! Please enter valid numbers.");
                return;
            }

            if (id < 0) {
                fl_alert("Product ID cannot be negative.");
                return;
            }

            if (quantity <= 0) {
                fl_alert("Quantity must be greater than zero.");
                return;
            }


            bool foundInFile = false;
            int totalBill = 0;
            int price = 0;
            string productName;

            ifstream inFile("products.txt");
            ofstream outFile("temp.txt");

            string line;
            while (getline(inFile, line)) {
                stringstream ss(line);
                int fileId, fileQuantity;
                float filePrice;
                string name;
                ss >> fileId >> name >> filePrice >> fileQuantity;

                if (fileId == id) {
                    foundInFile = true;

                    if (fileQuantity < quantity) {
                        fl_alert("Insufficient stock in file.");
                        inFile.close();
                        outFile.close();
                        remove("temp.txt");
                        return;
                    }

                    fileQuantity -= quantity;
                    totalBill = quantity * filePrice;
                    price = filePrice;
                    productName = name;

                    outFile << fileId << "\t" << name << "\t" << filePrice << "\t" << fileQuantity << '\n';
                } else {
                    outFile << line << '\n';
                }
            }

            inFile.close();
            outFile.close();

            if (foundInFile) {
                remove("products.txt");
                rename("temp.txt", "products.txt");
            } else {
                remove("temp.txt");
                fl_alert("Product not found in file.");
                return;
            }

            linkedList::productNode* product = c->productHead;
            while (product != nullptr) {
                if (product->productId == id) {
                    if (product->productQuantity < quantity) {
                        fl_alert("Insufficient stock in memory.");
                        return;
                    }
                    product->productQuantity -= quantity;
                    break;
                }
                product = product->next;
            }

            ofstream file("cart.txt", ios::app);
            if (file.is_open()) {
                file << id << "\t" << quantity << "\t" << price << "\t" << c->user << "\t" << c->pass
                    << "\t" << c->phone << "\t" << c->address << "\t" << totalBill << "\n";
                file.close();
            } else {
                fl_alert("Error opening file to save cart data.");
                return;
            }

            string msg = "Product added to cart successfully!\nProduct: " + productName +
                        "\nPrice per item: " + to_string(price) +
                        "\nTotal: " + to_string(totalBill);
            fl_message(msg.c_str());
        }, this);


        Fl_Button* returnBtn = new Fl_Button(665, customerWindow->h() - 230, 120, 35, "Return");
        returnBtn->align(FL_ALIGN_CENTER);
        returnBtn->color(FL_BLUE);
        returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback([](Fl_Widget*, void* data) {
            ((customer*)data)->goLoginPanel();
        }, this);


        customerWindow->end();
        customerWindow->show();

        return Fl::run();
    }

    void goLoginPanel();
};


class employee{
public:
    linkedList l;
    linkedList::employeeNode* empHead = nullptr;

    Fl_Input* employeeIdInput;
    Fl_Input* employeeNameInput;
    Fl_Input* employeePositionInput;
    Fl_Input* employeeSalaryInput;

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
        }, this);

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
            ((employee*)data)->deleteEmployee();
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
        employeeIdInput = new Fl_Input(400, 150, 200, 30);
        employeeIdInput->type(FL_NORMAL_INPUT);

        Fl_Box* employeeNameLabel = new Fl_Box(300, 220, 80, 30, "Employee Name:");
        employeeNameLabel->labelcolor(FL_WHITE);
        employeeNameInput = new Fl_Input(400, 220, 200, 30);
        employeeNameInput->type(FL_NORMAL_INPUT);

        Fl_Box* employeePositionLabel = new Fl_Box(300, 290, 80, 30, "Designation:");
        employeePositionLabel->labelcolor(FL_WHITE);
        employeePositionInput = new Fl_Input(400, 290, 200, 30);
        employeePositionInput->type(FL_NORMAL_INPUT);

        Fl_Box* employeeSalaryLabel = new Fl_Box(300, 360, 80, 30, "Salary:");
        employeeSalaryLabel->labelcolor(FL_WHITE);
        employeeSalaryInput = new Fl_Input(400, 360, 200, 30);
        employeeSalaryInput->type(FL_NORMAL_INPUT);

        Fl_Button* submitBtn = new Fl_Button(250, 450, 200, 50, "Submit");
        submitBtn->color(FL_BLUE);
        submitBtn->labelcolor(FL_WHITE);
        submitBtn->callback([](Fl_Widget*, void* data) {
             employee* emp = (employee*)data;

            try {
                int id = stoi(emp->employeeIdInput->value());
                string name = emp->employeeNameInput->value();
                float salary = stof(emp->employeeSalaryInput->value());
                string designation = emp->employeePositionInput->value();

                if (id < 0 || salary < 0) {
                    fl_alert("Invalid input! Please enter valid values.");
                    return;
                }
                if (name.empty() || designation.empty()) {
                    fl_alert("Fields cannot be empty!");
                    return;
                }

                if (emp->l.exists(emp->empHead, id)) {
                    fl_alert("Employee ID already exists!");
                    return;
                }

                emp->addEmployee(id, name, designation, salary);

                ofstream file("employee.txt", ios::app);
                if (file.is_open()) {
                    file << id << "\t" << name << "\t" << designation << "\t" << salary << "\n";
                    file.close();
                } else {
                    fl_alert("Error saving employee data to file.");
                    return;
                }

                emp->employeeIdInput->value("");
                emp->employeeNameInput->value("");
                emp->employeePositionInput->value("");
                emp->employeeSalaryInput->value("");
                fl_message("Employee added successfully!");
                emp->employeePage();

            } catch (const invalid_argument&) {
                fl_alert("Invalid input! Ensure all fields are correct.");
            } catch (const out_of_range&) {
                fl_alert("Input values are out of range.");
            }
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

    int deleteEmployee(){
        if(Fl::first_window()) Fl::first_window()->hide();

        Fl_Window* deleteEmployeeWindow = new Fl_Window(900, 600, "E-Mart Employee Management");
        deleteEmployeeWindow->color(FL_BLACK);

        Fl_Box* heading = new Fl_Box(0, 40, deleteEmployeeWindow->w(), 50, "Delete Employee");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->align(FL_ALIGN_CENTER);

        Fl_Scroll* scroll = new Fl_Scroll(50, 100, deleteEmployeeWindow->w() - 400, deleteEmployeeWindow->h() - 200);
        scroll->color(FL_BLACK);
        
        string employeeList = l.displayEmployees(empHead);

        Fl_Multiline_Output* output = new Fl_Multiline_Output(60, 110, scroll->w() - 20, scroll->h() - 20);
        output->color(FL_WHITE);
        output->textcolor(FL_BLACK);
        output->value(employeeList.c_str());

        scroll->add(output);
        scroll->end();

        int rightStartX = 550; // X-position for right side elements

        Fl_Box* employeeIdLabel = new Fl_Box(rightStartX, 150, 100, 30, "Employee ID:");
        employeeIdLabel->labelcolor(FL_WHITE);

        employeeIdInput = new Fl_Input(rightStartX + 110, 150, 200, 30);
        employeeIdInput->type(FL_NORMAL_INPUT);

        Fl_Button* submitBtn = new Fl_Button(rightStartX + 100, 220, 150, 40, "Delete");
        submitBtn->color(FL_BLUE);
        submitBtn->labelcolor(FL_WHITE);
        submitBtn->callback([](Fl_Widget*, void* data) {
            employee* emp = (employee*)data;
            string idStr = emp->employeeIdInput->value();
            if (idStr.empty()) {
                fl_alert("Please enter a product ID.");
                return;
            }

            int id;
            try {
                id = stoi(idStr);
            } catch (const invalid_argument&) {
                fl_alert("Invalid ID. Please enter a number.");
                return;
            }

            if (id < 0) {
                fl_alert("Product ID cannot be negative.");
                return;
            }

            bool foundInFile = false;
            ifstream inFile("employee.txt");
            ofstream outFile("temp.txt");

            string line;
            while (getline(inFile, line)) {
                int currentId = stoi(line.substr(0, line.find('\t')));
                if (currentId == id) {
                    foundInFile = true;
                    continue;
                }
                outFile << line << '\n';
            }

            inFile.close();
            outFile.close();

            if (foundInFile) {
                remove("employee.txt");
                rename("temp.txt", "employee.txt");
            } else {
                remove("temp.txt");
            }

            bool foundInList = emp->l.exists(emp->empHead, id);
            if (foundInList) {
                emp->l.deleteEmployee(emp->empHead, id);
            }

            if (foundInFile || foundInList) {
                fl_alert("employee deleted successfully!");
                emp->employeeIdInput->value("");
                emp->employeePage();
            } else {
                fl_alert("Employee ID not found in file or memory.");
            }
        }, this);

        Fl_Button* returnBtn = new Fl_Button(rightStartX + 100, 280, 150, 40, "Return");
        returnBtn->color(FL_BLUE);
        returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback([](Fl_Widget*, void* data) {
            ((employee*)data)->employeePage();
        }, this);
     
        deleteEmployeeWindow->end();
        deleteEmployeeWindow->show();
        return Fl::run();
    }

    void goAdminPanel(); 

    void addEmployee(int id, string name, string position, float salary) {
        l.insertEmployee(empHead, id, name, position, salary);
    }
};



class transaction{
public:
    linkedList l;
    linkedList::customerNode* customerHead = nullptr;
    linkedList::productNode* productHead = nullptr;
    queue::cartNode* cartHead = nullptr;

    
    int transactionPage(){
        if(Fl::first_window()) Fl::first_window()->hide();

        Fl_Window* transactionWindow = new Fl_Window(900, 600, "E-Mart");
        transactionWindow->color(FL_BLACK);

        Fl_Box* heading = new Fl_Box(0, 40, transactionWindow->w(), 50, "Transactions");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->align(FL_ALIGN_CENTER);

        Fl_Scroll * scroll = new Fl_Scroll(50, 100, transactionWindow->w() - 100, transactionWindow->h() - 200);
        scroll->color(FL_BLACK);

        queue q;
        q.loadCartFromFile(q);
        string cartContent = q.displayGroupedCart(q);
        if (cartContent.empty()) {
            cartContent = "Your history is empty.";
        }

        Fl_Multiline_Output* output = new Fl_Multiline_Output(60, 110, scroll->w() - 20, scroll->h() - 20);
        output->color(FL_WHITE);
        output->textcolor(FL_BLACK);
        output->value(cartContent.c_str());
        scroll->add(output);

        scroll->end();

        Fl_Button* returnBtn = new Fl_Button(390, transactionWindow->h() - 100, 120, 35, "Return");
        returnBtn->align(FL_ALIGN_CENTER);
        returnBtn->color(FL_BLUE);
        returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback([](Fl_Widget*, void* data) {
            ((transaction*)data)->goAdminPanel();
        }, this);

        transactionWindow->end();
        transactionWindow->show();

        return Fl::run();
    }

    void goAdminPanel(); 
};


class product{
public:
    linkedList l;
    linkedList::productNode* productHead = nullptr;
    Fl_Input* productIdInput;
    Fl_Input* productNameInput;
    Fl_Input* productPriceInput;
    Fl_Input* productQuantityInput;


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
            product* p = (product*)data;
            p->l.clearProduct(p->productHead); 
            p->viewProduct();
        }, this);

        Fl_Button* deleteProductBtn = new Fl_Button(350, 350, 200, 50, "Delete Product");
        deleteProductBtn->align(FL_ALIGN_CENTER);
        deleteProductBtn->color(FL_BLUE);
        deleteProductBtn->labelcolor(FL_WHITE);   
        deleteProductBtn->callback([](Fl_Widget*, void* data) {
            ((product*)data)->deleteProduct();
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
        productIdInput = new Fl_Input(400, 150, 200, 30);
        productIdInput->type(FL_NORMAL_INPUT);

        Fl_Box* productNameLabel = new Fl_Box(300, 220, 80, 30, "Product Name:");
        productNameLabel->labelcolor(FL_WHITE);
        productNameInput = new Fl_Input(400, 220, 200, 30);
        productNameInput->type(FL_NORMAL_INPUT);

        Fl_Box* productPriceLabel = new Fl_Box(300, 290, 80, 30, "Product Price:");
        productPriceLabel->labelcolor(FL_WHITE);
        productPriceInput = new Fl_Input(400, 290, 200, 30);
        productPriceInput->type(FL_NORMAL_INPUT);

        Fl_Box* productQuantityLabel = new Fl_Box(300, 360, 80, 30, "Quantity:");
        productQuantityLabel->labelcolor(FL_WHITE);
        productQuantityInput = new Fl_Input(400, 360, 200, 30);
        productQuantityInput->type(FL_NORMAL_INPUT);

        Fl_Button* submitBtn = new Fl_Button(250, 450, 200, 50, "Submit");
        submitBtn->color(FL_BLUE);
        submitBtn->labelcolor(FL_WHITE);
        submitBtn->callback([](Fl_Widget*, void* data) {
             product* p = (product*)data;

            try {
                int id = stoi(p->productIdInput->value());
                string name = p->productNameInput->value();
                float price = stof(p->productPriceInput->value());
                int quantity = stoi(p->productQuantityInput->value());

                if (id < 0 || price < 0 || quantity < 0) {
                    fl_alert("Invalid input! Please enter valid values.");
                    return;
                }
                if (name.empty()) {
                    fl_alert("Product name cannot be empty!");
                    return;
                }

                if (p->l.exists(p->productHead, id)) {
                    fl_alert("Product ID already exists!");
                    return;
                }

                p->addProduct(id, name, price, quantity);
                p->productIdInput->value("");   
                p->productNameInput->value("");
                p->productPriceInput->value("");
                p->productQuantityInput->value("");
                fl_alert("Product added successfully!");
                
                ofstream file("products.txt", ios::app);
                if (file.is_open()) {
                    file << id << "\t" << name << "\t" << price << "\t" << quantity << "\n";
                    file.close();
                }else {
                    fl_alert("Error opening file to save product data.");
                }
                
                p->productPage();

            } catch (const invalid_argument&) {
                fl_alert("Invalid input! Ensure all fields are filled correctly.");
            } catch (const out_of_range&) {
                fl_alert("Input values are out of range. Please try again.");
            }
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

    int deleteProduct(){
        if(Fl::first_window()) Fl::first_window()->hide();

        Fl_Window* deleteProductWindow = new Fl_Window(900, 600, "E-Mart Product Management");
        deleteProductWindow->color(FL_BLACK);

        Fl_Box* heading = new Fl_Box(0, 40, deleteProductWindow->w(), 50, "Delete Product");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->align(FL_ALIGN_CENTER);

        Fl_Scroll* scroll = new Fl_Scroll(50, 100, deleteProductWindow->w() - 400, deleteProductWindow->h() - 200);
        scroll->color(FL_BLACK);
        
        string productList = l.displayProducts(productHead);

        Fl_Multiline_Output* output = new Fl_Multiline_Output(60, 110, scroll->w() - 20, scroll->h() - 20);
        output->color(FL_WHITE);
        output->textcolor(FL_BLACK);
        output->value(productList.c_str());

        scroll->add(output);
        scroll->end();

        int rightStartX = 550; // X-position for right side elements

        Fl_Box* productIdLabel = new Fl_Box(rightStartX, 150, 100, 30, "Product ID:");
        productIdLabel->labelcolor(FL_WHITE);

        productIdInput = new Fl_Input(rightStartX + 110, 150, 200, 30);
        productIdInput->type(FL_NORMAL_INPUT);

        Fl_Button* submitBtn = new Fl_Button(rightStartX + 100, 220, 150, 40, "Delete");
        submitBtn->color(FL_BLUE);
        submitBtn->labelcolor(FL_WHITE);
        submitBtn->callback([](Fl_Widget*, void* data) {
            product* p = (product*)data;
            string idStr = p->productIdInput->value();
            if (idStr.empty()) {
                fl_alert("Please enter a product ID.");
                return;
            }

            int id;
            try {
                id = stoi(idStr);
            } catch (const invalid_argument&) {
                fl_alert("Invalid ID. Please enter a number.");
                return;
            }

            if (id < 0) {
                fl_alert("Product ID cannot be negative.");
                return;
            }

            bool foundInFile = false;
            ifstream inFile("products.txt");
            ofstream outFile("temp.txt");

            string line;
            while (getline(inFile, line)) {
                int currentId = stoi(line.substr(0, line.find('\t')));
                if (currentId == id) {
                    foundInFile = true;
                    continue;
                }
                outFile << line << '\n';
            }

            inFile.close();
            outFile.close();

            if (foundInFile) {
                remove("products.txt");
                rename("temp.txt", "products.txt");
            } else {
                remove("temp.txt");
            }

            bool foundInList = p->l.exists(p->productHead, id);
            if (foundInList) {
                p->l.deleteProduct(p->productHead, id);
            }

            if (foundInFile || foundInList) {
                fl_message("Product deleted successfully!");
                p->productIdInput->value("");
                p->productPage();
            } else {
                fl_alert("Product ID not found in file or memory.");
            }
        }, this);

        Fl_Button* returnBtn = new Fl_Button(rightStartX + 100, 280, 150, 40, "Return");
        returnBtn->color(FL_BLUE);
        returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback([](Fl_Widget*, void* data) {
            ((product*)data)->productPage();
        }, this);

        deleteProductWindow->end();
        deleteProductWindow->show();
        
        return Fl::run();
    }


    void goAdminPanel();
    void addProduct(int id, string name, float price, int quantity) {
        l.insertProduct(productHead, id, name, price, quantity);
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
        transactionBtn->callback([](Fl_Widget*, void* d) {
            ((admin*)d)->goTransactionPage();
        }, this);

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
    void goTransactionPage();
};


class signup {
public:
    linkedList l;
    linkedList::customerNode* customerHead = nullptr;

    Fl_Input* usernameInput;
    Fl_Input* phoneInput;
    Fl_Input* addressInput;
    Fl_Input* passwordInput;
    Fl_Input* confirmPasswordInput;
    Fl_Check_Button* showPasswordCheck;

    
    void gologinPage();

    int signUpPage(){
        if(Fl::first_window()) Fl::first_window()->hide();
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
        this->usernameInput = new Fl_Input(centerX, startY, elementWidth, elementHeight);

        Fl_Box* phoneLabel = new Fl_Box(centerX - labelWidth - 10, startY + spacing, labelWidth, labelHeight, "Phone:");
        phoneLabel->labelcolor(FL_WHITE);
        phoneLabel->align(FL_ALIGN_CENTER);
        this->phoneInput = new Fl_Input(centerX, startY + spacing, elementWidth, elementHeight);

        Fl_Box* addressLabel = new Fl_Box(centerX - labelWidth - 10, startY + 2 * spacing, labelWidth, labelHeight, "Address:");
        addressLabel->labelcolor(FL_WHITE);
        addressLabel->align(FL_ALIGN_CENTER);
        this->addressInput = new Fl_Input(centerX, startY + 2 * spacing, elementWidth, elementHeight);

        Fl_Box* passwordLabel = new Fl_Box(centerX - labelWidth - 10, startY + 3 * spacing, labelWidth, labelHeight, "Password:");
        passwordLabel->labelcolor(FL_WHITE);
        passwordLabel->align(FL_ALIGN_CENTER);
        this->passwordInput = new Fl_Input(centerX, startY + 3 * spacing, elementWidth, elementHeight);
        passwordInput->type(FL_SECRET_INPUT);

        Fl_Box* confirmPasswordLabel = new Fl_Box(centerX - labelWidth - 10, startY + 4 * spacing, labelWidth, labelHeight, "Confirm Password:");
        confirmPasswordLabel->labelcolor(FL_WHITE);
        confirmPasswordLabel->align(FL_ALIGN_CENTER);
        this->confirmPasswordInput = new Fl_Input(centerX, startY + 4 * spacing, elementWidth, elementHeight);
        confirmPasswordInput->type(FL_SECRET_INPUT);

        this->showPasswordCheck = new Fl_Check_Button(centerX, startY + 5 * spacing, 150, 25, "Show Password");
        showPasswordCheck->labelcolor(FL_WHITE);
        showPasswordCheck->callback([](Fl_Widget*, void* data) {
            signup* s = (signup*)data;
            s->confirmPasswordInput->type(s->showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
            s->passwordInput->type(s->showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
            s->confirmPasswordInput->redraw();
            s->passwordInput->redraw();
        }, this);

        Fl_Button* signupButton = new Fl_Button(centerX - 40, startY + 6 * spacing, 120, 35, "Sign Up");
        signupButton->color(FL_BLUE);
        signupButton->labelcolor(FL_WHITE);
        signupButton->callback([](Fl_Widget*, void* data) {
            signup* s = (signup*)data;

            try{

                string username = s->usernameInput->value();
                string phone = s->phoneInput->value();
                string address = s->addressInput->value();
            string password = s->passwordInput->value();
            string confirmPassword = s->confirmPasswordInput->value();

            if (username.empty() || phone.empty() || address.empty() || password.empty() || confirmPassword.empty()) {
                fl_alert("All fields must be filled out!");
                return;
            }
            if (password != confirmPassword) {
                fl_alert("Passwords do not match!");
                return;
            }
            if (password.length() < 4) {
                fl_alert("Password must be at least 4 characters long!");
                return;
            }

            int phoneNumber;
            try {
                phoneNumber = stoi(phone);
            } catch (const invalid_argument&) {
                fl_alert("Invalid phone number! Please enter a valid number.");
                return;
            }

            if (phoneNumber < 0) {
                fl_alert("Phone number cannot be negative!");
                return;
            }
            
            ifstream file("users.txt");
            if (file.is_open()) {
                string line;
                while (getline(file, line)) {
                    if (line.find(username) != string::npos) {
                        fl_alert("Username already exists!");
                        file.close();
                        return;
                    }
                }
                file.close();
            }

            s->l.insertCustomer(s->customerHead, username, phoneNumber, address, password);
            fl_alert("Sign up successful!");
            s->gologinPage();
            
            ofstream outFile("users.txt", ios::app);
            if (outFile.is_open()) {
                outFile << username << "\t" << phone << "\t" << address << "\t" << password << "\n";
                outFile.close();
            }
            
        } catch (const invalid_argument&) {
            fl_alert("Invalid input! Please ensure all fields are filled correctly.");
        } catch (const out_of_range&) {
            fl_alert("Input values are out of range. Please try again.");
        }
            
        }, this);

        Fl_Button* returnBtn = new Fl_Button(centerX + 110, startY + 6 * spacing, 120, 35, "Return");
        returnBtn->color(FL_BLUE);
        returnBtn->labelcolor(FL_WHITE);
        returnBtn->callback([](Fl_Widget*, void* data) {
            ((signup*)data)->gologinPage();
        }, this);

        mainWindow->end();
        mainWindow->show();

        return Fl::run();

    }

        
};



class login{
    public:
    Fl_Input* usernameInput;
    Fl_Input* passwordInput;
    Fl_Check_Button* showPasswordCheck;
    Fl_Window* loginWindow;

    linkedList l;
    linkedList::customerNode* customerHead = nullptr;

    int loginPage(){

        if (Fl::first_window()) Fl::first_window()->hide();
        loginWindow = new Fl_Window(900, 600, "E-Mart Login");
        loginWindow->color(FL_BLACK);

        Fl_Box* heading = new Fl_Box(0, 40, loginWindow->w(), 50, "E Mart Login");
        heading->labelsize(30);
        heading->labelcolor(FL_WHITE);
        heading->labelfont(FL_BOLD);
        heading->align(FL_ALIGN_CENTER);

        int elementWidth = 250;
        int elementHeight = 30;
        int labelWidth = 150;
        int labelHeight = 30;
        int spacing = 50;
        int centerX = loginWindow->w() / 2 - elementWidth / 2; // Horizontal centering
        int startY = loginWindow->h() / 3; // Start Y position for elements

        Fl_Box* usernameLabel = new Fl_Box(centerX - labelWidth - 10, startY, labelWidth, labelHeight, "Username:");
        usernameLabel->labelcolor(FL_WHITE);
        usernameLabel->align(FL_ALIGN_CENTER);
        usernameInput = new Fl_Input(centerX, startY, elementWidth, elementHeight);

        Fl_Box* passwordLabel = new Fl_Box(centerX - labelWidth - 10, startY + spacing, labelWidth, labelHeight, "Password:");
        passwordLabel->labelcolor(FL_WHITE);
        passwordLabel->align(FL_ALIGN_CENTER);
        passwordInput = new Fl_Input(centerX, startY + spacing, elementWidth, elementHeight);
        passwordInput->type(FL_SECRET_INPUT);

        showPasswordCheck = new Fl_Check_Button(centerX, startY + 2 * spacing, elementWidth, elementHeight, "Show Password");
        showPasswordCheck->labelcolor(FL_WHITE);
        showPasswordCheck->callback([](Fl_Widget*, void* data) {
            login* log = (login*)data;
            log->passwordInput->type(log->showPasswordCheck->value() ? FL_NORMAL_INPUT : FL_SECRET_INPUT);
            log->passwordInput->redraw();
        }, this);

        // Login Button
        Fl_Button* loginButton = new Fl_Button(centerX - 20, startY + 3 * spacing, 120, 35, "Login");
        loginButton->color(FL_BLUE);
        loginButton->labelcolor(FL_WHITE);
        loginButton->callback([](Fl_Widget*, void* data) {
            login* log = (login*)data;

            string user = log->usernameInput->value();
            string pass = log->passwordInput->value();

            if (user.empty() || pass.empty()) {
                fl_alert("Please enter both user and password.");
                return;
            }

            if (pass.length() < 4) {
                fl_alert("Password must be at least 4 characters long.");
                return;
            }

            if(user == "admin" && pass == "1122") {
                log->loginWindow->hide();
                admin* a = new admin();
                a->adminPage();
                return;
            }

            ifstream file("users.txt");
            string line;
            bool userFound = false;

            int phone;
            string addr;

            if (file.is_open()) {
            string fileUsername, filePhoneStr, fileAddress, filePassword;
            while (file >> fileUsername >> filePhoneStr >> fileAddress >> filePassword) {
                if (fileUsername == user && filePassword == pass) {
                    userFound = true;
                    phone = stoi(filePhoneStr);
                    addr = fileAddress;
                    break;
                }
            }
            file.close();
        }

            
            bool userFoundLL = log->l.existsCustomer(log->customerHead, user, pass);

            if (userFound || userFoundLL) {
                customer* customerObj = new customer();
                customerObj->customerPanel(user, pass, phone, addr);

                log->usernameInput->value("");
                log->passwordInput->value("");
                log->showPasswordCheck->value(0);
            } else {
                fl_alert("Invalid username or password. Please try again.");
            }
        }, this);

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

    void gosignUpPage();
    void togglePassword() {
        
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

void admin::goTransactionPage() {
        if (Fl::first_window()) Fl::first_window()->hide();

        transaction* t = new transaction();
        t->transactionPage();
    }

void transaction::goAdminPanel() {
        if (Fl::first_window()) Fl::first_window()->hide();

        admin* a = new admin();
        a->adminPage();
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

void customer::goLoginPanel() {
        if (Fl::first_window()) Fl::first_window()->hide();

        login* log = new login();
        log->loginPage();
    }

 
string linkedList::displayProducts(productNode* head) {      
        
    
        ifstream file("products.txt");
        if (file.is_open()) {
            int id;
            string name;
            float price;
            int quantity;
            
            while (file >> id >> name >> price >> quantity) {
                bool idexists = exists(head, id);
                if (idexists){
                    continue;  
                }
                insertProduct(head, id, name, price, quantity);
            }

            file.close();
        }

        if (head == nullptr) {
            return "No products available.";
        }


        stringstream ss;

        ss << left
        << setw(10) << "ID"
        << setw(20) << "Name"
        << setw(15) << "Price"
        << setw(17) << "Quantity" << "\n";

        ss << string(55, '-') << "\n";

        productNode* temp = head;
        while (temp != nullptr) {
            ss << left
            << setw(10) << temp->productId
            << setw(20) << temp->productName
            << setw(20) << fixed << setprecision(2) << temp->productPrice
            << setw(20) << temp->productQuantity << "\n";
            temp = temp->next;
        }

        // Convert to string and return
        return ss.str();
    }   
    

string linkedList::displayEmployees(employeeNode* head) {
        ifstream file("employee.txt");
        if (file.is_open()) {
            int id;
            string name, position;
            float salary;

            while (file >> id >> name >> position >> salary) {
                if (exists(head, id)) {
                    continue;
                }
                insertEmployee(head, id, name, position, salary);
            }
            file.close();
        }

        if (head == nullptr) {
            return "No employees available.";
        }

        stringstream ss;
        ss << left
        << setw(10) << "ID"
        << setw(20) << "Name"
        << setw(20) << "Position"
        << setw(15) << "Salary" << "\n";

        ss << string(65, '-') << "\n";

        employeeNode* temp = head;
        while (temp != nullptr) {
            ss << left
            << setw(10) << temp->employeeId
            << setw(20) << temp->employeeName
            << setw(20) << temp->employeePosition
            << setw(15) << fixed << setprecision(2) << temp->employeeSalary << "\n";
            temp = temp->next;
        }

        return ss.str();
    }


    string linkedList::displayCustomers(customerNode* head) {
        ifstream file("users.txt");
        if (file.is_open()) {
            string username, phone, email, address, password;

            while (file >> username >> phone >> email >> address >> password) {
                if (existsCustomer(head, username, password)) {
                    continue;
                }
                insertCustomer(head, username, stoi(phone), address, password);
            }
            file.close();
        }

        if (head == nullptr) {
            return "No customers available.";
        }

        stringstream ss;
        ss << left
        << setw(20) << "Username"
        << setw(20) << "Phone"
        << setw(30) << "Address" << "\n";

        ss << string(95, '-') << "\n";

        customerNode* temp = head;
        while (temp != nullptr) {
            ss << left
            << setw(20) << temp->username
            << setw(20) << temp->phone
            << setw(30) << temp->address << "\n";
            temp = temp->next;
        }

        return ss.str();
    }

int main() {
    login log;
    log.loginPage();
    return Fl::run();
}   