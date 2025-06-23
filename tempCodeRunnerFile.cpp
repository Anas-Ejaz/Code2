
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

                p->addProduct(id, name, price, quantity);
                p->productPage();
                p->productIdInput->value("");   
                p->productNameInput->value("");
                p->productPriceInput->value("");
                p->productQuantityInput->value("");
                fl_message("Product added successfully!");

                ofstream file("products.txt", ios::app); // Open in append mode
                if (file.is_open()) {
                    file << id << "\t" << name << "\t" << price << "\t" << quantity << "\n";
                    file.close();
                }else {
                    fl_alert("Error opening file to save product data.");
                }

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
            int id = stoi(p->productIdInput->value());

            ifstream inFile("products.txt");
            ofstream outFile("temp.txt");

            string line;
            while (getline(inFile, line)) {
                int currentId = stoi(line.substr(0, line.find('\t')));
                if (currentId != id) {
                    outFile << line << '\n';
                }
            }

            inFile.close();
            outFile.close();

            remove("products.txt");
            rename("temp.txt", "products.txt");

            p->l.deleteProduct(p->productHead, id);
            fl_message("Product deleted successfully!");
            p->productPage();
            p->productIdInput->value("");
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