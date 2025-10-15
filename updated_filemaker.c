#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[100];
    int age;
} User;

const char *USER_FILE = "users.txt";

FILE *safeOpenFile(const char *filename, const char *mode);
void addUser();
void listUsers();
void editUser();
void removeUser();
void showMenu();

int main(void) {
    int choice;

    while (1) {
        showMenu();

        /* read menu selection safely */
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); /* flush bad input */
            printf("\nThat wasn't a number — please try again.\n");
            continue;
        }

        switch (choice) {
            case 1: addUser();   break;
            case 2: listUsers(); break;
            case 3: editUser();  break;
            case 4: removeUser();break;
            case 5:
                printf("\nGoodbye — exiting program.\n");
                return 0;
            default:
                printf("\nPlease choose a number between 1 and 5.\n");
        }
    }
}

void showMenu() {
    printf("\n=== Simple User Manager ===\n");
    printf("1. Add a new user\n");
    printf("2. Show all users\n");
    printf("3. Update a user\n");
    printf("4. Delete a user\n");
    printf("5. Exit\n");
    printf("---------------------------\n");
    printf("Your choice: ");
}

FILE *safeOpenFile(const char *filename, const char *mode) {
    FILE *f = fopen(filename, mode);
    if (f == NULL) {
        if (strcmp(mode, "r") == 0) {
        } else {
            printf("Error: cannot open '%s' with mode '%s'.\n", filename, mode);
        }
    }
    return f;
}

void addUser() {
    User newUser;
    User existing;
    int duplicate = 0;
    FILE *fp;

    printf("\nEnter a unique numeric ID for the new user: ");
    if (scanf("%d", &newUser.id) != 1) {
        while (getchar() != '\n');
        printf("Invalid ID input — cancelling add.\n");
        return;
    }

    /* Check for duplicate ID (only open for reading if file exists) */
    fp = safeOpenFile(USER_FILE, "r");
    if (fp != NULL) {
        while (fscanf(fp, "%d,%99[^,],%d\n", &existing.id, existing.name, &existing.age) == 3) {
            if (existing.id == newUser.id) {
                duplicate = 1;
                break;
            }
        }
        fclose(fp);
    }

    if (duplicate) {
        printf("A user with ID %d already exists. Please choose another ID.\n", newUser.id);
        return;
    }
    printf("Enter the user's full name: ");
    while (getchar() != '\n'); /* clear leftover newline from previous scanf */
    fgets(newUser.name, sizeof(newUser.name), stdin);
    newUser.name[strcspn(newUser.name, "\n")] = '\0'; /* remove trailing newline */

    printf("Enter the user's age (numeric): ");
    if (scanf("%d", &newUser.age) != 1) {
        while (getchar() != '\n');
        printf("Invalid age input — cancelling add.\n");
        return;
    }

    /* Append the new user to file */
    fp = safeOpenFile(USER_FILE, "a");
    if (fp == NULL) {
        printf("Failed to save the new user.\n");
        return;
    }
    fprintf(fp, "%d,%s,%d\n", newUser.id, newUser.name, newUser.age);
    fclose(fp);

    printf("User added successfully!\n");
}


void listUsers() {
    User u;
    FILE *fp;
    int count = 0;

    fp = safeOpenFile(USER_FILE, "r");
    if (fp == NULL) {
        printf("\nNo users found (file missing or empty).\n");
        return;
    }

    printf("\n--- All Users ---\n");
    printf("%-5s | %-30s | %-5s\n", "ID", "Name", "Age");
    printf("-----------------------------------------------\n");

    while (fscanf(fp, "%d,%99[^,],%d\n", &u.id, u.name, &u.age) == 3) {
        printf("%-5d | %-30s | %-5d\n", u.id, u.name, u.age);
        count++;
    }

    if (count == 0) {
        printf("No user records found in the file.\n");
    }
    

    fclose(fp);
}
void editUser() {
    int targetId;
    int found = 0;
    User u;
    FILE *in, *out;

    printf("\nEnter the ID of the user to update: ");
    if (scanf("%d", &targetId) != 1) {
        while (getchar() != '\n');
        printf("Invalid ID input — cancelling update.\n");
        return;
    }

    in = safeOpenFile(USER_FILE, "r");
    if (in == NULL) {
        printf("Cannot open user file — nothing to update.\n");
        return;
    }

    out = safeOpenFile("temp.txt", "w");
    if (out == NULL) {
        printf("Could not create temporary file — cancelling.\n");
        fclose(in);
        return;
    }

    while (fscanf(in, "%d,%99[^,],%d\n", &u.id, u.name, &u.age) == 3) {
        if (u.id == targetId) {
            found = 1;
            printf("\nUser found (ID %d). Enter new values.\n", targetId);

            printf("New name: ");
            while (getchar() != '\n'); /* clear newline */
            fgets(u.name, sizeof(u.name), stdin);
            u.name[strcspn(u.name, "\n")] = '\0';

            printf("New age: ");
            if (scanf("%d", &u.age) != 1) {
                while (getchar() != '\n');
                printf("Invalid age input. Keeping old age for this user.\n");
            }
        }
        fprintf(out, "%d,%s,%d\n", u.id, u.name, u.age);
    }

    fclose(in);
    fclose(out);

    if (found) {
        remove(USER_FILE);
        rename("temp.txt", USER_FILE);
        printf("User (ID %d) updated successfully.\n", targetId);
    } else {
        remove("temp.txt");
        printf("No user with ID %d was found.\n", targetId);
    }
}


void removeUser() {
    int targetId;
    int found = 0;
    User u;
    FILE *in, *out;

    printf("\nEnter the ID of the user to delete: ");
    if (scanf("%d", &targetId) != 1) {
        while (getchar() != '\n');
        printf("Invalid ID input — cancelling delete.\n");
        return;
    }

    in = safeOpenFile(USER_FILE, "r");
    if (in == NULL) {
        printf("Cannot open user file — nothing to delete.\n");
        return;
    }

    out = safeOpenFile("temp.txt", "w");
    if (out == NULL) {
        printf("Could not create temporary file — cancelling.\n");
        fclose(in);
        return;
    }

    while (fscanf(in, "%d,%99[^,],%d\n", &u.id, u.name, &u.age) == 3) {
        if (u.id == targetId) {
            found = 1; /* skip writing this record to delete it */
        } else {
            fprintf(out, "%d,%s,%d\n", u.id, u.name, u.age);
        }
    }

    fclose(in);
    fclose(out);

    if (found) {
        remove(USER_FILE);
        rename("temp.txt", USER_FILE);
        printf("User (ID %d) deleted successfully.\n", targetId);
    } else {
        remove("temp.txt");
        printf("No user with ID %d was found.\n", targetId);
    }
}


