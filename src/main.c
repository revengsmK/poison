#include <windows.h>

#include "poison.h"
#include "resource.h"
#include "ufmod.h"

#define TIMER_ID 200


// Global variables

int g_xPos = 290;

HICON hIcon;
HPEN greenPen;
HPEN grayPen;
HGDIOBJ originalObject;
HGDIOBJ gdiObj;
RECT rect;
RECT rect2;
HWND hwnd_static_scroll;



INT_PTR CALLBACK donateDlgProc(HWND hwndDlgDonate, UINT message, WPARAM wParam,LPARAM lParam){
	
	switch(message){
		
		case WM_INITDIALOG:
				SetDlgItemText(hwndDlgDonate,IDC_TXTBTCADDR,"bc1qjwedefp3xruv7qak72l89hzx5mx0c0ftq8qem7");
		break;
		
		case WM_COMMAND:{
		
			switch(LOWORD(wParam)){
				
				case IDC_X:
					EndDialog(hwndDlgDonate,0);
				break;
				
				case IDC_COPYADDR:
					SendDlgItemMessage(hwndDlgDonate,IDC_TXTBTCADDR,EM_SETSEL,0,-1);
					SendDlgItemMessage(hwndDlgDonate,IDC_TXTBTCADDR,WM_COPY,0,0);
					
				break;
			}
			
		}
		break;
		
		case WM_CTLCOLORDLG:
			return (LONG)GetStockObject(BLACK_BRUSH);
			
		case WM_CTLCOLORSTATIC:{
			HDC hdc = (HDC)wParam;
			SetBkMode(hdc,TRANSPARENT);

			SetTextColor(hdc,RGB(210,210,210));
			return (LONG)GetStockObject(BLACK_BRUSH);
		}
		
		
		default:
			return FALSE;
	}
	
	return TRUE;
}

INT_PTR CALLBACK abtDlgProc(HWND hwndDlg2, UINT message, WPARAM wParam,LPARAM lParam){
	
	switch(message)
	{
		
		case WM_INITDIALOG:
		{
			hwnd_static_scroll = GetDlgItem(hwndDlg2,IDC_STATIC_SCROLL);
			SetTimer(hwndDlg2,TIMER_ID,19,NULL);
			uFMOD_PlaySong((void*)IDR_RCDATA1,0,XM_RESOURCE);
				
		}
		break;
		// *very* simple scroller here
		case WM_TIMER:
		{
			
			g_xPos--;
			
			// check if label got to far left
			if( g_xPos < -710)
					g_xPos  = 290;
				
			// move label
			
			MoveWindow(hwnd_static_scroll,g_xPos,15,680,20,TRUE);

			return 0;
		}
		
		
		case WM_CTLCOLORDLG:
			return (LONG)GetStockObject(BLACK_BRUSH);
			
		
		case WM_DRAWITEM:
		{
			
			DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*) lParam;
			
			originalObject = SelectObject(dis->hDC,GetStockObject(BLACK_BRUSH));
			
			SelectObject(dis->hDC,greenPen);
			Rectangle(dis->hDC,dis->rcItem.left,dis->rcItem.top,dis->rcItem.right,dis->rcItem.bottom);
			
			SetBkMode(dis->hDC,TRANSPARENT);
			SetTextColor(dis->hDC,RGB(207,207,207));
			
			if(dis->CtlID == IDOK)
			{
					if(dis->itemState & ODS_SELECTED)
						TextOut(dis->hDC,26,5,"close",5);
					else                                 
						TextOut(dis->hDC,25,4,"close",5);
			}
			
			// restore original object
			SelectObject(dis->hDC,originalObject);
			
			return TRUE;
		}
		
				
		case WM_CTLCOLORSTATIC:
		{
			HDC hdc = (HDC)wParam;
			SetBkMode(hdc,TRANSPARENT);
			
			if(GetDlgCtrlID((HWND)lParam) == IDC_STATIC_INFO)
				SetTextColor(hdc,RGB(91,236,55));
		   else
				SetTextColor(hdc,RGB(248,221,73));
		   
			return (LONG)GetStockObject(BLACK_BRUSH);
		}
		

		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				
				case IDOK:
				{
					// stop scrolling
					KillTimer(hwndDlg2,TIMER_ID);
					
					// reset scroll label position
					g_xPos = 290;
					
					uFMOD_StopSong();
					EndDialog(hwndDlg2,0);
					
				}
				break;
				
			}
		break;
		
		default:
			return FALSE;
		
	}
	
	return TRUE;
}


INT_PTR CALLBACK mainDlgProc(HWND hwndDlg , UINT msg, WPARAM wParam, LPARAM lParam){
	
	switch(msg)
	{
		
		case WM_INITDIALOG:
		{
			
			hIcon = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,GetSystemMetrics(SM_CXICON),GetSystemMetrics(SM_CYICON),0);
			
			if(hIcon)
				SendMessage(hwndDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
			
			
			PostMessage(GetDlgItem(hwndDlg,IDC_RADIOAUTO), BM_SETCHECK, BST_CHECKED,1);
			PostMessage(GetDlgItem(hwndDlg,IDC_RADIOMEMEXPLOIT), BM_SETCHECK, BST_CHECKED,1);
			
			GetClientRect(GetDlgItem(hwndDlg,IDC_EDIT),&rect);
			GetClientRect(GetDlgItem(hwndDlg,IDC_TXTPID),&rect2);
			
			greenPen = CreatePen(PS_SOLID,0,RGB(126,172,109));
			grayPen = CreatePen(PS_SOLID,0,RGB(83,83,83));
		}
		break;
		
			
		case WM_DRAWITEM:
		{
			
			DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*) lParam;
			
			originalObject = SelectObject(dis->hDC,GetStockObject(BLACK_BRUSH));
			
			SelectObject(dis->hDC,greenPen);
			
			Rectangle(dis->hDC,dis->rcItem.left,dis->rcItem.top,dis->rcItem.right,dis->rcItem.bottom);
			
			
			SetBkMode(dis->hDC,TRANSPARENT);
			SetTextColor(dis->hDC,RGB(207,207,207));
			
		
			switch(dis->CtlID)
			{
				
				case IDC_FORCERUN:
				
					if(dis->itemState & ODS_SELECTED)
						TextOut(dis->hDC,10,5,"force run surfblocker",21);
					else                          
						TextOut(dis->hDC,9,4,"force run surfblocker",21);
					
				break;
				
				case IDC_GETPASS:
				
					if(dis->itemState & ODS_SELECTED)
						TextOut(dis->hDC,15,5,"get password",12);
					else                          
						TextOut(dis->hDC,14,4,"get password",12);
					
				break;
				
				case IDC_ABOUT:
				
					if(dis->itemState & ODS_SELECTED)
						TextOut(dis->hDC,28,5,"about",5);
					else                          
						TextOut(dis->hDC,27,4,"about",5);
					
				break;
				
				case IDC_EXIT:
				
					if(dis->itemState & ODS_SELECTED)
						TextOut(dis->hDC,34,5,"exit",4);
					else                       
						TextOut(dis->hDC,33,4,"exit",4);
					
				break;
				
				case IDC_COPY:
				
					if(dis->itemState & ODS_SELECTED)
						TextOut(dis->hDC,12,3,"copy",4);
					else                          
						TextOut(dis->hDC,11,2,"copy",4);
				break;
			
				
				case IDC_DONATE:{
				
					SelectObject(dis->hDC,grayPen);
					Rectangle(dis->hDC,dis->rcItem.left,dis->rcItem.top,dis->rcItem.right,dis->rcItem.bottom);
		
					if(dis->itemState & ODS_SELECTED)
						TextOut(dis->hDC,6,3,"donate",6);
					else                          
						TextOut(dis->hDC,5,2,"donate",6);
				break;
				
				}
			}
				
			// restore original GDI object
			SelectObject(dis->hDC,originalObject);
			
			return TRUE;
		}
		
			
		case WM_CTLCOLORDLG:
			return (LONG)GetStockObject(BLACK_BRUSH);
			
		case WM_CTLCOLORSTATIC:
		{
			HDC hdc = (HDC)wParam;
			SetBkMode(hdc,TRANSPARENT);

			SetTextColor(hdc,RGB(71,166,0));
			
			return (LONG)GetStockObject(BLACK_BRUSH);
		}
		
		case WM_CTLCOLOREDIT:
		{
			
			gdiObj = SelectObject((HDC)wParam,GetStockObject(BLACK_BRUSH));
			
			SelectObject((HDC)wParam,grayPen);

			
			if((GetDlgCtrlID((HWND)lParam)) == IDC_EDIT)
				Rectangle((HDC)wParam,rect.left-2,rect.top-2,rect.right,rect.bottom-2);
			else
				Rectangle((HDC)wParam,rect2.left-2,rect2.top-2,rect2.right,rect2.bottom-2);
				
			
			SetBkMode((HDC)wParam,TRANSPARENT);
			SetTextColor((HDC)wParam,RGB(207,207,207));
	
			// restore original GDI object
			SelectObject((HDC)wParam,gdiObj);

			
			return (LONG)GetStockObject(NULL_BRUSH);
		}
		
		case WM_CLOSE:
		{
			
			DeleteObject(greenPen);
			DeleteObject(grayPen);
			DestroyIcon(hIcon);
			EndDialog(hwndDlg,0);
		}
		break;
		
		case WM_COMMAND:
		
			switch(LOWORD(wParam))
			{
				
				case IDC_FORCERUN:
					
					forceRun(hwndDlg);
				break;
				
				case IDC_GETPASS:
				{
					if(IsDlgButtonChecked(hwndDlg,IDC_RADIOMEMEXPLOIT) == BST_CHECKED)
							runExploit(hwndDlg);
					else if(IsDlgButtonChecked(hwndDlg,IDC_RADIODECRYPT) == BST_CHECKED)
							decryptPass(hwndDlg);
				}
				break;
				
				case IDC_ABOUT:
					DialogBoxParam(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_DIALOG2),hwndDlg,abtDlgProc,0);
				break;
				
				case IDC_EXIT:
					SendMessage(hwndDlg,WM_CLOSE,0,0);
				break;
				
				case IDC_DONATE:
					DialogBoxParam(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_DIALOG3),hwndDlg,donateDlgProc,0);
				break;
				
				case IDC_COPY:
					SendDlgItemMessage(hwndDlg,IDC_EDIT,EM_SETSEL,0,-1);
					SendDlgItemMessage(hwndDlg,IDC_EDIT,WM_COPY,0,0);
				break;
	
				case IDC_RADIODECRYPT:
				{
					EnableWindow(GetDlgItem(hwndDlg,IDC_RADIOAUTO),0);
					EnableWindow(GetDlgItem(hwndDlg,IDC_RADIOENTERPID),0);
				}
				break;
				
				case IDC_RADIOMEMEXPLOIT:
				{
					EnableWindow(GetDlgItem(hwndDlg,IDC_RADIOAUTO),1);
					EnableWindow(GetDlgItem(hwndDlg,IDC_RADIOENTERPID),1);
				}
				break;
		
			}
			
		break;

		default:
			return FALSE;
	}
	
	return TRUE;
	
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int nCmdShow){
	
	return DialogBoxParamW(hInstance,MAKEINTRESOURCEW(IDD_DIALOG1),NULL,mainDlgProc,0);
	
}