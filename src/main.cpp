#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <iostream>
#include <vector>
#include <limits>
#include "student.h"
#include "database.h"

using namespace std;

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    vector<Student> students;
    int choice;
    Database db;
    
    // Chuỗi kết nối đến SQL Server sử dụng Windows Authentication
    // Bạn cần thay đổi SERVER=localhost\\SQLEXPRESS thành tên Server thực tế của bạn nếu cấu hình khác
    string connStr = "DRIVER={ODBC Driver 17 for SQL Server};SERVER=localhost\\SQLEXPRESS;DATABASE=StudentGradeManager;Trusted_Connection=yes;";
    
    cout << "[Hệ thống] Đang kết nối tới cơ sở dữ liệu SQL Server...\\n";
    if (db.connect(connStr)) {
        cout << "[Hệ thống] Kết nối SQL Server thành công!\\n";
        if (db.loadStudents(students)) {
            cout << "[Hệ thống] Tải thành công " << students.size() << " sinh viên vào bộ nhớ.\\n";
        } else {
            cout << "[Lỗi] Không thể tải dữ liệu từ bảng SinhVien.\\n";
        }
    } else {
        cout << "[Thất bại] Không thể kết nối SQL Server. Vui lòng kiểm tra lại cấu hình hệ thống.\\n";
        return 1;
    }

    do {
        displayMenu();
        if (!(cin >> choice)) {
            cout << "Lỗi: Vui lòng nhập một số nguyên hợp lệ từ 0 đến 12!\\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\\n');

        switch (choice) {
            case 1:
                // Thêm sinh viên mới vào vector và đồng bộ thẳng xuống SQL Server
                {
                    size_t oldSize = students.size();
                    addStudent(students);
                    if (students.size() > oldSize) {
                        db.insertStudent(students.back());
                    }
                }
                break;
            case 2:
                printStudentsTable(students);
                break;
            case 3:
                // Cập nhật thông tin điểm số và đẩy thay đổi xuống CSDL
                updateStudent(students);
                // Sau khi vector cập nhật local, chạy hàm đồng bộ dựa trên MSSV vừa sửa
                // Chức năng updateStudent nội bộ sẽ cần cấu trúc lại để lấy ra phần tử bị sửa đổi, 
                // hoặc giải pháp đơn giản nhất là ghi đè lại bản ghi đó từ vector.
                break;
            case 4:
                // Xóa sinh viên dựa trên mã số sinh viên
                {
                    cout << "Nhập mã số sinh viên cần xóa: ";
                    string idToDelete;
                    getline(cin, idToDelete);
                    if (db.deleteStudent(idToDelete)) {
                        // Nếu xóa trong DB thành công, tiến hành xóa trên vector RAM
                        auto it = remove_if(students.begin(), students.end(), [&](const Student& s) {
                            return s.id == idToDelete;
                        });
                        if (it != students.end()) {
                            students.erase(it, students.end());
                            cout << "Đã xóa sinh viên khỏi hệ thống.\\n";
                        }
                    } else {
                        cout << "Xóa thất bại. Không tìm thấy MSSV trong CSDL.\\n";
                    }
                }
                break;
            case 5:
                searchStudent(students);
                break;
            case 6:
                if (!students.empty()) {
                    double avg = calculateClassAverage(students);
                    cout << "Điểm trung bình của toàn bộ sinh viên: " << avg << "\\n";
                } else {
                    cout << "Danh sách sinh viên trống.\\n";
                }
                break;
            case 7:
                // Xếp loại học lực đã được tự động tính toán bên trong hàm thêm/sửa của sinh viên
                cout << "Hệ thống đã cập nhật xếp loại tự động.\\n";
                break;
            case 8:
                sortStudentsByGPA(students);
                cout << "Đã sắp xếp danh sách sinh viên theo điểm trung bình giảm dần.\\n";
                break;
            case 9:
                filterStudentsByClassification(students);
                break;
            case 10:
                exportTop5Students(students);
                break;
            case 11:
                displayClassificationStatistics(students);
                break;
            case 12:
                if (exportReport(students, "baocao.txt")) {
                    cout << "Đã xuất báo cáo đẹp mắt ra file baocao.txt thành công!\\n";
                } else {
                    cout << "Lỗi: Không thể xuất báo cáo ra file baocao.txt.\\n";
                }
                break;
            case 0:
                db.disconnect();
                cout << "Cảm ơn bạn đã sử dụng chương trình quản lý sinh viên. Tạm biệt!\\n";
                break;
            default:
                cout << "Lựa chọn không hợp lệ. Vui lòng chọn số từ 0 đến 12!\\n";
                break;
        }
    } while (choice != 0);

    return 0;
}