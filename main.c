#include <stdio.h>
#include "database.h"

int main() {
    sqlite3 *db;
    if (sqlite3_open("university.db", &db) != SQLITE_OK) return 1;
    init_database(db);

    int choice;
    while (1) {
        printf("\n--- 🏛️ University Management System ---\n");
        printf("1. Departments (Add/List)\n");
        printf("2. Students (Add/List)\n");
        printf("3. Instructors (Add/List)\n");
        printf("4. Courses (Add/List)\n");
        printf("5. Enrollment (Enroll Student in Course)\n");
        printf("6. View Transcript\n");
        printf("7. Search Students by Dept\n");
        printf("8. Search Instructors by Salary\n");
        printf("0. Exit\n");
        printf("Selection: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: add_department(db); break;
            case 2: add_student(db); break;
            case 3: add_instructor(db); break;
            case 4: add_course(db); break;
            case 5: enroll_student(db); break;
            case 6: view_student_transcript(db); break;
            case 7: search_students_by_dept(db); break;
            case 8: search_instructors_by_salary(db); break;
            case 0: sqlite3_close(db); return 0;
            default: printf("Invalid choice.\n");
        }
    }
}