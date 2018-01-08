//Dialog_new.cpp: 定义新建通道对话框的入口点。
//

#include "stdafx.h"
#include "Co.h"
#include "utils.h"
#include "Iphlpapi.h"
#include "Winsock2.h"
#pragma comment(lib, "ws2_32.lib")
#include "WS2tcpip.h"
#pragma comment(lib, "Iphlpapi.lib")  //GetIpAddrTable

#include "commctrl.h"
void Ctrl_init(HWND hDlg);
void on_click_TCPS(HWND hDlg);
void on_click_TCPC(HWND hDlg);
void on_click_UDP(HWND hDlg);
void on_click_multicase(HWND hDlg);
void on_click_ping(HWND hDlg);
void on_click_serial(HWND hDlg);
// “新建通道”框的消息处理程序。
INT_PTR CALLBACK Dialog_new_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		Ctrl_init(hDlg);
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDR_TCPS:
			on_click_TCPS(hDlg);
			break;
		case IDR_TCPC:
			on_click_TCPC(hDlg);
			break;
		case IDR_UDP:
			on_click_UDP(hDlg);
			break;
		case IDR_UDPM:
		case IDR_UDPR:
			on_click_multicase(hDlg);
			SetWindowText(GetDlgItem(hDlg, IDC_RIP), _TEXT("组播IP"));
			SetWindowText(GetDlgItem(hDlg, IDC_RPORT), _TEXT("组播端口"));
			break;
		case IDR_PING:
			on_click_ping(hDlg);
			break;
		case IDR_SERIAL:
			on_click_serial(hDlg);
			break;
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		default:
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
void set_notice(HWND hDlg, int ID);

void Ctrl_init(HWND hDlg)
{
	//子窗口居中
	Dialog_center(hDlg);

	HWND hCtrl;

	//设置TCP Server为默认选中 IDR_TCPS
	SendMessage(GetDlgItem(hDlg, IDR_TCPS), BM_SETCHECK, (WPARAM)BST_CHECKED, (LPARAM)0);

	//初始化 本地IP IDE_LIP, 以下为获取本地IP列表, 并写入IDE_LIP
	hCtrl = GetDlgItem(hDlg, IDE_LIP);
	PMIB_IPADDRTABLE pIPAddrTable;
	ULONG dwSize = 0; //IP列表的大小
	pIPAddrTable = (MIB_IPADDRTABLE *)malloc(sizeof(MIB_IPADDRTABLE));
	if (pIPAddrTable == NULL)
	{
		return;
	}
	if (GetIpAddrTable(pIPAddrTable, &dwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER)
	{
		free(pIPAddrTable);
		pIPAddrTable = (MIB_IPADDRTABLE *)malloc(dwSize);
		if (pIPAddrTable == NULL || GetIpAddrTable(pIPAddrTable, &dwSize, TRUE) != NO_ERROR)
		{
			free(pIPAddrTable);
			return;
		}
	}
	TCHAR szIP[64];
	IN_ADDR IPAddr;     // 保存IP地址的结构体
	IPAddr.S_un.S_addr = htonl(INADDR_ANY); //0.0.0.0
	InetNtop(AF_INET, (PVOID)&IPAddr, szIP, 64);
	SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)szIP); //向Combo Box中添加条目
	for (int i = 0; i < (int)pIPAddrTable->dwNumEntries; i++)
	{
		IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwAddr;
		InetNtop(AF_INET, (PVOID)&IPAddr, szIP, 64);
		SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)szIP);//向Combo Box中添加条目

	}
	free(pIPAddrTable);
	SendMessage(hCtrl,CB_SETCURSEL,(WPARAM)0, (LPARAM) 0);  //把第(WPARAM)0项作为Combo Box的默认值显示

	//初始化 本地IP IDE_RIP
	hCtrl = GetDlgItem(hDlg, IDE_RIP);
	SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)_TEXT("0.0.0.0"));
	SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)_TEXT("127.0.0.1"));
	SendMessage(hCtrl, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);  //把第(WPARAM)0项作为Combo Box的默认值显示

	//初始化本地PORT IDE_LPORT
	SetWindowText(GetDlgItem(hDlg, IDE_LPORT), _TEXT("2404"));
	//初始化远端PORT IDE_RPORT
	SetWindowText(GetDlgItem(hDlg, IDE_RPORT), _TEXT("2404"));

	//初始化起始IP IDE_SIP
	SendMessage(GetDlgItem(hDlg, IDE_SIP), IPM_SETADDRESS, (WPARAM)0, (LPARAM)MAKEIPADDRESS(192,168,1,0));

	//初始化起始IP IDE_EIP
	SendMessage(GetDlgItem(hDlg, IDE_EIP), IPM_SETADDRESS, (WPARAM)0, (LPARAM)MAKEIPADDRESS(192, 168, 1, 255));

	//初始化超时 IDE_TIMEOUT
	SetWindowText(GetDlgItem(hDlg, IDE_TIMEOUT), _TEXT("5000"));


	//初始化串口号 IDE_SER_NO
	hCtrl = GetDlgItem(hDlg, IDE_SER_NO);
	TCHAR szPortName[64];
	for (int iCnt = 1; iCnt != 256; iCnt++) { //初始化串口号选择 ComboBoxEx
		wsprintf(szPortName, _TEXT("\\\\.\\COM%d"), iCnt);
		HANDLE hComm = CreateFile(szPortName,//串口号
			GENERIC_READ | GENERIC_WRITE,//允许读写
			0,//通讯设备必须以独占方式打开
			NULL,//无安全属性
			OPEN_EXISTING,//打开而不是创建
			0,//同步I/O
			NULL);//通讯设备不能用模板打开 
		if (hComm == INVALID_HANDLE_VALUE) continue; //串口打开失败, 无此串口
		CloseHandle(hComm);
		SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)&szPortName[4]);
	}
	SendMessage(hCtrl, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	//初始化 IDE_SER_BAUD IDE_SER_START IDE_SER_STOP IDE_SER_CHECK IDE_SER_BITSIZE
	LPTSTR szBaud[] = { 
		L"110", L"300", L"600", L"1200",
		L"2400", L"4800", L"9600",  L"14400",  
		L"19200", L"38400", L"56000",  L"57600",
		L"115200",  L"128000",  L"256000" }; //15个 波特率类型
	LPTSTR szBitSize[] = { L"4位",  L"5位", L"6位",  L"7位", L"8位" }; //5个 数据位
	LPTSTR szStopBit[] = { L"1停止位",  L"1.5停止位", L"2停止位" }; //3个 停止位
	LPTSTR szCheck[] = { L"无校验",  L"奇校验", L"偶校验", L"Mark校验", L"Space校验" }; // 5个 校验类型

	//初始化波特率 IDE_SER_BAUD
	hCtrl = GetDlgItem(hDlg, IDE_SER_BAUD);
	for(int i =0;i!=15;i++) SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)szBaud[i]);
	SendMessage(hCtrl, CB_SETCURSEL, (WPARAM)6, (LPARAM)0);

	//初始化数据位 IDE_SER_BITSIZE
	hCtrl = GetDlgItem(hDlg, IDE_SER_BITSIZE);
	for (int i = 0; i != 5; i++) SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)szBitSize[i]);
	SendMessage(hCtrl, CB_SETCURSEL, (WPARAM)4, (LPARAM)0);

	//初始化停止位 IDE_SER_STOP
	hCtrl = GetDlgItem(hDlg, IDE_SER_STOP);
	for (int i = 0; i != 3; i++) SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)szStopBit[i]);
	SendMessage(hCtrl, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	//初始化校验位 IDE_SER_CHECK
	hCtrl = GetDlgItem(hDlg, IDE_SER_CHECK);
	for (int i = 0; i != 5; i++) SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)szCheck[i]);
	SendMessage(hCtrl, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	//初始化说明 IDC_NOTICE
	set_notice(hDlg, IDS_TCPS);
}

void set_enable_serial(HWND hDlg, bool bEnable);
void set_enable_lnet(HWND hDlg, bool bEnable);
void set_enable_rnet(HWND hDlg, bool bEnable);
void set_enable_ping(HWND hDlg, bool bEnable);
void set_static_text(HWND hDlg)
{
	SetWindowText(GetDlgItem(hDlg, IDC_RIP), _TEXT("远端IP"));
	SetWindowText(GetDlgItem(hDlg, IDC_RPORT), _TEXT("远端端口"));
}
void on_click_TCPS(HWND hDlg)
{
	set_notice(hDlg, IDS_TCPS);
	set_enable_serial(hDlg, FALSE);
	set_enable_lnet(hDlg, TRUE);
	set_enable_rnet(hDlg, FALSE);
	set_enable_ping(hDlg, FALSE);
	set_static_text(hDlg);
}
void on_click_TCPC(HWND hDlg)
{
	set_notice(hDlg, IDS_TCPC);
	set_enable_serial(hDlg, FALSE);
	set_enable_lnet(hDlg, FALSE);
	set_enable_rnet(hDlg, TRUE);
	set_enable_ping(hDlg, FALSE);
	set_static_text(hDlg);
}
void on_click_UDP(HWND hDlg)
{
	set_notice(hDlg, IDS_UDP);
	set_enable_serial(hDlg, FALSE);
	set_enable_lnet(hDlg, TRUE);
	set_enable_rnet(hDlg, TRUE);
	set_enable_ping(hDlg, FALSE);
	set_static_text(hDlg);
}
void on_click_multicase(HWND hDlg)
{
	set_notice(hDlg, IDS_MULTICASE);
	set_enable_serial(hDlg, FALSE);
	set_enable_lnet(hDlg, TRUE);
	set_enable_rnet(hDlg, TRUE);
	set_enable_ping(hDlg, FALSE);
}
void on_click_ping(HWND hDlg)
{
	set_notice(hDlg, IDS_PING);
	set_enable_serial(hDlg, FALSE);
	set_enable_lnet(hDlg, FALSE);
	set_enable_rnet(hDlg, FALSE);
	set_enable_ping(hDlg, TRUE);
}
void on_click_serial(HWND hDlg)
{
	set_notice(hDlg, IDS_SERIAL);
	set_enable_serial(hDlg, TRUE);
	set_enable_lnet(hDlg, FALSE);
	set_enable_rnet(hDlg, FALSE);
	set_enable_ping(hDlg, FALSE);
}
void set_notice(HWND hDlg, int ID)
{
	TCHAR szNotice[512];
	LoadStringW((HINSTANCE)GetWindowLong(NULL, GWL_HINSTANCE), ID, szNotice, 512);
	SetWindowText(GetDlgItem(hDlg, IDC_NOTICE), szNotice);
}

void set_enable_serial(HWND hDlg, bool bEnable)
{
	EnableWindow(GetDlgItem(hDlg, IDE_SER_NO), bEnable);
	EnableWindow(GetDlgItem(hDlg, IDE_SER_BAUD), bEnable);
	EnableWindow(GetDlgItem(hDlg, IDE_SER_CHECK), bEnable);
	EnableWindow(GetDlgItem(hDlg, IDE_SER_BITSIZE), bEnable);
	EnableWindow(GetDlgItem(hDlg, IDE_SER_STOP), bEnable);
}
void set_enable_lnet(HWND hDlg, bool bEnable)
{
	EnableWindow(GetDlgItem(hDlg, IDE_LIP), bEnable);
	EnableWindow(GetDlgItem(hDlg, IDE_LPORT), bEnable);
}
void set_enable_rnet(HWND hDlg, bool bEnable)
{
	EnableWindow(GetDlgItem(hDlg, IDE_RIP), bEnable);
	EnableWindow(GetDlgItem(hDlg, IDE_RPORT), bEnable);
}
void set_enable_ping(HWND hDlg, bool bEnable)
{
	EnableWindow(GetDlgItem(hDlg, IDE_SIP), bEnable);
	EnableWindow(GetDlgItem(hDlg, IDE_EIP), bEnable);
	EnableWindow(GetDlgItem(hDlg, IDE_TIMEOUT), bEnable);
}