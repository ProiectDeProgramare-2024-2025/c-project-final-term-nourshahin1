#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PROPERTIES 6
#define PROPERTY_FILE "properties.txt"

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"

struct Property {
    char name[50];
    char address[100];
    int price;
    int size;
    int rooms;
    int bathrooms;
    int parking;
};

struct Property properties[MAX_PROPERTIES];
struct Property favorites[MAX_PROPERTIES];
int favorite_count = 0;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pause() {
    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void loadPropertiesFromFile() {
    FILE *file = fopen(PROPERTY_FILE, "r");
    if (file == NULL) {
        printf(RED "No property file found.\n" RESET);
        return;
    }

    int i = 0;
    while (i < MAX_PROPERTIES && fscanf(file, "%49[^|]|%99[^|]|%d|%d|%d|%d|%d\n",
                                        properties[i].name, properties[i].address,
                                        &properties[i].price, &properties[i].size,
                                        &properties[i].rooms, &properties[i].bathrooms,
                                        &properties[i].parking) == 7) {
        i++;
    }
    fclose(file);
}

int readInteger(const char *prompt) {
    int num;
    printf(GREEN "%s" RESET, prompt);
    while (scanf("%d", &num) != 1) {
        printf(RED "Invalid input! Please enter a number: " RESET);
        while (getchar() != '\n');
    }
    while (getchar() != '\n'); 
    return num;
}

void readString(const char *prompt, char *buf, int size) {
    printf(GREEN "%s" RESET, prompt);
    fgets(buf, size, stdin);
    buf[strcspn(buf, "\n")] = '\0'; 
}


int validateName(const char *name) {
    int wordCount = 0;
    int inWord = 0;
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha(name[i]) && name[i] != ' ') return 0;
        if (isalpha(name[i]) && !inWord) {
            wordCount++;
            inWord = 1;
        } else if (name[i] == ' ') {
            inWord = 0;
        }
    }
    return (wordCount > 0 && wordCount <= 3);
}

int validateAddress(const char *address) {
    int i = 0;
    while (isdigit(address[i])) i++;
    if (i == 0 || address[i] != ' ') return 0;
    i++;
    int wordCount = 0, inWord = 0;
    for (; address[i] != '\0'; i++) {
        if (!isalpha(address[i]) && address[i] != ' ') return 0;
        if (isalpha(address[i]) && !inWord) {
            wordCount++;
            inWord = 1;
        } else if (address[i] == ' ') {
            inWord = 0;
        }
    }
    return (wordCount == 2);
}

int validateNumberLength(int num, int maxDigits) {
    int digits = 0;
    if (num == 0) return 1;
    while (num != 0) {
        num /= 10;
        digits++;
    }
    return digits <= maxDigits;
}

void savePropertyToFile(const struct Property *prop) {
    FILE *file = fopen(PROPERTY_FILE, "a");
    if (file == NULL) {
        printf(RED "Error saving property to file.\n" RESET);
        return;
    }
    fprintf(file, "%s|%s|%d|%d|%d|%d|%d\n",
            prop->name, prop->address, prop->price,
            prop->size, prop->rooms, prop->bathrooms,
            prop->parking);
    fclose(file);
}

void addProperty() {
    if (strlen(properties[MAX_PROPERTIES - 1].name) != 0) {
        printf(RED "Property list is full!\n" RESET);
        pause();
        return;
    }

    struct Property newProp;
    clearScreen();
    printf(CYAN "===== Add New Property =====\n" RESET);

    do {
        printf(GREEN "Enter property name : " RESET);
        fgets(newProp.name, sizeof(newProp.name), stdin);
        newProp.name[strcspn(newProp.name, "\n")] = '\0';
        if (!validateName(newProp.name)) {
            printf(RED " Invalid name. Use only letters and max 3 words.\n" RESET);
        }
    } while (!validateName(newProp.name));

    do {
        printf(GREEN "Enter address : " RESET);
        fgets(newProp.address, sizeof(newProp.address), stdin);
        newProp.address[strcspn(newProp.address, "\n")] = '\0';
        if (!validateAddress(newProp.address)) {
            printf(RED " Invalid address. Start with a number followed by 2 words.\n" RESET);
        }
    } while (!validateAddress(newProp.address));

    do {
        newProp.price = readInteger("Enter price : ");
        if (!validateNumberLength(newProp.price, 6)) {
            printf(RED " Price must have at most 6 digits.\n" RESET);
        }
    } while (!validateNumberLength(newProp.price, 6));

    do {
        newProp.size = readInteger("Enter size in sqm : ");
        if (!validateNumberLength(newProp.size, 3)) {
            printf(RED " Size must have at most 3 digits.\n" RESET);
        }
    } while (!validateNumberLength(newProp.size, 3));

    newProp.rooms = readInteger("Enter number of rooms: ");
    newProp.bathrooms = readInteger("Enter number of bathrooms: ");

    do {
        newProp.parking = readInteger("Enter parking : ");
        if (newProp.parking != 0 && newProp.parking != 1) {
            printf(RED " Parking must be 1 or 0.\n" RESET);
        }
    } while (newProp.parking != 0 && newProp.parking != 1);

    for (int i = 0; i < MAX_PROPERTIES; i++) {
        if (strlen(properties[i].name) == 0) {
            properties[i] = newProp;
            break;
        }
    }

    savePropertyToFile(&newProp);
    printf(GREEN " Property added successfully!\n" RESET);

    pause();
}

void header() {
    printf(CYAN "-------------------------\n" RESET);
    printf(GREEN "1. View Properties\n" RESET);
    printf(GREEN "2. Favorites\n" RESET);
    printf(GREEN "3. Purchase Property\n" RESET);
    printf(GREEN "4. Add Property\n" RESET);
    printf(GREEN "5. Exit\n" RESET);
    printf(CYAN "-------------------------\n" RESET);
}

void displayProperty(const struct Property *p, int index) {
    if (strlen(p->name) == 0) return;
    printf("\n%sProperty %d:%s\n", CYAN, index + 1, RESET);
    printf("Name: " GREEN "%s" RESET "\n", p->name);
    printf("Address: %s\n", p->address);
    printf("Price: " GREEN "$%d" RESET "\n", p->price);
    printf("Size: %d sq. meters\n", p->size);
    printf("Rooms: %d\n", p->rooms);
    printf("Bathrooms: %d\n", p->bathrooms);
    printf("Parking: %s\n", (p->parking ? GREEN "Yes" RESET : RED "No" RESET));
}

void viewProperties() {
    clearScreen();
    printf(CYAN "===== Available Properties =====\n" RESET);
    for (int i = 0; i < MAX_PROPERTIES; i++) {
        displayProperty(&properties[i], i);
    }
    pause();
}

void manageFavorites() {
    clearScreen();
    if (favorite_count >= MAX_PROPERTIES) {
        printf(RED "Favorites list is full.\n" RESET);
        pause();
        return;
    }

    int choice = readInteger("Enter property number to add to favorites (1-6): ");

    if (choice < 1 || choice > MAX_PROPERTIES) {
        printf(RED "Invalid selection.\n" RESET);
    } else {
        favorites[favorite_count++] = properties[choice - 1];
        printf(GREEN " Property added to favorites.\n" RESET);
    }

    pause();
}

void viewFavorites() {
    clearScreen();
    printf(CYAN "===== Favorite Properties =====\n" RESET);

    if (favorite_count == 0) {
        printf(RED "No favorites added yet.\n" RESET);
    } else {
        for (int i = 0; i < favorite_count; i++) {
            displayProperty(&favorites[i], i);
        }
    }

    pause();
}

void purchaseProperty() {
    clearScreen();
    int choice = readInteger("Enter property number to purchase (1-6): ");

    if (choice < 1 || choice > MAX_PROPERTIES || strlen(properties[choice - 1].name) == 0) {
        printf(RED "Invalid selection.\n" RESET);
    } else {
        printf(GREEN " You have successfully purchased: %s\n" RESET, properties[choice - 1].name);

        memset(&properties[choice - 1], 0, sizeof(struct Property));

        FILE *file = fopen(PROPERTY_FILE, "w");
        if (file == NULL) {
            printf(RED "Error hold on.\n" RESET);
        } else {
            for (int i = 0; i < MAX_PROPERTIES; i++) {
                if (strlen(properties[i].name) > 0) {
                    fprintf(file, "%s|%s|%d|%d|%d|%d|%d\n",
                        properties[i].name, properties[i].address, properties[i].price,
                        properties[i].size, properties[i].rooms, properties[i].bathrooms,
                        properties[i].parking);
                }
            }
            fclose(file);
        }
    }

    pause();
}


void menu(int op) {
    switch (op) {
        case 1:
            viewProperties();
            break;
        case 2:
            viewFavorites();
            manageFavorites();
            break;
        case 3:
            purchaseProperty();
            break;
        case 4:
            addProperty();
            break;
        default:
            printf(GREEN "Goodbye!\n" RESET);
    }
}

int main() {
    loadPropertiesFromFile();
    int option;

    do {
        clearScreen();
        header();
        option = readInteger("Enter option: ");
        menu(option);
    } while (option >= 1 && option <= 4);

    return 0;
}
