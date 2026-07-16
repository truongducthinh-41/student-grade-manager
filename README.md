# HỆ THỐNG QUẢN LÝ SINH VIÊN (C++ & SQL SERVER)

Đây là dự án ứng dụng Console C++ kết nối trực tiếp với cơ sở dữ liệu SQL Server. Tài liệu này sẽ hướng dẫn bạn cách thiết lập môi trường, cách sử dụng từng tính năng trong hệ thống và cách tự gỡ lỗi (debug) nếu gặp sự cố.

---

## PHẦN 1: HƯỚNG DẪN CÀI ĐẶT (SETUP)

### 1. Chuẩn bị Cơ sở dữ liệu (SQL Server)
Bạn cần mở SQL Server Management Studio (SSMS) và chạy đoạn script sau để khởi tạo bảng và dữ liệu mẫu. **Lưu ý:** Bắt buộc phải chạy script này để tránh lỗi Ràng buộc khóa ngoại (Foreign Key) khi thêm sinh viên.

```sql
CREATE DATABASE StudentGradeManager;
GO
USE StudentGradeManager;
GO

-- 1. Tạo bảng danh mục Lớp Học
CREATE TABLE LopHoc (
    MaLop VARCHAR(20) PRIMARY KEY,
    TenLop NVARCHAR(100)
);

-- 2. Tạo bảng Sinh Viên
CREATE TABLE SinhVien (
    MaSV VARCHAR(20) PRIMARY KEY,
    HoTen NVARCHAR(100) NOT NULL,
    NgaySinh VARCHAR(15),
    GioiTinh BIT,
    DiaChi NVARCHAR(255),
    MaLop VARCHAR(20) FOREIGN KEY REFERENCES LopHoc(MaLop)
);

-- 3. Tạo bảng Điểm
CREATE TABLE Diem (
    MaSV VARCHAR(20) FOREIGN KEY REFERENCES SinhVien(MaSV),
    MaMH VARCHAR(20),
    LanThi INT,
    DiemSo FLOAT
);

-- 4. Thêm dữ liệu Lớp học mẫu (Bắt buộc để thêm được Sinh viên)
INSERT INTO LopHoc (MaLop, TenLop) VALUES ('K31PM1', N'Công nghệ phần mềm 1');
INSERT INTO LopHoc (MaLop, TenLop) VALUES ('K31PM2', N'Công nghệ phần mềm 2');

## PHẦN 2: HƯỚNG DẪN SỬ DỤNG CHI TIẾT
Khi chạy chương trình, hệ thống sẽ hiển thị Menu chính từ 0 đến 4. Vui lòng chỉ nhập số nguyên. Nếu nhập chữ cái, hệ thống sẽ báo lỗi và yêu cầu nhập lại.

### Tính năng 1: Thêm sinh viên mới
Chức năng này lưu dữ liệu thẳng xuống SQL Server và đồng bộ lên RAM. Các bước nhập liệu cần tuân thủ quy tắc sau:

Mã SV: Nhập không quá 20 ký tự (Ví dụ: SV001). Hệ thống sẽ tự động kiểm tra, nếu mã đã tồn tại trên RAM, bạn sẽ phải nhập mã khác.

Họ Tên: Nhập tiếng Việt có dấu bình thường. Không được để trống.

Ngày Sinh: Bắt buộc nhập đúng chuẩn 10 ký tự YYYY-MM-DD (Ví dụ: 2005-01-15). Nếu sai dấu - hoặc sai độ dài, hệ thống sẽ bắt nhập lại.

Giới Tính: Chỉ được nhập số 1 (Nam) hoặc 0 (Nữ).

Địa Chỉ: Nhập tự do, hỗ trợ tiếng Việt có dấu.

Mã Lớp: Phải nhập chính xác mã đã có trong CSDL (Ví dụ: K31PM1). Nếu nhập sai, SQL Server sẽ từ chối lưu dữ liệu.

### Tính năng 2: Hiển thị danh sách
Hệ thống sẽ in ra bảng danh sách sinh viên hiện có trên bộ nhớ RAM. Bảng được căn lề hoàn hảo và hỗ trợ hiển thị 100% tiếng Việt có dấu nhờ bộ mã hóa UTF-8.

### Tính năng 3: Cập nhật thông tin
Nhập Mã SV cần sửa.

Hệ thống in ra thông tin cũ. Tại mỗi mục cần sửa, hãy nhập dữ liệu mới.

Mẹo: Nếu mục nào bạn không muốn thay đổi, chỉ cần nhấn phím Enter (để trống), hệ thống sẽ tự động giữ nguyên dữ liệu cũ của mục đó.

### Tính năng 4: Xóa sinh viên
Nhập Mã SV cần xóa.

Hệ thống sẽ tiến hành xóa dưới CSDL trước. Nếu thành công, dữ liệu trên RAM cũng sẽ được dọn dẹp để đồng bộ.

## PHẦN 3: CẨM NÀNG GỠ LỖI (DEBUG & TROUBLESHOOTING)

Nếu chương trình hoạt động không như ý muốn, hãy tra cứu bảng lỗi dưới đây để tự xử lý:

### 1. Lỗi hệ thống: Không thể kết nối SQL Server
Hiện tượng: Vừa mở app lên đã báo thất bại và thoát chương trình.

Cách Debug: 1. Kiểm tra lại chuỗi connStr trong main.cpp xem tên SERVER đã đúng với máy bạn chưa.
2. Bật Run (Win + R), gõ services.msc, tìm dịch vụ có tên SQL Server (...) và đảm bảo nó đang ở trạng thái Running.
3. Kiểm tra xem máy đã cài đặt ODBC Driver 17 for SQL Server chưa.

### 2. Lỗi font chữ: Chữ tiếng Việt bị biến dạng (ô vuông, dấu hỏi)
Hiện tượng: Chữ in ra Console bị lỗi font, viền bảng bị lệch.

Cách Debug: Code đã tự động gọi SetConsoleOutputCP(CP_UTF8). Nếu vẫn lỗi, nguyên nhân do cửa sổ Console của bạn đang dùng font chữ không hỗ trợ Unicode. Hãy Click chuột phải vào thanh tiêu đề Console > Chọn Properties > Tab Font > Đổi sang font Consolas hoặc Cascadia Code.

### 3. Không thể lưu sinh viên mới vào SQL Server
Hiện tượng: Đã nhập xong mọi thông tin nhưng hệ thống báo: [Thất bại] Không thể lưu vào SQL Server. Vui lòng kiểm tra lại tính chính xác của Mã lớp...

Cách Debug: Nguyên nhân 99% là do bạn nhập một Mã Lớp chưa hề tồn tại trong bảng LopHoc của CSDL. Vì có ràng buộc Khóa ngoại (Foreign Key), SQL Server đã từ chối. Hãy mở SSMS lên và thêm mã lớp đó vào bảng LopHoc trước.

### 4. Thêm sinh viên thành công, nhưng xem trong DB thấy Họ tên bị lỗi dấu ???
Hiện tượng: Trên Console C++ hiển thị tiếng Việt bình thường, nhưng mở SSMS (SQL Server) lên xem thì cột HoTen bị lỗi dấu hỏi ???.

Cách Debug: Kiểm tra lại file database.cpp. Khi dùng câu lệnh INSERT INTO hoặc UPDATE với dữ liệu kiểu NVARCHAR, bắt buộc phải có chữ N đằng trước chuỗi. (Code hiện tại đã cấu hình chuẩn: N'" + s.HoTen + "', nếu bạn tự sửa code, hãy lưu ý giữ lại chữ N này).

### 5. Lỗi khi Xóa sinh viên
Hiện tượng: Hệ thống báo [Thất bại] Không thể xóa... sinh viên này đang có dữ liệu điểm số ràng buộc khóa ngoại trong bảng Diem.

Cách Debug: Đây là tính năng bảo vệ dữ liệu của cơ sở dữ liệu. Bạn không thể xóa một sinh viên nếu sinh viên đó đã có bài thi. Để giải quyết, bạn phải vào bảng Diem xóa sạch điểm của sinh viên đó trước, sau đó mới dùng chức năng số 4 để xóa thông tin cá nhân.
