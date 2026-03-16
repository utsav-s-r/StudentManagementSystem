#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

// Database core
void init_database(sqlite3 *db);

// Department Functions
void add_department(sqlite3 *db);
void list_departments(sqlite3 *db);

// Student Functions (Updated for the new schema)
void add_student(sqlite3 *db);
void list_students(sqlite3 *db);

// Instructor Functions
void add_instructor(sqlite3 *db);

// Course & Enrollment
void add_course(sqlite3 *db);
void enroll_student(sqlite3 *db); // This interacts with the 'takes' table

void view_student_transcript(sqlite3 *db);

void search_students_by_dept(sqlite3 *db);
void search_instructors_by_salary(sqlite3 *db);

#endif