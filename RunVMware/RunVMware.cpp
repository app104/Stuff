// RunVMware.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "RunVMware.h"
#include "commctrl.h"
#include "shlwapi.h"
#pragma comment(lib, "shlwapi.lib")
#include "Tlhelp32.h"

#include "shellapi.h"
#pragma warning(disable: 4996 ) 
#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
HWND hDlg;
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
int WindowVersion = 0; //Windows版本
TCHAR szVMwarePath[260];
TCHAR szVMPlayerPath[260];
SC_HANDLE scHandle; //服务句柄
typedef struct{
	int NUM;
	union { TCHAR NAME[32]; TCHAR Service[32]; };
	union { int   ID; int state; };
	TCHAR Abbre[32]; //abbreviate 缩写
}WV;
#define WINVER_10	1 //Windows 10
#define WINVER_8	2 //Windows 8
#define WINVER_7	3 //Windows 7
#define WINVER_V	4 //Windows Vista
#define WINVER_XP	5 //Windows XP
#define WINVER_2000 6 //Windows 2000
#define WINVER_ME   7 //Windows ME
#define WINVER_98   8 //Windows 98
WV wv[] = {
	{0,_TEXT("Windows 10"), WINVER_10,_TEXT("") },
	{1, _TEXT("Windows 8"), WINVER_8,_TEXT("") },
	{2, _TEXT("Windows 7"), WINVER_7,_TEXT("") },
	{3, _TEXT("Windows Vista"), WINVER_V ,_TEXT("") },
	{4, _TEXT("Windows XP"), WINVER_XP,_TEXT("") },
	{5, _TEXT("Windows 2000"), WINVER_2000,_TEXT("") },
	{6, _TEXT("Windows ME"), WINVER_ME,_TEXT("") },
	{7, _TEXT("Windows 98"), WINVER_98 ,_TEXT("") }
};
typedef WV SS;
SS ss[]{
	{0,_TEXT("VMware Workstation Server"), 0,_TEXT("VMwareHostd") },
	{1,_TEXT("VMware Authorization Service"), 0,_TEXT("VMAuthdService") },
	{2,_TEXT("VMware USB Arbitration Service"),0,_TEXT("VMUSBArbService") },
	{3,_TEXT("VMware DHCP Service"),0 ,_TEXT("VMnetDHCP") },
	{4,_TEXT("VMware NAT Service"), 0,_TEXT("VMware NAT Service") },
};
TCHAR* StateInfo[]
{
	_TEXT(""), // 0
	_TEXT("已停止"), // SERVICE_STOPPED 0x00000001
	_TEXT("正在启动"), //SERVICE_START_PENDING 0x00000002
	_TEXT("正在停止"), //SERVICE_STOP_PENDING 0x00000003
	_TEXT("运行中"), //SERVICE_RUNNING 0x00000004
	_TEXT("正在继续"), //SERVICE_CONTINUE_PENDING 0x00000005
	_TEXT("正在暂停"), //SERVICE_PAUSE_PENDING 0x00000006
	_TEXT("暂停中"), //SERVICE_PAUSED 0x00000007
};
// 此代码模块中包含的函数的前向声明: 
INT_PTR CALLBACK    MainDlg_proc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。
	LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RUNVMWARE));

    return (int)DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), NULL, MainDlg_proc);
}

int MainDlg_init(HWND hDlg);
int StartServiceALL(HWND hDlg);
int StopServiceALL(HWND hDlg);
int ListViewState_set(HWND hDlg, int num);
int CheckProgramState(TCHAR* program, int do_kill);
INT_PTR CALLBACK MainDlg_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
	TCHAR* prog1 = _TEXT("vmware.exe");
	TCHAR* prog2 = _TEXT("vmplayer.exe");
	TCHAR* prog3 = _TEXT("vmnetcfg.exe");
    switch (message)
    {
    case WM_INITDIALOG:
	{
		::hDlg = hDlg;
		scHandle = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ENUMERATE_SERVICE);
		int x = GetSystemMetrics(SM_CXSCREEN);
		int y = GetSystemMetrics(SM_CYSCREEN);
		RECT rc;
		GetWindowRect(hDlg, &rc);
		MoveWindow(hDlg, (x - rc.right) / 2, (y - rc.bottom) / 2, rc.right, rc.bottom, TRUE);
		MainDlg_init(hDlg);
		SetTimer(hDlg, 100, 1000, NULL);
	}
        return (INT_PTR)TRUE;
	case WM_TIMER:
		for (int i = 0; i != 5; i++)
		{
			ListViewState_set(hDlg, i);
		}
		break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
		case IDB_VMWARE:
			StartServiceALL(hDlg);
			if (CheckProgramState(prog1, 0) == 0)
			{
				TCHAR process[260];
				swprintf(process, _TEXT("%s\\%s"), szVMwarePath,prog1);
				ShellExecute(0, _TEXT("runas"), process, _TEXT(""), szVMwarePath, SW_SHOWNORMAL);
				goto END_PROCESS;
			}
			break;
		case IDB_VMPLAYER:
			StartServiceALL(hDlg);
			if (CheckProgramState(prog2, 0) == 0)
			{
				TCHAR process[260];
				swprintf(process, _TEXT("%s\\%s"), szVMPlayerPath, prog2);
				ShellExecute(0, _TEXT("runas"), process, _TEXT(""), szVMPlayerPath, SW_SHOWNORMAL);
				goto END_PROCESS;
			}
			break;
		case IDB_VMNETCFG:
			StartServiceALL(hDlg);
			if (CheckProgramState( prog3, 0) == 0)
			{
				TCHAR process[260];
				swprintf(process, _TEXT("%s\\%s"), szVMPlayerPath, prog3);
				ShellExecute(0, _TEXT("runas"), process, _TEXT(""), szVMPlayerPath, SW_SHOWNORMAL);
				goto END_PROCESS;
			}
			break;
		case IDB_VMSTOP:
			StopServiceALL(hDlg);
			CheckProgramState(prog1, 1);
			CheckProgramState(prog2, 1);
			CheckProgramState(prog3, 1);
			//break;
		case IDOK:
		case IDCANCEL:
END_PROCESS:
			CloseServiceHandle(scHandle);
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

int ListViewInfo_init(HWND hDlg);
int ListViewState_init(HWND hDlg);
int MainDlg_init(HWND hDlg)
{
	HWND hCtrl1 = GetDlgItem(hDlg, IDL_INFO);
	HWND hCtrl2 = GetDlgItem(hDlg, IDL_STATE);
	RECT rc1,rc2;
	GetClientRect(hCtrl1, &rc1);
	
	GetClientRect(hCtrl2, &rc2);
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

	lvc.iSubItem = 0;
	lvc.fmt = LVCFMT_CENTER;
	lvc.pszText = _TEXT("名称");
	lvc.cx = rc1.right / 3;     // width of column in pixels
	ListView_InsertColumn(hCtrl1, 0, &lvc);
	lvc.cx = rc2.right / 5 * 4;     // width of column in pixels
	ListView_InsertColumn(hCtrl2, 0, &lvc);
	lvc.fmt = LVCFMT_LEFT;
	lvc.pszText = _TEXT("属性");
	lvc.cx = rc1.right / 3*2;
	ListView_InsertColumn(hCtrl1, 1, &lvc);
	lvc.cx = rc2.right / 5;
	ListView_InsertColumn(hCtrl2, 1, &lvc);
	/*以上完成ListView标题的初始化*/
	//以下要添加项目
	ListViewInfo_init(hDlg);
	ListViewState_init(hDlg);
	return 0;
}
int GetSystemVersion(TCHAR* info);
int GetVMwareVersion(TCHAR* info, int sign);
int ListViewInfo_init(HWND hDlg)
{
	HWND hCtrl = GetDlgItem(hDlg, IDL_INFO);
	LVITEM lvI;
	TCHAR tstr[260] = {};
	lvI.mask = LVIF_TEXT;
	lvI.iItem = 0;
	lvI.pszText = tstr; //260个TCHAR的限制
	lvI.iSubItem = 0;
	swprintf(tstr, _TEXT("Windows版本"));
	ListView_InsertItem(hCtrl, &lvI);
	lvI.iSubItem = 1;
	GetSystemVersion(tstr);
	ListView_SetItem(hCtrl, &lvI);

	lvI.iItem = 1;// 第二行
	lvI.iSubItem = 0;
	swprintf(tstr, _TEXT("VMware Workstation版本"));
	ListView_InsertItem(hCtrl, &lvI);
	lvI.iSubItem = 1;
	GetVMwareVersion(tstr,1);
	ListView_SetItem(hCtrl, &lvI);

	lvI.iItem = 2;// 第三行
	lvI.iSubItem = 0;
	swprintf(tstr, _TEXT("VMware Player版本"));
	ListView_InsertItem(hCtrl, &lvI);
	lvI.iSubItem = 1;
	GetVMwareVersion(tstr, 0);
	ListView_SetItem(hCtrl, &lvI);
	return 0;
}
int GetVMwareVersion(TCHAR* info,int sign)
{
	TCHAR pSubKey[260] = {};
	switch (WindowVersion)
	{
	case WINVER_10:
		swprintf(pSubKey, _TEXT("SOFTWARE\\WOW6432Node\\VMware, Inc.\\"));
		break;
	case WINVER_7:
		swprintf(pSubKey, _TEXT("SOFTWARE\\VMware, Inc.\\"));
		break;
	default:
		break;
	}
	if (sign) 
	{
		StrCat(pSubKey, _T("VMware Workstation"));
	}
	else
	{
		StrCat(pSubKey, _T("VMware Player"));
	}
	TCHAR pValue[260];
	swprintf(pValue, _TEXT("InstallPath"));
	DWORD dwType = REG_SZ;
	DWORD dwSize = 260;
	TCHAR * ptstr;
	if (sign) ptstr = szVMwarePath; else ptstr = szVMPlayerPath;
	
	RegGetValue(HKEY_LOCAL_MACHINE, pSubKey, pValue, RRF_RT_REG_SZ, &dwType, ptstr, &dwSize);
	if(lstrlen(ptstr) == 0)
	{
		if (sign)
		{
			EnableWindow(GetDlgItem(hDlg, IDB_VMWARE), FALSE);
			
			EnableWindow(GetDlgItem(hDlg, IDB_VMNETCFG), FALSE);
		}
		else
		{
			EnableWindow(GetDlgItem(hDlg, IDB_VMPLAYER), FALSE);
		}
	}
	TCHAR szValue[260] = {};
	dwSize = 260;
	swprintf(pValue, _TEXT("ProductVersion"));
	RegGetValue(HKEY_LOCAL_MACHINE, pSubKey, pValue, RRF_RT_REG_SZ, &dwType, szValue, &dwSize);
	swprintf(info, _TEXT("安装路径:%s 版本:%s"), ptstr, szValue);
	return 0;
}
int GetSystemVersion(TCHAR* info)
{
	TCHAR pSubKey[260];
	swprintf(pSubKey, _TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"));
	TCHAR pValue[260];
	swprintf(pValue, _TEXT("ProductName"));
	DWORD dwType = REG_SZ;
	DWORD dwSize = 260;
	TCHAR str1[260] = {};
	RegGetValue(HKEY_LOCAL_MACHINE, pSubKey, pValue, RRF_RT_REG_SZ, &dwType, str1, &dwSize);
	if (dwSize != 0)
	{
		for (int i = 0; i != 8; i++)
		{
			if (StrStr(str1, wv->NAME) == str1)
			{
				WindowVersion = wv->ID;
				break;
			}
		}

	}
	TCHAR str2[260] = {};
	swprintf(pValue, _TEXT("BuildLabEx"));
	dwSize = 260;
	RegGetValue(HKEY_LOCAL_MACHINE, pSubKey, pValue, RRF_RT_REG_SZ, &dwType, str2, &dwSize);
	swprintf(info, _TEXT("%s %s"), str1, str2);
	return 0;
}

int GetServiceState(TCHAR* Service, int* pstate);
int ListViewState_init(HWND hDlg)
{
	HWND hCtrl = GetDlgItem(hDlg, IDL_STATE);
	LVITEM lvI;
	TCHAR tstr[260] = {};
	lvI.mask = LVIF_TEXT;
	for (int i = 0; i != 5; i++)
	{
		GetServiceState(ss[i].Abbre, &(ss[i].state));
		lvI.iItem = i;
		lvI.pszText = tstr; //260个TCHAR的限制
		lvI.iSubItem = 0;
		swprintf(tstr, ss[i].Service);
		ListView_InsertItem(hCtrl, &lvI);
		lvI.iSubItem = 1;
		swprintf(tstr, StateInfo[ss[i].state]);
		ListView_SetItem(hCtrl, &lvI);
	}

	return 0;
}
int ListViewState_set(HWND hDlg, int num)
{
	HWND hCtrl = GetDlgItem(hDlg, IDL_STATE);
	LVITEM lvI;
	TCHAR tstr[260] = {};
	lvI.mask = LVIF_TEXT;
	GetServiceState(ss[num].Abbre, &(ss[num].state));
	lvI.iItem = num;
	lvI.pszText = tstr; //260个TCHAR的限制
	lvI.iSubItem = 1;
	swprintf(tstr, StateInfo[ss[num].state]);
	ListView_SetItem(hCtrl, &lvI);
	return 0;
}
int GetServiceState(TCHAR* abb, int* pstate)
{
	SC_HANDLE hService = OpenService(scHandle, abb, GENERIC_READ);
	SERVICE_STATUS sss = {};
	QueryServiceStatus(hService, &sss);
	*pstate = sss.dwCurrentState;
	CloseServiceHandle(hService);
	return *pstate;
}

int SetServiceState(TCHAR* abb, int *p, int state)
{
	for (int i = 0; i != 5; i++)
	{
		int j = 0;
		for (; j != 30; j++)
		{
			GetServiceState(ss[i].Abbre, &(ss[i].state));
			if (ss[i].state != SERVICE_STOPPED || ss[i].state != SERVICE_RUNNING)
			{
				if (ss[i].state == SERVICE_STOP_PENDING || ss[i].state == SERVICE_START_PENDING)
				{
					Sleep(300);
					continue;
				}
				break;
			}
		}
		if (j == 30)
		{
			MessageBox(NULL, _TEXT("服务状态未定, 不能操作服务"), _TEXT("错误"), MB_OK);
			return -1;
		}
	}
	SC_HANDLE hService = OpenService(scHandle, abb, GENERIC_ALL);
	if (hService == NULL)
	{
		int x = GetLastError();
		int y = 0;
	}
	SERVICE_STATUS sss = {};
	QueryServiceStatus(hService, &sss);
	//state = sss.dwCurrentState;
	if (state == SERVICE_RUNNING && sss.dwCurrentState == SERVICE_STOPPED)
	{
		StartService(hService, 0, NULL);//启动一项服务
		*p = SERVICE_START_PENDING;
	}
	else if (state == SERVICE_STOPPED && sss.dwCurrentState != SERVICE_STOPPED )
	{
		ControlService(hService, SERVICE_CONTROL_STOP, &sss); //停止一项服务
		*p = SERVICE_STOP_PENDING;
	}
	CloseServiceHandle(hService);
	return state;
}
int StartServiceALL(HWND hDlg)
{
	for (int i = 0; i != 5; i++)
	{
		SetServiceState(ss[i].Abbre,&(ss[i].state),SERVICE_RUNNING);
		ListViewState_set(hDlg, i);
	}
	return 0;
}
int StopServiceALL(HWND hDlg)
{
	for (int i = 0; i != 5; i++)
	{
		SetServiceState(ss[i].Abbre, &(ss[i].state), SERVICE_STOPPED);
		ListViewState_set(hDlg, i);
	}
	return 0;
}

int CheckProgramState(TCHAR* program, int do_kill)
{
	int ret = 0;
	HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	PROCESSENTRY32 ppe = {};
	PROCESSENTRY32 ppe2 = {};
	ppe.dwSize = sizeof(PROCESSENTRY32);
	BOOL retval = Process32First(Snapshot,&ppe);
	while(retval)
	{
		if (memcmp(&ppe, &ppe2,sizeof(PROCESSENTRY32)) == 0)//和上一次的一模一样,就退出,在win10下这个函数有问题
		{
			break;
		}
		memcpy(&ppe2, &ppe, sizeof(PROCESSENTRY32));
		if(StrCmp(ppe.szExeFile, program) == 0)
		{
			ret = 1;
			if (do_kill)
			{
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, ppe.th32ProcessID);
				if (hProcess != NULL)
				{
					TerminateProcess(hProcess, 0);
					WaitForSingleObject(hProcess, 3000);
				}
				CloseHandle(hProcess);
			}
			break;
		}
		else
		{
			Process32Next(Snapshot,&ppe);
		}
	}
	CloseHandle(Snapshot);
	return ret;
}