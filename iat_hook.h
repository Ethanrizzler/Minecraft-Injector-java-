#pragma once
#include <Windows.h>
#include <string>  
#include "utils.h"
// iat_hook.h
bool HookImportToEntry(HANDLE hProcess, const std::string& dllName, const std::string& funcName, LPVOID newFunc);
