#include "sqlite.h"

void SQLite_Open(SQLite *sqlite, const char *db_name) {
    sqlite3_open(db_name, &sqlite->db);
}

void SQLite_Close(SQLite *sqlite) {
    sqlite3_close(sqlite->db);
}

void SQLite_Execute(SQLite *sqlite, const char *sql, int (*callback)(void*,int,char**,char**), void *data) {
    sqlite3_exec(sqlite->db, sql, callback, data, &sqlite->err_msg);
}

static int callback(void *data, int argc, char **argv, char **azColName) {
    bool *exists = (bool *) data;
    *exists = true;
    return 0;
}

static int callback2(void *data, int argc, char **argv, char **azColName) {
    bool *correct = (bool *) data;
    *correct = true;
    return 0;
}

void SQLite_UserExists(SQLite *sqlite, const char *username, bool *exists) {
    char sql[512];
    sprintf(sql, "SELECT * FROM users WHERE username = '%s'", username);
    sqlite3_exec(sqlite->db, sql, callback, exists, &sqlite->err_msg);
}

void SQLite_UserInsert(SQLite *sqlite, const char *username, const char *password) {
    char sql[512];
    sprintf(sql, "INSERT INTO users (username, password) VALUES ('%s', '%s')", username, password);
    sqlite3_exec(sqlite->db, sql, NULL, NULL, &sqlite->err_msg);
}

void SQLite_UserCheck(SQLite *sqlite, const char *username, const char *password, bool *correct) {
    char sql[512];
    sprintf(sql, "SELECT * FROM users WHERE username = '%s' AND password = '%s'", username, password);
    sqlite3_exec(sqlite->db, sql, callback2, correct, &sqlite->err_msg);
}