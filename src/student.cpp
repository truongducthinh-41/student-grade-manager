#include "student.h"
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <ctime>

// Hàm đếm số lượng ký tự UTF-8 (code points) thực tế thay vì đếm số bytes
size_t utf8_length(const string& str) {
    size_t len = 0;
    for (char c : str) {
        if ((c & 0xC0) != 0x80) {
            len++;
        }
    }
    return len;
}

// Căn lề phải với độ rộng chỉ định dựa trên UTF-8 length
string padRight(const string& str, size_t width) {
    size_t len = utf8_length(str);
    if (len >= width) return str;
    return str + string(width - len, ' ');
}

// Căn lề trái với độ rộng chỉ định dựa trên UTF-8 length
string padLeft(const string& str, size_t width) {
    size_t len = utf8_length(str);
    if (len >= width) return str;
    return string(width - len, ' ') + str;
}

// Chuyển chuỗi sang chữ thường (đơn giản hóa cho ASCII và xử lý đ/Đ cơ bản)
string toLowerCase(const string& str) {
    string res = str;
    for (size_t i = 0; i < res.length(); ++i) {
        // Xử lý 'Đ' (\xC4\x90) sang 'đ' (\xC4\x91) trong UTF-8
        if ((unsigned char)res[i] == 0xC4 && i + 1 < res.length() && (unsigned char)res[i+1] == 0x90) {
            res[i+1] = 0x91;
            i++;
        } else {
            res[i] = (char)tolower((unsigned char)res[i]);
        }
    }
    return res;
}

// Lấy ngày giờ hiện tại định dạng YYYY-MM-DD HH:MM:SS
string getCurrentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    #ifdef _WIN32
        localtime_s(&tstruct, &now);
    #else
        tstruct = *localtime(&now);
    #endif
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);
    return string(buf);
}

// Quy tắc xếp loại học lực
string evaluateClassification(double gpa) {
    if (gpa >= 8.0) return "Giỏi";
    if (gpa >= 6.5) return "Khá";
    if (gpa >= 5.0) return "Trung bình";
    return "Yếu";
}

// Nhập điểm hợp lệ từ 0 đến 10
double inputValidGrade(const string &prompt) {
    double val;
    while (true) {
        cout << prompt;
        if (cin >> val) {
            if (val >= 0.0 && val <= 10.0) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return val;
            } else {
                cout << "Lỗi: Điểm phải nằm trong khoảng từ 0.0 đến 10.0. Vui lòng nhập lại!\n";
            }
        } else {
            cout << "Lỗi: Nhập sai định dạng số. Vui lòng nhập lại!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Menu điều khiển bằng tiếng Việt
void displayMenu() {
    cout << "\n=============================================\n";
    cout << "        HỆ THỐNG QUẢN LÝ SINH VIÊN           \n";
    cout << "=============================================\n";
    cout << " 1. Thêm sinh viên mới\n";
    cout << " 2. Hiển thị danh sách sinh viên\n";
    cout << " 3. Cập nhật điểm sinh viên (theo MSSV)\n";
    cout << " 4. Xóa sinh viên (theo MSSV)\n";
    cout << " 5. Tìm kiếm sinh viên (theo MSSV hoặc Tên)\n";
    cout << " 6. Sắp xếp sinh viên theo Điểm trung bình\n";
    cout << " 7. Lọc sinh viên theo Học lực\n";
    cout << " 8. Hiển thị Top 5 sinh viên điểm cao nhất\n";
    cout << " 9. Thống kê số lượng học lực\n";
    cout << "10. Lưu danh sách vào file CSV (danhsach.csv)\n";
    cout << "11. Đọc danh sách từ file CSV (danhsach.csv)\n";
    cout << "12. Xuất báo cáo đẹp mắt (baocao.txt)\n";
    cout << " 0. Thoát chương trình\n";
    cout << "=============================================\n";
    cout << "Lựa chọn của bạn (0-12): ";
}

// Thêm sinh viên mới
void addStudent(vector<Student> &students) {
    cout << "\n--- THÊM SINH VIÊN MỚI ---\n";
    string id;
    while (true) {
        cout << "Nhập MSSV (hoặc nhấn Enter để hủy): ";
        getline(cin, id);
        if (id.empty()) {
            cout << "Đã hủy thao tác thêm sinh viên.\n";
            return;
        }
        
        // Kiểm tra xem MSSV đã tồn tại hay chưa
        bool duplicate = false;
        for (const auto& s : students) {
            if (s.id == id) {
                duplicate = true;
                break;
            }
        }
        if (duplicate) {
            cout << "Lỗi: MSSV '" << id << "' đã tồn tại trong hệ thống. Vui lòng nhập mã khác!\n";
        } else {
            break;
        }
    }
    
    string name;
    while (true) {
        cout << "Nhập Họ và tên: ";
        getline(cin, name);
        if (!name.empty()) break;
        cout << "Lỗi: Họ và tên không được để trống!\n";
    }
    
    double diemToan = inputValidGrade("Nhập điểm Toán (0-10): ");
    double diemLy = inputValidGrade("Nhập điểm Lý (0-10): ");
    double diemHoa = inputValidGrade("Nhập điểm Hóa (0-10): ");
    
    double diemTB = (diemToan + diemLy + diemHoa) / 3.0;
    string hocLuc = evaluateClassification(diemTB);
    
    students.push_back({id, name, diemToan, diemLy, diemHoa, diemTB, hocLuc});
    cout << "Thêm sinh viên thành công!\n";
}

// Cập nhật điểm sinh viên
void updateStudent(vector<Student> &students) {
    cout << "\n--- CẬP NHẬT ĐIỂM SINH VIÊN ---\n";
    if (students.empty()) {
        cout << "Danh sách sinh viên trống.\n";
        return;
    }
    
    cout << "Nhập MSSV cần cập nhật: ";
    string id;
    getline(cin, id);
    
    int index = -1;
    for (size_t i = 0; i < students.size(); ++i) {
        if (students[i].id == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        cout << "Không tìm thấy sinh viên có MSSV: " << id << endl;
        return;
    }
    
    cout << "Thông tin hiện tại của sinh viên:\n";
    cout << " - Họ và tên: " << students[index].name << endl;
    cout << " - Điểm Toán: " << students[index].diemToan << endl;
    cout << " - Điểm Lý: " << students[index].diemLy << endl;
    cout << " - Điểm Hóa: " << students[index].diemHoa << endl;
    cout << " - Điểm TB: " << fixed << setprecision(2) << students[index].diemTB << " (" << students[index].hocLuc << ")\n";
    
    cout << "\nNhập điểm mới:\n";
    students[index].diemToan = inputValidGrade("Nhập điểm Toán mới: ");
    students[index].diemLy = inputValidGrade("Nhập điểm Lý mới: ");
    students[index].diemHoa = inputValidGrade("Nhập điểm Hóa mới: ");
    
    students[index].diemTB = (students[index].diemToan + students[index].diemLy + students[index].diemHoa) / 3.0;
    students[index].hocLuc = evaluateClassification(students[index].diemTB);
    
    cout << "Cập nhật thông tin sinh viên thành công!\n";
}

// Xóa sinh viên theo mã số
void deleteStudent(vector<Student> &students) {
    cout << "\n--- XÓA SINH VIÊN ---\n";
    if (students.empty()) {
        cout << "Danh sách sinh viên trống.\n";
        return;
    }
    
    cout << "Nhập MSSV cần xóa: ";
    string id;
    getline(cin, id);
    
    int index = -1;
    for (size_t i = 0; i < students.size(); ++i) {
        if (students[i].id == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1) {
        cout << "Không tìm thấy sinh viên có MSSV: " << id << endl;
        return;
    }
    
    cout << "Bạn có chắc chắn muốn xóa sinh viên sau đây? (Y/N)\n";
    cout << " - MSSV: " << students[index].id << endl;
    cout << " - Họ và tên: " << students[index].name << endl;
    cout << "Lựa chọn của bạn: ";
    string confirm;
    getline(cin, confirm);
    if (confirm == "Y" || confirm == "y" || confirm == "Có" || confirm == "có" || confirm == "co" || confirm == "CO" || confirm == "C" || confirm == "c") {
        students.erase(students.begin() + index);
        cout << "Đã xóa sinh viên thành công!\n";
    } else {
        cout << "Hủy thao tác xóa.\n";
    }
}

// Tìm kiếm sinh viên theo mã hoặc tên
void searchStudent(const vector<Student> &students) {
    cout << "\n--- TÌM KIẾM SINH VIÊN ---\n";
    if (students.empty()) {
        cout << "Danh sách sinh viên trống.\n";
        return;
    }
    
    cout << "Nhập MSSV hoặc Tên sinh viên cần tìm: ";
    string query;
    getline(cin, query);
    
    vector<Student> results;
    for (const auto& s : students) {
        if (toLowerCase(s.id).find(toLowerCase(query)) != string::npos ||
            toLowerCase(s.name).find(toLowerCase(query)) != string::npos) {
            results.push_back(s);
        }
    }
    
    if (results.empty()) {
        cout << "Không tìm thấy sinh viên nào khớp với từ khóa: \"" << query << "\"\n";
    } else {
        cout << "Tìm thấy " << results.size() << " kết quả phù hợp:\n";
        printStudentsTable(results);
    }
}

// Hiển thị danh sách sinh viên dưới dạng bảng ASCII
void printStudentsTable(const vector<Student> &students) {
    if (students.empty()) {
        cout << "Danh sách sinh viên trống.\n";
        return;
    }
    
    // Khởi tạo separator tương thích với độ rộng cột
    string sep = "+--------------+------------------------------+--------+--------+--------+----------+--------------+";
    cout << sep << endl;
    cout << "| " << padRight("MSSV", 12) 
         << " | " << padRight("Họ và Tên", 28) 
         << " | " << padRight("Toán", 6) 
         << " | " << padRight("Lý", 6) 
         << " | " << padRight("Hóa", 6) 
         << " | " << padRight("Điểm TB", 8) 
         << " | " << padRight("Học lực", 12) << " |" << endl;
    cout << sep << endl;
    
    for (const auto& s : students) {
        stringstream ssToan, ssLy, ssHoa, ssTB;
        ssToan << fixed << setprecision(1) << s.diemToan;
        ssLy << fixed << setprecision(1) << s.diemLy;
        ssHoa << fixed << setprecision(1) << s.diemHoa;
        ssTB << fixed << setprecision(2) << s.diemTB;
        
        cout << "| " << padRight(s.id, 12)
             << " | " << padRight(s.name, 28)
             << " | " << padLeft(ssToan.str(), 6)
             << " | " << padLeft(ssLy.str(), 6)
             << " | " << padLeft(ssHoa.str(), 6)
             << " | " << padLeft(ssTB.str(), 8)
             << " | " << padRight(s.hocLuc, 12) << " |" << endl;
    }
    cout << sep << endl;
}

// Tính điểm trung bình của toàn lớp
double calculateClassAverage(const vector<Student> &students) {
    if (students.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& s : students) {
        sum += s.diemTB;
    }
    return sum / students.size();
}

// Sắp xếp sinh viên theo điểm trung bình giảm dần
void sortStudentsByGPA(vector<Student> &students) {
    if (students.empty()) {
        cout << "Danh sách sinh viên trống.\n";
        return;
    }
    
    sort(students.begin(), students.end(), [](const Student& a, const Student& b) {
        return a.diemTB > b.diemTB;
    });
    
    cout << "Đã sắp xếp danh sách sinh viên theo Điểm trung bình giảm dần!\n";
    printStudentsTable(students);
}

// Lọc sinh viên theo học lực
void filterStudentsByClassification(const vector<Student> &students) {
    cout << "\n--- LỌC SINH VIÊN THEO HỌC LỰC ---\n";
    if (students.empty()) {
        cout << "Danh sách sinh viên trống.\n";
        return;
    }
    
    cout << "Chọn học lực muốn lọc:\n";
    cout << "1. Giỏi\n";
    cout << "2. Khá\n";
    cout << "3. Trung bình\n";
    cout << "4. Yếu\n";
    cout << "Lựa chọn của bạn: ";
    int choice;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Lựa chọn không hợp lệ!\n";
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    string targetClass = "";
    if (choice == 1) targetClass = "Giỏi";
    else if (choice == 2) targetClass = "Khá";
    else if (choice == 3) targetClass = "Trung bình";
    else if (choice == 4) targetClass = "Yếu";
    else {
        cout << "Lựa chọn không hợp lệ!\n";
        return;
    }
    
    vector<Student> filtered;
    for (const auto& s : students) {
        if (s.hocLuc == targetClass) {
            filtered.push_back(s);
        }
    }
    
    if (filtered.empty()) {
        cout << "Không có sinh viên nào xếp loại: " << targetClass << endl;
    } else {
        cout << "Danh sách sinh viên xếp loại " << targetClass << " (" << filtered.size() << " sinh viên):\n";
        printStudentsTable(filtered);
    }
}

// Hiển thị Top 5 sinh viên có điểm trung bình cao nhất
void exportTop5Students(const vector<Student> &students) {
    cout << "\n--- TOP 5 SINH VIÊN CÓ ĐIỂM TRUNG BÌNH CAO NHẤT ---\n";
    if (students.empty()) {
        cout << "Danh sách sinh viên trống.\n";
        return;
    }
    
    vector<Student> temp = students;
    sort(temp.begin(), temp.end(), [](const Student& a, const Student& b) {
        return a.diemTB > b.diemTB;
    });
    
    size_t limit = min((size_t)5, temp.size());
    vector<Student> top5(temp.begin(), temp.begin() + limit);
    
    cout << "Top " << limit << " sinh viên cao nhất:\n";
    printStudentsTable(top5);
}

// Thống kê số lượng sinh viên theo từng loại học lực
void displayClassificationStatistics(const vector<Student> &students) {
    cout << "\n--- THỐNG KÊ HỌC LỰC SINH VIÊN ---\n";
    if (students.empty()) {
        cout << "Danh sách sinh viên trống.\n";
        return;
    }
    
    int countGioi = 0, countKha = 0, countTB = 0, countYeu = 0;
    for (const auto& s : students) {
        if (s.hocLuc == "Giỏi") countGioi++;
        else if (s.hocLuc == "Khá") countKha++;
        else if (s.hocLuc == "Trung bình") countTB++;
        else if (s.hocLuc == "Yếu") countYeu++;
    }
    
    double total = (double)students.size();
    
    string sep = "+--------------+------------+------------+";
    cout << sep << endl;
    cout << "| " << padRight("Học lực", 12)
         << " | " << padRight("Số lượng", 10)
         << " | " << padRight("Tỷ lệ", 10) << " |" << endl;
    cout << sep << endl;
    
    auto printRow = [&](const string& hl, int count) {
        stringstream ssCount, ssPct;
        ssCount << count;
        ssPct << fixed << setprecision(2) << (count / total * 100.0) << "%";
        cout << "| " << padRight(hl, 12)
             << " | " << padLeft(ssCount.str(), 10)
             << " | " << padLeft(ssPct.str(), 10) << " |" << endl;
    };
    
    printRow("Giỏi", countGioi);
    printRow("Khá", countKha);
    printRow("Trung bình", countTB);
    printRow("Yếu", countYeu);
    cout << sep << endl;
    cout << "Tổng số sinh viên: " << students.size() << endl;
}

// Chuyển chuỗi thành định dạng an toàn trong CSV (bọc dấu nháy kép nếu chứa dấu phẩy hoặc nháy kép)
string escapeCSVField(const string& str) {
    if (str.find(',') != string::npos || str.find('"') != string::npos) {
        string res = "\"";
        for (char c : str) {
            if (c == '"') {
                res += "\"\""; // Nhân đôi dấu nháy kép
            } else {
                res += c;
            }
        }
        res += "\"";
        return res;
    }
    return str;
}

// Hàm loại bỏ khoảng trắng ở đầu và cuối chuỗi
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Tách các trường từ một dòng CSV, hỗ trợ dấu nháy kép
vector<string> parseCSVLine(const string& line) {
    vector<string> result;
    string token = "";
    bool inQuotes = false;
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (inQuotes && i + 1 < line.length() && line[i+1] == '"') {
                token += '"';
                i++;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            result.push_back(token);
            token = "";
        } else {
            token += c;
        }
    }
    result.push_back(token);
    return result;
}

// Lưu danh sách sinh viên vào file CSV
bool saveToCSV(const vector<Student> &students, const string &filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    // Ghi tiêu đề CSV
    file << "MSSV,HoTen,DiemToan,DiemLy,DiemHoa,DiemTB,HocLuc\n";
    for (const auto& s : students) {
        file << escapeCSVField(s.id) << ","
             << escapeCSVField(s.name) << ","
             << s.diemToan << ","
             << s.diemLy << ","
             << s.diemHoa << ","
             << s.diemTB << ","
             << escapeCSVField(s.hocLuc) << "\n";
    }
    file.close();
    return true;
}

// Đọc danh sách sinh viên từ file CSV
bool loadFromCSV(vector<Student> &students, const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    if (!getline(file, line)) {
        file.close();
        return false;
    }
    
    vector<Student> loadedStudents;
    int lineNum = 1;
    bool hasError = false;
    
    while (getline(file, line)) {
        lineNum++;
        if (trim(line).empty()) continue;
        
        vector<string> fields = parseCSVLine(line);
        if (fields.size() < 7) {
            cout << "Cảnh báo dòng " << lineNum << ": Định dạng không hợp lệ (thiếu cột). Dòng bị bỏ qua.\n";
            hasError = true;
            continue;
        }
        
        string id = trim(fields[0]);
        string name = trim(fields[1]);
        
        double diemToan = 0, diemLy = 0, diemHoa = 0;
        try {
            diemToan = stod(fields[2]);
            diemLy = stod(fields[3]);
            diemHoa = stod(fields[4]);
        } catch (...) {
            cout << "Cảnh báo dòng " << lineNum << ": Lỗi định dạng điểm số. Dòng bị bỏ qua.\n";
            hasError = true;
            continue;
        }
        
        if (diemToan < 0.0 || diemToan > 10.0 || diemLy < 0.0 || diemLy > 10.0 || diemHoa < 0.0 || diemHoa > 10.0) {
            cout << "Cảnh báo dòng " << lineNum << ": Điểm ngoài khoảng 0-10. Dòng bị bỏ qua.\n";
            hasError = true;
            continue;
        }
        
        // Kiểm tra xem MSSV đã có trong danh sách vừa đọc chưa để tránh trùng lặp mã
        bool duplicate = false;
        for (const auto& s : loadedStudents) {
            if (s.id == id) {
                duplicate = true;
                break;
            }
        }
        if (duplicate) {
            cout << "Cảnh báo dòng " << lineNum << ": Trùng mã MSSV '" << id << "' trong file CSV. Dòng bị bỏ qua.\n";
            hasError = true;
            continue;
        }
        
        double diemTB = (diemToan + diemLy + diemHoa) / 3.0;
        string hocLuc = evaluateClassification(diemTB);
        
        loadedStudents.push_back({id, name, diemToan, diemLy, diemHoa, diemTB, hocLuc});
    }
    
    file.close();
    
    if (loadedStudents.empty() && hasError) {
        return false; // Trả về false nếu không đọc được dòng hợp lệ nào mà có lỗi
    }
    
    students = loadedStudents;
    return true;
}

// Xuất báo cáo đẹp mắt dưới dạng tệp văn bản (.txt)
bool exportReport(const vector<Student> &students, const string &filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "========================================================================================\n";
    file << "                       BÁO CÁO DANH SÁCH SINH VIÊN VÀ ĐIỂM SỐ                           \n";
    file << "========================================================================================\n";
    file << "Thời gian xuất báo cáo: " << getCurrentDateTime() << "\n";
    file << "Tổng số sinh viên:      " << students.size() << "\n";
    
    double avgClass = calculateClassAverage(students);
    file << "Điểm trung bình lớp:    " << fixed << setprecision(2) << avgClass << "\n\n";
    
    file << "I. DANH SÁCH CHI TIẾT SINH VIÊN\n";
    string sep = "+--------------+------------------------------+--------+--------+--------+----------+--------------+";
    file << sep << "\n";
    file << "| " << padRight("MSSV", 12) 
         << " | " << padRight("Họ và Tên", 28) 
         << " | " << padRight("Toán", 6) 
         << " | " << padRight("Lý", 6) 
         << " | " << padRight("Hóa", 6) 
         << " | " << padRight("Điểm TB", 8) 
         << " | " << padRight("Học lực", 12) << " |\n";
    file << sep << "\n";
    
    int countGioi = 0, countKha = 0, countTB = 0, countYeu = 0;
    for (const auto& s : students) {
        if (s.hocLuc == "Giỏi") countGioi++;
        else if (s.hocLuc == "Khá") countKha++;
        else if (s.hocLuc == "Trung bình") countTB++;
        else if (s.hocLuc == "Yếu") countYeu++;
        
        stringstream ssToan, ssLy, ssHoa, ssTB;
        ssToan << fixed << setprecision(1) << s.diemToan;
        ssLy << fixed << setprecision(1) << s.diemLy;
        ssHoa << fixed << setprecision(1) << s.diemHoa;
        ssTB << fixed << setprecision(2) << s.diemTB;
        
        file << "| " << padRight(s.id, 12)
             << " | " << padRight(s.name, 28)
             << " | " << padLeft(ssToan.str(), 6)
             << " | " << padLeft(ssLy.str(), 6)
             << " | " << padLeft(ssHoa.str(), 6)
             << " | " << padLeft(ssTB.str(), 8)
             << " | " << padRight(s.hocLuc, 12) << " |\n";
    }
    file << sep << "\n\n";
    
    file << "II. THỐNG KÊ XẾP LOẠI HỌC LỰC\n";
    string sepStat = "+--------------+------------+------------+";
    file << sepStat << "\n";
    file << "| " << padRight("Học lực", 12)
         << " | " << padRight("Số lượng", 10)
         << " | " << padRight("Tỷ lệ", 10) << " |\n";
    file << sepStat << "\n";
    
    double total = (double)students.size();
    auto printRowFile = [&](const string& hl, int count) {
        stringstream ssCount, ssPct;
        ssCount << count;
        if (total > 0) {
            ssPct << fixed << setprecision(2) << (count / total * 100.0) << "%";
        } else {
            ssPct << "0.00%";
        }
        file << "| " << padRight(hl, 12)
             << " | " << padLeft(ssCount.str(), 10)
             << " | " << padLeft(ssPct.str(), 10) << " |\n";
    };
    
    printRowFile("Giỏi", countGioi);
    printRowFile("Khá", countKha);
    printRowFile("Trung bình", countTB);
    printRowFile("Yếu", countYeu);
    file << sepStat << "\n";
    
    file.close();
    return true;
}