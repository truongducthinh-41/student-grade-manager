#include "Utils.h"
#include <iostream>
#include <algorithm>
#include <ctime>

size_t utf8_length(const std::string& str) {
    size_t len = 0;
    for (char c : str) {
        if ((c & 0xC0) != 0x80) len++;
    }
    return len;
}

std::string padRight(const std::string& str, size_t width) {
    size_t len = utf8_length(str);
    if (len >= width) return str;
    return str + std::string(width - len, ' ');
}

std::string padLeft(const std::string& str, size_t width) {
    size_t len = utf8_length(str);
    if (len >= width) return str;
    return std::string(width - len, ' ') + str;
}

std::string toLowerCase(const std::string& str) {
    std::string res = str;
    for (size_t i = 0; i < res.length(); ++i) {
        if ((unsigned char)res[i] == 0xC4 && i + 1 < res.length() && (unsigned char)res[i+1] == 0x90) {
            res[i+1] = 0x91; i++;
        } else {
            res[i] = (char)tolower((unsigned char)res[i]);
        }
    }
    return res;
}

double inputValidGrade(const std::string &prompt) {
    double val;
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);
        size_t first = input.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return 0.0;
        size_t last = input.find_last_not_of(" \t\r\n");
        input = input.substr(first, (last - first + 1));
        if (toLowerCase(input) == "none") return 0.0;
        try {
            val = std::stod(input);
            if (val >= 0.0 && val <= 10.0) return val;
            std::cout << "Điểm phải từ 0.0 đến 10.0. Vui lòng nhập lại!\n";
        } catch (...) {
            std::cout << "Nhập sai định dạng. Vui lòng nhập lại!\n";
        }
    }
}

std::string escapeCSVField(const std::string& str) {
    if (str.find(',') != std::string::npos || str.find('"') != std::string::npos) {
        std::string res = "\"";
        for (char c : str) {
            if (c == '"') res += "\"\"";
            else res += c;
        }
        res += "\"";
        return res;
    }
    return str;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string getCurrentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[11];
#ifdef _WIN32
    localtime_s(&tstruct, &now);
#else
    tstruct = *localtime(&now);
#endif
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tstruct);
    return std::string(buf);
}

std::string sanitizeSQL(const std::string& input) {
    std::string output = "";
    for (char c : input) {
        if (c == '\'') output += "''"; // Thay 1 nháy đơn thành 2 nháy đơn (SQL Escape)
        else output += c;
    }
    return output;
}