#include <Windows.h>
#include <iostream>
#include "injector.h"
// main.cpp
int main() {
    const char* targetProcessName = "javaw.exe";
    const char* dllName = "MC.dll";

    std::cout << "waiting for Minecraft...\n";

    DWORD pid = ProcessId(targetProcessName);
    if (!pid) {
        std::cerr << "[!] Minecraft not found.\n";
        return 1;
    }

    std::cout << "[+] (PID: " << pid << ")\n";
    if (!InjectReflective(pid, dllName)) {
        std::cerr << "[!] ailed.\n";
        return 1;
    }

    std::cout << "[+] Injected!\n";
    return 0;
}
