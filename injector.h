#pragma once
#include <Windows.h>
#include <string>
#include "utils.h"
// injector.h
DWORD ProcessId(const std::string& processName);
bool InjectReflective(DWORD pid, const std::string& dllName);
