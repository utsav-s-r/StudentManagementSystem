#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> // Keep this here so the functions know what sqlite3 is
#include "database.h" // This links the file to your prototypes

// Paste all your functions here (add_student, list_students, etc.)
// ... (The code you've already written)

// --- 1. INITIALIZE DATABASE ---
void init_database(sqlite3 *db) {
    char *sql = "CREATE TABLE IF NOT EXISTS Students("
                "Id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "Name TEXT NOT NULL, "
                "Grade REAL);";
    sqlite3_exec(db, sql, 0, 0, 0);
}

// --- 2. ADD STUDENT (Safe Prepared Statements) ---
void add_student(sqlite3 *db) {
    sqlite3_stmt *res;
    char name[100];
    float grade;

    printf("Enter Student Name: ");
    scanf(" %[^\n]s", name); // This trick allows spaces in names (e.g., "John Doe")
    printf("Enter Grade (0-100): ");
    scanf("%f", &grade);

    char *sql = "INSERT INTO Students(Name, Grade) VALUES(?, ?);";
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    
    // Bind data to prevent SQL Injection
    sqlite3_bind_text(res, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_double(res, 2, grade);

    if (sqlite3_step(res) == SQLITE_DONE) {
        printf("✅ Success: %s added to database.\n", name);
    } else {
        printf("❌ Database Error.\n");
    }
    sqlite3_finalize(res);
}

// --- 3. LIST ALL STUDENTS ---
void list_students(sqlite3 *db) {
    sqlite3_stmt *res;
    char *sql = "SELECT Id, Name, Grade FROM Students;";

    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
        printf("Failed to fetch data.\n");
        return;
    }

    printf("\n%-4s | %-20s | %-6s\n", "ID", "STUDENT NAME", "GRADE");
    printf("------------------------------------------\n");

    while (sqlite3_step(res) == SQLITE_ROW) {
        int id = sqlite3_column_int(res, 0);
        const unsigned char *name = sqlite3_column_text(res, 1);
        double grade = sqlite3_column_double(res, 2);
        
        printf("%-4d | %-20s | %-6.2f\n", id, name, grade);
    }
    sqlite3_finalize(res);
}

// --- 4. Delete Student Data ---
void delete_student(sqlite3 *db) {
    sqlite3_stmt *res;
    int id;

    printf("Enter the ID of the student to delete: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid ID.\n");
        while (getchar() != '\n'); 
        return;
    }

    char *sql = "DELETE FROM Students WHERE Id = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
        printf("❌ Failed to prepare delete statement.\n");
        return;
    }

    // Bind the ID to the '?' placeholder
    sqlite3_bind_int(res, 1, id);

    if (sqlite3_step(res) == SQLITE_DONE) {
        // Check if a row was actually deleted
        if (sqlite3_changes(db) > 0) {
            printf("✅ Student with ID %d deleted.\n", id);
        } else {
            printf("⚠️ No student found with ID %d.\n", id);
        }
    } else {
        printf("❌ Database error during deletion.\n");
    }

    sqlite3_finalize(res);
}

// --- 5. Update/Modify data ---
void update_student(sqlite3 *db) {
    sqlite3_stmt *res;
    int id;
    float new_grade;

    printf("Enter Student ID to update: ");
    scanf("%d", &id);
    printf("Enter New Grade: ");
    scanf("%f", &new_grade);

    char *sql = "UPDATE Students SET Grade = ? WHERE Id = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
        printf("❌ Failed to prepare update statement.\n");
        return;
    }

    // Bind the new grade and the ID
    sqlite3_bind_double(res, 1, new_grade);
    sqlite3_bind_int(res, 2, id);

    if (sqlite3_step(res) == SQLITE_DONE) {
        if (sqlite3_changes(db) > 0) {
            printf("✅ Grade updated for ID %d.\n", id);
        } else {
            printf("⚠️ Student ID %d not found.\n", id);
        }
    } else {
        printf("❌ Error updating record.\n");
    }

    sqlite3_finalize(res);
}

// --- 6. Search ---
void search_student(sqlite3 *db) {
    sqlite3_stmt *res;
    char search_query[100];
    char sql_with_wildcards[110];

    printf("Enter name to search for: ");
    scanf(" %[^\n]s", search_query);

    // Prepare the search string with SQL wildcards (e.g., %John%)
    sprintf(sql_with_wildcards, "%%%s%%", search_query);

    char *sql = "SELECT Id, Name, Grade FROM Students WHERE Name LIKE ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
        printf("❌ Search failed to initialize.\n");
        return;
    }

    sqlite3_bind_text(res, 1, sql_with_wildcards, -1, SQLITE_STATIC);

    printf("\nSearch Results for '%s':\n", search_query);
    printf("%-4s | %-20s | %-6s\n", "ID", "STUDENT NAME", "GRADE");
    printf("------------------------------------------\n");

    int found = 0;
    while (sqlite3_step(res) == SQLITE_ROW) {
        found = 1;
        printf("%-4d | %-20s | %-6.2f\n", 
               sqlite3_column_int(res, 0), 
               sqlite3_column_text(res, 1), 
               sqlite3_column_double(res, 2));
    }

    if (!found) {
        printf("No students found matching that name.\n");
    }

    sqlite3_finalize(res);
}

// --- 7. View Stats ---
void view_statistics(sqlite3 *db) {
    sqlite3_stmt *res;
    // SQL Aggregate functions: COUNT, AVG, MAX, MIN
    char *sql = "SELECT COUNT(*), AVG(Grade), MAX(Grade), MIN(Grade) FROM Students;";

    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
        printf("❌ Failed to calculate statistics.\n");
        return;
    }

    if (sqlite3_step(res) == SQLITE_ROW) {
        int count = sqlite3_column_int(res, 0);
        
        if (count == 0) {
            printf("\n⚠️ The database is empty. Add students first!\n");
        } else {
            double avg = sqlite3_column_double(res, 1);
            double max = sqlite3_column_double(res, 2);
            double min = sqlite3_column_double(res, 3);

            printf("\n📊 --- CLASS STATISTICS ---\n");
            printf("Total Students: %d\n", count);
            printf("Average Grade:  %.2f\n", avg);
            printf("Highest Grade:  %.2f\n", max);
            printf("Lowest Grade:   %.2f\n", min);
            printf("---------------------------\n");
        }
    }

    sqlite3_finalize(res);
}

// --- 8. Export CSV ---
void export_to_csv(sqlite3 *db) {
    sqlite3_stmt *res;
    char *sql = "SELECT Id, Name, Grade FROM Students;";
    
    // 1. Create/Open a file for writing
    FILE *fp = fopen("students_report.csv", "w");
    if (fp == NULL) {
        printf("❌ Error: Could not create file.\n");
        return;
    }

    // 2. Write the CSV Headers
    fprintf(fp, "ID,Student Name,Grade\n");

    // 3. Get data from SQL and write to file
    sqlite3_prepare_v2(db, sql, -1, &res, 0);

    while (sqlite3_step(res) == SQLITE_ROW) {
        int id = sqlite3_column_int(res, 0);
        const unsigned char *name = sqlite3_column_text(res, 1);
        double grade = sqlite3_column_double(res, 2);
        
        // Write row to CSV (comma separated)
        fprintf(fp, "%d,%s,%.2f\n", id, name, grade);
    }

    fclose(fp); // Always close your files!
    sqlite3_finalize(res);
    printf("✅ Data exported to students_report.csv successfully!\n");
}