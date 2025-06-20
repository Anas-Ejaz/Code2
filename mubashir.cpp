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

#ifdef _WIN32
#include <strings.h>
#define strcasecmp _stricmp
#else
#include <strings.h>
#endif

using namespace std;

// Global declarations for UI widgets, data structures and functions
// ... (Refer to full code in the previous message)

// --- Update Product List with Scroll Management ---
void updateProductList() {
    char buffer[4000] = "Available Products:\n\n";
    for (size_t i = 0; i < allProducts.size(); i++) {
        char line[150];
        snprintf(line, sizeof(line), "%zu. %s - Rs.%d\n", i + 1, allProducts[i].name, allProducts[i].price);
        strcat(buffer, line);
    }

    if (prodManageOutputContent) {
        prodManageOutputContent->value(buffer);
        if (adminProductScroll) {
            int num_lines = 0;
            for (int k = 0; buffer[k] != '\0'; ++k) {
                if (buffer[k] == '\n') num_lines++;
            }
            int lineHeight = prodManageOutputContent->textsize() + 4;
            int contentHeight = (num_lines + 2) * lineHeight;

            prodManageOutputContent->size(
                prodManageOutputContent->w(),
                max(adminProductScroll->h(), contentHeight)
            );

            adminProductScrollContentGroup->size(
                adminProductScrollContentGroup->w(),
                max(adminProductScroll->h(), contentHeight)
            );
            adminProductScroll->scroll_to(0, 0);
            adminProductScrollContentGroup->redraw();
            adminProductScroll->redraw();
        }
    }
    updateCustomerProductDisplay();
}

// --- Update Customer Buy Product Buttons with Scroll ---
void updateCustomerProductDisplay() {
    if (!customerProductScroll || !customerProductScrollContentGroup) return;

    customerProductScrollContentGroup->clear();
    customerProductScrollContentGroup->begin();

    int y_offset = 10;
    const int button_height = 30;
    const int button_spacing = 10;
    const int button_width = customerProductScrollContentGroup->w() - 20;

    for (size_t i = 0; i < allProducts.size(); i++) {
        char* buttonLabel = (char*)malloc(100);
        snprintf(buttonLabel, 100, "%s (Rs.%d)", allProducts[i].name, allProducts[i].price);

        Fl_Button* prodBtn = new Fl_Button(10, y_offset, button_width, button_height);
        prodBtn->label(buttonLabel);
        prodBtn->callback(addProductToCart, reinterpret_cast<void*>(i));
        prodBtn->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
        prodBtn->box(FL_UP_BOX);

        y_offset += button_height + button_spacing;
    }

    customerProductScrollContentGroup->end();

    int contentHeight = y_offset;
    customerProductScrollContentGroup->size(
        customerProductScrollContentGroup->w(),
        max(customerProductScroll->h(), contentHeight)
    );
    customerProductScroll->scroll_to(0, 0);
    customerProductScrollContentGroup->redraw();
    customerProductScroll->redraw();
}

// Entry point, UI layout, callback wiring, and other functionality remain unchanged.
// Modularization can separate UI-building logic, callbacks, and utility logic into multiple files later.

int main() {
    // Application setup, show windows, FLTK loop
    // ...
    return Fl::run();
}




