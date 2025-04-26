#pragma once
#include <Windows.h>
#include <vector>
#include <string>
void Log(const std::string& msg);
std::string GetFullPath(const std::string& fileName);
bool ReadFileToMemory(const std::string& filePath, std::vector<char>& buffer);

// utils.h