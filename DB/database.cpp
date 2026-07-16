#include "database.h"
#include <iostream>

// Hàm khởi tạo (Constructor) - Cài đặt trạng thái ban đầu cho các Handle
Database::Database() {
    hEnv = SQL_NULL_HENV;
    hDbc = SQL_NULL_HDBC;
    hStmt = SQL_NULL_HSTMT;
    connected = false;
}

// Hàm hủy (Destructor) - Tự động ngắt kết nối khi đối tượng bị giải phóng
Database::~Database() {
    disconnect();
}

// Hàm kết nối tới SQL Server sử dụng Connection String
bool Database::connect(const std::string& connectionString) {
    if (connected) return true;

    // 1. Cấp phát môi trường ODBC (Environment Handle)
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv))) {
        return false;
    }

    // 2. Thiết lập phiên bản ứng dụng là ODBC 3.x
    if (!SQL_SUCCEEDED(SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))) {
        return false;
    }

    // 3. Cấp phát kết nối (Connection Handle)
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc))) {
        return false;
    }

    // 4. Thực hiện kết nối thực tế tới SQL Server
    SQLCHAR outConnStr[1024];
    SQLSMALLINT outConnStrLen;
    SQLRETURN ret = SQLDriverConnectA(hDbc, NULL, (SQLCHAR*)connectionString.c_str(), SQL_NTS,
                                     outConnStr, sizeof(outConnStr), &outConnStrLen, SQL_DRIVER_NOPROMPT);

    if (SQL_SUCCEEDED(ret)) {
        connected = true;
        return true;
    } else {
        disconnect();
        return false;
    }
}

// Hàm giải phóng bộ nhớ và ngắt kết nối an toàn
void Database::disconnect() {
    if (hStmt != SQL_NULL_HSTMT) {
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        hStmt = SQL_NULL_HSTMT;
    }
    if (hDbc != SQL_NULL_HDBC) {
        SQLDisconnect(hDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
        hDbc = SQL_NULL_HDBC;
    }
    if (hEnv != SQL_NULL_HENV) {
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
        hEnv = SQL_NULL_HENV;
    }
    connected = false;
}

// Hàm đọc danh sách sinh viên từ bảng SinhVien trong SQL Server
// Hàm ví dụ đọc từ VIEW
bool Database::loadBaoCaoDiem(std::vector<string>& baoCaoList) {
    if (!connected) return false;
    baoCaoList.clear();
    
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) return false;
    
    // Truy vấn dữ liệu từ View
    std::string query = "SELECT MaSV, HoTen, TenLop, TenMH, LanThi, DiemSo FROM v_BaoCaoDiem";
    SQLRETURN ret = SQLExecDirectA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);
    
    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        hStmt = SQL_NULL_HSTMT;
        return false;
    }
    
    // Khai báo biến hứng dữ liệu (Tương tự như hàm loadSinhVien hiện tại của bạn)
    char szMaSV[20], szHoTen[100], szTenLop[100], szTenMH[100];
    SQLINTEGER nLanThi;
    SQLFLOAT fDiemSo;
    SQLLEN cbMaSV, cbHoTen, cbTenLop, cbTenMH, cbLanThi, cbDiemSo;
    
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_CHAR, szMaSV, sizeof(szMaSV), &cbMaSV);
        SQLGetData(hStmt, 2, SQL_C_CHAR, szHoTen, sizeof(szHoTen), &cbHoTen);
        // ... Lấy các dữ liệu còn lại ...
        
        // Push vào danh sách báo cáo
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    hStmt = SQL_NULL_HSTMT;
    return true;
}

// Hàm thêm mới một sinh viên vào bảng SinhVien (Hỗ trợ ký tự tiếng Việt với tiền tố N)
bool Database::insertSinhVien(const SinhVien& s) {
    if (!connected) return false;
    
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) {
        return false;
    }
    

    std::string query = "EXEC sp_ThemSinhVien '" + s.MaSV + "', N'" + s.HoTen + "', '" + 
                        s.NgaySinh + "', " + (s.GioiTinh ? "1" : "0") + ", N'" + 
                        s.DiaChi + "', '" + s.MaLop + "'";
                        
    SQLRETURN ret = SQLExecDirectA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    hStmt = SQL_NULL_HSTMT;
    return SQL_SUCCEEDED(ret);
}

// Hàm cập nhật thông tin sinh viên dựa trên MaSV
bool Database::updateSinhVien(const SinhVien& s) {
    if (!connected) return false;
    
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) {
        return false;
    }
    
    std::string query = "UPDATE SinhVien SET HoTen = N'" + s.HoTen + 
                        "', NgaySinh = '" + s.NgaySinh + 
                        "', GioiTinh = " + (s.GioiTinh ? "1" : "0") + 
                        ", DiaChi = N'" + s.DiaChi + 
                        "', MaLop = '" + s.MaLop + 
                        "' WHERE MaSV = '" + s.MaSV + "'";
                        
    SQLRETURN ret = SQLExecDirectA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    hStmt = SQL_NULL_HSTMT;
    return SQL_SUCCEEDED(ret);
}

// Hàm xóa một sinh viên khỏi bảng dựa trên MaSV truyền vào
bool Database::deleteSinhVien(const std::string& maSV) {
    if (!connected) return false;
    
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) {
        return false;
    }
    
    // Gọi Stored Procedure: sp_XoaSinhVien để tự động xóa Cascading
    std::string query = "EXEC sp_XoaSinhVien '" + maSV + "'";
    SQLRETURN ret = SQLExecDirectA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    hStmt = SQL_NULL_HSTMT;
    return SQL_SUCCEEDED(ret);
}