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
bool Database::loadSinhVien(std::vector<SinhVien>& students) {
    if (!connected) return false;
    students.clear();
    
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) {
        return false;
    }
    
    std::string query = "SELECT MaSV, HoTen, NgaySinh, GioiTinh, DiaChi, MaLop FROM SinhVien";
    SQLRETURN ret = SQLExecDirectA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        hStmt = SQL_NULL_HSTMT;
        return false;
    }
    
    // Khai báo bộ đệm để hứng dữ liệu từ SQL Server đổ về
    char szMaSV[20], szHoTen[100], szNgaySinh[15], szDiaChi[255], szMaLop[20];
    SQLCHAR cGioiTinh; 
    SQLLEN cbMaSV, cbHoTen, cbNgaySinh, cbGioiTinh, cbDiaChi, cbMaLop;
    
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SQLGetData(hStmt, 1, SQL_C_CHAR, szMaSV, sizeof(szMaSV), &cbMaSV);
        SQLGetData(hStmt, 2, SQL_C_CHAR, szHoTen, sizeof(szHoTen), &cbHoTen);
        SQLGetData(hStmt, 3, SQL_C_CHAR, szNgaySinh, sizeof(szNgaySinh), &cbNgaySinh);
        SQLGetData(hStmt, 4, SQL_C_BIT, &cGioiTinh, 0, &cbGioiTinh);
        SQLGetData(hStmt, 5, SQL_C_CHAR, szDiaChi, sizeof(szDiaChi), &cbDiaChi);
        SQLGetData(hStmt, 6, SQL_C_CHAR, szMaLop, sizeof(szMaLop), &cbMaLop);
        
        SinhVien sv;
        sv.MaSV = (cbMaSV == SQL_NULL_DATA) ? "" : szMaSV;
        sv.HoTen = (cbHoTen == SQL_NULL_DATA) ? "" : szHoTen;
        sv.NgaySinh = (cbNgaySinh == SQL_NULL_DATA) ? "" : szNgaySinh;
        sv.GioiTinh = (cbGioiTinh == SQL_NULL_DATA) ? false : (cGioiTinh != 0);
        sv.DiaChi = (cbDiaChi == SQL_NULL_DATA) ? "" : szDiaChi;
        sv.MaLop = (cbMaLop == SQL_NULL_DATA) ? "" : szMaLop;
        
        students.push_back(sv);
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
    
    // Sử dụng tiền tố N trước các chuỗi Unicode như HoTen, DiaChi để không bị lỗi font tiếng Việt
    std::string query = "INSERT INTO SinhVien (MaSV, HoTen, NgaySinh, GioiTinh, DiaChi, MaLop) VALUES ('" + 
                        s.MaSV + "', N'" + s.HoTen + "', '" + s.NgaySinh + "', " + 
                        (s.GioiTinh ? "1" : "0") + ", N'" + s.DiaChi + "', '" + s.MaLop + "')";
                        
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
    
    std::string query = "DELETE FROM SinhVien WHERE MaSV = '" + maSV + "'";
    SQLRETURN ret = SQLExecDirectA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    hStmt = SQL_NULL_HSTMT;
    return SQL_SUCCEEDED(ret);
}