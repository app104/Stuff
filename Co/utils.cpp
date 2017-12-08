#include "utils.h"

//������InputBox ����������
#ifdef _WIN64
#define GWL_HINSTANCE GWLP_HINSTANCE
#define DWL_DLGPROC     DWLP_DLGPROC
#endif

LRESULT  CALLBACK InputBox_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

typedef struct _INPUTBOX_USE_ {
	int type;
	void* buf;
	int  buf_len;
	TCHAR* lpCaption;
}INPUTBOX_USE;

int InputBox(HWND hWnd, void* buf, int len,int Type, TCHAR* lpCaption , int Width, int Higth)
{
	if ((Type == INPUT_INT && len < sizeof(long)) || (Type == INPUT_DOUBLE && len < sizeof(double))) return -1;
	INPUTBOX_USE ibu = {};
	ibu.type = Type;
	ibu.buf = buf;
	ibu.lpCaption = lpCaption;
	ibu.buf_len = len;

	RECT rect;
	GetWindowRect(hWnd, &rect);
	int x = (rect.right - rect.left - Width) / 2 + rect.left;
	int y = (rect.bottom - rect.top - Higth) / 2 + rect.top;
	if (Width < 300) Width = 300;
	if (Higth < 100) Higth = 100;
	HWND hChild = CreateWindowEx(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,
		_TEXT("#32770"),
		lpCaption,
		WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU,
		x > 0 ? x : 0,
		y > 0 ? y : 0,
		Width,
		Higth,
		hWnd,
		NULL,
		(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
		NULL/*LPVOID(&cs)*/);
	if (!hChild) return 0;
	EnableWindow(hWnd, FALSE); //���ø�����

	SetWindowLong(hChild, DWL_DLGPROC, (LONG)InputBox_Proc); //�����Ӵ��ڵĻص�����
	SendMessage(hChild, WM_INITDIALOG, 0, LPARAM(&ibu)); //���ͳ�ʼ������

	MSG msg;
	BOOL bRet;
	// ����Ϣѭ��:
	// GetMessage()�յ�WM_QUIT ����0, �������ط�0, ������-1
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1) //����
		{
			msg.wParam = 0;
			break;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	EnableWindow(hWnd, TRUE); //���ø�����
	SetFocus(hWnd);
	return (int)msg.wParam;
}

LRESULT  CALLBACK InputBox_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//UNREFERENCED_PARAMETER(lParam);
	static INPUTBOX_USE * pibu = NULL;
	switch (message)
	{
	case WM_INITDIALOG:
	{
		pibu = ((INPUTBOX_USE*)lParam);
		RECT rect;
		GetClientRect(hWnd, &rect);
		DWORD dwStyle = WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | ES_LEFT | ES_AUTOHSCROLL | ES_WANTRETURN ;
		switch (pibu->type)
		{
		case INPUT_INT:
		case INPUT_DOUBLE:
			dwStyle |= ES_NUMBER;
			break;
		case INPUT_PASSWORD:
			dwStyle |= ES_PASSWORD;
		case INPUT_UPPERCASE:
			dwStyle |= ES_UPPERCASE;
		case INPUT_LOWERCASE:
			dwStyle |= ES_LOWERCASE;
			break;
		default:
			break;
		}

		CreateWindowEx(WS_EX_NOPARENTNOTIFY | WS_EX_CLIENTEDGE,
			L"Edit",
			_TEXT("������!"),
			dwStyle,
			0,
			0,
			rect.right,
			rect.bottom  - 20,
			hWnd,
			HMENU(3),
			NULL,
			NULL);

		CreateWindowEx(WS_EX_NOPARENTNOTIFY,
			L"BUTTON",
			L"ȷ��",
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,
			rect.right - 180,
			rect.bottom - 20,
			80,
			20,
			hWnd,
			HMENU(IDOK),
			NULL,
			NULL);
		CreateWindowEx(WS_EX_NOPARENTNOTIFY,
			L"BUTTON",
			L"ȡ��",
			WS_CHILDWINDOW | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_TEXT,
			rect.right -80,
			rect.bottom - 20,
			80,
			20,
			hWnd,
			HMENU(IDCANCEL),
			NULL,
			NULL);
	}
	return (INT_PTR)TRUE;
	case WM_COMMAND:
		// �����˵�ѡ��:
		switch (LOWORD(wParam))
		{
		case 3:
			if (HIWORD(wParam) == EN_SETFOCUS)
			{
				SetWindowText((HWND)lParam, L"");
			}
			break;
		case IDOK:
			if (pibu->type == INPUT_INT || pibu->type == INPUT_DOUBLE)
			{
				TCHAR lpText[256] = {};
				pibu->buf_len = Edit_GetText(GetDlgItem(hWnd, 3), lpText, 255);
				if (pibu->type == INPUT_INT)
				{
					_stscanf_s(lpText,_TEXT("%ld"), (long*)(pibu->buf));
				}
				else
				{
					_stscanf_s(lpText, _TEXT("%lf"), (double*)(pibu->buf));
				}
				
			}
			else
			{
				pibu->buf_len = Edit_GetText(GetDlgItem(hWnd, 3), (TCHAR*)(pibu->buf), pibu->buf_len -1);
			}
			DestroyWindow(hWnd);
			break;
		case IDCANCEL:
			pibu->buf_len = 0;
			DestroyWindow(hWnd);
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(pibu->buf_len);
		break;
	}
	return (INT_PTR)FALSE;
}

//������InputBox����������

//������vertical spliter ��ֱ�ָ���
typedef struct __SPLITER_STRUCT_
{
	HWND    hParent; //�������
	int		type; //����, ������ˮƽ�Ļ��Ǵ�ֱ��
	POINT	oldPos; //����������ʱ�ڷָ����ϵ�λ��.
	POINT	LTPos; //�ָ������Ͻ��ڸ������е�����
	RECT    rc; //�Ӵ��ڵĴ�С
	POINT   pPos;//���������½ǵĵ��λ��,���ڲ��÷ָ����Ƴ������� ����������ϵ
	int     initialized; //��ʼ��
}SPLITER_STRUCT;

LRESULT  CALLBACK Spliter_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND Spliter(HWND hParent,int type, int x, int y, int Width, int Higth)
{
	if (type & 0xFFFFFFFE) return NULL; //type ����0��1 �ͷ���NULL
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);
	TCHAR szClassName[][32] = { _TEXT("SPLITER_VCLASSNAME"), _TEXT("SPLITER_HCLASSNAME") };
	static ATOM sclass[2] = {};
	if (!sclass[type])
	{
		WNDCLASSEXW wcex={};
		
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Spliter_Proc;
		wcex.hInstance = hInst;
		wcex.hCursor = LoadCursor(nullptr, type ? IDC_SIZENS : IDC_SIZEWE);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME + 1);
		wcex.lpszClassName = szClassName[type];
		sclass[type] = RegisterClassExW(&wcex);
		if (sclass[type] = RegisterClassExW(&wcex)) return NULL;
	}
	//��һ�����õĿؼ�ID
	int id = 0xABCD;
	for (int i = 0; i != 500; i++, id++)
	{
		if (GetDlgItem(hParent, id) == NULL) break;
	}

	SPLITER_STRUCT* ss = (SPLITER_STRUCT*)malloc(sizeof(SPLITER_STRUCT));
	memset(ss, 0, sizeof(SPLITER_STRUCT));
	if (ss == NULL) return NULL;
	ss->hParent = hParent;
	ss->type = type;
	ss->LTPos.x = x;
	ss->LTPos.y = y;
	ss->rc.right = Width;
	ss->rc.bottom = Higth;
	RECT prc;
	GetClientRect(hParent, &(prc));
	ss->pPos.x = prc.right - Width; //����������ϵ, ���½ǵĵ�
	ss->pPos.y = prc.bottom - Higth;

	return CreateWindowEx(0, szClassName[type], NULL, WS_CHILD | WS_VISIBLE,
		x, y, Width, Higth, hParent, HMENU(id), hInst, (LPVOID(ss)));
}

LRESULT  CALLBACK Spliter_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		SPLITER_STRUCT* ss = (SPLITER_STRUCT*)((LPCREATESTRUCT(lParam))->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(ss));
		//SPLITER_STRUCT* ss = (SPLITER_STRUCT*)malloc(sizeof(SPLITER_STRUCT));
		//if (ss == NULL) return NULL;
		//ss->hParent = GetParent(hWnd);
		//ss->type = (int)(((CREATESTRUCT*)lParam)->lpCreateParams);
		//GetClientRect(hWnd, &(ss->rc));
		//SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(ss));
		return (INT_PTR)TRUE;
	}
	case MSG_SPLITERSTART:
	{
		SPLITER_STRUCT* ss = (SPLITER_STRUCT*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (ss->type) //���type��0���Ǵ�ֱ�ָ�, ������ˮƽ�ָ�
		{ //��ˮƽ�ָ�, ˮƽ��Y���겻�ܶ�
			
			SetWindowPos(hWnd, NULL, \
				(int)wParam, \
				ss->LTPos.y, \
				ss->rc.right + ss->LTPos.x - (int)wParam, \
				ss->rc.bottom, \
				SWP_NOZORDER);
			ss->LTPos.x = (int)wParam;
			GetClientRect(hWnd, &(ss->rc));
		}
		else //��ֱ�ָ�
		{
			SetWindowPos(hWnd, NULL, \
				ss->LTPos.x, \
				(int)wParam, \
				ss->rc.right, \
				ss->rc.bottom + ss->LTPos.y - (int)wParam, \
				SWP_NOZORDER);
			ss->LTPos.y = (int)wParam;
			GetClientRect(hWnd, &(ss->rc));
		}
		break;
	}
	case MSG_SPLITEREND:
	{
		SPLITER_STRUCT* ss = (SPLITER_STRUCT*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (ss->type) //���type��0���Ǵ�ֱ�ָ�, ������ˮƽ�ָ�
		{ //��ˮƽ�ָ�, ˮƽ��Y���겻�ܶ�
			SetWindowPos(hWnd, NULL, \
				ss->LTPos.x, \
				ss->LTPos.y, \
				(int)wParam, \
				ss->rc.bottom, \
				SWP_NOZORDER);
			GetClientRect(hWnd, &(ss->rc));
		}
		else //��ֱ�ָ�
		{
			SetWindowPos(hWnd, NULL, \
				ss->LTPos.x, \
				ss->LTPos.y, \
				ss->rc.right, \
				(int)wParam, \
				SWP_NOZORDER);
			GetClientRect(hWnd, &(ss->rc));
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		SPLITER_STRUCT* ss = (SPLITER_STRUCT*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		GetCursorPos(&(ss->oldPos));
		ss->LTPos.x = 0;
		ss->LTPos.y = 0;
		MapWindowPoints(hWnd, ss->hParent, &(ss->LTPos), 1);//�ӱ��ؼ�������ӳ�䵽����������
		SetCapture(hWnd);
		break;
	}
	case WM_LBUTTONUP:
	case WM_NCLBUTTONUP:
	{
		SPLITER_STRUCT* ss = (SPLITER_STRUCT*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		ss->LTPos.x = 0;
		ss->LTPos.y = 0;
		MapWindowPoints(hWnd, ss->hParent, &(ss->LTPos), 1);//�ӱ��ؼ�������ӳ�䵽����������
		ReleaseCapture();
		break;
	}
	case WM_MOUSEMOVE:
	case WM_NCMOUSEMOVE:
	{
		if(wParam == MK_LBUTTON)
		{
			SPLITER_STRUCT* ss = (SPLITER_STRUCT*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			POINT	p= {};
			GetCursorPos(&p);
			if (ss->type) //���type��0���Ǵ�ֱ�ָ�, ������ˮƽ�ָ�
			{ //��ˮƽ�ָ�, ˮƽ��Y���겻�ܶ�
				int shift = p.y - ss->oldPos.y;
				int pp = ss->LTPos.y + shift;
				if (pp > ss->pPos.y || pp < ss->rc.bottom) break;
				SetWindowPos(hWnd, NULL, \
					ss->LTPos.x, \
					pp, \
					ss->rc.right, \
					ss->rc.bottom, \
					SWP_NOZORDER);
				SendMessage(ss->hParent, MSG_HSPLITER, WPARAM(shift), (LPARAM)hWnd);
			}
			else //��ֱ�ָ�
			{

				int shift = p.x - ss->oldPos.x;
				int qq = ss->LTPos.x + shift;
				if (qq > ss->pPos.x || qq < ss->rc.right) break;
				SetWindowPos(hWnd, NULL, \
					qq, \
					ss->LTPos.y, \
					ss->rc.right, \
					ss->rc.bottom, \
					SWP_NOZORDER);
				SendMessage(ss->hParent, MSG_VSPLITER, WPARAM(shift), (LPARAM)hWnd);
			}
		}
		break;
	}
	case WM_DESTROY:
	{
		SPLITER_STRUCT* ss = (SPLITER_STRUCT*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (ss) free(ss);
		break;
	}

	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return (INT_PTR)FALSE;
}
