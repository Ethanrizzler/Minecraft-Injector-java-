#include "utils.h"
#include <iostream>
#include <fstream>
// this file made me crash out btw
void Log(const std::string& msg) {
    std::cout << msg << std::endl;
}

std::string GetFullPath(const std::string& fileName) {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);

    std::string fullPath(buffer);
    size_t pos = fullPath.find_last_of("\\/");
    return fullPath.substr(0, pos + 1) + fileName;
}

bool ReadFileToMemory(const std::string& filePath, std::vector<char>& buffer) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file) return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer.resize(size);
    if (!file.read(buffer.data(), size)) return false;

    return true;
}
