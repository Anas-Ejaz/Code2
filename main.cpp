#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

class Item{
    public:
    void itemPanel(){
        int option;
        do{
            system("cls");
            cout<<"Item"<<endl;
            cout<<endl;

            cout<<"1. Add Items"<<endl;
            cout<<"2. View Items"<<endl;
            cout<<"3. Delete Items"<<endl;
            cout<<"4. Admins Portal"<<endl;
            cout<<"Enter Option: ";
            cin>>option;
            while (cin.fail()) {
                cin.clear();  
                while (cin.get() != '\n');
                    cout<<"Invalid ! Please enter a valid number : ";
                    cin>>option;
            }

            switch(option){
                case 1:
                    addItem();
                    break;
                
                case 2:
                    viewItem();
                    break;

                case 3:
                    deleteItem();
                    break;
                    
                case 4:
                    cout<<"Returning to Admin's portal"<<endl;
                    Sleep(2000);
                    // Login login;
                    break;

                default:
                    cout<<"Invalid option"<<endl;
                    break;
            }
        }while(option != 4);
    }

    void addItem(){
        // Code to add item
        cout<<"Adding item"<<endl;
    }
    
    void viewItem(){
        // Code to view item
        cout<<"View item"<<endl;
    }
    
    void deleteItem(){
        // Code to delete item
        cout<<"Deleting item"<<endl;
    }
};

class Employees{
    public:
    void empPanel(){
        int option;
        do{
            system("cls");
            cout<<"Employee"<<endl;
            cout<<endl;

            cout<<"1. Add Employee"<<endl;
            cout<<"2. View Employee"<<endl;
            cout<<"3. Remove Employee"<<endl;
            cout<<"4. Admins Portal"<<endl;
            cout<<"Enter Option: ";
            cin>>option;
            while (cin.fail()) {
                cin.clear();  
                while (cin.get() != '\n');
                    cout<<"Invalid ! Please enter a valid number : ";
                    cin>>option;
            }

            switch(option){
                case 1:
                    addEmployees();
                    break;
                
                case 2:
                    viewEmployees();
                    break;
                    
                case 3:
                    deleteEmployees();
                    break;
                    
                case 4:
                    cout<<"Returning to Admin's portal"<<endl;
                    Sleep(2000);
                    // Login login;
                    break;
                    
                default:
                    cout<<"Invalid option"<<endl;
                    break;
                }
        }while(option != 4);
    }

    void addEmployees(){
        // Code to add item
        cout<<"Adding Employee"<<endl;
    }
    
    void viewEmployees(){
        // Code to view item
        cout<<"Viewing Employee"<<endl;
    }
    
    void deleteEmployees(){
        // Code to delete item
        cout<<"Deleting employees"<<endl;
    }
};

class Admin{
    public:
    void adminPanel(){
        int option;
        do{
            system("cls");
            cout<<"Admin "<<endl;
            cout<<endl;
            
            cout<<"1. Manage Items"<<endl;
            cout<<"2. Manage Employees"<<endl;
            cout<<"3. Transaction"<<endl;
            cout<<"4. Login Portal"<<endl;
            cout<<"Enter Option: ";
            cin>>option;
            while (cin.fail()) {
                cin.clear();  
                while (cin.get() != '\n');
                    cout<<"Invalid ! Please enter a valid number : ";
                    cin>>option;
            }

            switch(option){
                case 1:
                    Item i;
                    i.itemPanel();
                    break;
                
                case 2:
                    Employees e;
                    e.empPanel();
                    break;

                case 3:
                    // Transaction transaction;
                    break;
                    
                case 4:
                    cout<<"Returning to login portal"<<endl;
                    Sleep(2000);
                    // Login login;
                    break;

                default:
                    cout<<"Invalid option"<<endl;
                    break;
            }
        }while(option != 4);
    }
};

int main(){
    Admin a;
    a.adminPanel();
}