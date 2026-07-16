#include "Database.h"
#include "Utils.h"
#include <iostream>

Database::Database() {
    hEnv = SQL_NULL_HENV;
    hDbc = SQL_NULL_HDBC;
    hStmt = SQL_NULL_HSTMT;
    connected = false;
}

Database::~Database() {
    disconnect();
}

bool Database::connect(const std::string& connectionString) {
    if (connected) return true;
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv))) return false;
    if (!SQL_SUCCEEDED(SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))) return false;
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc))) return false;
    
    SQLCHAR outConnStr;
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

std::string wstring_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr, (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr, (int)wstr.size(), &strTo, size_needed, NULL, NULL);
    return strTo;
}

std::wstring utf8_to_wstring(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str, (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str, (int)str.size(), &wstrTo, size_needed);
    return wstrTo;
}

void Database::disconnect() {
    if (hStmt != SQL_NULL_HSTMT) SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    hStmt = SQL_NULL_HSTMT;
    if (hDbc != SQL_NULL_HDBC) { SQLDisconnect(hDbc); SQLFreeHandle(SQL_HANDLE_DBC, hDbc); }
    hDbc = SQL_NULL_HDBC;
    if (hEnv != SQL_NULL_HENV) SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
    hEnv = SQL_NULL_HENV;
    connected = false;
}

bool Database::loadSinhVien(std::vector<SinhVien>& students) {
    if (!connected) return false;
    students.clear();
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) return false;
    std::string query = "SELECT s.MaSV, s.HoTen, CONVERT(VARCHAR, s.NgaySinh, 23), s.GioiTinh, s.DiaChi, s.MaLop, "
                        "ISNULL(AVG(CAST(d.DiemSo AS FLOAT)), 0) AS DiemTB "
                        "FROM SinhVien s LEFT JOIN Diem d ON s.MaSV = d.MaSV "
                        "GROUP BY s.MaSV, s.HoTen, s.NgaySinh, s.GioiTinh, s.DiaChi, s.MaLop";
    SQLRETURN ret = SQLExecDirectA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) { SQLFreeHandle(SQL_HANDLE_STMT, hStmt); hStmt = SQL_NULL_HSTMT; return false; }
    
    SQLCHAR szMaSV[12], szNgaySinh[13], szMaLop[12];
    SQLWCHAR wszHoTen[14], wszDiaChi;
    SQLCHAR cGioiTinh; SQLFLOAT fTB;
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
        sv.hocLuc = evaluateClassification(sv.diemTB);
        students.push_back(sv);
    }
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    hStmt = SQL_NULL_HSTMT;
    return true;
}

bool Database::insertSinhVien(const SinhVien& s, bool silent) {
    if (!connected) return false;
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) return false;
    std::string valMaLop = (s.MaLop == "0" || s.MaLop == "none" || s.MaLop.empty()) ? "NULL" : "'" + sanitizeSQL(s.MaLop) + "'";
    std::string query = "EXEC sp_ThemSinhVien '" + sanitizeSQL(s.MaSV) + "', N'" + sanitizeSQL(s.HoTen) + "', '" +
                        sanitizeSQL(s.NgaySinh) + "', " + (s.GioiTinh ? "1" : "0") + ", N'" +
                        sanitizeSQL(s.DiaChi) + "', " + valMaLop;
    std::wstring wquery = utf8_to_wstring(query);
    SQLRETURN ret = SQLExecDirectW(hStmt, (SQLWCHAR*)wquery.c_str(), SQL_NTS);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt); hStmt = SQL_NULL_HSTMT;
    return SQL_SUCCEEDED(ret);
}

bool Database::updateSinhVien(const SinhVien& s) {
    if (!connected) return false;
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) return false;
    std::string valMaLop = (s.MaLop == "0" || s.MaLop == "none" || s.MaLop.empty()) ? "NULL" : "'" + sanitizeSQL(s.MaLop) + "'";
    std::string query = "UPDATE SinhVien SET HoTen = N'" + sanitizeSQL(s.HoTen) +
                        "', NgaySinh = '" + sanitizeSQL(s.NgaySinh) +
                        "', GioiTinh = " + (s.GioiTinh ? "1" : "0") +
                        ", DiaChi = N'" + sanitizeSQL(s.DiaChi) +
                        "', MaLop = " + valMaLop + " WHERE MaSV = '" + sanitizeSQL(s.MaSV) + "'";
    std::wstring wquery = utf8_to_wstring(query);
    SQLRETURN ret = SQLExecDirectW(hStmt, (SQLWCHAR*)wquery.c_str(), SQL_NTS);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt); hStmt = SQL_NULL_HSTMT;
    return SQL_SUCCEEDED(ret);
}

bool Database::deleteSinhVien(const std::string& maSV) {
    if (!connected) return false;
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) return false;
    std::string query = "EXEC sp_XoaSinhVien '" + sanitizeSQL(maSV) + "'";
    std::wstring wquery = utf8_to_wstring(query);
    SQLRETURN ret = SQLExecDirectW(hStmt, (SQLWCHAR*)wquery.c_str(), SQL_NTS);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt); hStmt = SQL_NULL_HSTMT;
    return (SQL_SUCCEEDED(ret) || ret == SQL_NO_DATA);
}

bool Database::updateDiem(const std::string& maSV, const std::string& maMH, double diemSo) {
    if (!connected) return false;
    if (!SQL_SUCCEEDED(SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt))) return false;
    std::string query = "MERGE Diem AS t USING (SELECT '" + sanitizeSQL(maSV) + "' as MaSV, '" + sanitizeSQL(maMH) + "' as MaMH, 1 as LanThi, " + std::to_string(diemSo) + " as DiemSo) AS s "
                        "ON t.MaSV = s.MaSV AND t.MaMH = s.MaMH "
                        "WHEN MATCHED THEN UPDATE SET DiemSo = s.DiemSo "
                        "WHEN NOT MATCHED THEN INSERT (MaSV, MaMH, LanThi, DiemSo) VALUES (s.MaSV, s.MaMH, s.LanThi, s.DiemSo);";
    std::wstring wquery = utf8_to_wstring(query);
    SQLRETURN ret = SQLExecDirectW(hStmt, (SQLWCHAR*)wquery.c_str(), SQL_NTS);
    SQLFreeHandle(SQL_HANDLE_STMT, hStmt); hStmt = SQL_NULL_HSTMT;
    return (SQL_SUCCEEDED(ret) || ret == SQL_NO_DATA);
}