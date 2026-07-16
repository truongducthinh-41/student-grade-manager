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
        SQLCHAR sqlState[6], msg[1024];
        SQLINTEGER nativeError;
        SQLSMALLINT msgLen;
        if (SQLGetDiagRecA(SQL_HANDLE_DBC, hDbc, 1, sqlState, &nativeError, msg, sizeof(msg), &msgLen) == SQL_SUCCESS) {
            std::cout << "\n[Lỗi Chi Tiết SQL] " << msg << "\n";
        }
        disconnect();
        return false;
    }
}

std::string wstring_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::wstring utf8_to_wstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// Hàm đọc toàn bộ danh sách Sinh Viên từ SQL Server lên C++
bool Database::loadSinhVien(std::vector<SinhVien>& students) {
    if (!connected) return false;
    students.clear();
    
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) return false;
    
    std::string query = 
        "SELECT s.MaSV, s.HoTen, CONVERT(VARCHAR, s.NgaySinh, 23), s.GioiTinh, s.DiaChi, s.MaLop, "
        "ISNULL(AVG(CAST(d.DiemSo AS FLOAT)), 0) AS DiemTB "
        "FROM SinhVien s "
        "LEFT JOIN Diem d ON s.MaSV = d.MaSV "
        "GROUP BY s.MaSV, s.HoTen, s.NgaySinh, s.GioiTinh, s.DiaChi, s.MaLop";
        
    SQLRETURN ret = SQLExecDirectA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);
    
    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        hStmt = SQL_NULL_HSTMT;
        return false;
    }
    
    SQLCHAR szMaSV[50], szNgaySinh[20], szMaLop[50];
    SQLWCHAR wszHoTen[101], wszDiaChi[256];
    SQLCHAR cGioiTinh; 
    SQLFLOAT fTB;
    
    SQLLEN cbMaSV, cbHoTen, cbNgaySinh, cbGioiTinh, cbDiaChi, cbMaLop, cbTB;
    
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        SinhVien sv;
        SQLGetData(hStmt, 1, SQL_C_CHAR, szMaSV, sizeof(szMaSV), &cbMaSV);
        SQLGetData(hStmt, 2, SQL_C_WCHAR, wszHoTen, sizeof(wszHoTen), &cbHoTen);
        SQLGetData(hStmt, 3, SQL_C_CHAR, szNgaySinh, sizeof(szNgaySinh), &cbNgaySinh);
        SQLGetData(hStmt, 4, SQL_C_BIT, &cGioiTinh, 0, &cbGioiTinh);
        SQLGetData(hStmt, 5, SQL_C_WCHAR, wszDiaChi, sizeof(wszDiaChi), &cbDiaChi);
        SQLGetData(hStmt, 6, SQL_C_CHAR, szMaLop, sizeof(szMaLop), &cbMaLop);
        SQLGetData(hStmt, 7, SQL_C_DOUBLE, &fTB, 0, &cbTB);
        
        sv.MaSV = (cbMaSV == SQL_NULL_DATA) ? "0" : (char*)szMaSV;
        sv.HoTen = (cbHoTen == SQL_NULL_DATA) ? "0" : wstring_to_utf8(std::wstring((wchar_t*)wszHoTen));
        sv.NgaySinh = (cbNgaySinh == SQL_NULL_DATA) ? "1900-01-01" : (char*)szNgaySinh;
        sv.GioiTinh = (cGioiTinh != 0); 
        sv.DiaChi = (cbDiaChi == SQL_NULL_DATA) ? "0" : wstring_to_utf8(std::wstring((wchar_t*)wszDiaChi));
        sv.MaLop = (cbMaLop == SQL_NULL_DATA) ? "0" : (char*)szMaLop;
        
        sv.diemTB = (cbTB == SQL_NULL_DATA) ? 0.0 : fTB;
        if (sv.diemTB >= 8.0) sv.hocLuc = "Giỏi";
        else if (sv.diemTB >= 6.5) sv.hocLuc = "Khá";
        else if (sv.diemTB >= 5.0) sv.hocLuc = "Trung bình";
        else sv.hocLuc = "Yếu";
        
        students.push_back(sv);
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    hStmt = SQL_NULL_HSTMT;
    return true;
}

bool Database::updateDiem(const std::string& maSV, const std::string& maMH, double diemSo) {
    if (!connected) return false;
    
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) return false;
    
    // Câu lệnh MERGE để Insert hoặc Update 1 môn
    std::string query = 
        "MERGE Diem AS t USING (SELECT '" + maSV + "' as MaSV, '" + maMH + "' as MaMH, 1 as LanThi, " + std::to_string(diemSo) + " as DiemSo) AS s "
        "ON t.MaSV = s.MaSV AND t.MaMH = s.MaMH "
        "WHEN MATCHED THEN UPDATE SET DiemSo = s.DiemSo "
        "WHEN NOT MATCHED THEN INSERT (MaSV, MaMH, LanThi, DiemSo) VALUES (s.MaSV, s.MaMH, s.LanThi, s.DiemSo);";
        
    std::wstring wquery = utf8_to_wstring(query);
    SQLRETURN ret = SQLExecDirectW(hStmt, (SQLWCHAR*)wquery.c_str(), SQL_NTS);
    
    if (!SQL_SUCCEEDED(ret) && ret != SQL_NO_DATA) {
        SQLCHAR sqlState[6], msg[1024];
        SQLINTEGER nativeError;
        SQLSMALLINT msgLen;
        if (SQLGetDiagRecA(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, msg, sizeof(msg), &msgLen) == SQL_SUCCESS) {
            std::cout << "\n[Chi tiết lỗi SQL (Cập nhật Điểm)] " << msg << "\n";
        }
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    hStmt = SQL_NULL_HSTMT;
    return (SQL_SUCCEEDED(ret) || ret == SQL_NO_DATA);
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
bool Database::insertSinhVien(const SinhVien& s, bool silent) {
    if (!connected) return false;
    
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) {
        return false;
    }
    
    std::string valMaLop = (s.MaLop == "0" || s.MaLop == "none" || s.MaLop.empty()) ? "NULL" : "'" + s.MaLop + "'";
    std::string query = "EXEC sp_ThemSinhVien '" + s.MaSV + "', N'" + s.HoTen + "', '" + 
                        s.NgaySinh + "', " + (s.GioiTinh ? "1" : "0") + ", N'" + 
                        s.DiaChi + "', " + valMaLop;
                        
    std::wstring wquery = utf8_to_wstring(query);
    SQLRETURN ret = SQLExecDirectW(hStmt, (SQLWCHAR*)wquery.c_str(), SQL_NTS);
    
    if (!SQL_SUCCEEDED(ret) && !silent) {
        SQLCHAR sqlState[6], msg[1024];
        SQLINTEGER nativeError;
        SQLSMALLINT msgLen;
        if (SQLGetDiagRecA(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, msg, sizeof(msg), &msgLen) == SQL_SUCCESS) {
            std::cout << "\n[Chi tiết lỗi SQL] " << msg << "\n";
        }
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    hStmt = SQL_NULL_HSTMT;
    return SQL_SUCCEEDED(ret);
}

// Hàm cập nhật thông tin sinh viên 
bool Database::updateSinhVien(const SinhVien& s) {
    if (!connected) return false;
    
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) {
        return false;
    }
    
    std::string valMaLop = (s.MaLop == "0" || s.MaLop == "none" || s.MaLop.empty()) ? "NULL" : "'" + s.MaLop + "'";
    std::string query = "UPDATE SinhVien SET HoTen = N'" + s.HoTen + 
                        "', NgaySinh = '" + s.NgaySinh + 
                        "', GioiTinh = " + (s.GioiTinh ? "1" : "0") + 
                        ", DiaChi = N'" + s.DiaChi + 
                        "', MaLop = " + valMaLop + 
                        " WHERE MaSV = '" + s.MaSV + "'";
                        
    std::wstring wquery = utf8_to_wstring(query);
    SQLRETURN ret = SQLExecDirectW(hStmt, (SQLWCHAR*)wquery.c_str(), SQL_NTS);
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    hStmt = SQL_NULL_HSTMT;
    return SQL_SUCCEEDED(ret);
}

// Hàm xóa sinh viên sử dụng Stored Procedure (Xóa điểm trước, xóa SV sau)
bool Database::deleteSinhVien(const std::string& maSV) {
    if (!connected) return false;
    
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) {
        return false;
    }
    
    // Gọi Stored Procedure: sp_XoaSinhVien 
    std::string query = "EXEC sp_XoaSinhVien '" + maSV + "'";
    
    std::wstring wquery = utf8_to_wstring(query);
    SQLRETURN ret = SQLExecDirectW(hStmt, (SQLWCHAR*)wquery.c_str(), SQL_NTS);
    
    // SQL_NO_DATA (100) happens if DELETE affects 0 rows, which we can consider harmless or just log it.
    if (!SQL_SUCCEEDED(ret) && ret != SQL_NO_DATA) {
        SQLCHAR sqlState[6], msg[1024];
        SQLINTEGER nativeError;
        SQLSMALLINT msgLen;
        if (SQLGetDiagRecA(SQL_HANDLE_STMT, hStmt, 1, sqlState, &nativeError, msg, sizeof(msg), &msgLen) == SQL_SUCCESS) {
            std::cout << "\n[Chi tiết lỗi SQL (Xóa)] " << msg << "\n";
        }
    }
    
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    hStmt = SQL_NULL_HSTMT;
    return (SQL_SUCCEEDED(ret) || ret == SQL_NO_DATA);
}