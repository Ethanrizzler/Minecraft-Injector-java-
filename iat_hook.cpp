#include "iat_hook.h"
#include "utils.h"
#include <tlhelp32.h>
#include <psapi.h>
// iat _hook.cpp
bool HookImportToEntry(HANDLE hProcess, const std::string& dllName, const std::string& funcName, LPVOID newFunc) {
    MODULEENTRY32 me32;
    me32.dwSize = sizeof(MODULEENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(hProcess));
    if (snapshot == INVALID_HANDLE_VALUE) {
        Log("[!] Faild snapshot  process modules.");
        return false;
    }

    if (Module32First(snapshot, &me32)) {
        do {
            if (_wcsicmp(me32.szModule, std::wstring(dllName.begin(), dllName.end()).c_str()) == 0) {
                PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)me32.modBaseAddr;
                PIMAGE_NT_HEADERS64 ntHeaders = (PIMAGE_NT_HEADERS64)((BYTE*)me32.modBaseAddr + dosHeader->e_lfanew);
                PIMAGE_IMPORT_DESCRIPTOR importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)me32.modBaseAddr + ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

                for (; importDescriptor->Name; importDescriptor++) {
                    char* importDllName = (char*)me32.modBaseAddr + importDescriptor->Name;
                    if (_stricmp(importDllName, dllName.c_str()) == 0) {
                        PIMAGE_THUNK_DATA originalThunk = (PIMAGE_THUNK_DATA)((BYTE*)me32.modBaseAddr + importDescriptor->OriginalFirstThunk);
                        PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)((BYTE*)me32.modBaseAddr + importDescriptor->FirstThunk);

                        for (; originalThunk->u1.Function; originalThunk++) {
                            FARPROC originalProc = (FARPROC)originalThunk->u1.Function;
                            if (originalProc == (FARPROC)GetProcAddress(GetModuleHandleA(dllName.c_str()), funcName.c_str())) {
                                DWORD oldProtect;
                                VirtualProtectEx(hProcess, &thunk->u1.Function, sizeof(LPVOID), PAGE_EXECUTE_READWRITE, &oldProtect);
                                thunk->u1.Function = (ULONG_PTR)newFunc;
                                VirtualProtectEx(hProcess, &thunk->u1.Function, sizeof(LPVOID), oldProtect, &oldProtect);
                                Log("[+] IAT hook applied.");
                                CloseHandle(snapshot);
                                return true;
                            }
                        }
                    }
                }
            }
        } while (Module32Next(snapshot, &me32));
    }

    CloseHandle(snapshot);
    Log("[!] Failed to hook IAT.");
    return false;
}
