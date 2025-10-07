#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[100];
    int age;
} User;

const char* FILENAME = "users.txt";

void createUser();
void readUsers();
void updateUser();
void deleteUser();
void printMenu();

int main() {
    int ch;

    while (1) {
        printMenu();

        if (scanf("%d", &ch) != 1) {
            while (getchar() != '\n');
            printf("\nOops! That wasn't a number. Please try again.\n");
            continue;
        }

        if (ch == 1) {
            createUser();
        } else if (ch == 2) {
            readUsers();
        } else if (ch == 3) {
            updateUser();
        } else if (ch == 4) {
            deleteUser();
        } else if (ch == 5) {
            printf("\nExiting program. Goodbye!\n");
            exit(0);
        } else {
            printf("\nInvalid choice. Please pick a number from 1 to 5.\n");
        }
    }
    return 0;
}

void printMenu() {
    printf("\n--- User Management System ---\n");
    printf("1. Add a New User\n");
    printf("2. Show All Users\n");
    printf("3. Update a User's Info\n");
    printf("4. Delete a User\n");
    printf("5. Exit\n");
    printf("------------------------------\n");
    printf("Enter your choice: ");
}

void createUser() {
    User u;
    FILE *fp;

    printf("\nEnter a unique ID for the new user: ");
    scanf("%d", &u.id);

    printf("Enter the user's name: ");
    getchar(); 
    fgets(u.name, sizeof(u.name), stdin);
    u.name[strcspn(u.name, "\n")] = 0; 

    printf("Enter the user's age: ");
    scanf("%d", &u.age);

    fp = fopen(FILENAME, "a");
    if (fp == NULL) {
        printf("Error: Could not open the file.\n");
        return;
    }

    fprintf(fp, "%d,%s,%d\n", u.id, u.name, u.age);
    fclose(fp);

    printf("\nUser added successfully!\n");
}

void readUsers() {
    User u;
    FILE *fp;
    int c = 0;

    fp = fopen(FILENAME, "r");
    if (fp == NULL) {
        printf("\nThere are no users yet, or the file is missing.\n");
        return;
    }

    printf("\n--- List of All Users ---\n");
    printf("%-5s | %-30s | %-5s\n", "ID", "Name", "Age");
    printf("-------------------------------------------\n");

    while (fscanf(fp, "%d,%99[^,],%d\n", &u.id, u.name, &u.age) == 3) {
        printf("%-5d | %-30s | %-5d\n", u.id, u.name, u.age);
        c++;
    }
    
    if (c == 0) {
        printf("The file is empty. No users to show.\n");
    }
    printf("-------------------------------------------\n");

    fclose(fp);
}

void updateUser() {
    int id, f = 0;
    User u;
    FILE *f1, *f2;

    printf("\nEnter the ID of the user you want to update: ");
    scanf("%d", &id);

    f1 = fopen(FILENAME, "r");
    if (f1 == NULL) {
        printf("\nCannot open the user file. Nothing to update.\n");
        return;
    }

    f2 = fopen("temp.txt", "w");
    if (f2 == NULL) {
        printf("Error: Could not create a temporary file.\n");
        fclose(f1);
        return;
    }

    while (fscanf(f1, "%d,%99[^,],%d\n", &u.id, u.name, &u.age) == 3) {
        if (u.id == id) {
            f = 1;
            printf("\n--- User Found! Enter the new information. ---\n");
            printf("Enter new Name: ");
            getchar();
            fgets(u.name, sizeof(u.name), stdin);
            u.name[strcspn(u.name, "\n")] = 0;
            printf("Enter new Age: ");
            scanf("%d", &u.age);
        }
        fprintf(f2, "%d,%s,%d\n", u.id, u.name, u.age);
    }

    fclose(f1);
    fclose(f2);

    if (f) {
        remove(FILENAME);
        rename("temp.txt", FILENAME);
        printf("\nUser with ID %d was updated successfully.\n", id);
    } else {
        remove("temp.txt");
        printf("\nSorry, a user with ID %d was not found.\n", id);
    }
}

void deleteUser() {
    int id, f = 0;
    User u;
    FILE *f1, *f2;

    printf("\nEnter the ID of the user you want to delete: ");
    scanf("%d", &id);

    f1 = fopen(FILENAME, "r");
    if (f1 == NULL) {
        printf("\nCannot open the user file. Nothing to delete.\n");
        return;
    }

    f2 = fopen("temp.txt", "w");
    if (f2 == NULL) {
        printf("Error: Could not create a temporary file.\n");
        fclose(f1);
        return;
    }

    while (fscanf(f1, "%d,%99[^,],%d\n", &u.id, u.name, &u.age) == 3) {
        if (u.id == id) {
            f = 1;
        } else {
            fprintf(f2, "%d,%s,%d\n", u.id, u.name, u.age);
        }
    }

    fclose(f1);
    fclose(f2);

    if (f) {
        remove(FILENAME);
        rename("temp.txt", FILENAME);
        printf("\nUser with ID %d was deleted successfully.\n", id);
    } else {
        remove("temp.txt");
        printf("\nSorry, a user with ID %d was not found.\n", id);
    }
}
