# HỆ THỐNG QUẢN LÝ SINH VIÊN (C++ & SQL SERVER)

Đây là dự án ứng dụng Console C++ kết nối trực tiếp với cơ sở dữ liệu SQL Server. Dự án đã hoàn thiện 100% các chức năng quản lý từ cơ bản đến nâng cao (12 chức năng chuyên sâu), hỗ trợ hiển thị Tiếng Việt UTF-8 chuẩn xác, và đồng bộ dữ liệu thời gian thực thông qua ODBC Driver.

---

## PHẦN 1: TÍNH NĂNG NỔI BẬT (FEATURES)

Dự án sở hữu một Menu Console chuyên nghiệp với đầy đủ 12 chức năng sau:

1. **Thêm sinh viên mới:** Thêm nhanh sinh viên với các trường bắt buộc. Tự động xử lý Ràng buộc Khóa ngoại (Foreign Key) nếu để trống lớp học. Tự động mã hóa Tiếng Việt (UTF-16) để lưu vào SQL Server mà không bị lỗi phông.
2. **Cập nhật thông tin & điểm sinh viên:** Sử dụng câu lệnh `MERGE` mạnh mẽ trong SQL Server để tối ưu hóa việc nhập điểm các môn học (MH01, MH02...).
3. **Xóa sinh viên (theo MSSV):** Tự động xử lý an toàn (xóa điểm tham chiếu trước, xóa thông tin cá nhân sau) thông qua Stored Procedure.
4. **Tìm kiếm sinh viên:** Chấp nhận tìm kiếm mờ theo cả Mã sinh viên và Họ tên.
5. **Hiển thị danh sách sinh viên:** Bảng dữ liệu chuẩn xác, lấy trực tiếp Điểm Trung Bình (GPA) bằng lệnh `AVG()` từ bảng Điểm dưới SQL Server.
6. **Sắp xếp sinh viên theo Điểm trung bình:** Sắp xếp theo chiều giảm dần để dễ dàng theo dõi.
7. **Lọc sinh viên theo Học lực:** Lọc nhanh các hạng Giỏi, Khá, Trung bình, Yếu.
8. **Hiển thị Top 5 sinh viên điểm cao nhất:** Ứng dụng giải thuật trích xuất nhanh Top 5.
9. **Thống kê số lượng học lực:** Phân tích dữ liệu để đếm tổng số lượng sinh viên đạt từng hạng học lực.
10. **Lưu danh sách vào file CSV:** Xuất toàn bộ dữ liệu ra định dạng `.csv` phổ biến.
11. **Đọc danh sách từ file CSV:** Hỗ trợ nhập (import) hàng loạt dữ liệu. Có chế độ "Silent Mode" tự động bỏ qua các bản ghi bị trùng lặp khóa chính (Primary Key) mà không làm rác màn hình Console.
12. **Xuất báo cáo (TXT):** Tạo báo cáo tổng quan số lượng và chi tiết sinh viên ra file `.txt`.

---

## PHẦN 2: HƯỚNG DẪN CÀI ĐẶT (SETUP)

### 1. Chuẩn bị Cơ sở dữ liệu (SQL Server)
Bạn cần mở SQL Server Management Studio (SSMS) và chạy đoạn script tạo CSDL (`QuanLyDiemSinhVien`). Đảm bảo hệ thống của bạn đã có đủ:
- Bảng `LopHoc`
- Bảng `SinhVien`
- Bảng `MonHoc`
- Bảng `Diem`
- Các Stored Procedure như `sp_ThemSinhVien`, `sp_XoaSinhVien`...

### 2. Trình biên dịch C++ (MinGW/GCC)
Mở Terminal tại thư mục gốc của dự án và chạy câu lệnh biên dịch sau (bắt buộc phải có cờ `-lodbc32` để nạp thư viện kết nối SQL):
```bash
g++ src/main.cpp DB/database.cpp -I src -I DB -o main.exe -lodbc32
```

Sau khi biên dịch thành công, chạy chương trình:
```bash
./main
```

---

## PHẦN 3: CẨM NÀNG GỠ LỖI (DEBUG & TROUBLESHOOTING)

Trong quá trình sử dụng, hệ thống được thiết kế để tự động in ra các thông báo **[Chi tiết lỗi SQL]** bằng tiếng Anh nếu phát sinh vấn đề vật lý. Dưới đây là các lỗi thường gặp:

### 1. Lỗi: Violation of PRIMARY KEY constraint
- **Hiện tượng:** Khi sử dụng chức năng 1 hoặc 11.
- **Cách giải quyết:** Lỗi này có nghĩa là bạn đang cố gắng thêm một sinh viên có MSSV đã tồn tại trong hệ thống. Hãy kiểm tra lại và nhập MSSV khác.

### 2. Lỗi: The MERGE statement conflicted with the FOREIGN KEY constraint
- **Hiện tượng:** Xảy ra ở chức năng 2 (Cập nhật điểm).
- **Cách giải quyết:** Bạn đang nhập Mã Môn Học không tồn tại trong bảng `MonHoc`. Hãy đảm bảo chỉ nhập các mã hợp lệ (VD: `MH01`, `MH02`...).

### 3. Lỗi font chữ tiếng Việt
- **Khắc phục:** Hệ thống C++ đã sử dụng `SetConsoleOutputCP(CP_UTF8)` và hàm chuyển đổi UTF-16 qua API Windows. Hãy đảm bảo Terminal/Console của bạn sử dụng Font chữ hỗ trợ Unicode (Ví dụ: Cascadia Code hoặc Consolas).
