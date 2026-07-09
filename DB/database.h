#ifndef DATABASE_H
#define DATABASE_H

#include <windows.h>
#include <sqlext.h>
#include <string>
#include <vector>
#include "student.h"

class Database {
private:
    SQLHENV hEnv;
    SQLHDBC hDbc;
    SQLHSTMT hStmt;
    bool connected;

public:
    Database();
    ~Database();
    
    bool connect(const std::string& connectionString);
    void disconnect();
    
    // Trong database.h
    bool loadSinhVien(std::vector<SinhVien>& students);
    bool insertSinhVien(const SinhVien& s);
    bool updateSinhVien(const SinhVien& s);
    bool deleteSinhVien(const std::string& maSV);
};

#endif