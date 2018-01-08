#include "stdafx.h"
#include "Co.h"
#include "utils.h"

LRESULT  CALLBACK CTU_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//显示TCP_UDP的控件,CTU就是 Controol TCP/UDP
HWND CTU(HWND hParent,int x, int y, int Width, int Higth)
{
	TCHAR* szClassName = _TEXT("CTU_CLASS");
	static ATOM sclass = {};
	if (!sclass)
	{
		HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);
		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = CTU_Proc;
		wcex.hInstance = hInstance;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_DESKTOP + 1);
		wcex.lpszClassName = szClassName;
		sclass = RegisterClassExW(&wcex);
		if (sclass = RegisterClassExW(&wcex)) return NULL;
	}
	int id = 0xBCDE;
	for (int i = 0; i != 500; i++, id++)
	{
		if (GetDlgItem(hParent, id) == NULL) break;
	}
	HWND hWnd = CreateWindowEx(0, szClassName,_TEXT("TCP/UDP测试"), WS_CHILD | WS_VISIBLE,
		x, y, Width, Higth, hParent, HMENU(id), hInst, NULL);
	return hWnd;
}
typedef struct __CTUS__
{
	HWND	hSpliter; //垂直的分隔条
	double	percent; //窗口分割的百分比
	HWND	hTree; //窗口左边TreeView Control
	HWND	hList; //窗口右边ListView Control
}CTUS;
void CTU_init(HWND hDlg);
LRESULT  CALLBACK CTU_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		CTU_init(hWnd);
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
	{
		CTUS *ctus = (CTUS*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		free(ctus);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
	}
		break;
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	return (INT_PTR)FALSE;
}

void CTU_init(HWND hWnd)
{
	CTUS* ctus = (CTUS*)malloc(sizeof(CTUS));
	if (ctus == NULL) return;

	SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(ctus));
	RECT rc;
	GetClientRect(hWnd, &rc);
	ctus->percent = 0.3;
	//ctus->hSpliter = Spliter(hWnd, TYPE_VSPLITER, int(ctus->percent * rc.right), 0, 5, rc.bottom);
	
}