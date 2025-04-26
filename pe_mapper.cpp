#include "pe_mapper.h"
#include "utils.h"
#include <winnt.h>
//pe_mapper.cpp
LPVOID ManualMapDLL(HANDLE hProcess, const std::vector<char>& dllBuffer) {
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)dllBuffer.data();
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        Log("[!] Invalid DOS signature.");
        return nullptr;
    }

    PIMAGE_NT_HEADERS64 ntHeaders = (PIMAGE_NT_HEADERS64)(dllBuffer.data() + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        Log("[!] Invalid NT signature.");
        return nullptr;
    }

    SIZE_T imageSize = ntHeaders->OptionalHeader.SizeOfImage;
    LPVOID remoteImage = VirtualAllocEx(hProcess, NULL, imageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!remoteImage) {
        Log("[!] VirtualAllocEx failed.");
        return nullptr;
    }

    // Write headers
    SIZE_T written;
    WriteProcessMemory(hProcess, remoteImage, dllBuffer.data(), ntHeaders->OptionalHeader.SizeOfHeaders, &written);

    // Write sections
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeaders);
    for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++, section++) {
        LPVOID sectionDest = (BYTE*)remoteImage + section->VirtualAddress;
        LPVOID sectionSrc = (BYTE*)dllBuffer.data() + section->PointerToRawData;
        WriteProcessMemory(hProcess, sectionDest, sectionSrc, section->SizeOfRawData, &written);
    }

    // Entry point = our custom loader
    LPVOID entryPoint = (BYTE*)remoteImage + ntHeaders->OptionalHeader.AddressOfEntryPoint;
    return entryPoint;
}
