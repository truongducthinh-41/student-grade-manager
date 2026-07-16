#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include <vector>

struct LopHoc {
    std::string MaLop;
    std::string TenLop;
    std::string Khoa;
};

struct MonHoc {
    std::string MaMH;
    std::string TenMH;
    int SoTinChi;
};

struct SinhVien {
    std::string MaSV;
    std::string HoTen;
    std::string NgaySinh;
    bool GioiTinh;
    std::string DiaChi;
    std::string MaLop;

    double diemToan = 0.0;
    double diemLy = 0.0;
    double diemHoa = 0.0;
    double diemTB = 0.0;
    std::string hocLuc = "Yếu";
};

// Các hàm xử lý giao diện/logic (không thay đổi gốc CSDL)
std::string evaluateClassification(double gpa);
void printStudentsTable(const std::vector<SinhVien> &students);
void sortStudentsByGPA(std::vector<SinhVien> &students);
void filterStudentsByClassification(const std::vector<SinhVien> &students);
void searchStudent(const std::vector<SinhVien> &students, const std::string& query);
void exportTop5Students(const std::vector<SinhVien> &students);
void displayStats(const std::vector<SinhVien> &students);
bool saveToCSV(const std::vector<SinhVien> &students, const std::string &filename);
bool exportReport(const std::vector<SinhVien> &students, const std::string &filename);

#endif