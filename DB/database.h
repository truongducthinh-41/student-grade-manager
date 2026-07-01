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
    
    // Các hàm ánh xạ chức năng cốt lõi
    bool loadStudents(std::vector<Student>& students);
    bool insertStudent(const Student& s);
    bool updateStudent(const Student& s);
    bool deleteStudent(const std::string& id);
};

#endif