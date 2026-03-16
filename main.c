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
            case 1: 
                printf("1. Add Dept\n2. List Depts\nChoice: ");
                int d_choice; scanf("%d", &d_choice);
                if(d_choice == 1) add_department(db); else list_departments(db);
                break;
            case 2: 
                printf("1. Add Student\n2. List Students\n3. Update Credits\n4. Delete Student\nChoice: ");
                int s_choice; scanf("%d", &s_choice);
                if(s_choice == 1) add_student(db); 
                else if(s_choice == 2) list_students(db);
                else if(s_choice == 3) update_student_credits(db);
                else delete_student(db);
                break;
            case 3:
                printf("1. Add Instructor\n2. List Instructors\n3. Update Salary\nChoice: ");
                int i_choice; scanf("%d", &i_choice);
                if(i_choice == 1) add_instructor(db);
                else if(i_choice == 2) list_instructors(db);
                else update_instructor_salary(db);
                break;
            case 4: 
                printf("1. Add Course\n2. List Courses\nChoice: ");
                int c_choice; scanf("%d", &c_choice);
                if(c_choice == 1) add_course(db); else list_courses(db);
                break;
            case 5: enroll_student(db); break;
            case 6: view_student_transcript(db); break;
            case 7: search_students_by_dept(db); break;
            case 8: search_instructors_by_salary(db); break;
            case 0: sqlite3_close(db); return 0;
            default: printf("Invalid choice.\n");
        }
    }
}