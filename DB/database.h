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
    
    bool loadSinhVien(std::vector<SinhVien>& students);
    bool insertSinhVien(const SinhVien& s, bool silent = false);
    bool updateSinhVien(const SinhVien& s);
    bool deleteSinhVien(const std::string& maSV);
    bool updateDiem(const std::string& maSV, const std::string& maMH, double diemSo);
    bool loadBaoCaoDiem(std::vector<string>& baoCaoList); 
};

#endif