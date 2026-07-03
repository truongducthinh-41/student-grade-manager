#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>

using namespace std;

struct SinhVien {
    string MaSV;
    string HoTen;
    string NgaySinh;
    bool GioiTinh; // BIT trong SQL ánh xạ sang bool
    string DiaChi;
    string MaLop;
};

// Nếu muốn quản lý thêm điểm, bạn có thể tạo thêm struct Diem, MonHoc...
struct Diem {
    string MaSV;
    string MaMH;
    int LanThi;
    double DiemSo;
};

#endif