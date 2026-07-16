#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>

using namespace std;

// Cấu trúc map trực tiếp với bảng LopHoc
struct LopHoc {
    string MaLop;
    string TenLop;
    string Khoa;
};

// Cấu trúc map trực tiếp với bảng SinhVien
struct SinhVien {
    string MaSV;
    string HoTen;
    string NgaySinh;
    bool GioiTinh; 
    string DiaChi;
    string MaLop;
};

// Cấu trúc map trực tiếp với bảng MonHoc
struct MonHoc {
    string MaMH;
    string TenMH;
    int SoTinChi;
};

// Cấu trúc map trực tiếp với bảng Diem
struct Diem {
    string MaSV;
    string MaMH;
    int LanThi;
    double DiemSo;
};

// (Giữ nguyên cấu trúc Student cũ nếu bạn vẫn muốn dùng cho việc tính GPA/Hiển thị)
struct Student {
    string id;
    string name;
    double diemToan;
    double diemLy;
    double diemHoa;
    double diemTB;
    string hocLuc;
};

#endif