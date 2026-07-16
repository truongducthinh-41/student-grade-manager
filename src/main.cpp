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

// Hàm bổ trợ hiển thị danh sách SinhVien dạng bảng cơ bản sau khi đổi cấu trúc
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
    
    // Chuỗi kết nối đến SQL Server sử dụng Windows Authentication
    // Thay DATABASE=StudentGradeManager thành DATABASE=QuanLyDiemSinhVien
string connStr = "DRIVER={ODBC Driver 17 for SQL Server};SERVER=localhost\\SQLEXPRESS;DATABASE=QuanLyDiemSinhVien;Trusted_Connection=yes;";
    cout << "Đang kết nối tới cơ sở dữ liệu SQL Server...\n";
    if (db.connect(connStr)) {
        cout << "Kết nối SQL Server thành công!\n";
        if (db.loadSinhVien(students)) {
            cout << "Tải thành công " << students.size() << " sinh viên vào bộ nhớ.\n";
        } else {
            cout << "Không thể tải dữ liệu từ bảng SinhVien.\n";
        }
    } else {
        cout << "Không thể kết nối SQL Server.\n";
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
        cout << "Nhập lựa chọn của bạn: ";
        
        if (!(cin >> choice)) {
            cout << "\nVui lòng chỉ nhập số nguyên hợp lệ từ 0 đến 4!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                // Thêm sinh viên mới vào vector và đồng bộ thẳng xuống SQL Server
                {
                    cout << "\n--- CHỨC NĂNG: THÊM SINH VIÊN MỚI ---\n";
                    SinhVien sv;
                    
                    // 1. Nhập và kiểm tra Mã Số Sinh Viên
                    while (true) {
                        cout << "Nhập Mã số sinh viên: ";
                        getline(cin, sv.MaSV);
                        if (sv.MaSV.empty()) {
                            cout << "Mã số sinh viên không được để trống!\n";
                            continue;
                        }
                        
                        // Kiểm tra trùng lặp mã trong bộ nhớ RAM hiện tại
                        bool duplicate = false;
                        for (const auto& s : students) {
                            if (s.MaSV == sv.MaSV) {
                                duplicate = true;
                                break;
                            }
                        }
                        if (duplicate) {
                            cout << "Mã SV '" << sv.MaSV << "' đã tồn tại. Vui lòng chọn mã khác!\n";
                        } else {
                            break;
                        }
                    }
                    
                    // 2. Nhập và kiểm tra Họ Tên
                    while (true) {
                        cout << "Nhập Họ và tên: ";
                        getline(cin, sv.HoTen);
                        if (sv.HoTen.empty()) {
                            cout << "Họ và tên không được để trống!\n";
                        } else {
                            break;
                        }
                    }
                    
                    // 3. Nhập và kiểm tra Ngày Sinh
                    while (true) {
                        cout << "Nhập Ngày sinh (YYYY-MM-DD): ";
                        getline(cin, sv.NgaySinh);
                        if (sv.NgaySinh.length() != 10 || sv.NgaySinh[4] != '-' || sv.NgaySinh[7] != '-') {
                            cout << "Sai định dạng! Yêu cầu nhập YYYY-MM-DD.\n";
                        } else {
                            break;
                        }
                    }
                    
                    // 4. Nhập và kiểm tra Giới Tính
                    while (true) {
                        cout << "Nhập Giới tính (1: Nam, 0: Nữ): ";
                        string genderStr;
                        getline(cin, genderStr);
                        if (genderStr == "1") {
                            sv.GioiTinh = true;
                            break;
                        } else if (genderStr == "0") {
                            sv.GioiTinh = false;
                            break;
                        } else {
                            cout << "Chỉ chấp nhận '1' hoặc '0'.\n";
                        }
                    }
                    
                    // 5. Nhập Địa Chỉ
                    cout << "Nhập Địa chỉ: ";
                    getline(cin, sv.DiaChi);
                    
                    // 6. Nhập và kiểm tra Mã Lớp
                    while (true) {
                        cout << "Nhập Mã lớp học: ";
                        getline(cin, sv.MaLop);
                        if (sv.MaLop.empty()) {
                            cout << "Mã lớp không được để trống!\n";
                        } else {
                            break;
                        }
                    }
                    
                    // Thực hiện chèn xuống SQL Server trước, nếu thành công mới lưu vào RAM
                    if (db.insertSinhVien(sv)) {
                        students.push_back(sv);
                        cout << "Đã thêm sinh viên '" << sv.HoTen << "' vào CSDL!\n";
                    } else {
                        cout << "Không thể lưu vào SQL Server.\n";
                    }
                }
                break;

            case 2:
                // Hiển thị danh sách sinh viên dưới dạng bảng
                hienThiDanhSachSinhVien(students);
                break;

            case 3:
                // Cập nhật thông tin sinh viên dựa trên MaSV
                {
                    cout << "\n--- CẬP NHẬT THÔNG TIN SINH VIÊN ---\n";
                    cout << "Nhập Mã số sinh viên cần sửa: ";
                    string idToUpdate;
                    getline(cin, idToUpdate);
                    
                    if (idToUpdate.empty()) {
                        cout << "Mã trống. Hủy thao tác cập nhật.\n";
                        break;
                    }
                    
                    // Tìm vị trí sinh viên trong vector RAM
                    auto it = find_if(students.begin(), students.end(), [&](const SinhVien& s) {
                        return s.MaSV == idToUpdate;
                    });
                    
                    if (it != students.end()) {
                        cout << ">> Đã tìm thấy sinh viên! Thông tin hiện tại:\n";
                        cout << " - Họ tên: " << it->HoTen << " | Ngày sinh: " << it->NgaySinh << " | Lớp: " << it->MaLop << "\n";
                        cout << "---------------------------------------------------------\n";
                        cout << "(Để trống và nhấn Enter nếu không thay đổi)\n";
                        
                        SinhVien updatedSV = *it;
                        
                        cout << "Nhập Họ và Tên mới: ";
                        string newName;
                        getline(cin, newName);
                        if (!newName.empty()) updatedSV.HoTen = newName;
                        
                        while (true) {
                            cout << "Nhập Ngày sinh mới (YYYY-MM-DD): ";
                            string newBirth;
                            getline(cin, newBirth);
                            if (newBirth.empty()) break;
                            if (newBirth.length() != 10 || newBirth[4] != '-' || newBirth[7] != '-') {
                                cout << "Sai cấu trúc ngày! Vui lòng điền đúng YYYY-MM-DD.\n";
                            } else {
                                updatedSV.NgaySinh = newBirth;
                                break;
                            }
                        }
                        
                        while (true) {
                            cout << "Nhập Giới tính mới (1: Nam, 0: Nữ): ";
                            string newGender;
                            getline(cin, newGender);
                            if (newGender.empty()) break;
                            if (newGender == "1") {
                                updatedSV.GioiTinh = true;
                                break;
                            } else if (newGender == "0") {
                                updatedSV.GioiTinh = false;
                                break;
                            } else {
                                cout << "Chỉ chấp nhận '1' hoặc '0'.\n";
                            }
                        }
                        
                        cout << "Nhập Địa chỉ mới: ";
                        string newAddress;
                        getline(cin, newAddress);
                        if (!newAddress.empty()) updatedSV.DiaChi = newAddress;
                        
                        cout << "Nhập Mã lớp học mới: ";
                        string newClass;
                        getline(cin, newClass);
                        if (!newClass.empty()) updatedSV.MaLop = newClass;
                        
                        // Đẩy dữ liệu cập nhật xuống SQL Server
                        if (db.updateSinhVien(updatedSV)) {
                            *it = updatedSV;
                            cout << "Cập nhật sinh viên " << idToUpdate << " thành công!\n";
                        } else {
                            cout << "Lỗi cập nhật dữ liệu.\n";
                        }
                    } else {
                        cout << "Không tìm thấy sinh viên có mã số '" << idToUpdate << "'.\n";
                    }
                }
                break;

            case 4:
                // Xóa sinh viên dựa trên mã số sinh viên
                {
                    cout << "\n--- XÓA SINH VIÊN ---\n";
                    cout << "Nhập mã số sinh viên cần xóa: ";
                    string idToDelete;
                    getline(cin, idToDelete);
                    
                    if (idToDelete.empty()) {
                        cout << "Mã trống. Hủy thao tác xóa.\n";
                        break;
                    }
                    
                    // Thực hiện xóa trực tiếp trong SQL Server trước để kiểm tra ràng buộc khóa ngoại
                    if (db.deleteSinhVien(idToDelete)) {
                        // Nếu xóa trong DB thành công, tiến hành xóa trên vector RAM để đồng bộ dữ liệu hiển thị
                        auto it = remove_if(students.begin(), students.end(), [&](const SinhVien& s) {
                            return s.MaSV == idToDelete;
                        });
                        
                        if (it != students.end()) {
                            students.erase(it, students.end());
                            cout << "Đã xóa sinh viên " << idToDelete << " thành công.\n";
                        } else {
                            cout << "Đã xóa trong CSDL.\n";
                        }
                    } else {
                        cout << "Không thể xóa. Có thể lỗi ràng buộc khóa ngoại hoặc không tìm thấy.\n";
                    }
                }
                break;

            case 0:
                db.disconnect();
                cout << "Đã ngắt kết nối CSDL.\n";
                cout << "Tạm biệt!\n";
                break;

            default:
                cout << "Lựa chọn không hợp lệ!\n";
                break;
        }
    } while (choice != 0);

    return 0;
}