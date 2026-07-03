bool Database::loadSinhVien(std::vector<SinhVien>& students) {
    if (!connected) return false;
    students.clear();
    
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    // Query theo cấu trúc mới
    std::string query = "SELECT MaSV, HoTen, NgaySinh, GioiTinh, DiaChi, MaLop FROM SinhVien";
    
    SQLRETURN ret = SQLExecDirectA(hStmt, (SQLCHAR*)query.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return false;
    }
    
    // Khai báo biến tạm
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