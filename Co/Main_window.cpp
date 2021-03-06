#include "stdafx.h"
#include "Co.h"
#include "utils.h"
#include "strsafe.h"

LRESULT CALLBACK	Main_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    About_proc(HWND, UINT, WPARAM, LPARAM);

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL Main_init(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // 将实例句柄存储在全局变量中

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Main_proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CO));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CO);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDC_CO));

	RegisterClassExW(&wcex);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  函数: Main_proc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK Main_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//static HWND hSiderbar = NULL;
	switch (message)
	{
	case WM_CREATE:
	{
		RECT rc;
		//GetClientRect(hWnd, &rc);
		//SB_ELEM se[] = {
		//	{ 2048,_TEXT("系统"),0,LoadIcon(hInst,MAKEINTRESOURCE(IDI_SYS)) ,SIZE{}},
		//	{ 2049,_TEXT("工程"),0,LoadIcon(hInst,MAKEINTRESOURCE(IDI_PROJ)),SIZE{}},
		//	{ 2050,_TEXT("配置"),0,LoadIcon(hInst,MAKEINTRESOURCE(IDI_CONFIG)),SIZE{}},
		//};
		//Sidebar(hWnd,20488, TYPE_SBLEFT,se,3,0,0,rc.bottom);

		//rc.left = SIDEBAR_WIDTH;
		//rc.right = 300;
		//PANE_ELEM ppa[] = {
		//	{ _TEXT("系统"),IDD_PROJ, NULL,PaneExample_Proc, NULL,RECT{} },
		//	{ _TEXT("工程"),IDD_PROJ, NULL,PaneExample_Proc, NULL,RECT{} },
		//	{ _TEXT("配置"),IDD_PROJ, NULL,PaneExample_Proc, NULL,RECT{} }, 
		//};
		//Pane(hWnd, 20489, ppa, 3, &rc, PANE_SP_RIGHT);
		//Pane_setsb(GetDlgItem(hWnd, 20489), GetDlgItem(hWnd, 20488));
		//ShowWindow(GetDlgItem(hWnd, 20489), SW_HIDE);
		GetClientRect(hWnd, &rc);
		Editor(hWnd, 20490,_TEXT("D:\\hex.c"), &rc);

		return TRUE;
	}
	case MSG_SBBUTTON:
	if(wParam){
		ShowWindow(GetDlgItem(hWnd, 20489), SW_SHOW);
	}
		break;
	case WM_SIZE:
	{
		//Sidebar_len(GetDlgItem(hWnd, 20488), HIWORD(lParam));
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_CMD:
			//Sidebar_timeout(GetDlgItem(hWnd, 20488), 3000);
			//Sidebar_pos(GetDlgItem(hWnd, 20488), 5, 10);
			break;
		case IDM_IMPUT:
		{
			TCHAR Text[256] = {};
			InputBox(hWnd, Text, 256);
			MessageBox(hWnd, Text, _TEXT("已输入的内容"), MB_OK);
			break;
		}
		case IDM_NEW_CHANNEL:
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_NEW_CHANNEL), hWnd, Dialog_new_proc);
			break;
		}
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About_proc);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...

		//RECT rc = { 400,400,528,528 };
		//DrawFrameControl(hdc, &rc, DFC_MENU, DFCS_MENUCHECK);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		Dialog_center(hDlg);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


