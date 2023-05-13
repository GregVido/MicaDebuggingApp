#include <dwmapi.h>
#include <iostream>

#define DWMWA_SYSTEMBACKDROP_TYPE DWORD(38)

typedef BOOL(WINAPI *pDwmSetWindowAttribute)(HWND, DWORD, int *, int);
typedef BOOL(WINAPI *pDwmExtendFrameIntoClientArea)(HWND, MARGINS *);