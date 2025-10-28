#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define maxname 50

typedef struct {
    int id;
    char name[maxname];
    float price;
    int stock;
} product;

product *list = NULL;
int total = 0;
int limit = 0;

void cleaninput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int readint(char *text) {
    char buf[64];
    int val;
    while (1) {
        printf("%s", text);
        if (fgets(buf, sizeof(buf), stdin)) {
            if (sscanf(buf, "%d", &val) == 1)
                return val;
        }
        printf("Please enter a valid number.\n");
    }
}


float readfloat(char *text) {
    char buf[64];
    float val;
    while (1) {
        printf("%s", text);
        if (fgets(buf, sizeof(buf), stdin)) {
            if (sscanf(buf, "%f", &val) == 1)
                return val;
        }
        printf("Please enter a valid price.\n");
    }
}


void readtext(char *text, char *out) {
    while (1) {
        printf("%s", text);
        if (!fgets(out, maxname, stdin)) continue;
        out[strcspn(out, "\n")] = '\0';

        int letters = 0;
        for (int i = 0; out[i]; i++) {
            if (isalpha((unsigned char)out[i])) letters = 1;
        }

        if (letters && strlen(out) > 0)
            break;
        else
            printf("Invalid name. Try again.\n");
    }
}


char* findword(const char *text, const char *word) {
    if (!*word) return (char*)text;
    for (; *text; text++) {
        const char *p1 = text, *p2 = word;
        while (*p1 && *p2 && tolower(*p1) == tolower(*p2)) {
            p1++; p2++;
        }
        if (!*p2) return (char*)text;
    }
    return NULL;
}


void checkspace() {
    if (total < limit) return;
    int newlimit = (limit == 0) ? 5 : limit * 2;
    product *temp = realloc(list, newlimit * sizeof(product));
    if (!temp) {
        printf("Out of memory. Exiting.\n");
        exit(1);
    }
    list = temp;
    limit = newlimit;
}


int sameid(int id) {
    for (int i = 0; i < total; i++)
        if (list[i].id == id) return 1;
    return 0;
}


void sortbyid() {
    for (int i = 0; i < total - 1; i++) {
        for (int j = i + 1; j < total; j++) {
            if (list[i].id > list[j].id) {
                product t = list[i];
                list[i] = list[j];
                list[j] = t;
            }
        }
    }
}


void additem() {
    int id;
    while (1) {
        id = readint("Enter item id: ");
        if (sameid(id))
            printf("That id already exists. Use another.\n");
        else break;
    }

    checkspace();
    list[total].id = id;
    readtext("Enter item name: ", list[total].name);
    list[total].price = readfloat("Enter price: ");
    list[total].stock = readint("Enter stock quantity: ");
    total++;

    sortbyid();
    printf("Item added successfully.\n");
}

void showitems() {
    if (total == 0) {
        printf("No items in list.\n");
        return;
    }
    printf("\n Item List \n");
    for (int i = 0; i < total; i++) {
        printf("ID:%d | %s | Price: %.2f | Stock: %d\n",
               list[i].id, list[i].name, list[i].price, list[i].stock);
    }
}


void changestock() {
    int id = readint("Enter item id to update: ");
    for (int i = 0; i < total; i++) {
        if (list[i].id == id) {
            list[i].stock = readint("Enter new quantity: ");
            printf("Stock updated.\n");
            return;
        }
    }
    printf("Item not found.\n");
}


void findbyid() {
    int id = readint("Enter item id to search: ");
    for (int i = 0; i < total; i++) {
        if (list[i].id == id) {
            printf("Found: %d | %s | %.2f | %d\n",
                   list[i].id, list[i].name, list[i].price, list[i].stock);
            return;
        }
    }
    printf("No item found with that id.\n");
}


void findbyname() {
    char key[maxname];
    readtext("Enter name or part of it: ", key);
    int found = 0;
    for (int i = 0; i < total; i++) {
        if (findword(list[i].name, key)) {
            printf("%d | %s | %.2f | %d\n",
                   list[i].id, list[i].name, list[i].price, list[i].stock);
            found = 1;
        }
    }
    if (!found) printf("No matching names found.\n");
}

void findbyprice() {
    float low = readfloat("Enter minimum price: ");
    float high = readfloat("Enter maximum price: ");
    int found = 0;
    for (int i = 0; i < total; i++) {
        if (list[i].price >= low && list[i].price <= high) {
            printf("%d | %s | %.2f | %d\n",
                   list[i].id, list[i].name, list[i].price, list[i].stock);
            found = 1;
        }
    }
    if (!found) printf("No items found in that range.\n");
}

void removeitem() {
    int id = readint("Enter id to delete: ");
    for (int i = 0; i < total; i++) {
        if (list[i].id == id) {
            for (int j = i; j < total - 1; j++)
                list[j] = list[j + 1];
            total--;
            printf("Item deleted.\n");
            return;
        }
    }
    printf("No item found with that id.\n");
}

int main() {
    printf("Simple Store Inventory\n");

    int start = readint("How many items to begin with? ");
    for (int i = 0; i < start; i++) {
        printf("\nEnter details for item %d:\n", i + 1);
        additem();
    }

    while (1) {
        printf("\nMenu:\n");
        printf("1. Add new item\n");
        printf("2. Show all items\n");
        printf("3. Update stock\n");
        printf("4. Search by id\n");
        printf("5. Search by name\n");
        printf("6. Search by price range\n");
        printf("7. Delete item\n");
        printf("8. Exit\n");

        int ch = readint("Choose an option: ");
        switch (ch) {
            case 1: additem(); break;
            case 2: showitems(); break;
            case 3: changestock(); break;
            case 4: findbyid(); break;
            case 5: findbyname(); break;
            case 6: findbyprice(); break;
            case 7: removeitem(); break;
            case 8:
                free(list);
                printf("Goodbye.\n");
                return 0;
            default:
                printf("Invalid option. Try again.\n");
        }
    }
}
