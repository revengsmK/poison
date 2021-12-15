#ifndef PFUNC_H
#define PFUNC_H

LPVOID GetProcessOEP(HANDLE hProcess, MODULEINFO modinfo);
DWORD GetProcessPID(char class_name[] , char  window_name[]);
unsigned int GetConstantAddress(HANDLE hProcess,LPVOID address);
DWORD GetBaseAddress(HWND hwnd, HANDLE hProc ,HANDLE hThread);
signed char memexploit(HANDLE hProc, unsigned int const_addr,  WCHAR *password);

#endif  /*PFUNC_H*/

