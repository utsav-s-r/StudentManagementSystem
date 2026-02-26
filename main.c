#include <stdio.h>
#include <stdlib.h>
#include "database.h" // This links your main file to your functions

int main() {
    sqlite3 *db;
    int rc = sqlite3_open("students.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    // Ensure the table exists before we start
    init_database(db);

    int choice;
    while (1) {
        printf("\n--- 🎓 Student Management System ---\n");
        printf("1. Add New Student\n");
        printf("2. View All Students\n");
        printf("3. Delete a Student\n");
        printf("4. Update Student Grade\n");
        printf("5. Search Student by Name\n");
        printf("6. View Class Statistics\n");
        printf("7. Export to CSV (Excel)\n");
        printf("0. Exit\n");
        printf("Select an option: ");
        
        // Safety: handle non-numeric input
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // clear buffer
            continue;
        }

        switch (choice) {
            case 1: add_student(db); break;
            case 2: list_students(db); break;
            case 3: delete_student(db); break;
            case 4: update_student(db); break;
            case 5: search_student(db); break;
            case 6: view_statistics(db); break;
            case 7: export_to_csv(db); break;
            case 0: 
                sqlite3_close(db);
                printf("Goodbye!\n");
                return 0;
            default: printf("Option not found. Try again.\n");
        }
    }
    return 0;
}