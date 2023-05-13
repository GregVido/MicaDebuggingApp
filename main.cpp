/*
Copyright 2023 GregVido

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <Windows.h>
#include <iostream>
#include <vector>
#include <psapi.h>
#include "app.h"

const int max_length = 256;
char *processSearch;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    std::vector<HWND> *handles = reinterpret_cast<std::vector<HWND> *>(lParam);

    DWORD processId;
    GetWindowThreadProcessId(hwnd, &processId);
    HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);

    char processName[max_length] = "";

    HMODULE moduleHandle;
    DWORD cbNeeded;
    if (EnumProcessModules(processHandle, &moduleHandle, sizeof(moduleHandle), &cbNeeded))
    {
        GetModuleBaseNameA(processHandle, moduleHandle, processName, sizeof(processName) / sizeof(char));
    }
    CloseHandle(processHandle);

    char *result = strstr(processName, processSearch);
    if (result)
    {
        std::cout << "Window process name: " << processName << " HWND: " << hwnd << std::endl;
        handles->push_back(hwnd);
    }
    return TRUE;
}

int applyEffect(char *processName, int effect)
{
    processSearch = processName;

    std::vector<HWND> notepadHandles;
    // Find Notepad window handle
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&notepadHandles));

    if (notepadHandles.empty())
    {
        std::cout << "No windows found." << std::endl;
        return 1;
    }

    const HINSTANCE dwmapi = LoadLibrary(TEXT("dwmapi.dll"));
    const pDwmSetWindowAttribute DwmSetWindowAttribute = (pDwmSetWindowAttribute)GetProcAddress(dwmapi, "DwmSetWindowAttribute");
    const pDwmExtendFrameIntoClientArea DwmExtendFrameIntoClientArea = (pDwmExtendFrameIntoClientArea)GetProcAddress(dwmapi, "DwmExtendFrameIntoClientArea");

    for (const HWND &hwnd : notepadHandles)
    {
        DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &effect, sizeof(int));

        MARGINS margins = {-1};
        DwmExtendFrameIntoClientArea(hwnd, &margins);
    }

    FreeLibrary(dwmapi);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--help") == 0)
    {
        std::cout << "Run the command like this : app.exe <processname> <effect>\n" << std::endl;
        std::cout << "<processname>" << std::endl;
        std::cout << "\tprocess name (ex: Notepad, explorer)\n"
                  << std::endl;
        std::cout << "<effect>" << std::endl;
        std::cout << "\teffect name" << std::endl;
        std::cout << "\t\t- mica : applies the mica effect" << std::endl;
        std::cout << "\t\t- tabbed_mica : applies the tabbed mica effect" << std::endl;
        std::cout << "\t\t- acrylic : apply an acrylic effect" << std::endl;
        std::cout << "\t\t- none : deactivate the effect" << std::endl;
        std::cout << "\nVersion: 1.0" << std::endl;
        return 1;
    }

    if (argc != 3)
    {
        std::cout << "Command failed (app.exe <processname> <effect>)" << std::endl;
        std::cout << "app.exe --help for more info" << std::endl;
        return 0;
    }

    if (strcmp(argv[2], "mica") == 0)
        return applyEffect(argv[1], 2);

    if (strcmp(argv[2], "tabbed_mica") == 0)
        return applyEffect(argv[1], 4);

    if (strcmp(argv[2], "acrylic") == 0)
        return applyEffect(argv[1], 3);

    if (strcmp(argv[2], "none") == 0)
        return applyEffect(argv[1], 1);

    std::cout << "Unknown effect" << std::endl;
    std::cout << "app.exe --help for more info" << std::endl;
    return 0;
}
