#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>

#define MAX_ITEMS 100
#define MAX_CUSTOMERS 100
#define MAX_INVOICES 100

// Structure to represent a menu item
typedef struct menu_item {
    char name[50];
    int price;
} MenuItem;

// Structure to represent a customer
typedef struct customer {
    char name[50];
} Customer;

// Structure to represent an invoice
typedef struct invoice {
    Customer customer;
    char date[50];
    MenuItem items[MAX_ITEMS];
    int num_items;
    int total;
} Invoice;

void clearScreen() {
    system("cls");
}

// Function to display the menu and get user input
int displayMenu() {
    printf("============================\n");
    printf("    Welcome to Restaurant   \n");
    printf("============================\n");
    printf("Select an option:\n");
    printf("1. Generate invoice\n");
    printf("2. Show previous invoices\n");
    printf("3. Search for invoice\n");
    printf("4. Exit\n");
    printf("Your choice: ");

    int choice;
    scanf("%d", &choice);

    return choice;
}

// Function to read the menu items from a file
int readMenu(MenuItem *menu, int max_items) {
    FILE *fp = fopen("menu.txt", "r");
    if (fp == NULL) {
        printf("Failed to open menu file.\n");
        return 0;
    }

    int num_items = 0;
    while (fscanf(fp, "%s %d", menu[num_items].name, &menu[num_items].price) != EOF && num_items < max_items) {
        num_items++;
    }
    fclose(fp);

    return num_items;
}

// Function to display the menu items
void displayMenuItems(MenuItem *menu, int num_items) {
    printf("Menu:\n");
    for (int i = 0; i < num_items; i++) {
        printf("%d. %s: $%d\n", i + 1, menu[i].name, menu[i].price);
    }
}

// Function to get a valid item number from the user
int getItemNumber(int num_items) {
    int item_number;
    printf("Enter item number (1-%d): ", num_items);
    scanf("%d", &item_number);

    while (item_number < 1 || item_number > num_items) {
        printf("Invalid item number. Please enter a number between 1 and %d: ", num_items);
        scanf("%d", &item_number);
    }

    return item_number;
}

// Function to generate a new invoice
void generateInvoice(MenuItem *menu, int num_menu_items, Invoice *invoices, int *num_invoices) {
    // Read the current date and time
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char date[50];
    strftime(date, sizeof(date), "%Y-%m-%d", tm);

    // Get the customer name
    char customer_name[50];
    printf("Please enter the name of the customer: ");
    scanf("%s", customer_name);

    // Get the items to order
    int selected_items[MAX_ITEMS];
    int num_selected_items = 0;
    while (1) {
        clearScreen();
        displayMenuItems(menu, num_menu_items);
        int item_number = getItemNumber(num_menu_items);

        selected_items[num_selected_items++] = item_number;

        char confirm;
        printf("Add another item? (y/n) ");
        scanf(" %c", &confirm);

        if (confirm == 'n' || confirm == 'N') {
            break;
        }
    }

    // Calculate the total price
    int total_price = 0;
    MenuItem ordered_items[MAX_ITEMS];
    for (int i = 0; i < num_selected_items; i++) {
        ordered_items[i] = menu[selected_items[i]-1];
        total_price += ordered_items[i].price;
    }

    // Display the selected items and total price
    printf("\nSelected items:\n");
    for (int i = 0; i < num_selected_items; i++) {
        printf("%d. %s: $%d\n", i + 1, ordered_items[i].name, ordered_items[i].price);
    }
    printf("Total price: $%d\n", total_price);

    // Ask for confirmation to save the invoice
    char confirm;
    printf("Do you want to save this invoice? (y/n) ");
    scanf(" %c", &confirm);

        Invoice new_invoice;
        strcpy(new_invoice.customer.name, customer_name);
        strcpy(new_invoice.date, date);
        new_invoice.num_items = num_selected_items;
        new_invoice.total = total_price;
        for (int i = 0; i < num_selected_items; i++) {
            new_invoice.items[i] = ordered_items[i];
        }
        invoices[(*num_invoices)++] = new_invoice;

        // Save the invoice to a file
        FILE *fp = fopen("invoices.txt", "a");
        if (fp == NULL) {
            printf("Failed to open invoices file.\n");
            return;
        }

        fprintf(fp, "%s %s ", customer_name, date);
        for (int i = 0; i < num_selected_items; i++) {
            fprintf(fp, "%s:%d ", ordered_items[i].name, ordered_items[i].price);
        }
        fprintf(fp, "$%d\n", total_price);
        fclose(fp);

        printf("Invoice saved.\n");
}

// Function to read the invoices from a file
int readInvoices(Invoice *invoices, int max_invoices) {
    FILE *fp = fopen("invoices.txt", "r");
    if (fp == NULL) {
        printf("Failed to open invoices file.\n");
        return 0;
    }

    int num_invoices = 0;
    while (fscanf(fp, "%s %s", invoices[num_invoices].customer.name, invoices[num_invoices].date) != EOF && num_invoices < max_invoices) {
        char item_name[50];
        int item_price;
        int num_items = 0;
        while (fscanf(fp, "%s:%d", item_name, &item_price) != EOF && num_items < MAX_ITEMS) {
            strcpy(invoices[num_invoices].items[num_items].name, item_name);
            invoices[num_invoices].items[num_items].price = item_price;
            num_items++;
        }
        invoices[num_invoices].num_items = num_items;
        fscanf(fp, "$%d\n", &invoices[num_invoices].total);
        num_invoices++;
    }
    fclose(fp);

    return num_invoices;
}

// Function to display a list of invoices
void displayInvoices(Invoice *invoices, int num_invoices) {
    printf("Previous invoices:\n");
    printf("-------------------\n");
    printf("%-20s %-20s %s\n", "Customer Name", "Date", "Order Total");
    printf("%-20s %-20s %s\n", "-------------", "----", "-----------");

    for (int i = 0; i < num_invoices; i++) {
        printf("%-20s %-20s $%d\n", invoices[i].customer.name, invoices[i].date, invoices[i].total);
    }
    getch();
}

// Function to search for an invoice by customer name or date
void searchInvoice(Invoice *invoices, int num_invoices) {
    printf("Search for an invoice:\n");
    printf("1. By customer name\n");
    printf("2. By date\n");
    printf("Your choice: ");

    int search_type;
    scanf("%d", &search_type);

    if (search_type == 1) {
        // Search by customer name
        char customer_name[50];
        printf("Enter customer name: ");
        scanf("%s", customer_name);

        int found = 0;
        for (int i = 0; i < num_invoices; i++) {
            if (strcmp(invoices[i].customer.name, customer_name) == 0) {
                printf("Invoice found:\n");
                printf("Customer name: %s\n", invoices[i].customer.name);
                printf("Date: %s\n", invoices[i].date);
                printf("Items:%s\n", invoices[i].items);

                for (int j = 0; j < invoices[i].num_items; j++) {
                    printf("  - %s: $%d\n", invoices[i].items[j].name, invoices[i].items[j].price);
                }
                printf("Total: $%d\n", invoices[i].total);
                found = 1;
            }
        }

        if (!found) {
            printf("No invoice found for customer: %s\n", customer_name);
        }
    } else if (search_type == 2) {
        // Search by date
        char date[50];
        printf("Enter date (YYYY-MM-DD): ");
        scanf("%s", date);

        int found = 0;
        for (int i = 0; i < num_invoices; i++) {
            if (strcmp(invoices[i].date, date) == 0) {
                printf("Invoice found:\n");
                printf("Customer name: %s\n", invoices[i].customer.name);
                printf("Date: %s\n", invoices[i].date);
                printf("Items:\n");
                for (int j = 0; j < invoices[i].num_items; j++) {
                    printf("  - %s: $%d\n", invoices[i].items[j].name, invoices[i].items[j].price);
                }
                printf("Total: $%d\n", invoices[i].total);
                found = 1;
            }
        }

        if (!found) {
            printf("No invoice found for date: %s\n", date);
        }
    } else {
        printf("Invalid search type.\n");
    }
    getch();
}

int main() {
    MenuItem menu[MAX_ITEMS];
    int num_menu_items = readMenu(menu, MAX_ITEMS);

    Invoice invoices[MAX_INVOICES];
    int num_invoices = readInvoices(invoices, MAX_INVOICES);

    while (1) {
        clearScreen();
        int choice = displayMenu();

        switch (choice) {
            case 1:
                clearScreen();
                generateInvoice(menu, num_menu_items, invoices, &num_invoices);
                break;
            case 2:
                clearScreen();
                displayInvoices(invoices, num_invoices);
                break;
            case 3:
                clearScreen();
                searchInvoice(invoices, num_invoices);
                break;
            case 4:
                clearScreen();
                printf("Exiting program...\n");
                return 0;
            default:
                printf("Invalid choice.\n");
                break;
        }
    }

    return 0;
}