#include <stdio.h>
#include <sqlite3.h>
#include "database.h"
#include <string.h>
#include <ctype.h>

int is_valid_grade(const char *grade) {
    char *valid[] = {"A", "B", "C", "D", "F", "P", "NP", "NULL"};
    for (int i = 0; i < 8; i++) {
        if (strcmp(grade, valid[i]) == 0) return 1;
    }
    return 0;
}

void init_database(sqlite3 *db) {
    char *sql[] = {
        // 1. Enable Foreign Keys (Crucial for SQLite!)
        "PRAGMA foreign_keys = ON;",

        // 2. Classroom table
        "CREATE TABLE IF NOT EXISTS classroom ("
        "building TEXT, room_no TEXT, capacity INTEGER, "
        "PRIMARY KEY (building, room_no));",

        // 3. Department table
        "CREATE TABLE IF NOT EXISTS department ("
        "dept_name TEXT PRIMARY KEY, building TEXT, "
        "budget REAL CHECK(budget >= 0));",

        // 4. Student table
        "CREATE TABLE IF NOT EXISTS student ("
        "ID INTEGER PRIMARY KEY, name TEXT NOT NULL, "
        "dept_name TEXT, tot_cred INTEGER, "
        "FOREIGN KEY (dept_name) REFERENCES department(dept_name));",

        // 5. Instructor table
        "CREATE TABLE IF NOT EXISTS instructor ("
        "ID INTEGER PRIMARY KEY, name TEXT NOT NULL, "
        "dept_name TEXT, "
        "salary REAL CHECK(salary >= 30000), "
        "FOREIGN KEY (dept_name) REFERENCES department(dept_name));",

        // 6. Course table
        "CREATE TABLE IF NOT EXISTS course ("
        "course_id TEXT PRIMARY KEY, title TEXT, dept_name TEXT, credits INTEGER, "
        "FOREIGN KEY (dept_name) REFERENCES department(dept_name));",

        // 7. Advisor table (Links Student to Instructor)
        "CREATE TABLE IF NOT EXISTS advisor ("
        "s_ID INTEGER PRIMARY KEY, i_ID INTEGER, "
        "FOREIGN KEY (s_ID) REFERENCES student(ID), "
        "FOREIGN KEY (i_ID) REFERENCES instructor(ID));",

        // 8. Time Slot table
        "CREATE TABLE IF NOT EXISTS time_slot ("
        "time_slot_id TEXT, day TEXT, start_time TEXT, end_time TEXT, "
        "PRIMARY KEY (time_slot_id, day, start_time));",

        // 9. Section table
        "CREATE TABLE IF NOT EXISTS section ("
        "course_id TEXT, sec_id TEXT, semester TEXT, year INTEGER, "
        "building TEXT, room_no TEXT, time_slot_id TEXT, "
        "PRIMARY KEY (course_id, sec_id, semester, year), "
        "FOREIGN KEY (course_id) REFERENCES course(course_id), "
        "FOREIGN KEY (building, room_no) REFERENCES classroom(building, room_no));",

        // 10. Takes table (Enrollment)
        "CREATE TABLE IF NOT EXISTS takes ("
        "ID INTEGER, course_id TEXT, sec_id TEXT, semester TEXT, year INTEGER, "
        "grade TEXT CHECK(grade IN ('A', 'B', 'C', 'D', 'F', 'P', 'NP', 'NULL')), "
        "PRIMARY KEY (ID, course_id, sec_id, semester, year), "
        "FOREIGN KEY (ID) REFERENCES student(ID));"

        // 11. Teaches table (Instructor Assignment)
        "CREATE TABLE IF NOT EXISTS teaches ("
        "ID INTEGER, course_id TEXT, sec_id TEXT, semester TEXT, year INTEGER, "
        "PRIMARY KEY (ID, course_id, sec_id, semester, year), "
        "FOREIGN KEY (ID) REFERENCES instructor(ID), "
        "FOREIGN KEY (course_id, sec_id, semester, year) REFERENCES section(course_id, sec_id, semester, year));",

        // 12. Prereq table
        "CREATE TABLE IF NOT EXISTS prereq ("
        "course_id TEXT, prereq_id TEXT, "
        "PRIMARY KEY (course_id, prereq_id), "
        "FOREIGN KEY (course_id) REFERENCES course(course_id), "
        "FOREIGN KEY (prereq_id) REFERENCES course(course_id));"
    };

    int num_queries = sizeof(sql) / sizeof(sql[0]);
    for(int i = 0; i < num_queries; i++) {
        char *errMsg = 0;
        int rc = sqlite3_exec(db, sql[i], 0, 0, &errMsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
        }
    }
}

// Add to database.c
void add_department(sqlite3 *db) {
    char name[50], bld[50];
    double budget;
    printf("Dept Name: "); scanf(" %49[^\n]", name);
    printf("Building: "); scanf(" %49[^\n]", bld);
    printf("Budget: "); scanf("%lf", &budget);

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "INSERT INTO department VALUES (?, ?, ?);", -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, bld, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, budget);
    
    if (sqlite3_step(stmt) == SQLITE_DONE) printf("✅ Dept Added!\n");
    else printf("❌ Error: Check if dept already exists.\n");
    sqlite3_finalize(stmt);
}

void add_student(sqlite3 *db) {
    int id, credits;
    char name[50], dept[50];
    printf("ID: "); scanf("%d", &id);
    printf("Name: "); scanf(" %49[^\n]", name);
    printf("Dept Name: "); scanf(" %49[^\n]", dept);
    printf("Total Credits: "); scanf("%d", &credits);

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "INSERT INTO student VALUES (?, ?, ?, ?);", -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, dept, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, credits);

    if (sqlite3_step(stmt) == SQLITE_DONE) printf("✅ Student Added!\n");
    else printf("❌ Error: Foreign Key Constraint failed (Does the Dept exist?).\n");
    sqlite3_finalize(stmt);
}

// --- Add to database.c ---

void add_instructor(sqlite3 *db) {
    int id;
    char name[50], dept[50];
    double salary;
    printf("\n--- Add Instructor ---\n");
    printf("ID: "); scanf("%d", &id);
    printf("Name: "); scanf(" %49[^\n]", name);
    printf("Dept Name: "); scanf(" %49[^\n]", dept);
    printf("Salary: "); scanf("%lf", &salary);

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "INSERT INTO instructor VALUES (?, ?, ?, ?);", -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_text(stmt, 2, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, dept, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, salary);

    if (sqlite3_step(stmt) == SQLITE_DONE) printf("✅ Instructor Added!\n");
    else printf("❌ Error: Check if Dept exists.\n");
    sqlite3_finalize(stmt);
}

void add_course(sqlite3 *db) {
    char id[10], title[50], dept[50];
    int credits;
    printf("\n--- Add Course ---\n");
    printf("Course ID (e.g., CS101): "); scanf("%s", id);
    printf("Title: "); scanf(" %49[^\n]", title);
    printf("Dept Name: "); scanf(" %49[^\n]", dept);
    printf("Credits: "); scanf("%d", &credits);

    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, "INSERT INTO course VALUES (?, ?, ?, ?);", -1, &stmt, 0);
    sqlite3_bind_text(stmt, 1, id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, dept, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, credits);

    if (sqlite3_step(stmt) == SQLITE_DONE) printf("✅ Course Added!\n");
    else printf("❌ Error: Dept likely doesn't exist.\n");
    sqlite3_finalize(stmt);
}

void enroll_student(sqlite3 *db) {
    int s_id, year;
    char c_id[10], sec_id[10], sem[20], grade[5];
    printf("\n--- Enroll Student ---\n");
    printf("Student ID: "); scanf("%d", &s_id);
    printf("Course ID: "); scanf("%s", c_id);
    printf("Section ID: "); scanf("%s", sec_id);
    printf("Semester: "); scanf("%s", sem);
    printf("Year: "); scanf("%d", &year);
    printf("Grade (A/B/C/etc or NULL): "); scanf("%s", grade);

    printf("Grade (A, B, C, D, F, P, NP, NULL): ");
    scanf("%s", grade);

    // VALIDATION STEP
    if (!is_valid_grade(grade)) {
        printf("⚠️ Invalid Grade! Enrollment cancelled.\n");
        return; 
    }

    sqlite3_stmt *stmt;
    // This inserts into the 'takes' table which links Students to Sections
    sqlite3_prepare_v2(db, "INSERT INTO takes VALUES (?, ?, ?, ?, ?, ?);", -1, &stmt, 0);
    sqlite3_bind_int(stmt, 1, s_id);
    sqlite3_bind_text(stmt, 2, c_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, sec_id, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, sem, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, year);
    sqlite3_bind_text(stmt, 6, grade, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE) printf("✅ Enrollment Successful!\n");
    else printf("❌ Error: Check if ID and Course/Section exist.\n");
    sqlite3_finalize(stmt);
}

void view_student_transcript(sqlite3 *db) {
    int s_id;
    printf("\nEnter Student ID for Transcript: ");
    scanf("%d", &s_id);

    sqlite3_stmt *res;
    // The MAGIC: Joining student, takes, and course tables
    char *sql = "SELECT s.name, t.course_id, c.title, t.grade "
                "FROM student s "
                "JOIN takes t ON s.ID = t.ID "
                "JOIN course c ON t.course_id = c.course_id "
                "WHERE s.ID = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) != SQLITE_OK) {
        printf("❌ SQL Error\n");
        return;
    }

    sqlite3_bind_int(res, 1, s_id);

    printf("\n--- 📜 TRANSCRIPT FOR ID: %d ---\n", s_id);
    printf("%-15s | %-10s | %-20s | %-5s\n", "Name", "Course ID", "Title", "Grade");
    printf("--------------------------------------------------------------\n");

    int found = 0;
    while (sqlite3_step(res) == SQLITE_ROW) {
        found = 1;
        printf("%-15s | %-10s | %-20s | %-5s\n", 
               sqlite3_column_text(res, 0),
               sqlite3_column_text(res, 1),
               sqlite3_column_text(res, 2),
               sqlite3_column_text(res, 3));
    }

    if (!found) printf("No records found for this student.\n");
    
    sqlite3_finalize(res);
}

void search_students_by_dept(sqlite3 *db) {
    char dept[50];
    printf("\nEnter Department Name to search: ");
    scanf("%49s", dept);

    sqlite3_stmt *res;
    char *sql = "SELECT ID, name, tot_cred FROM student WHERE dept_name LIKE ?;";
    
    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    
    // Using % allows partial matches (e.g., "Comp" finds "Computer Science")
    char search_term[60];
    sprintf(search_term, "%%%s%%", dept); 
    sqlite3_bind_text(res, 1, search_term, -1, SQLITE_TRANSIENT);

    printf("\n--- 🔍 Search Results: %s ---\n", dept);
    printf("%-5s | %-20s | %-10s\n", "ID", "Name", "Credits");
    printf("----------------------------------------\n");

    while (sqlite3_step(res) == SQLITE_ROW) {
        printf("%-5d | %-20s | %-10d\n", 
               sqlite3_column_int(res, 0),
               sqlite3_column_text(res, 1),
               sqlite3_column_int(res, 2));
    }
    sqlite3_finalize(res);
}

void search_instructors_by_salary(sqlite3 *db) {
    double min_salary;
    printf("\nEnter Minimum Salary: ");
    scanf("%lf", &min_salary);

    sqlite3_stmt *res;
    char *sql = "SELECT name, dept_name, salary FROM instructor WHERE salary >= ? ORDER BY salary DESC;";

    sqlite3_prepare_v2(db, sql, -1, &res, 0);
    sqlite3_bind_double(res, 1, min_salary);

    printf("\n--- 💰 High Earners (>= %.2f) ---\n", min_salary);
    printf("%-20s | %-15s | %-10s\n", "Name", "Department", "Salary");
    printf("--------------------------------------------------\n");

    while (sqlite3_step(res) == SQLITE_ROW) {
        printf("%-20s | %-15s | %-10.2f\n", 
               sqlite3_column_text(res, 0),
               sqlite3_column_text(res, 1),
               sqlite3_column_double(res, 2));
    }
    sqlite3_finalize(res);
}

void list_departments(sqlite3 *db) {
    sqlite3_stmt *res;
    char *sql = "SELECT dept_name, building, budget FROM department;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {
        printf("\n--- 🏢 Department List ---\n");
        printf("%-20s | %-15s | %-10s\n", "Dept Name", "Building", "Budget");
        printf("--------------------------------------------------\n");
        while (sqlite3_step(res) == SQLITE_ROW) {
            printf("%-20s | %-15s | %-10.2f\n", 
                   sqlite3_column_text(res, 0),
                   sqlite3_column_text(res, 1),
                   sqlite3_column_double(res, 2));
        }
    }
    sqlite3_finalize(res);
}

void list_students(sqlite3 *db) {
    sqlite3_stmt *res;
    char *sql = "SELECT ID, name, dept_name, tot_cred FROM student;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {
        printf("\n--- 🎓 Student List ---\n");
        printf("%-5s | %-20s | %-15s | %-5s\n", "ID", "Name", "Dept", "Cred");
        printf("----------------------------------------------------------\n");
        while (sqlite3_step(res) == SQLITE_ROW) {
            printf("%-5d | %-20s | %-15s | %-5d\n", 
                   sqlite3_column_int(res, 0),
                   sqlite3_column_text(res, 1),
                   sqlite3_column_text(res, 2),
                   sqlite3_column_int(res, 3));
        }
    }
    sqlite3_finalize(res);
}

void list_courses(sqlite3 *db) {
    sqlite3_stmt *res;
    char *sql = "SELECT course_id, title, dept_name, credits FROM course;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {
        printf("\n--- 📚 Course List ---\n");
        printf("%-10s | %-25s | %-15s | %-5s\n", "ID", "Title", "Dept", "Cred");
        printf("--------------------------------------------------------------\n");
        while (sqlite3_step(res) == SQLITE_ROW) {
            printf("%-10s | %-25s | %-15s | %-5d\n", 
                   sqlite3_column_text(res, 0),
                   sqlite3_column_text(res, 1),
                   sqlite3_column_text(res, 2),
                   sqlite3_column_int(res, 3));
        }
    }
    sqlite3_finalize(res);
}

void list_instructors(sqlite3 *db) {
    sqlite3_stmt *res;
    char *sql = "SELECT ID, name, dept_name, salary FROM instructor;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &res, 0) == SQLITE_OK) {
        printf("\n--- 👨‍🏫 Instructor List ---\n");
        printf("%-5s | %-20s | %-15s | %-10s\n", "ID", "Name", "Dept", "Salary");
        printf("------------------------------------------------------------\n");
        while (sqlite3_step(res) == SQLITE_ROW) {
            printf("%-5d | %-20s | %-15s | %-10.2f\n", 
                   sqlite3_column_int(res, 0),
                   sqlite3_column_text(res, 1),
                   sqlite3_column_text(res, 2),
                   sqlite3_column_double(res, 3));
        }
    }
    sqlite3_finalize(res);
}

// --- Update & Delete Functions ---

void update_student_credits(sqlite3 *db) {
    int id, new_credits;
    printf("\nEnter Student ID to update: ");
    scanf("%d", &id);
    printf("Enter New Total Credits: ");
    scanf("%d", &new_credits);

    sqlite3_stmt *stmt;
    const char *sql = "UPDATE student SET tot_cred = ? WHERE ID = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, new_credits);
        sqlite3_bind_int(stmt, 2, id);
        
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            if (sqlite3_changes(db) > 0) printf("✅ Student credits updated!\n");
            else printf("⚠️ No student found with ID %d.\n", id);
        } else {
            printf("❌ Error updating record.\n");
        }
    }
    sqlite3_finalize(stmt);
}

void update_instructor_salary(sqlite3 *db) {
    int id;
    double new_salary;
    printf("\nEnter Instructor ID to update: ");
    scanf("%d", &id);
    printf("Enter New Salary: ");
    scanf("%lf", &new_salary);

    sqlite3_stmt *stmt;
    const char *sql = "UPDATE instructor SET salary = ? WHERE ID = ?;";
    
    sqlite3_prepare_v2(db, sql, -1, &stmt, 0);
    sqlite3_bind_double(stmt, 1, new_salary);
    sqlite3_bind_int(stmt, 2, id);

    if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0) 
        printf("✅ Salary updated successfully!\n");
    else 
        printf("❌ Update failed. Check ID or Salary constraint (>= 30000).\n");
        
    sqlite3_finalize(stmt);
}

void delete_student(sqlite3 *db) {
    int id;
    printf("\n⚠️ CAUTION: This will remove the student and their enrollment records.\n");
    printf("Enter Student ID to delete: ");
    scanf("%d", &id);

    sqlite3_stmt *stmt;
    // Note: If you have foreign keys with ON DELETE CASCADE, 'takes' records will vanish too.
    const char *sql = "DELETE FROM student WHERE ID = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, id);
        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0)
            printf("🗑️ Student record deleted.\n");
        else
            printf("⚠️ Student ID not found.\n");
    }
    sqlite3_finalize(stmt);
}