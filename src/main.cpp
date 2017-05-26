#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <stdio.h>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
DWORD gPidToFind=0;
HWND gTargetWindowHwnd = NULL;
int c = 0;

BOOL CALLBACK wndEnum(HWND hwCurHwnd, LPARAM lpMylp)
{
    DWORD dwCurPid = 0;
    char lpWndText[256];
    char lpWndClass[256];

    ::GetWindowText(hwCurHwnd, lpWndText, sizeof(lpWndText));
    ::GetClassName(hwCurHwnd, lpWndClass, sizeof(lpWndClass));

    GetWindowThreadProcessId(hwCurHwnd, &dwCurPid);

//    if(dwCurPid == gPidToFind) // not worked with IE because it have of many hidden windows, that stays visible
    if(dwCurPid == gPidToFind && (strcmp(lpWndClass,"IEFrame") == 0))
    {
        //c++;
        SetWindowPos(hwCurHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE|SWP_NOMOVE);
    }

    return TRUE;
}

int APIENTRY WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    char buf[256];
    int argc;
    char** argv;

    LPWSTR* lpArgv = CommandLineToArgvW(GetCommandLineW(), &argc);
    argv = (char**)malloc(argc*sizeof(char*));
    int size;
    for(int i=0;i<argc;++i) {
        size=wcslen(lpArgv[i])+1;
        argv[i]=(char *)malloc(size);
        wcstombs(argv[i],lpArgv[i],size);
    }
    if (argc == 0)
    {
         MessageBoxA(NULL, "Usage: startIEtop <URL>", "Error", MB_OK);
         return 10;
    }

    for (int i=1;i<argc;++i)
        sprintf(buf+strlen(buf), "%s ", argv[i]);

    STARTUPINFO cif;
	ZeroMemory(&cif,sizeof(STARTUPINFO));
	PROCESS_INFORMATION pi;
    CreateProcess(NULL, buf, NULL, NULL, FALSE, NULL, NULL, NULL, &cif, &pi);
    gPidToFind = pi.dwProcessId;
    int s=0;
    for (int i=0; i<15; ++i) // need some time to draw launched application
    {
        c=0;
        EnumWindows(wndEnum, NULL);
        if ((s==0)&&(c>0)) s=1;
        if ((s==1)&&(c==0)) break;
        Sleep(1000);
    }

    for(int i=0;i<argc;++i) {
        free(argv[i]);
        free(argv);
    }
    LocalFree(lpArgv);
    return 1;
}

