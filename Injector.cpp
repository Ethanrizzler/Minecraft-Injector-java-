#include "injector.h"
#include "pe_mapper.h"
#include "iat_hook.h"
#include "utils.h"
#include <tlhelp32.h>
#include <fstream>
// Injector.cpp
DWORD ProcessId(const std::string& processName) {
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE) return 0;

    if (Process32First(snapshot, &pe32)) {
        do {
            if (_wcsicmp(pe32.szExeFile, std::wstring(processName.begin(), processName.end()).c_str()) == 0) { 
              //  if (_stricmp(pe32.szExeFile, processName.c_str()) == 0) {    // [Error Line]
                // convert szExeFile (which is a WCHAR*) to char* or std::string before comparing it with processName.c_str().
                CloseHandle(snapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32Next(snapshot, &pe32));
    }

    CloseHandle(snapshot);
    return 0;
}

bool InjectReflective(DWORD pid, const std::string& dllName) {
    std::string dllPath = GetFullPath(dllName);
    std::vector<char> dllBuffer;

    if (!ReadFileToMemory(dllPath, dllBuffer)) {
        Log("[!] Failed to read DLL from disk.");
        return false;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess) {
        Log("[!] Failed to open target process.");
        return false;
    }

    LPVOID remoteBase = ManualMapDLL(hProcess, dllBuffer);
    if (!remoteBase) {
        Log("[!] mapping failed.");  // real
        CloseHandle(hProcess);
        return false;
    }

    if (!HookImportToEntry(hProcess, "msvcrt.dll", "HeapAlloc", remoteBase)) {
        Log("[!] IAT hook failed.");
        CloseHandle(hProcess);
        return false;
    }

    Log("[+] DLL injected and IAT hook placed.");
    CloseHandle(hProcess);
    return true;
}
