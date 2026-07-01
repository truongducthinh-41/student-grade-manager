#ifndef STUDENT_H
#define STUDENT_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Định nghĩa cấu trúc dữ liệu Student
struct Student {
    string id;         // Mã số sinh viên (MSSV)
    string name;       // Họ và tên
    double diemToan;   // Điểm môn Toán
    double diemLy;     // Điểm môn Lý
    double diemHoa;    // Điểm môn Hóa
    double diemTB;     // Điểm trung bình (GPA)
    string hocLuc;     // Xếp loại học lực (Giỏi, Khá, Trung bình, Yếu)
};

// Khai báo các hàm tiện ích hiển thị UTF-8 tiếng Việt
size_t utf8_length(const string& str);
string padRight(const string& str, size_t width);
string padLeft(const string& str, size_t width);

// Khai báo các chữ ký hàm quản lý sinh viên
void displayMenu();
void addStudent(vector<Student> &students);
void updateStudent(vector<Student> &students);
void deleteStudent(vector<Student> &students);
void searchStudent(const vector<Student> &students);
void printStudentsTable(const vector<Student> &students);
double calculateClassAverage(const vector<Student> &students);
void sortStudentsByGPA(vector<Student> &students);
void filterStudentsByClassification(const vector<Student> &students);
void exportTop5Students(const vector<Student> &students);
void displayClassificationStatistics(const vector<Student> &students);

// Đọc/Ghi dữ liệu từ file CSV và Xuất báo cáo
bool saveToCSV(const vector<Student> &students, const string &filename);
bool loadFromCSV(vector<Student> &students, const string &filename);
bool exportReport(const vector<Student> &students, const string &filename);

// Các hàm bổ trợ
string evaluateClassification(double gpa);
double inputValidGrade(const string &prompt);

#endif // STUDENT_H