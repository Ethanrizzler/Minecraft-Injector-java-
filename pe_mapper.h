#pragma once
#include <Windows.h>
#include <vector>
#include "utils.h"
// pe_mapper.h
LPVOID ManualMapDLL(HANDLE hProcess, const std::vector<char>& dllBuffer);
