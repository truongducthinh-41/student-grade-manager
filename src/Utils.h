#ifndef UTILS_H
#define UTILS_H

#include <string>

size_t utf8_length(const std::string& str);
std::string padRight(const std::string& str, size_t width);
std::string padLeft(const std::string& str, size_t width);
std::string toLowerCase(const std::string& str);
double inputValidGrade(const std::string &prompt);
std::string escapeCSVField(const std::string& str);
std::string trim(const std::string& str);
std::string getCurrentDateTime();
std::string sanitizeSQL(const std::string& input);

#endif