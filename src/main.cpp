#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include <iostream>
#include <vector>
#include <string>
#include "Student.h"
#include "Database.h"
#include "Utils.h"

using namespace std;

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    vector<SinhVien> students;
    Database db;
    string connStr = "DRIVER={SQL Server};SERVER=localhost\\SQLEXPRESS;DATABASE=QuanLyDiemSinhVien;Trusted_Connection=yes;";
    
    cout << "Đang kết nối tới cơ sở dữ liệu SQL Server...\n";
    if (db.connect(connStr)) {
        cout << "Kết nối SQL Server thành công!\n";
        db.loadSinhVien(students);
    } else {
        cout << "Không thể kết nối SQL Server.\n";
        return 1;
    }

    int choice;
    do {
        cout << "\n=============================================\n";
        cout << "        HỆ THỐNG QUẢN LÝ SINH VIÊN           \n";
        cout << "=============================================\n";
        cout << " 1. Thêm sinh viên mới\n";
        cout << " 2. Cập nhật thông tin & điểm sinh viên\n";
        cout << " 3. Xóa sinh viên (theo MSSV)\n";
        cout << " 4. Tìm kiếm sinh viên\n";
        cout << " 5. Hiển thị danh sách sinh viên\n";
        cout << " 6. Sắp xếp sinh viên theo Điểm trung bình\n";
        cout << " 7. Lọc sinh viên theo Học lực\n";
        cout << " 8. Hiển thị Top 5 sinh viên điểm cao nhất\n";
        cout << " 9. Thống kê số lượng học lực\n";
        cout << "10. Lưu danh sách vào file CSV\n";
        cout << "11. Xuất báo cáo (TXT)\n";
        cout << " 0. Thoát chương trình\n";
        cout << "=============================================\n";
        cout << "Nhập lựa chọn của bạn: ";
        
        if (!(cin >> choice)) {
            cin.clear(); cin.ignore(10000, '\n'); continue;
        }
        cin.ignore(10000, '\n');

        switch (choice) {
            case 1: {
                SinhVien sv;
                cout << "Nhập MSSV: "; getline(cin, sv.MaSV);
                cout << "Nhập Họ và tên: "; getline(cin, sv.HoTen);
                cout << "Nhập Ngày sinh (YYYY-MM-DD): "; getline(cin, sv.NgaySinh);
                cout << "Nhập Giới tính (1: Nam, 0: Nữ): "; 
                string gt; getline(cin, gt); sv.GioiTinh = (gt != "0");
                cout << "Nhập Địa chỉ: "; getline(cin, sv.DiaChi);
                cout << "Nhập Mã lớp: "; getline(cin, sv.MaLop);

                if (db.insertSinhVien(sv)) {
                    cout << "Đã thêm sinh viên vào CSDL!\n";
                    cout << "Nhập Mã Môn Học (hoặc Enter để bỏ qua): ";
                    string mmh; getline(cin, mmh);
                    if (!mmh.empty()) {
                        double diem = inputValidGrade("Nhập Điểm Số: ");
                        db.updateDiem(sv.MaSV, mmh, diem);
                    }
                    db.loadSinhVien(students);
                } else cout << "Lỗi lưu CSDL.\n";
                break;
            }
            case 2: {
                cout << "Nhập MSSV cần sửa điểm: "; string id; getline(cin, id);
                cout << "Nhập Mã Môn Học cần sửa (VD: MH01): "; string mmh; getline(cin, mmh);
                if (!mmh.empty()) {
                    double diem = inputValidGrade("Nhập Điểm Số mới: ");
                    if (db.updateDiem(id, mmh, diem)) {
                        cout << "Cập nhật điểm thành công!\n"; db.loadSinhVien(students);
                    } else cout << "Cập nhật thất bại.\n";
                }
                break;
            }
            case 3: {
                cout << "Nhập MSSV cần xóa: "; string id; getline(cin, id);
                if (db.deleteSinhVien(id)) {
                    cout << "Đã xóa sinh viên!\n"; db.loadSinhVien(students);
                } else cout << "Lỗi xóa!\n";
                break;
            }
            case 4: {
                cout << "Nhập MSSV hoặc Tên: "; string q; getline(cin, q);
                searchStudent(students, q); break;
            }
            case 5: printStudentsTable(students); break;
            case 6: sortStudentsByGPA(students); break;
            case 7: filterStudentsByClassification(students); break;
            case 8: exportTop5Students(students); break;
            case 9: displayStats(students); break;
            case 10: 
                if (saveToCSV(students, "danhsach.csv")) cout << "Lưu CSV thành công!\n";
                break;
            case 11: 
                if (exportReport(students, "baocao.txt")) cout << "Xuất báo cáo TXT thành công!\n";
                break;
            case 0:
                db.disconnect(); cout << "Đã ngắt kết nối.\n"; break;
        }
    } while (choice != 0);
    return 0;
}