#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <iostream>
#include <vector>
#include <limits>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "student.h"
#include "database.h"

using namespace std;

// === CÁC HÀM TIỆN ÍCH ===

size_t utf8_length(const string& str) {
    size_t len = 0;
    for (char c : str) {
        if ((c & 0xC0) != 0x80) len++;
    }
    return len;
}

string padRight(const string& str, size_t width) {
    size_t len = utf8_length(str);
    if (len >= width) return str;
    return str + string(width - len, ' ');
}

string padLeft(const string& str, size_t width) {
    size_t len = utf8_length(str);
    if (len >= width) return str;
    return string(width - len, ' ') + str;
}

string toLowerCase(const string& str) {
    string res = str;
    for (size_t i = 0; i < res.length(); ++i) {
        res[i] = (char)tolower((unsigned char)res[i]);
    }
    return res;
}

double inputValidGrade(const string &prompt) {
    double val;
    while (true) {
        cout << prompt;
        string input;
        getline(cin, input);
        
        size_t first = input.find_first_not_of(" \t\r\n");
        if (first == string::npos) return 0.0;
        size_t last = input.find_last_not_of(" \t\r\n");
        input = input.substr(first, (last - first + 1));
        
        if (toLowerCase(input) == "none") return 0.0;
        
        try {
            val = stod(input);
            if (val >= 0.0 && val <= 10.0) return val;
            cout << "Điểm phải nằm trong khoảng từ 0.0 đến 10.0. Vui lòng nhập lại!\n";
        } catch (...) {
            cout << "Nhập sai định dạng số. Vui lòng nhập lại!\n";
        }
    }
}

// === CÁC HÀM HIỂN THỊ VÀ XỬ LÝ DANH SÁCH ===

void printStudentsTable(const vector<SinhVien>& students) {
    if (students.empty()) {
        cout << "Danh sách sinh viên đang trống.\n";
        return;
    }
    string sep = "+--------------+------------------------------+------------+-----------+------------+----------+--------------+";
    cout << "\n" << sep << "\n";
    cout << "| " << padRight("MSSV", 12) 
         << " | " << padRight("Họ và Tên", 28) 
         << " | " << padRight("Ngày Sinh", 10) 
         << " | " << padRight("Giới Tính", 9) 
         << " | " << padRight("Mã Lớp", 10) 
         << " | " << padRight("Điểm TB", 8) 
         << " | " << padRight("Học lực", 12) << " |\n";
    cout << sep << "\n";
    
    for (const auto& s : students) {
        stringstream ssTB;
        ssTB << fixed << setprecision(2) << s.diemTB;
        
        cout << "| " << padRight(s.MaSV, 12)
             << " | " << padRight(s.HoTen, 28)
             << " | " << padRight(s.NgaySinh, 10)
             << " | " << padRight(s.GioiTinh ? "Nam" : "Nữ", 9)
             << " | " << padRight(s.MaLop, 10)
             << " | " << padLeft(ssTB.str(), 8)
             << " | " << padRight(s.hocLuc, 12) << " |\n";
    }
    cout << sep << "\n";
}

void sortStudentsByGPA(vector<SinhVien>& students) {
    sort(students.begin(), students.end(), [](const SinhVien& a, const SinhVien& b) {
        return a.diemTB > b.diemTB;
    });
    cout << "Đã sắp xếp danh sách sinh viên theo Điểm trung bình giảm dần!\n";
    printStudentsTable(students);
}

void filterStudentsByClassification(const vector<SinhVien>& students) {
    cout << "Chọn học lực muốn lọc (1. Giỏi, 2. Khá, 3. Trung bình, 4. Yếu): ";
    int choice;
    if (!(cin >> choice)) {
        cin.clear(); cin.ignore(10000, '\n');
        return;
    }
    cin.ignore(10000, '\n');
    
    string target = "";
    if (choice == 1) target = "Giỏi";
    else if (choice == 2) target = "Khá";
    else if (choice == 3) target = "Trung bình";
    else if (choice == 4) target = "Yếu";
    else return;
    
    vector<SinhVien> filtered;
    for (const auto& s : students) {
        if (s.hocLuc == target) filtered.push_back(s);
    }
    printStudentsTable(filtered);
}

void displayStats(const vector<SinhVien>& students) {
    int g = 0, k = 0, tb = 0, y = 0;
    for (const auto& s : students) {
        if (s.hocLuc == "Giỏi") g++;
        else if (s.hocLuc == "Khá") k++;
        else if (s.hocLuc == "Trung bình") tb++;
        else y++;
    }
    cout << "\n--- THỐNG KÊ ---\n";
    cout << "Giỏi: " << g << " | Khá: " << k << " | Trung bình: " << tb << " | Yếu: " << y << "\n";
}

// === MAIN ===

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
        cout << "11. Đọc danh sách từ file CSV\n";
        cout << "12. Xuất báo cáo (TXT)\n";
        cout << " 0. Thoát chương trình\n";
        cout << "=============================================\n";
        cout << "Nhập lựa chọn của bạn: ";
        
        if (!(cin >> choice)) {
            cin.clear(); cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(10000, '\n');

        switch (choice) {
            case 1: {
                SinhVien sv;
                cout << "Nhập Mã số sinh viên: "; getline(cin, sv.MaSV);
                if (sv.MaSV.empty() || sv.MaSV == "none") sv.MaSV = "0";
                
                cout << "Nhập Họ và tên: "; getline(cin, sv.HoTen);
                if (sv.HoTen.empty() || sv.HoTen == "none") sv.HoTen = "0";
                
                cout << "Nhập Ngày sinh (YYYY-MM-DD): "; getline(cin, sv.NgaySinh);
                if (sv.NgaySinh.empty() || sv.NgaySinh == "none") sv.NgaySinh = "1900-01-01";
                
                cout << "Nhập Giới tính (1: Nam, 0: Nữ, mặc định Nam): ";
                string gt; getline(cin, gt);
                sv.GioiTinh = (gt == "0") ? false : true;
                
                cout << "Nhập Địa chỉ: "; getline(cin, sv.DiaChi);
                if (sv.DiaChi.empty() || sv.DiaChi == "none") sv.DiaChi = "Chưa rõ";
                
                cout << "Nhập Mã lớp (VD: K31PM1, K31PM2, K31KHMT): "; getline(cin, sv.MaLop);
                if (sv.MaLop.empty() || sv.MaLop == "none") sv.MaLop = "none"; // Mặc định hợp lệ với FK

                
                if (db.insertSinhVien(sv)) {
                    cout << "Đã thêm sinh viên vào CSDL!\n";
                    cout << "Nhập Mã Môn Học (ví dụ MH01) hoặc Enter để bỏ qua: ";
                    string mmh; getline(cin, mmh);
                    if (!mmh.empty() && mmh != "none") {
                        double diem = inputValidGrade("Nhập Điểm Số: ");
                        db.updateDiem(sv.MaSV, mmh, diem);
                    }
                    db.loadSinhVien(students);
                } else {
                    cout << "Lỗi lưu CSDL.\n";
                }
                break;
            }
            case 2: {
                cout << "Nhập MSSV cần sửa thông tin/điểm: ";
                string id; getline(cin, id);
                bool found = false;
                for (auto& s : students) {
                    if (s.MaSV == id) {
                        found = true;
                        cout << "1. Cập nhật thông tin sinh viên\n2. Cập nhật điểm môn học\nLựa chọn: ";
                        int subchoice;
                        if (cin >> subchoice) {
                            cin.ignore(10000, '\n');
                            if (subchoice == 1) {
                                cout << "Tính năng cập nhật thông tin đang được xử lý ở một module khác.\n";
                            } else if (subchoice == 2) {
                                cout << "Nhập Mã Môn Học cần cập nhật (VD: MH01, MH02): ";
                                string mmh; getline(cin, mmh);
                                if (!mmh.empty()) {
                                    double diem = inputValidGrade("Nhập Điểm Số mới: ");
                                    if (db.updateDiem(s.MaSV, mmh, diem)) {
                                        cout << "Cập nhật điểm thành công!\n";
                                        db.loadSinhVien(students);
                                    } else {
                                        cout << "Cập nhật thất bại. Kiểm tra lại Mã Môn Học.\n";
                                    }
                                }
                            }
                        } else {
                            cin.clear(); cin.ignore(10000, '\n');
                        }
                        break;
                    }
                }
                if (!found) cout << "Không tìm thấy sinh viên!\n";
                break;
            }
            case 3: {
                cout << "Nhập MSSV cần xóa: ";
                string id; getline(cin, id);
                if (db.deleteSinhVien(id)) {
                    cout << "Đã xóa sinh viên!\n";
                    db.loadSinhVien(students);
                } else cout << "Lỗi xóa!\n";
                break;
            }
            case 4: {
                cout << "Nhập MSSV hoặc Tên: ";
                string q; getline(cin, q);
                vector<SinhVien> res;
                for (const auto& s : students) {
                    if (toLowerCase(s.MaSV).find(toLowerCase(q)) != string::npos || 
                        toLowerCase(s.HoTen).find(toLowerCase(q)) != string::npos) {
                        res.push_back(s);
                    }
                }
                printStudentsTable(res);
                break;
            }
            case 5:
                printStudentsTable(students);
                break;
            case 6:
                sortStudentsByGPA(students);
                break;
            case 7:
                filterStudentsByClassification(students);
                break;
            case 8: {
                vector<SinhVien> temp = students;
                sort(temp.begin(), temp.end(), [](const SinhVien& a, const SinhVien& b) { return a.diemTB > b.diemTB; });
                if (temp.size() > 5) temp.resize(5);
                printStudentsTable(temp);
                break;
            }
            case 9:
                displayStats(students);
                break;
            case 10: {
                ofstream f("danhsach.csv");
                if (f.is_open()) {
                    f << "MSSV,HoTen,NgaySinh,GioiTinh,DiaChi,MaLop,DiemTB,HocLuc\n";
                    for (const auto& s : students) {
                        f << s.MaSV << "," << s.HoTen << "," << s.NgaySinh << "," << (s.GioiTinh?"1":"0") << "," << s.DiaChi << "," << s.MaLop << "," << s.diemTB << "," << s.hocLuc << "\n";
                    }
                    cout << "Lưu file thành công!\n";
                }
                break;
            }
            case 11: {
                ifstream f("danhsach.csv");
                if (!f.is_open()) {
                    cout << "Không tìm thấy file danhsach.csv!\n";
                    break;
                }
                string line;
                getline(f, line); // Skip header
                int count = 0;
                while (getline(f, line)) {
                    vector<string> tokens;
                    stringstream ss(line);
                    string item;
                    while (getline(ss, item, ',')) tokens.push_back(item);
                    
                    if (tokens.size() < 8) continue; // Phải đủ các trường cơ bản
                    
                    SinhVien sv;
                    sv.MaSV = tokens[0];
                    sv.HoTen = tokens[1];
                    sv.NgaySinh = tokens[2];
                    sv.GioiTinh = (tokens[3] == "1" || tokens[3] == "Nam");
                    sv.MaLop = tokens[tokens.size() - 3];
                    
                    // Nối lại địa chỉ nếu có chứa dấu phẩy
                    string addr = tokens[4];
                    for (size_t i = 5; i < tokens.size() - 3; i++) {
                        addr += "," + tokens[i];
                    }
                    sv.DiaChi = addr;
                    
                    // Thử insert vào CSDL (sẽ bỏ qua nếu trùng MSSV do SQL khóa, thêm true để không in lỗi rác)
                    if (db.insertSinhVien(sv, true)) count++;
                }
                db.loadSinhVien(students);
                cout << "Đã đọc file CSV và thêm thành công " << count << " sinh viên mới vào CSDL!\n";
                break;
            }
            case 12: {
                ofstream f("baocao.txt");
                if (f.is_open()) {
                    f << "BÁO CÁO DANH SÁCH SINH VIÊN\n";
                    f << "Tổng số sinh viên: " << students.size() << "\n\n";
                    for (const auto& s : students) {
                        f << s.MaSV << " - " << s.HoTen << " - Điểm TB: " << fixed << setprecision(2) << s.diemTB << " (" << s.hocLuc << ")\n";
                    }
                    cout << "Đã xuất báo cáo ra file baocao.txt!\n";
                }
                break;
            }
            case 0:
                db.disconnect();
                cout << "Đã ngắt kết nối.\n";
                break;
        }
    } while (choice != 0);

    return 0;
}