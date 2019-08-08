#include <Windows.h>
#include <iostream>

int wmain(int argc, wchar_t *argv[]) {
    WCHAR *target = argv[1];
    WCHAR *lib = argv[2];

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));

    // Create process of target binary with CREATE_SUSPENDED flag.
    CreateProcessW(target, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);

    // Get address of kernel32.LoadLibraryW
    HMODULE hKernel32 = LoadLibraryW(L"kernel32.dll");
    LPTHREAD_START_ROUTINE lpLoadLibraryW = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");

    // Remote memory allocation
    SIZE_T dwLength = (wcslen(lib) + 1) * 2;
    LPVOID lpLibName = VirtualAllocEx(pi.hProcess, NULL, dwLength, MEM_COMMIT, PAGE_READWRITE);

    // Write Brancher dll path.
    SIZE_T written;
    WriteProcessMemory(pi.hProcess, lpLibName, lib, dwLength, &written);

    // Run LoadLibraryW with allocated memory.
    HANDLE hThread = CreateRemoteThread(pi.hProcess, NULL, NULL, lpLoadLibraryW, lpLibName, NULL, NULL);
    WaitForSingleObject(hThread, INFINITE);

    // Run process.
    CloseHandle(hThread);
    ResumeThread(pi.hThread);

    // Free allocated memory,
    VirtualFreeEx(pi.hProcess, lpLibName, dwLength, MEM_RELEASE);

    // Waiting for termination of target process.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close handle.
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 0;
}