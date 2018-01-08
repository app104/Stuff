//Dialog_new.cpp: �����½�ͨ���Ի������ڵ㡣
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
// ���½�ͨ���������Ϣ�������
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
			SetWindowText(GetDlgItem(hDlg, IDC_RIP), _TEXT("�鲥IP"));
			SetWindowText(GetDlgItem(hDlg, IDC_RPORT), _TEXT("�鲥�˿�"));
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
	//�Ӵ��ھ���
	Dialog_center(hDlg);

	HWND hCtrl;

	//����TCP ServerΪĬ��ѡ�� IDR_TCPS
	SendMessage(GetDlgItem(hDlg, IDR_TCPS), BM_SETCHECK, (WPARAM)BST_CHECKED, (LPARAM)0);

	//��ʼ�� ����IP IDE_LIP, ����Ϊ��ȡ����IP�б�, ��д��IDE_LIP
	hCtrl = GetDlgItem(hDlg, IDE_LIP);
	PMIB_IPADDRTABLE pIPAddrTable;
	ULONG dwSize = 0; //IP�б�Ĵ�С
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
	IN_ADDR IPAddr;     // ����IP��ַ�Ľṹ��
	IPAddr.S_un.S_addr = htonl(INADDR_ANY); //0.0.0.0
	InetNtop(AF_INET, (PVOID)&IPAddr, szIP, 64);
	SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)szIP); //��Combo Box�������Ŀ
	for (int i = 0; i < (int)pIPAddrTable->dwNumEntries; i++)
	{
		IPAddr.S_un.S_addr = (u_long)pIPAddrTable->table[i].dwAddr;
		InetNtop(AF_INET, (PVOID)&IPAddr, szIP, 64);
		SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)szIP);//��Combo Box�������Ŀ

	}
	free(pIPAddrTable);
	SendMessage(hCtrl,CB_SETCURSEL,(WPARAM)0, (LPARAM) 0);  //�ѵ�(WPARAM)0����ΪCombo Box��Ĭ��ֵ��ʾ

	//��ʼ�� ����IP IDE_RIP
	hCtrl = GetDlgItem(hDlg, IDE_RIP);
	SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)_TEXT("0.0.0.0"));
	SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)_TEXT("127.0.0.1"));
	SendMessage(hCtrl, CB_SETCURSEL, (WPARAM)1, (LPARAM)0);  //�ѵ�(WPARAM)0����ΪCombo Box��Ĭ��ֵ��ʾ

	//��ʼ������PORT IDE_LPORT
	SetWindowText(GetDlgItem(hDlg, IDE_LPORT), _TEXT("2404"));
	//��ʼ��Զ��PORT IDE_RPORT
	SetWindowText(GetDlgItem(hDlg, IDE_RPORT), _TEXT("2404"));

	//��ʼ����ʼIP IDE_SIP
	SendMessage(GetDlgItem(hDlg, IDE_SIP), IPM_SETADDRESS, (WPARAM)0, (LPARAM)MAKEIPADDRESS(192,168,1,0));

	//��ʼ����ʼIP IDE_EIP
	SendMessage(GetDlgItem(hDlg, IDE_EIP), IPM_SETADDRESS, (WPARAM)0, (LPARAM)MAKEIPADDRESS(192, 168, 1, 255));

	//��ʼ����ʱ IDE_TIMEOUT
	SetWindowText(GetDlgItem(hDlg, IDE_TIMEOUT), _TEXT("5000"));


	//��ʼ�����ں� IDE_SER_NO
	hCtrl = GetDlgItem(hDlg, IDE_SER_NO);
	TCHAR szPortName[64];
	for (int iCnt = 1; iCnt != 256; iCnt++) { //��ʼ�����ں�ѡ�� ComboBoxEx
		wsprintf(szPortName, _TEXT("\\\\.\\COM%d"), iCnt);
		HANDLE hComm = CreateFile(szPortName,//���ں�
			GENERIC_READ | GENERIC_WRITE,//�����д
			0,//ͨѶ�豸�����Զ�ռ��ʽ��
			NULL,//�ް�ȫ����
			OPEN_EXISTING,//�򿪶����Ǵ���
			0,//ͬ��I/O
			NULL);//ͨѶ�豸������ģ��� 
		if (hComm == INVALID_HANDLE_VALUE) continue; //���ڴ�ʧ��, �޴˴���
		CloseHandle(hComm);
		SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)&szPortName[4]);
	}
	SendMessage(hCtrl, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	//��ʼ�� IDE_SER_BAUD IDE_SER_START IDE_SER_STOP IDE_SER_CHECK IDE_SER_BITSIZE
	LPTSTR szBaud[] = { 
		L"110", L"300", L"600", L"1200",
		L"2400", L"4800", L"9600",  L"14400",  
		L"19200", L"38400", L"56000",  L"57600",
		L"115200",  L"128000",  L"256000" }; //15�� ����������
	LPTSTR szBitSize[] = { L"4λ",  L"5λ", L"6λ",  L"7λ", L"8λ" }; //5�� ����λ
	LPTSTR szStopBit[] = { L"1ֹͣλ",  L"1.5ֹͣλ", L"2ֹͣλ" }; //3�� ֹͣλ
	LPTSTR szCheck[] = { L"��У��",  L"��У��", L"żУ��", L"MarkУ��", L"SpaceУ��" }; // 5�� У������

	//��ʼ�������� IDE_SER_BAUD
	hCtrl = GetDlgItem(hDlg, IDE_SER_BAUD);
	for(int i =0;i!=15;i++) SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)szBaud[i]);
	SendMessage(hCtrl, CB_SETCURSEL, (WPARAM)6, (LPARAM)0);

	//��ʼ������λ IDE_SER_BITSIZE
	hCtrl = GetDlgItem(hDlg, IDE_SER_BITSIZE);
	for (int i = 0; i != 5; i++) SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)szBitSize[i]);
	SendMessage(hCtrl, CB_SETCURSEL, (WPARAM)4, (LPARAM)0);

	//��ʼ��ֹͣλ IDE_SER_STOP
	hCtrl = GetDlgItem(hDlg, IDE_SER_STOP);
	for (int i = 0; i != 3; i++) SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)szStopBit[i]);
	SendMessage(hCtrl, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	//��ʼ��У��λ IDE_SER_CHECK
	hCtrl = GetDlgItem(hDlg, IDE_SER_CHECK);
	for (int i = 0; i != 5; i++) SendMessage(hCtrl, CB_ADDSTRING, (WPARAM)0, (LPARAM)szCheck[i]);
	SendMessage(hCtrl, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	//��ʼ��˵�� IDC_NOTICE
	set_notice(hDlg, IDS_TCPS);
}

void set_enable_serial(HWND hDlg, bool bEnable);
void set_enable_lnet(HWND hDlg, bool bEnable);
void set_enable_rnet(HWND hDlg, bool bEnable);
void set_enable_ping(HWND hDlg, bool bEnable);
void set_static_text(HWND hDlg)
{
	SetWindowText(GetDlgItem(hDlg, IDC_RIP), _TEXT("Զ��IP"));
	SetWindowText(GetDlgItem(hDlg, IDC_RPORT), _TEXT("Զ�˶˿�"));
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