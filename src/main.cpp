#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <iostream>
#include <vector>
#include <limits>
#include <string>
#include <algorithm>
#include "student.h"
#include "database.h"

using namespace std;

void hienThiDanhSachSinhVien(const vector<SinhVien>& students) {
    if (students.empty()) {
        cout << "[Thông báo] Danh sách sinh viên đang trống.\n";
        return;
    }
    cout << "\n====================================================================================\n";
    cout << "                         DANH SÁCH CHI TIẾT SINH VIÊN                               \n";
    cout << "====================================================================================\n";
    cout << "Mã SV       | Họ và Tên                    | Ngày Sinh  | Giới Tính | Mã Lớp\n";
    cout << "------------------------------------------------------------------------------------\n";
    for (const auto& s : students) {
        printf("%-11s | %-28s | %-10s | %-9s | %s\n", 
               s.MaSV.c_str(), 
               s.HoTen.c_str(), 
               s.NgaySinh.c_str(), 
               (s.GioiTinh ? "Nam" : "Nữ"), 
               s.MaLop.c_str());
    }
    cout << "====================================================================================\n";
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    vector<SinhVien> students;
    int choice;
    Database db;
    
    // Đã cập nhật chuỗi kết nối sang DATABASE=QuanLyDiemSinhVien
    string connStr = "DRIVER={ODBC Driver 17 for SQL Server};SERVER=localhost\\SQLEXPRESS;DATABASE=QuanLyDiemSinhVien;Trusted_Connection=yes;";
    
    cout << "[Hệ thống] Đang kết nối tới cơ sở dữ liệu SQL Server...\n";
    if (db.connect(connStr)) {
        cout << "[Hệ thống] Kết nối SQL Server thành công!\n";
        if (db.loadSinhVien(students)) {
            cout << "[Hệ thống] Tải thành công " << students.size() << " sinh viên vào bộ nhớ RAM.\n";
        } else {
            cout << "[Lỗi] Không thể tải dữ liệu từ bảng SinhVien.\n";
        }
    } else {
        cout << "[Thất bại] Không thể kết nối SQL Server. Vui lòng kiểm tra lại cấu hình hệ thống.\n";
        return 1;
    }

    do {
        cout << "\n=============================================\n";
        cout << "        HỆ THỐNG QUẢN LÝ SINH VIÊN (NEW)     \n";
        cout << "=============================================\n";
        cout << " 1. Thêm sinh viên mới (Đồng bộ SQL)\n";
        cout << " 2. Hiển thị danh sách sinh viên hiện tại\n";
        cout << " 3. Cập nhật thông tin sinh viên (theo Mã SV)\n";
        cout << " 4. Xóa sinh viên (theo Mã SV)\n";
        cout << " 0. Thoát chương trình và ngắt kết nối\n";
        cout << "=============================================\n";
        cout << "[Hệ thống] Nhập lựa chọn của bạn (Số nguyên từ 0 đến 4): ";
        
        if (!(cin >> choice)) {
            cout << "\n[Lỗi] Vui lòng chỉ nhập số nguyên hợp lệ từ 0 đến 4!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                {
                    cout << "\n--- CHỨC NĂNG: THÊM SINH VIÊN MỚI ---\n";
                    SinhVien sv;
                    
                    // 1. Nhập Mã Số Sinh Viên
                    while (true) {
                        cout << "[Nhập liệu] Nhập Mã số sinh viên (Enter hoặc 'none' để gán '0'): ";
                        getline(cin, sv.MaSV);
                        if (sv.MaSV.empty() || sv.MaSV == "none") {
                            sv.MaSV = "0";
                        }
                        
                        bool duplicate = false;
                        for (const auto& s : students) {
                            if (s.MaSV == sv.MaSV) { duplicate = true; break; }
                        }
                        if (duplicate) {
                            cout << "[Lỗi] Mã SV '" << sv.MaSV << "' đã tồn tại! Vui lòng thử lại.\n";
                        } else {
                            break;
                        }
                    }
                    
                    // 2. Nhập Họ Tên
                    cout << "[Nhập liệu] Nhập Họ và tên sinh viên (Enter hoặc 'none' để gán '0'): ";
                    getline(cin, sv.HoTen);
                    if (sv.HoTen.empty() || sv.HoTen == "none") sv.HoTen = "0";
                    
                    // 3. Nhập Ngày Sinh
                    cout << "[Nhập liệu] Nhập Ngày sinh YYYY-MM-DD (Enter hoặc 'none' để gán '1900-01-01'): ";
                    getline(cin, sv.NgaySinh);
                    if (sv.NgaySinh.empty() || sv.NgaySinh == "none" || sv.NgaySinh == "0") {
                        sv.NgaySinh = "1900-01-01"; // Ép kiểu ngày hợp lệ cho SQL Server
                    }
                    
                    // 4. Nhập Giới Tính
                    cout << "[Nhập liệu] Nhập Giới tính (1: Nam | 0: Nữ | Enter/none để gán 0): ";
                    string genderStr;
                    getline(cin, genderStr);
                    if (genderStr.empty() || genderStr == "none" || genderStr == "0") {
                        sv.GioiTinh = false; // 0
                    } else {
                        sv.GioiTinh = true;  // 1
                    }
                    
                    // 5. Nhập Địa Chỉ
                    cout << "[Nhập liệu] Nhập Địa chỉ nơi ở (Enter hoặc 'none' để gán '0'): ";
                    getline(cin, sv.DiaChi);
                    if (sv.DiaChi.empty() || sv.DiaChi == "none") sv.DiaChi = "0";
                    
                    // 6. Nhập Mã Lớp
                    cout << "[Nhập liệu] Nhập Mã lớp học (Enter hoặc 'none' để gán '0'): ";
                    getline(cin, sv.MaLop);
                    if (sv.MaLop.empty() || sv.MaLop == "none") sv.MaLop = "0";
                    
                    // Đẩy dữ liệu xuống Stored Procedure
                    if (db.insertSinhVien(sv)) {
                        students.push_back(sv);
                        cout << "[Thành công] Đã thêm mới sinh viên '" << sv.HoTen << "' vào CSDL!\n";
                    } else {
                        cout << "[Thất bại] SQL Server từ chối dữ liệu. (Lưu ý: Nếu bạn gán Mã Lớp = 0, bảng LopHoc của SQL Server phải có sẵn lớp mang mã '0').\n";
                    }
                }
                break;

            case 2:
                hienThiDanhSachSinhVien(students);
                break;

            case 3:
                {
                    cout << "\n--- CHỨC NĂNG: CẬP NHẬT THÔNG TIN SINH VIÊN ---\n";
                    cout << "[Nhập liệu] Nhập chính xác Mã số sinh viên (MaSV) cần sửa thông tin: ";
                    string idToUpdate;
                    getline(cin, idToUpdate);
                    
                    if (idToUpdate.empty()) {
                        cout << "[Thông báo] Hủy thao tác.\n";
                        break;
                    }
                    
                    auto it = find_if(students.begin(), students.end(), [&](const SinhVien& s) {
                        return s.MaSV == idToUpdate;
                    });
                    
                    if (it != students.end()) {
                        cout << ">> Đã tìm thấy sinh viên! Thông tin hiện tại:\n";
                        cout << " - Họ tên: " << it->HoTen << " | Ngày sinh: " << it->NgaySinh << " | Lớp: " << it->MaLop << "\n";
                        cout << "---------------------------------------------------------\n";
                        
                        SinhVien updatedSV = *it;
                        
                        cout << "[Nhập liệu] Nhập Họ và Tên mới (Enter hoặc 'none' để gán '0'): ";
                        string newName;
                        getline(cin, newName);
                        if (newName.empty() || newName == "none") updatedSV.HoTen = "0";
                        else updatedSV.HoTen = newName;
                        
                        cout << "[Nhập liệu] Nhập Ngày sinh mới (Enter/none để gán '1900-01-01'): ";
                        string newBirth;
                        getline(cin, newBirth);
                        if (newBirth.empty() || newBirth == "none" || newBirth == "0") updatedSV.NgaySinh = "1900-01-01";
                        else updatedSV.NgaySinh = newBirth;
                        
                        cout << "[Nhập liệu] Nhập Giới tính mới (1: Nam | 0: Nữ | Enter/none để gán 0): ";
                        string newGender;
                        getline(cin, newGender);
                        if (newGender.empty() || newGender == "none" || newGender == "0") updatedSV.GioiTinh = false;
                        else updatedSV.GioiTinh = true;
                        
                        cout << "[Nhập liệu] Nhập Địa chỉ mới (Enter hoặc 'none' để gán '0'): ";
                        string newAddress;
                        getline(cin, newAddress);
                        if (newAddress.empty() || newAddress == "none") updatedSV.DiaChi = "0";
                        else updatedSV.DiaChi = newAddress;
                        
                        cout << "[Nhập liệu] Nhập Mã lớp học mới (Enter hoặc 'none' để gán '0'): ";
                        string newClass;
                        getline(cin, newClass);
                        if (newClass.empty() || newClass == "none") updatedSV.MaLop = "0";
                        else updatedSV.MaLop = newClass;
                        
                        if (db.updateSinhVien(updatedSV)) {
                            *it = updatedSV; 
                            cout << "[Thành công] Thông tin sinh viên " << idToUpdate << " đã được cập nhật!\n";
                        } else {
                            cout << "[Thất bại] Lỗi CSDL. Hãy chắc chắn Mã lớp mới tồn tại.\n";
                        }
                    } else {
                        cout << "[Thất bại] Không tìm thấy sinh viên '" << idToUpdate << "'.\n";
                    }
                }
                break;

            case 4:
                {
                    cout << "\n--- CHỨC NĂNG: XÓA SINH VIÊN KHỎI HỆ THỐNG ---\n";
                    cout << "[Nhập liệu] Nhập mã số sinh viên cần xóa: ";
                    string idToDelete;
                    getline(cin, idToDelete);
                    
                    if (idToDelete.empty()) break;
                    
                    if (db.deleteSinhVien(idToDelete)) {
                        auto it = remove_if(students.begin(), students.end(), [&](const SinhVien& s) {
                            return s.MaSV == idToDelete;
                        });
                        
                        if (it != students.end()) {
                            students.erase(it, students.end());
                            cout << "[Thành công] Đã xóa hoàn toàn sinh viên " << idToDelete << ".\n";
                        }
                    } else {
                        cout << "[Thất bại] Không thể xóa. Vui lòng kiểm tra lại CSDL.\n";
                    }
                }
                break;

            case 0:
                db.disconnect();
                cout << "[Hệ thống] Đã ngắt kết nối an toàn với CSDL.\n";
                break;

            default:
                cout << "[Cảnh báo] Lựa chọn không hợp lệ!\n";
                break;
        }
    } while (choice != 0);

    return 0;
}