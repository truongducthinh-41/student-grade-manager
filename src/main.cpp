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
            cout << "\n[Lỗi] Vui lòng chỉ nhập số nguyên hợp lệ từ 0 đến 4! Không nhập chữ cái hoặc ký tự đặc biệt.\n";
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
                        cout << "[Nhập liệu] Nhập Mã số sinh viên (Ví dụ: SV005, không quá 20 ký tự): ";
                        getline(cin, sv.MaSV);
                        if (sv.MaSV.empty()) {
                            cout << "[Lỗi] Mã số sinh viên bắt buộc phải nhập, không được để trống!\n";
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
                            cout << "[Lỗi] Mã SV '" << sv.MaSV << "' đã tồn tại trong hệ thống. Vui lòng chọn mã khác!\n";
                        } else {
                            break;
                        }
                    }
                    
                    // 2. Nhập và kiểm tra Họ Tên
                    while (true) {
                        cout << "[Nhập liệu] Nhập Họ và tên sinh viên (Có thể nhập tiếng Việt có dấu): ";
                        getline(cin, sv.HoTen);
                        if (sv.HoTen.empty()) {
                            cout << "[Lỗi] Họ và tên sinh viên không được để trống!\n";
                        } else {
                            break;
                        }
                    }
                    
                    // 3. Nhập và kiểm tra Ngày Sinh
                    while (true) {
                        cout << "[Nhập liệu] Nhập Ngày sinh (Định dạng mẫu: YYYY-MM-DD, Ví dụ: 2005-01-15): ";
                        getline(cin, sv.NgaySinh);
                        if (sv.NgaySinh.length() != 10 || sv.NgaySinh[4] != '-' || sv.NgaySinh[7] != '-') {
                            cout << "[Lỗi] Sai định dạng! Bạn phải nhập đúng chuẩn 10 ký tự: YYYY-MM-DD.\n";
                        } else {
                            break;
                        }
                    }
                    
                    // 4. Nhập và kiểm tra Giới Tính
                    while (true) {
                        cout << "[Nhập liệu] Nhập Giới tính (Nhập số 1 nếu là Nam | Nhập số 0 nếu là Nữ): ";
                        string genderStr;
                        getline(cin, genderStr);
                        if (genderStr == "1") {
                            sv.GioiTinh = true;
                            break;
                        } else if (genderStr == "0") {
                            sv.GioiTinh = false;
                            break;
                        } else {
                            cout << "[Lỗi] Dữ liệu sai! Chỉ chấp nhận ký tự số '1' hoặc '0'. Vui lòng nhập lại.\n";
                        }
                    }
                    
                    // 5. Nhập Địa Chỉ
                    cout << "[Nhập liệu] Nhập Địa chỉ nơi ở (Ví dụ: Quận 1, TP.HCM): ";
                    getline(cin, sv.DiaChi);
                    
                    // 6. Nhập và kiểm tra Mã Lớp
                    while (true) {
                        cout << "[Nhập liệu] Nhập Mã lớp học (Phải khớp với bảng LopHoc, Ví dụ: K31PM1): ";
                        getline(cin, sv.MaLop);
                        if (sv.MaLop.empty()) {
                            cout << "[Lỗi] Mã lớp không được phép để trống!\n";
                        } else {
                            break;
                        }
                    }
                    
                    // Thực hiện chèn xuống SQL Server trước, nếu thành công mới lưu vào RAM
                    if (db.insertSinhVien(sv)) {
                        students.push_back(sv);
                        cout << "[Thành công] Đã thêm mới và đồng bộ sinh viên '" << sv.HoTen << "' vào CSDL!\n";
                    } else {
                        cout << "[Thất bại] Không thể lưu vào SQL Server. Vui lòng kiểm tra lại tính chính xác của Mã lớp hoặc kết nối.\n";
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
                    cout << "\n--- CHỨC NĂNG: CẬP NHẬT THÔNG TIN SINH VIÊN ---\n";
                    cout << "[Nhập liệu] Nhập chính xác Mã số sinh viên (MaSV) cần sửa thông tin: ";
                    string idToUpdate;
                    getline(cin, idToUpdate);
                    
                    if (idToUpdate.empty()) {
                        cout << "[Thông báo] Mã trống. Hủy thao tác cập nhật.\n";
                        break;
                    }
                    
                    // Tìm vị trí sinh viên trong vector RAM
                    auto it = find_if(students.begin(), students.end(), [&](const SinhVien& s) {
                        return s.MaSV == idToUpdate;
                    });
                    
                    if (it != students.end()) {
                        cout << ">> Đã tìm thấy sinh viên! Thông tin hiện tại trên hệ thống:\n";
                        cout << " - Họ tên: " << it->HoTen << " | Ngày sinh: " << it->NgaySinh << " | Lớp: " << it->MaLop << "\n";
                        cout << "---------------------------------------------------------\n";
                        cout << "(Lưu ý: Để trống và nhấn Enter nếu bạn không muốn thay đổi mục đó)\n";
                        
                        SinhVien updatedSV = *it;
                        
                        cout << "[Nhập liệu] Nhập Họ và Tên mới: ";
                        string newName;
                        getline(cin, newName);
                        if (!newName.empty()) updatedSV.HoTen = newName;
                        
                        while (true) {
                            cout << "[Nhập liệu] Nhập Ngày sinh mới (YYYY-MM-DD): ";
                            string newBirth;
                            getline(cin, newBirth);
                            if (newBirth.empty()) break; // Giữ nguyên ngày cũ
                            if (newBirth.length() != 10 || newBirth[4] != '-' || newBirth[7] != '-') {
                                cout << "[Lỗi] Sai cấu trúc ngày! Vui lòng điền đúng dạng YYYY-MM-DD hoặc bỏ trống.\n";
                            } else {
                                updatedSV.NgaySinh = newBirth;
                                break;
                            }
                        }
                        
                        while (true) {
                            cout << "[Nhập liệu] Nhập Giới tính mới (1: Nam, 0: Nữ): ";
                            string newGender;
                            getline(cin, newGender);
                            if (newGender.empty()) break; // Giữ nguyên
                            if (newGender == "1") {
                                updatedSV.GioiTinh = true;
                                break;
                            } else if (newGender == "0") {
                                updatedSV.GioiTinh = false;
                                break;
                            } else {
                                cout << "[Lỗi] Chỉ chấp nhận nhập '1' hoặc '0'.\n";
                            }
                        }
                        
                        cout << "[Nhập liệu] Nhập Địa chỉ mới: ";
                        string newAddress;
                        getline(cin, newAddress);
                        if (!newAddress.empty()) updatedSV.DiaChi = newAddress;
                        
                        cout << "[Nhập liệu] Nhập Mã lớp học mới: ";
                        string newClass;
                        getline(cin, newClass);
                        if (!newClass.empty()) updatedSV.MaLop = newClass;
                        
                        // Đẩy dữ liệu cập nhật xuống SQL Server
                        if (db.updateSinhVien(updatedSV)) {
                            *it = updatedSV; // Cập nhật lại biến cục bộ trên RAM
                            cout << "[Thành công] Thông tin sinh viên " << idToUpdate << " đã được cập nhật đồng bộ CSDL!\n";
                        } else {
                            cout << "[Thất bại] Lỗi hệ thống. Không thể cập nhật dữ liệu. Hãy chắc chắn Mã lớp mới tồn tại.\n";
                        }
                    } else {
                        cout << "[Thất bại] Không tìm thấy sinh viên nào có mã số '" << idToUpdate << "' trên hệ thống RAM.\n";
                    }
                }
                break;

            case 4:
                // Xóa sinh viên dựa trên mã số sinh viên
                {
                    cout << "\n--- CHỨC NĂNG: XÓA SINH VIÊN KHỎI HỆ THỐNG ---\n";
                    cout << "[Nhập liệu] Nhập mã số sinh viên cần xóa (Ví dụ: SV001): ";
                    string idToDelete;
                    getline(cin, idToDelete);
                    
                    if (idToDelete.empty()) {
                        cout << "[Thông báo] Mã trống. Hủy thao tác xóa.\n";
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
                            cout << "[Thành công] Đã xóa hoàn toàn sinh viên có mã " << idToDelete << " khỏi hệ thống và bộ nhớ.\n";
                        } else {
                            cout << "[Thông báo] Đã xóa trong CSDL, dữ liệu bộ nhớ tạm thời không bị ảnh hưởng.\n";
                        }
                    } else {
                        cout << "[Thất bại] Không thể xóa. Không tìm thấy mã số '" << idToDelete << "' hoặc sinh viên này đang có dữ liệu điểm số ràng buộc khóa ngoại trong bảng Diem.\n";
                    }
                }
                break;

            case 0:
                db.disconnect();
                cout << "[Hệ thống] Đã ngắt kết nối an toàn với CSDL SQL Server.\n";
                cout << "Cảm ơn bạn đã sử dụng hệ thống quản lý sinh viên. Tạm biệt!\n";
                break;

            default:
                cout << "[Cảnh báo] Lựa chọn không nằm trong danh mục. Vui lòng chọn chính xác từ số 0 đến số 4!\n";
                break;
        }
    } while (choice != 0);

    return 0;
}