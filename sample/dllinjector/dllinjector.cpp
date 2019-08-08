#include <Windows.h>
#include <iostream>

int wmain(int argc, wchar_t *argv[]) {
    if (argc < 3) {
        std::cout << "usage: helper [TARGET] [DLL]" << std::endl;
        return 1;
    }

    WCHAR *target = argv[1];
    WCHAR *lib = argv[2];

    std::wcout << L"[*] target: " << target << std::endl;
    std::wcout << L"[*] lib: " << lib << std::endl;

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));

    // Create process of target binary with CREATE_SUSPENDED flag.
    if (!CreateProcessW(target, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        std::cout << "[*] CreateProcess fail " << GetLastError() << std::endl;
        return 1;
    }

    // Get address of kernel32.LoadLibraryW
    HMODULE hKernel32 = LoadLibraryW(L"kernel32.dll");
    LPTHREAD_START_ROUTINE lpLoadLibraryW = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");

    // Remote memory allocation
    SIZE_T dwLength = (wcslen(lib) + 1) * 2;
    LPVOID lpLibName = VirtualAllocEx(pi.hProcess, NULL, dwLength, MEM_COMMIT, PAGE_READWRITE);
    if (lpLibName == NULL) {
        std::cout << "[*] VirtualAllocEx fail " << GetLastError() << std::endl;
        return 1;
    }

    // Write Brancher dll path.
    SIZE_T written;
    if (!WriteProcessMemory(pi.hProcess, lpLibName, lib, dwLength, &written)) {
        std::cout << "[*] WriteProcessMemory fail " << GetLastError() << std::endl;
        return 1;
    }

    // Run LoadLibraryW with allocated memory.
    HANDLE hThread = CreateRemoteThread(pi.hProcess, NULL, NULL, lpLoadLibraryW, lpLibName, NULL, NULL);
    if (hThread == NULL) {
        std::cout << "[*] CreateRemoteThread fail " << GetLastError() << std::endl;
        return 1;
    }

    WaitForSingleObject(hThread, INFINITE);

    // Run process.
    CloseHandle(hThread);
    ResumeThread(pi.hThread);

    // Free allocated memory,
    VirtualFreeEx(pi.hProcess, lpLibName, dwLength, MEM_RELEASE);

    std::cout << "[*] Waiting for process.." << std::endl;

    // Waiting for termination of target process.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close handle.
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return 0;
}