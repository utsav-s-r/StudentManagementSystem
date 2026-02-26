#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>

// Function Prototypes
void init_database(sqlite3 *db);
void add_student(sqlite3 *db);
void list_students(sqlite3 *db);
void delete_student(sqlite3 *db);
void update_student(sqlite3 *db);
void search_student(sqlite3 *db);
void view_statistics(sqlite3 *db);
void export_to_csv(sqlite3 *db);

#endif