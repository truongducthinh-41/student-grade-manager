#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>

using namespace std;

struct SinhVien {
    string MaSV;
    string HoTen;
    string NgaySinh;
    bool GioiTinh; 
    string DiaChi;
    string MaLop;
};

struct MonHoc {
    string MaMH;
    string TenMH;
    int SoTinChi;
};

struct Diem {
    string MaSV;
    string MaMH;
    int LanThi;
    double DiemSo;
};

struct LopHoc {
    string MaLop;
    string TenLop;
    string Khoa;
};

#endif