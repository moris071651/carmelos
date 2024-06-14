#ifndef SQLITE_H
#define SQLITE_H

#include <sqlite3.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
    sqlite3 *db;
    char *err_msg;
} SQLite;

void SQLite_Open(SQLite *sqlite, const char *db_name);

void SQLite_Close(SQLite *sqlite);

void SQLite_Execute(SQLite *sqlite, const char *sql, int (*callback)(void*,int,char**,char**), void *data);

void SQLite_UsserExists(SQLite *sqlite, const char *username, bool *exists);

void SQLite_UserInsert(SQLite *sqlite, const char *username, const char *password);

void SQLite_UserCheck(SQLite *sqlite, const char *username, const char *password, bool *correct);

#endif