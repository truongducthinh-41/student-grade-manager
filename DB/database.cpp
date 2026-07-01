#include "database.h"
#include <iostream>

Database::Database() : hEnv(SQL_NULL_HENV), hDbc(SQL_NULL_HDBC), hStmt(SQL_NULL_HSTMT), connected(false) {}

Database::~Database() {
    disconnect();
}

bool Database::connect(const std::string& connectionString) {
    // 1. Cấp phát môi trường (Environment Handle)
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
    SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    
    // 2. Cấp phát kết nối (Connection Handle)
    SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
    
    // 3. Kết nối bằng Connection String
    SQLCHAR outConnStr[1024];
    SQLSMALLINT outConnStrLen;
    SQLRETURN ret = SQLDriverConnectA(hDbc, NULL, (SQLCHAR*)connectionString.c_str(), SQL_NTS,
                                      outConnStr, sizeof(outConnStr), &outConnStrLen, SQL_DRIVER_NOPROMPT);
    
    if (SQL_SUCCEEDED(ret)) {
        connected = true;
        return true;
    }
    return false;
}

void Database::disconnect() {
    if (hStmt != SQL_NULL_HSTMT) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    if (connected && hDbc != SQL_NULL_HDBC) {
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
    }
    if (hEnv != SQL_NULL_HENV) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    connected = false;
}

// Hàm SELECT dữ liệu từ SQL Server đổ vào vector C++
bool Database::loadStudents(std::vector<Student>& students) {
    if (!connected) return false;
    students.clear();
    
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    std::string query = "SELECT MSSV, HoTen, DiemToan, DiemLy, DiemHoa, DiemTB, HocLuc FROM SinhVien";
    
    SQLRETURN ret = SQLExecDirectA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return false;
    }
    
    // Khai báo biến tạm hứng dữ liệu trả về từ SQL Server
    char szId[20], szName[100], szHocLuc[20];
    double dToan, dLy, dHoa, dTB;
    SQLLEN cbId, cbName, cbToan, cbLy, cbHoa, cbTB, cbHocLuc;
    
    // Duyệt qua từng dòng dữ liệu thu được (Result Set)
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_CHAR, szId, sizeof(szId), &cbId);
        SQLGetData(hStmt, 2, SQL_C_CHAR, szName, sizeof(szName), &cbName);
        SQLGetData(hStmt, 3, SQL_C_DOUBLE, &dToan, 0, &cbToan);
        SQLGetData(hStmt, 4, SQL_C_DOUBLE, &dLy, 0, &cbLy);
        SQLGetData(hStmt, 5, SQL_C_DOUBLE, &dHoa, 0, &cbHoa);
        SQLGetData(hStmt, 6, SQL_C_DOUBLE, &dTB, 0, &cbTB);
        SQLGetData(hStmt, 7, SQL_C_CHAR, szHocLuc, sizeof(szHocLuc), &cbHocLuc);
        
        Student s;
        s.id = szId;
        s.name = szName; // Lưu ý: Do SQL Server dùng NVARCHAR trả về UTF-8/hoặc mã hóa Windows, cần xử lý đồng bộ chuỗi
        s.diemToan = dToan;
        s.diemLy = dLy;
        s.diemHoa = dHoa;
        s.diemTB = dTB;
        s.hocLuc = szHocLuc;
        
        students.push_back(s);
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    hStmt = SQL_NULL_HSTMT;
    return true;
}