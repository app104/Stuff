#include "utils.h"

//以下是InputBox 的所有内容
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

int InputBox(HWND hWnd, void* buf, int len,int Type, TCHAR* lpCaption , int Width, int Heigth)
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
	int y = (rect.bottom - rect.top - Heigth) / 2 + rect.top;
	if (Width < 300) Width = 300;
	if (Heigth < 100) Heigth = 100;
	HWND hChild = CreateWindowEx(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CONTROLPARENT,
		_TEXT("#32770"),
		lpCaption,
		WS_CAPTION | WS_POPUP | WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU,
		x > 0 ? x : 0,
		y > 0 ? y : 0,
		Width,
		Heigth,
		hWnd,
		NULL,
		(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
		NULL/*LPVOID(&cs)*/);
	if (!hChild) return 0;
	EnableWindow(hWnd, FALSE); //禁用父窗口

	SetWindowLong(hChild, DWL_DLGPROC, (LONG)InputBox_Proc); //设置子窗口的回调函数
	SendMessage(hChild, WM_INITDIALOG, 0, LPARAM(&ibu)); //发送初始化命令

	MSG msg;
	BOOL bRet;
	// 主消息循环:
	// GetMessage()收到WM_QUIT 返回0, 其它返回非0, 出错返回-1
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bRet == -1) //出错
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
	EnableWindow(hWnd, TRUE); //启用父窗口
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
			_TEXT("Edit"),
			_TEXT("请输入!"),
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
			_TEXT("BUTTON"),
			_TEXT("确定"),
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
			_TEXT("BUTTON"),
			_TEXT("取消"),
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
		// 分析菜单选择:
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
//以上是InputBox的所有内容

//以下是vertical spliter 垂直分隔条
typedef struct __SPLITER_STRUCT_
{
	HWND    hParent; //父窗口，接收Spliter消息的窗口句柄
	int		type; //类型, 是水平的或是垂直的
	POINT	LBDownPos; //鼠标在分隔条上左键点击时的鼠标位置，屏幕坐标系.
	int		rangeA;
	int		rangeB;
	HDC		hdcDesktop;
	HPEN    hPen;
	int		szFrame; //父窗口边框的宽度
}SPLITER;

LRESULT  CALLBACK Spliter_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND Spliter(HWND hParent, int ID, int type, int rangeA, int rangeB, int posX, int posY, int len)
{
	if (type & 0xFFFFFFFE) return NULL; //type 不是0或1 就返回NULL
 
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);
	TCHAR szClassName[][32] = { _TEXT("SPLITER_VCLASS"), _TEXT("SPLITER_HCLASS") };
	SPLITER* ss = (SPLITER*)malloc(sizeof(SPLITER));
	memset(ss, 0, sizeof(SPLITER));
	if (ss == NULL) return NULL;
	ss->hParent = hParent;
	ss->type = type;
	ss->rangeA = rangeA;
	ss->rangeB = rangeB;

	ss->hPen = CreatePen(PS_SOLID, SPLITER_WIDTH, RGB(50, 50, 50));
	static ATOM sclass[2] = {};
	if (!sclass[type])
	{
		WNDCLASSEXW wcex={};
		
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Spliter_Proc;
		wcex.hInstance = hInst;
		wcex.hCursor = LoadCursor(NULL, ss->type ? IDC_SIZENS : IDC_SIZEWE);
		wcex.hbrBackground = (HBRUSH)(COLOR_GRAYTEXT + 1);
		wcex.lpszClassName = szClassName[ss->type];
		sclass[type] = RegisterClassExW(&wcex);
		if (sclass[type] = RegisterClassExW(&wcex)) return NULL;
	}

	RECT prc;
	GetClientRect(hParent, &(prc));
	HWND hCtrl = CreateWindowEx(0, szClassName[ss->type], NULL, WS_CHILD | WS_VISIBLE,
		posX, posY,((type) ? len : SPLITER_WIDTH), ((type)? SPLITER_WIDTH:len) , 
		hParent, HMENU(ID), hInst, (LPVOID(ss)));
	if (hCtrl) return hCtrl;
	free(ss);
	return NULL;
}

LRESULT  CALLBACK Spliter_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		SPLITER* ss = (SPLITER*)((LPCREATESTRUCT(lParam))->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(ss));
		//计算父窗口边框的宽度
		RECT rcC, rcW;
		GetClientRect(ss->hParent, &rcC);
		GetWindowRect(ss->hParent, &rcW);
		ss->szFrame = (rcW.right - rcW.left - rcC.right) / 2;
		//规范rangeA和rangeB的范围
		if (ss->rangeB == -1) ss->rangeB = (ss->type)? rcC.bottom: rcC.right;
		return (INT_PTR)TRUE;
	}
	case WM_LBUTTONDOWN:
	if(MK_LBUTTON == wParam)
	{
		SPLITER* ss = (SPLITER*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		GetCursorPos(&(ss->LBDownPos));
		SetCapture(hWnd);
	}
	break;
	case WM_LBUTTONUP:
	case WM_NCLBUTTONUP:
	{
		SPLITER* ss = (SPLITER*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		POINT p0 = {};
		MapWindowPoints(hWnd, ss->hParent, &p0, 1);//从本控件的坐标映射到父窗口坐标
		ReleaseCapture();
		POINT mp = {};  //鼠标左键松开时的坐标， 屏幕坐标系
		GetCursorPos(&mp);
		RECT rcC;
		GetClientRect(hWnd, &rcC);
		if (ss->type) //如果type是0就是垂直分隔, 否则是水平分隔
		{ //是水平分隔, 水平的Y坐标不能动
			int shift = mp.y - ss->LBDownPos.y;
			int pp = p0.y + shift;
			if ((pp >= ss->rangeA) && (pp <= ss->rangeB))
			{
				SetWindowPos(hWnd, NULL, \
					p0.x, \
					pp, \
					rcC.right, \
					rcC.bottom, \
					SWP_NOZORDER);
				PostMessage(ss->hParent, MSG_HSPLITER, WPARAM(shift), (LPARAM)hWnd);
			}
		}
		else //垂直分隔
		{

			int shift = mp.x - ss->LBDownPos.x;
			int qq = p0.x + shift;
			if ((qq >= ss->rangeA) && (qq <= ss->rangeB))
			{
				SetWindowPos(hWnd, NULL, \
					qq, \
					p0.y, \
					rcC.right, \
					rcC.bottom, \
					SWP_NOZORDER);
				PostMessage(ss->hParent, MSG_VSPLITER, WPARAM(shift), (LPARAM)hWnd);
			}

		}
		InvalidateRect(NULL, NULL, TRUE);//强制整个分隔条重绘
		break;
	}
	case WM_MOUSEMOVE:
	case WM_NCMOUSEMOVE:
	{
		if (wParam == MK_LBUTTON)
		{
			SPLITER* ss = (SPLITER*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			POINT p;
			GetCursorPos(&p);
			
			RECT rc;
			GetWindowRect(hWnd, &rc);//获取分隔条在屏幕坐标系的位置
			InvalidateRect(NULL, &rc, TRUE);//强制整个父窗口重绘
			ss->hdcDesktop = GetDC(NULL); //获取屏幕HDC
			HGDIOBJ hOld = SelectObject(ss->hdcDesktop, ss->hPen);
			if (ss->type) // 水平分隔
			{
				POINT pf[2] = { {0, ss->rangeA },{0, ss->rangeB} };
				MapWindowPoints(ss->hParent, NULL, pf, 2);//从父窗口坐标映射到屏幕坐标系
				if (p.y < pf[0].y) p.y = pf[0].y; else if (p.y > pf[1].y) p.y = pf[1].y;
				MoveToEx(ss->hdcDesktop, rc.left + 2, p.y, NULL);
				LineTo(ss->hdcDesktop, rc.right - 2, p.y);
			}
			else// 垂直分隔
			{
				POINT pf[2] = { { ss->rangeA,0 },{ ss->rangeB,0} };
				MapWindowPoints(ss->hParent, NULL , pf, 2);//从父窗口坐标映射到屏幕坐标系
				if (p.x < pf[0].x) p.x = pf[0].x; else if (p.x > pf[1].x) p.x = pf[1].x;
				MoveToEx(ss->hdcDesktop, p.x, rc.top+2, NULL); //屏幕坐标系
				LineTo(ss->hdcDesktop, p.x, rc.bottom-2);

			}
			SelectObject(ss->hdcDesktop, hOld);
			ReleaseDC(NULL, ss->hdcDesktop);//释放屏幕HDC
		}
		break;
	}
	case MSG_SPLITERPOS:
	{
		SPLITER* ss = (SPLITER*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		RECT rcC;
		POINT p0 = {};
		MapWindowPoints(hWnd, ss->hParent, &p0, 1);//从本控件的坐标映射到父窗口坐标
		GetClientRect(hWnd, &rcC);
		if (ss->type) //如果type是0就是垂直分隔, 否则是水平分隔
		{ //是水平分隔, 水平的Y坐标不能动
			SetWindowPos(hWnd, NULL, \
				(int)wParam, \
				p0.y, \
				(int)lParam - (int)wParam , \
				rcC.bottom, \
				SWP_NOZORDER);
		}
		else //垂直分隔
		{
			SetWindowPos(hWnd, NULL, \
				p0.x, \
				(int)wParam, \
				rcC.right, \
				(int)lParam - (int)wParam , \
				SWP_NOZORDER);
		}
		break;
	}
	case MSG_SPLITERSTART:
	{
		SPLITER* ss = (SPLITER*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		RECT rcC;
		POINT p0 = {};
		MapWindowPoints(hWnd, ss->hParent, &p0, 1);//从本控件的坐标映射到父窗口坐标
		GetClientRect(hWnd, &rcC);
		if (ss->type) //如果type是0就是垂直分隔, 否则是水平分隔
		{ //是水平分隔, 水平的Y坐标不能动
			SetWindowPos(hWnd, NULL, \
				(int)wParam, \
				p0.y, \
				rcC.right + p0.x - (int)wParam, \
				rcC.bottom, \
				SWP_NOZORDER);
		}
		else //垂直分隔
		{
			SetWindowPos(hWnd, NULL, \
				p0.x, \
				(int)wParam, \
				rcC.right, \
				rcC.bottom + p0.y - (int)wParam, \
				SWP_NOZORDER);
		}
		break;
	}
	case MSG_SPLITEREND:
	{
		SPLITER* ss = (SPLITER*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		RECT rcC;
		GetClientRect(hWnd, &rcC);
		POINT p0 = {};
		MapWindowPoints(hWnd, ss->hParent, &p0, 1);//从本控件的坐标映射到父窗口坐标
		if (ss->type) //如果type是0就是垂直分隔, 否则是水平分隔
		{ //是水平分隔, 水平的Y坐标不能动
			SetWindowPos(hWnd, NULL, \
				p0.x, \
				p0.y, \
				rcC.right - p0.x +(int)wParam, \
				rcC.bottom, \
				SWP_NOZORDER);
		}
		else //垂直分隔
		{
			SetWindowPos(hWnd, NULL, \
				p0.x, \
				p0.y, \
				rcC.right, \
				rcC.bottom - p0.y + (int)wParam, \
				SWP_NOZORDER);
		}
		break;
	}
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
		SPLITER* ss = (SPLITER*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (ss) free(ss);
		SetWindowLongPtr(hWnd, GWLP_USERDATA,NULL);
		break;
	}

	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return (INT_PTR)FALSE;
}


//以下是侧边栏

typedef struct __SIDEBAR_STRUCT__
{
	HINSTANCE hInst;
	int		type; //侧边栏类型
	int		posX; //左上角点坐标X, 父窗口坐标系
	int		posY; //左上角点坐标Y, 父窗口坐标系
	int		len;  //长度,水平的就是宽度, 垂直的就是高度
	int     ctrlLen; //控件需要的长度
	int		ctrlShow; //控件显示开始的位置
	int		ctrlID; //正在显示的按钮的ID
	HFONT	hFont; //字体
	int		fontHeigth;
	SB_ELEM* pse; //指向按钮元素
	int     num;  //按钮元素的数量
	HWND	hParent;
	HWND	hReceive; //消息接收句柄
	int		timeout;
	UINT_PTR timeID;
	HDC		hdc;     //
	HBITMAP hBitmap; //
	HBRUSH  hBrush;
	HBRUSH  hBrush2;
	HWND	hScroll;
}SIDEBAR;
LRESULT  CALLBACK Sidebar_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND Sidebar(HWND hParent,int ID, int type, SB_ELEM* pse, int num, int posX, int posY, int len)
{
	SB_ELEM* pse_temp = (SB_ELEM*)malloc(sizeof(SB_ELEM)*num);
	if (pse_temp == NULL) return NULL;
	memcpy(pse_temp, pse,  sizeof(SB_ELEM)* num);
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);
	TCHAR* szClassName = _TEXT("SIDEBAR_CLASS");
	static ATOM sclass = 0;
	if (!sclass)
	{
		WNDCLASSEXW wcex={};		
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Sidebar_Proc;
		wcex.hInstance = hInst;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME + 1); //注释这行，防止闪铄
		wcex.lpszClassName = szClassName;
		sclass = RegisterClassExW(&wcex);
		if (sclass = RegisterClassExW(&wcex)) return NULL;
	}
	
	SIDEBAR* sbs = (SIDEBAR*)malloc(sizeof(SIDEBAR));
	if (sbs == NULL) return NULL;

	memset(sbs, 0, sizeof(SIDEBAR));//不能对vector类用memset初始化
	sbs->hParent = hParent;
	sbs->hReceive = hParent;
	sbs->hInst = hInst;
	sbs->type = type;
	sbs->posX = posX;
	sbs->posY = posY;
	sbs->len = len;
	sbs->pse = pse_temp;
	sbs->num = num;
	sbs->ctrlID = -1;
	RECT rcP; //父窗口大小
	GetClientRect(hParent, &rcP);
	HWND hSiderbar = CreateWindowEx(0, szClassName, NULL, WS_CHILD | WS_VISIBLE,\
		posX, posY,\
		((type & 0x0FFFFFFFE) ? len: SIDEBAR_WIDTH), ((type & 0xFFFFFFFE)?SIDEBAR_WIDTH:len),\
		hParent, HMENU(ID), hInst, (LPVOID)sbs);
	if (hSiderbar) return hSiderbar;
	delete sbs;
	return NULL;
}
void SpiterPaint(HWND hWnd, SIDEBAR* sbs);
LRESULT  CALLBACK Sidebar_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		SIDEBAR* sbs = (SIDEBAR*)((LPCREATESTRUCT(lParam))->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(sbs));

		HDC hdc = GetDC(hWnd);
		sbs->hdc = CreateCompatibleDC(hdc);
		sbs->hBitmap = CreateCompatibleBitmap(hdc, 100, SIDEBAR_WIDTH);
		//查找字体高度
		TEXTMETRIC ptm = {};
		GetTextMetrics(hdc, &ptm);
		sbs->fontHeigth = ptm.tmHeight;
		sbs->hFont = CreateFont(
			0/*默认高度*/, 0/*默认宽度*/, -900/*字体顺时针转90度*/, 0/*不用管*/, FW_NORMAL /*一般这个值设为400*/,
			FALSE/*不带斜体*/, FALSE/*不带下划线*/, FALSE/*不带删除线*/,
			DEFAULT_CHARSET, //这里我们使用默认字符集，还有其他以 _CHARSET 结尾的常量可用
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, //这行参数不用管
			DEFAULT_QUALITY, //默认输出质量
			FF_DONTCARE, //不指定字体族*/
			NULL //字体名
		);
		sbs->hBrush = CreateSolidBrush(RGB(240, 240, 240));
		sbs->hBrush2 = CreateSolidBrush(RGB(255, 255, 255));
		HGDIOBJ hOld = NULL;
		if (!(sbs->type & 0x0FFFFFFFE)) hOld = SelectObject(hdc, sbs->hFont);
		for (int i = 0; i != sbs->num; i++, sbs->ctrlLen += 5) //计算一个按钮的大小
		{
			//sbs->pse[i].ID = i; //直接覆盖原来填写的ID
			sbs->pse[i].szLen = lstrlen(sbs->pse[i].szName);
			GetTextExtentExPoint(hdc, sbs->pse[i].szName, sbs->pse[i].szLen, 0, NULL, NULL, &(sbs->pse[i].szButton));
			if (sbs->pse[i].icon) sbs->pse[i].szButton.cx += (SIDEBAR_WIDTH + 5);//加5是为了显示按钮的边
			sbs->ctrlLen += sbs->pse[i].szButton.cx;
		}
		if (!(sbs->type & 0x0FFFFFFFE)) SelectObject(hdc, hOld);
		ReleaseDC(hWnd, hdc);
		sbs->hScroll = CreateWindowEx(
			0L,                          // no extended styles 
			_TEXT("SCROLLBAR"),                 // scroll bar control class 
			NULL,                // text for window title bar 
			WS_CHILD | ((sbs->type & 0x0FFFFFFFE)?SBS_HORZ:SBS_VERT),         // scroll bar styles 
			0,                           // horizontal position 
			0,                           // vertical position 
			SIDEBAR_WIDTH,                         // width of the scroll bar 
			SIDEBAR_WIDTH,               // default height 
			hWnd,                   // handle to main window 
			(HMENU)1024,           // no menu for a scroll bar 
			sbs->hInst,                  // instance owning this window 
			(LPVOID)NULL           // pointer not needed 
		);
		ShowWindow(sbs->hScroll, SW_HIDE);
		return (INT_PTR)TRUE;
	}
	case WM_SIZE:
	{
		SIDEBAR* sbs = (SIDEBAR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		SpiterPaint(hWnd, sbs);
		RECT rc;
		GetClientRect(hWnd, &rc);
		if (sbs->type & 0x0FFFFFFFE)
		{//水平
			MoveWindow(sbs->hScroll, rc.right - SIDEBAR_WIDTH, 0, SIDEBAR_WIDTH, SIDEBAR_WIDTH, FALSE);
			ShowWindow(sbs->hScroll, (rc.right < sbs->ctrlLen) ? SW_SHOW : SW_HIDE);
		}
		else
		{//垂直
			MoveWindow(sbs->hScroll, 0, rc.bottom - SIDEBAR_WIDTH, SIDEBAR_WIDTH, SIDEBAR_WIDTH, FALSE);
			ShowWindow(sbs->hScroll, (rc.bottom < sbs->ctrlLen) ? SW_SHOW : SW_HIDE);
		}
		break;
	}
#define TIMEEVENTID 19999
	case WM_MOUSEMOVE: //检测鼠标是否在按钮上
		if (wParam == 0) //只是鼠标移动到按钮上,没有其它动作
		{
			SIDEBAR * sbs = (SIDEBAR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (sbs->type & 0x0FFFFFFFE)
			{//水平
				int len = sbs->ctrlShow + GET_X_LPARAM(lParam);
				int count = 0, i = 0;
				for (; i != sbs->num; i++)
				{
					count += sbs->pse[i].szButton.cx;
					if (count > len)
					{
						if (sbs->ctrlID != i)
						{
							if (sbs->ctrlID != -1)
							{
								SendMessage(sbs->hReceive, MSG_SBBUTTON, WPARAM(FALSE), LPARAM(sbs->ctrlID)); //发送前一个按钮FALSE
								if (sbs->timeID)
								{
									KillTimer(hWnd, sbs->timeID);
									sbs->timeID = 0;
								}
							}
							SendMessage(sbs->hReceive, MSG_SBBUTTON, WPARAM(TRUE), LPARAM(i)); //发送这个按钮的TRUE
							if (sbs->timeID)
							{
								KillTimer(hWnd, sbs->timeID);
								sbs->timeID = 0;
							}
							else if (sbs->timeout)
							{
								sbs->timeID = SetTimer(hWnd, TIMEEVENTID, sbs->timeout, NULL);
							}
							sbs->ctrlID = i;
						}
						break;
					}
					count += 5;
					if (count > len) break;
				}
				if (i != sbs->num)
				{
					SpiterPaint(hWnd, sbs);
					InvalidateRect(hWnd, NULL, TRUE);
				}
			}
			else
			{//垂直
				int len = sbs->ctrlShow + GET_Y_LPARAM(lParam);
				int count = 0, i =0;
				for (; i != sbs->num; i++)
				{
					count += sbs->pse[i].szButton.cx;
					if (count > len)
					{
						if (sbs->ctrlID != i)
						{
							if (sbs->ctrlID != -1)
							{
								SendMessage(sbs->hReceive, MSG_SBBUTTON, WPARAM(FALSE), LPARAM(sbs->ctrlID)); //发送前一个按钮FALSE
								if (sbs->timeID)
								{
									KillTimer(hWnd, sbs->timeID);
									sbs->timeID = 0;
								}
							}
							SendMessage(sbs->hReceive, MSG_SBBUTTON, WPARAM(TRUE), LPARAM(i)); //发送这个按钮的TRUE
							if (sbs->timeID)
							{
								KillTimer(hWnd, sbs->timeID);
								sbs->timeID = 0;
							}
							else if(sbs->timeout) 
							{
								sbs->timeID = SetTimer(hWnd, TIMEEVENTID, sbs->timeout, NULL);
							}
							sbs->ctrlID = i;
						}
						break;
					}
					count += 5;
					if (count > len) break;
				}
				if (i != sbs->num)
				{
					SpiterPaint(hWnd, sbs);
					InvalidateRect(hWnd, NULL, TRUE);
				}
			}
		}
		break;
	case WM_TIMER:
	{
		if (wParam == TIMEEVENTID)
		{
			SIDEBAR * sbs = (SIDEBAR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (sbs->ctrlID != -1)
			{
				sbs->ctrlID = -1;
				KillTimer(hWnd, sbs->timeID);
				sbs->timeID = 0;
				SpiterPaint(hWnd, sbs);
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
		break;
	}
#define SCROLL_MOVE 30
	case WM_VSCROLL:
	{
		SIDEBAR * sbs = (SIDEBAR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (wParam == SB_LINEUP)
		{
			sbs->ctrlShow -= SCROLL_MOVE;
			if (sbs->ctrlShow < 0) sbs->ctrlShow = 0;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		else if (wParam == SB_LINEDOWN)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			sbs->ctrlShow += SCROLL_MOVE;
			rc.bottom -= SIDEBAR_WIDTH;
			int x323 = sbs->ctrlLen - rc.bottom;
			if (sbs->ctrlShow > x323) sbs->ctrlShow = x323;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	}
	case WM_HSCROLL:
	{
		SIDEBAR * sbs = (SIDEBAR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (wParam == SB_LINELEFT)
		{
			sbs->ctrlShow -= SCROLL_MOVE;
			if (sbs->ctrlShow < 0) sbs->ctrlShow = 0;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		else if (wParam == SB_LINERIGHT)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			sbs->ctrlShow += SCROLL_MOVE;
			rc.right -= SIDEBAR_WIDTH;
			int x253 = sbs->ctrlLen - rc.right;
			if (sbs->ctrlShow > x253) sbs->ctrlShow = x253;
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	}
#undef SCROLL_MOVE
	case WM_PAINT:
	{
		SIDEBAR * sbs = (SIDEBAR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		PAINTSTRUCT ps;
		RECT rc;
		GetClientRect(hWnd, &rc);
		HDC hdc = BeginPaint(hWnd, &ps);
		if (sbs->type & 0x0FFFFFFFE)
		{//水平
			if (rc.right < sbs->ctrlLen) rc.right -= SIDEBAR_WIDTH;
			BitBlt(hdc, 0, 0, rc.right, rc.bottom, sbs->hdc, sbs->ctrlShow, 0, SRCCOPY);
		}
		else
		{//垂直
			if (rc.bottom < sbs->ctrlLen) rc.bottom -= SIDEBAR_WIDTH;
			BitBlt(hdc, 0, 0, rc.right, rc.bottom, sbs->hdc, 0, sbs->ctrlShow, SRCCOPY);
		}
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
	{
		SIDEBAR* sbs = (SIDEBAR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		DeleteObject(sbs->hFont);
		DeleteObject(sbs->hBrush);
		DeleteObject(sbs->hBitmap);
		DeleteDC(sbs->hdc);
		free(sbs->pse);
		free(sbs);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(NULL));
		break;
	}
	case MSG_SBRECEIVE:
	{
		SIDEBAR* sbs = (SIDEBAR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		sbs->hReceive = HWND(lParam);
		break;
	}
	case MSG_SBTIMEOUT:
	{
		SIDEBAR* sbs = (SIDEBAR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		sbs->timeout = lParam;
		if (sbs->timeout && sbs->ctrlID != -1)
		{
			sbs->timeID = SetTimer(hWnd, TIMEEVENTID, sbs->timeout, NULL);
		}
		break;
	}
	case MSG_SBPOS:
	{
		RECT rc;
		GetClientRect(hWnd, &rc);
		MoveWindow(hWnd, wParam, lParam, rc.right, rc.bottom, FALSE);
		break;
	}
	case MSG_SBLEN:
	{
		SIDEBAR* sbs = (SIDEBAR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		RECT rc;
		GetClientRect(hWnd, &rc);
		MapWindowPoints(hWnd, sbs->hParent, LPPOINT(&rc), 2);//从本控件的坐标映射到父窗口坐标
		if (sbs->type & 0x0FFFFFFFE)
		{//水平
			if (sbs->ctrlShow && lParam > sbs->ctrlLen) sbs->ctrlShow = 0;
			MoveWindow(hWnd, rc.left, rc.top,  lParam,rc.bottom - rc.top, FALSE);
		}
		else
		{//垂直
			if (sbs->ctrlShow && lParam > sbs->ctrlLen) sbs->ctrlShow = 0;
			MoveWindow(hWnd, rc.left, rc.top, rc.right - rc.left, lParam, FALSE);
		}
		break;
	}
	case MSG_SBRESET:
	{
		SIDEBAR* sbs = (SIDEBAR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (sbs->ctrlID != -1)
		{
			sbs->ctrlID = -1;
			SpiterPaint(hWnd, sbs);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	}
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	return (INT_PTR)FALSE;
}

void SpiterPaint(HWND hWnd, SIDEBAR* sbs)
{
	HDC hdc = GetDC(hWnd);
	//创建兼容DC
	RECT rc = {};
	DeleteObject(sbs->hBitmap);
	if (sbs->type & 0x0FFFFFFFE)
	{
		sbs->hBitmap = CreateCompatibleBitmap(hdc, sbs->ctrlLen, SIDEBAR_WIDTH);
		rc.right = sbs->ctrlLen;
		rc.bottom = SIDEBAR_WIDTH;
	}
	else
	{
		sbs->hBitmap = CreateCompatibleBitmap(hdc, SIDEBAR_WIDTH, sbs->ctrlLen);
		rc.right = SIDEBAR_WIDTH;
		rc.bottom = sbs->ctrlLen;
	}
	SelectObject(sbs->hdc, sbs->hBitmap);
	//DC初始化为灰色
	SetBkColor(sbs->hdc, RGB(240, 240, 240));
	ExtTextOut(sbs->hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	if (sbs->type & 0x0FFFFFFFE)//TYPE_SBTOP或TYPE_SBBOTTOM 或TYPE_SBHORIZONTAL 水平的边栏
	{
		SelectObject(sbs->hdc, sbs->hBrush);
		int len = 0;
		for (int i = 0; i != sbs->num; i++)
		{
			HGDIOBJ hOld2 = NULL;
			if (sbs->ctrlID == i)
			{
				hOld2 = SelectObject(sbs->hdc, sbs->hBrush2);
				SetBkColor(sbs->hdc, RGB(255, 255, 255));
			}
			RoundRect(sbs->hdc, len, 0, len + sbs->pse[i].szButton.cx, SIDEBAR_WIDTH,  5, 5); //画外框
			if (sbs->pse[i].icon)
			{
				DrawIconEx(sbs->hdc, 4 + len, 4, sbs->pse[i].icon, 16, 16, 0, NULL, DI_NORMAL); //画图标
				TextOut(sbs->hdc, SIDEBAR_WIDTH + len, 4, sbs->pse[i].szName, sbs->pse[i].szLen);
			}
			else
			{
				TextOut(sbs->hdc, len, 4, sbs->pse[i].szName, sbs->pse[i].szLen);
			}
			len += (sbs->pse[i].szButton.cx + 5);
			if (hOld2)
			{
				SelectObject(sbs->hdc, hOld2);
				SetBkColor(sbs->hdc, RGB(240, 240, 240));
			}
		}
	}
	else
	{
		SelectObject(sbs->hdc, sbs->hBrush);
		HGDIOBJ hOld = SelectObject(sbs->hdc, sbs->hFont);
		int len = 0;
		for (int i = 0; i != sbs->num; i++)
		{
			HGDIOBJ hOld2 = NULL;
			if (sbs->ctrlID == i)
			{
				hOld2 = SelectObject(sbs->hdc, sbs->hBrush2);
				SetBkColor(sbs->hdc, RGB(255, 255, 255));
			}
			RoundRect(sbs->hdc, 0, len, SIDEBAR_WIDTH, len + sbs->pse[i].szButton.cx, 5, 5); //画外框
			if (sbs->pse[i].icon)
			{
				DrawIconEx(sbs->hdc, 4, 4 + len, sbs->pse[i].icon, 16, 16, 0, NULL, DI_NORMAL); //画图标
				TextOut(sbs->hdc, 22, SIDEBAR_WIDTH + len, sbs->pse[i].szName, sbs->pse[i].szLen);
			}
			else 
			{
				TextOut(sbs->hdc, 22, len, sbs->pse[i].szName, sbs->pse[i].szLen);
			}
			len += (sbs->pse[i].szButton.cx + 5);
			if (hOld2)
			{
				SelectObject(sbs->hdc, hOld2);
				SetBkColor(sbs->hdc, RGB(240, 240, 240));
			}
		}
		SelectObject(sbs->hdc, hOld);
	}
	
	ReleaseDC(hWnd, hdc);
}



//以下是通用子窗口容器
typedef struct __PANE_STRUCT__
{
	HINSTANCE hInst; //进程实例句柄
	PANE_ELEM* ppa; //指向PANE_ELEM
	int     num;  //通用子窗口容器包含的子窗口的数量
	HWND	hParent; //通用子窗口容器的父窗口句柄
	HWND	hReceive; //消息接收句柄
	HDC		hdc;     //通用子窗口容器的标题hdc
	HBITMAP hBitmap; //通用子窗口容器的标题Bitmap
	int		fontStart; //字体离上边框的距离
	HBRUSH  hBrush;
	unsigned int mark; //标志控件位置
	TCHAR	titleName[64]; //当前需要显示的标题
	int		szTitleName ; //当前需要显示的标题
	HICON	hIcon[8];		
	int		markMouse;		//标记鼠标移动到哪个按钮上了
	int		markMouseClick; //标记鼠标点击了钉这个按钮
	HMENU	hMenu;			//用于响应菜单按钮
	HWND	hSpliter;
	HWND	hSidebar;
	HWND	hSidebarConnected;//与此关连的hSidebarConnected
}PANE;

LRESULT  CALLBACK Pane_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND Pane(HWND hParent, int ID, PANE_ELEM* ppa, int num,RECT* rc, unsigned int mark)
{
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);
	TCHAR* szClassName = _TEXT("PANE_CLASS");
	static ATOM sclass = 0;
	if (!sclass)
	{
		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Pane_Proc;
		wcex.hInstance = hInst;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		//wcex.hbrBackground = (HBRUSH)(COLOR_MENU + 1);
		wcex.lpszClassName = szClassName;
		sclass = RegisterClassExW(&wcex);
		if (sclass = RegisterClassExW(&wcex)) return NULL;
	}

	PANE* ppe = (PANE*)malloc(sizeof(PANE));
	if (ppe == NULL) return NULL;
	memset(ppe, 0, sizeof(PANE));
	ppe->hInst = hInst;
	ppe->hParent = hParent;
	ppe->ppa = ppa;
	ppe->num = num;
	ppe->mark = mark;
	HWND hChild = CreateWindowEx(0, szClassName, NULL, WS_CHILD | WS_VISIBLE| WS_DLGFRAME, \
		rc->left, rc->top, \
		rc->right, rc->bottom, \
		hParent, HMENU(ID), hInst, (LPVOID)ppe);
	if (!hChild) return NULL;
	return hChild;
}
BYTE ANDmaskIconClose[];
BYTE ANDmaskIconDing1[];
BYTE ANDmaskIconDing2[];
BYTE ANDmaskIconDown[];
BYTE XORmaskIcon[];
BYTE XORmaskIconEmpty[];
void Pane_Title_paint(HWND hWnd, RECT* prct);
LRESULT  CALLBACK Pane_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#define PANE_ID_SP 19200
#define TITLE_SIZE 18
	switch (message)
	{
	case WM_CREATE:
	{
		PANE* ppe = (PANE*)((LPCREATESTRUCT(lParam))->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(ppe));
		RECT rc,rcP;
		GetClientRect(hWnd, &rc);
		GetClientRect(ppe->hParent, &rcP);
		MapWindowPoints(hWnd, ppe->hParent, LPPOINT(&rcP), 2);//从本控件的坐标映射到父窗口坐标

		if (ppe->mark & PANE_SP_LEFT)
		{
			rc.left = SPLITER_WIDTH;
			ppe->hSpliter = Spliter(hWnd, PANE_ID_SP, TYPE_VSPLITER, 100, rcP.right, 0, 0, rc.bottom);
		}
		else if (ppe->mark & PANE_SP_RIGHT)
		{
			rc.right -= SPLITER_WIDTH;
			ppe->hSpliter = Spliter(hWnd, PANE_ID_SP, TYPE_VSPLITER, 100, 500, rc.right, 0, rc.bottom);
		}
		else if (ppe->mark & PANE_SP_TOP)
		{
			rc.top = SPLITER_WIDTH;
			ppe->hSpliter = Spliter(hWnd, PANE_ID_SP, TYPE_VSPLITER, 100, rcP.bottom, 0, 0, rc.right);
		}
		else if (ppe->mark & PANE_SP_BOTTOM)
		{
			rc.bottom -= SPLITER_WIDTH;
			ppe->hSpliter = Spliter(hWnd, PANE_ID_SP, TYPE_VSPLITER, 100, rcP.bottom, rc.bottom, 0, rc.right);
		}

		if (ppe->ppa)
		{
			for (int i = 0; i != ppe->num; i++)
			{
				ppe->ppa[i].lParam = & ppe->ppa[i].rc;
				memcpy(ppe->ppa[i].lParam, &rc, sizeof(RECT));
				ppe->ppa[i].hChild = CreateDialogParam(ppe->hInst, \
					MAKEINTRESOURCE(ppe->ppa[i].ID), \
					hWnd, \
					ppe->ppa[i].lpDialogFunc, \
					LPARAM(ppe->ppa[i].lParam));
			}
			lstrcpyn(ppe->titleName,ppe->ppa[0].szName,64);
			ppe->szTitleName = lstrlen(ppe->titleName);
			ShowWindow(ppe->ppa[0].hChild, SW_HIDE);
		}

		HDC hdc = GetDC(hWnd);
		ppe->hdc = CreateCompatibleDC(hdc);
		ppe->hBitmap = CreateCompatibleBitmap(hdc, 10, TITLE_SIZE);
		TEXTMETRIC ptm = {};
		GetTextMetrics(hdc, &ptm);
		ppe->fontStart = (TITLE_SIZE - ptm.tmHeight)/2;
		if (ppe->fontStart < 0) ppe->fontStart = 0;
		ReleaseDC(hWnd, hdc);
		//加载ICON
		const int iconsize = 64;

		ppe->hIcon[0] = CreateIcon(ppe->hInst, iconsize, iconsize, 1, 1, ANDmaskIconClose, XORmaskIconEmpty);
		ppe->hIcon[1] = CreateIcon(ppe->hInst, iconsize, iconsize, 1, 1, ANDmaskIconClose, XORmaskIcon);
		ppe->hIcon[2] = CreateIcon(ppe->hInst, iconsize, iconsize, 1, 1, ANDmaskIconDing1, XORmaskIconEmpty);
		ppe->hIcon[3] = CreateIcon(ppe->hInst, iconsize, iconsize, 1, 1, ANDmaskIconDing1, XORmaskIcon);
		ppe->hIcon[4] = CreateIcon(ppe->hInst, iconsize, iconsize, 1, 1, ANDmaskIconDing2, XORmaskIconEmpty);
		ppe->hIcon[5] = CreateIcon(ppe->hInst, iconsize, iconsize, 1, 1, ANDmaskIconDing2, XORmaskIcon);
		ppe->hIcon[6] = CreateIcon(ppe->hInst, iconsize, iconsize, 1, 1, ANDmaskIconDown, XORmaskIconEmpty);
		ppe->hIcon[7] = CreateIcon(ppe->hInst, iconsize, iconsize, 1, 1, ANDmaskIconDown, XORmaskIcon);
		//加载菜单
		ppe->hMenu = CreatePopupMenu();
		TCHAR tchMenu[][16] = {
			_TEXT("关闭"),
			_TEXT("打开"),
		};
		for (int i = 0; i != 2; i++)
		{
			InsertMenu(ppe->hMenu, i, MF_BYPOSITION, PANE_ID_SP + 2 + i, tchMenu[i]);
		}

		return (INT_PTR)TRUE;
	}
	case MSG_VSPLITER:
	{
		PANE* ppe = (PANE*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		RECT rc;
		GetWindowRect(hWnd, &rc);
		MapWindowPoints(HWND_DESKTOP, ppe->hParent, (LPPOINT)&rc, 2); //改变自身大小获取的要是包括窗口边框的大小, 不是客户区的大小
		MoveWindow(hWnd, rc.left, rc.top, rc.right + wParam - rc.left, rc.bottom - rc.top, TRUE);
		break;
	}
	case MSG_HSPLITER:
	{
		PANE* ppe = (PANE*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		RECT rc;
		GetWindowRect(hWnd, &rc);
		MapWindowPoints(HWND_DESKTOP, ppe->hParent, (LPPOINT)&rc, 2); //获取的要是包括窗口边框的大小, 不是客户区的大小
		MoveWindow(hWnd, rc.left, rc.top, rc.right-rc.left, rc.bottom + wParam-rc.top, TRUE);
		break;
	}
	case WM_SIZE:
	{
		RECT rc;
		GetClientRect(hWnd,&rc);
		rc.bottom = TITLE_SIZE;
		Pane_Title_paint(hWnd, &rc);
	}
	case WM_MOUSEMOVE:
	{
		if (wParam == 0 && HIWORD(lParam) < TITLE_SIZE) //只是鼠标在移动,并且在标题上
		{
			PANE* ppe = (PANE*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			RECT rc;
			GetClientRect(hWnd, &rc);
			int szPos = LOWORD(lParam);
			int place = rc.right - ((ppe->mark & 0xFFFFFFFC) ? SPLITER_WIDTH : 0);
			int markRepaint = 0;
			if ((szPos < place && szPos > place - TITLE_SIZE))
			{
				if ((ppe->markMouse & 0x01) == 0) ppe->markMouse = 0x01, markRepaint = 1;
			}
			else if (szPos < place - (TITLE_SIZE + 5) && szPos >place - (TITLE_SIZE * 2 + 5))
			{
				if ((ppe->markMouse & 0x02) == 0) ppe->markMouse = 0x02, markRepaint = 1;
			}
			else if (szPos <place - (TITLE_SIZE * 2 + 10) && szPos > place - (TITLE_SIZE * 3 + 10))
			{
				if ((ppe->markMouse & 0x04) == 0) ppe->markMouse = 0x04, markRepaint = 1;
			}
			else
			{
				if (ppe->markMouse)ppe->markMouse = 0, markRepaint = 1;
			}
			if (markRepaint)
			{
				RECT rc;
				GetClientRect(hWnd, &rc);
				rc.bottom = TITLE_SIZE;
				InvalidateRect(hWnd, &rc, TRUE);
				Pane_Title_paint(hWnd, &rc);
			}
		}
		break;
	}
	case WM_LBUTTONDOWN: //鼠标左键点下
	{
		if (MK_LBUTTON == wParam && HIWORD(lParam) < TITLE_SIZE)//点击的只是左键,并且在标题上
		{
			PANE* ppe = (PANE*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			RECT rc;
			GetClientRect(hWnd, &rc);
			int szPos = LOWORD(lParam);
			int place = rc.right - ((ppe->mark & 0xFFFFFFFC) ? SPLITER_WIDTH : 0);
			if ((szPos < place && szPos > place - TITLE_SIZE))
			{
				if ((ppe->markMouseClick & 0x01) == 0) ppe->markMouseClick |= 0x01;
			}
			else if (szPos < place - (TITLE_SIZE + 5) && szPos >place - (TITLE_SIZE * 2 + 5))
			{
				if ((ppe->markMouseClick & 0x02) == 0) ppe->markMouseClick |= 0x02;
			}
			else if (szPos <place - (TITLE_SIZE * 2 + 10) && szPos > place - (TITLE_SIZE * 3 + 10))
			{
				if ((ppe->markMouseClick & 0x08) == 0) ppe->markMouseClick |= 0x08;
			}
		}
		break;
	}
	case WM_LBUTTONUP: //鼠标左键点下又松开
	{
		if (0 == wParam && HIWORD(lParam) < TITLE_SIZE)//点击的只是左键,并且在标题上
		{
			PANE* ppe = (PANE*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			RECT rc;
			GetClientRect(hWnd, &rc);
			int szPos = LOWORD(lParam);
			int place = rc.right - ((ppe->mark & 0xFFFFFFFC) ? SPLITER_WIDTH : 0);
			int markRepaint = 0;
			if ((szPos < place && szPos > place - TITLE_SIZE))
			{
				if ((ppe->markMouseClick & 0x01)) /*此处是已点击关闭按钮需要响应的代码*/
				{
					ShowWindow(hWnd, SW_HIDE);
					if (ppe->hSidebarConnected) Sidebar_reset(ppe->hSidebarConnected);
				}
			}
			else if (szPos < place - (TITLE_SIZE + 5) && szPos >place - (TITLE_SIZE * 2 + 5))
			{
				if ((ppe->markMouseClick & 0x02)) 
				{ 
					markRepaint = 1;
					ppe->markMouseClick ^= 0x04; //翻转第三位
					/*此处是已点击钉按钮需要响应的代码*/
				}
			}
			else if (szPos <place - (TITLE_SIZE * 2 + 10) && szPos > place - (TITLE_SIZE * 3 + 10))
			{
				if ((ppe->markMouseClick & 0x08)) 
				{/*此处是已点击菜单按钮需要响应的代码*/
					POINT	p;
					GetCursorPos(&p);
					TrackPopupMenu(ppe->hMenu, TPM_LEFTBUTTON,p.x, p.y, 0, hWnd, NULL);
				}
			}
			if (markRepaint)
			{
				RECT rc;
				GetClientRect(hWnd, &rc);
				rc.bottom = TITLE_SIZE;
				InvalidateRect(hWnd, &rc, TRUE);
				Pane_Title_paint(hWnd, &rc);
			}
		}
		break;
	}
	case WM_PAINT:
	{
		PANE* ppe = (PANE*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		RECT rc = {};
		GetClientRect(hWnd, &rc);
		rc.right -= (ppe->mark & PANE_SP_LEFT) ? SPLITER_WIDTH : 0;
		rc.bottom = TITLE_SIZE;
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, (ppe->mark & PANE_SP_LEFT) ? SPLITER_WIDTH : 0, 0, rc.right, rc.bottom, ppe->hdc, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_KILLFOCUS:
		MessageBox(hWnd, _TEXT("Closea"), _TEXT("Closea"), MB_OK);
		break;
	case WM_COMMAND:
		switch (wParam)
		{
		case PANE_ID_SP + 2 + 0:
			MessageBox(hWnd, _TEXT("Close"), _TEXT("Close"), MB_OK);
			break;
		case PANE_ID_SP + 2 + 1:
			MessageBox(hWnd, _TEXT("打开"), _TEXT("打开"), MB_OK);
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:
	{
		PANE* ppe = (PANE*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		for (int i = 0; i != 8; i++)
		{
			DestroyIcon(ppe->hIcon[i]);
		}
		DeleteObject(ppe->hBitmap);
		DeleteDC(ppe->hdc);
		break;
	}
	case MSG_PANE_SB:
	{
		PANE* ppe = (PANE*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		ppe->hSidebarConnected = (HWND)lParam;
		break;
	}
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	return (INT_PTR)FALSE;
}

void Pane_Title_paint(HWND hWnd, RECT* prct)
{
	PANE* ppe = (PANE*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	//创建标题用的hdc
	DeleteObject(ppe->hBitmap);
	HDC hdc = GetDC(hWnd);
	prct->right -= ((ppe->mark & 0xFFFFFFFC) ? SPLITER_WIDTH : 0);
	ppe->hBitmap = CreateCompatibleBitmap(hdc, prct->right, prct->bottom);
	SelectObject(ppe->hdc, ppe->hBitmap);

	SetBkColor(ppe->hdc, RGB(0, 0, 255));
	ExtTextOut(ppe->hdc, 0, 0, ETO_OPAQUE, prct, NULL, 0, NULL);
	TextOut(ppe->hdc, 0, ppe->fontStart, ppe->titleName, ppe->szTitleName);
	int place = prct->right - TITLE_SIZE - SPLITER_WIDTH;
	DrawIconEx(ppe->hdc, place, 0, ppe->hIcon[(ppe->markMouse & 0x01) ? 1 : 0], TITLE_SIZE, TITLE_SIZE, 0, NULL, DI_NORMAL);
	place -= (TITLE_SIZE + 5);
	DrawIconEx(ppe->hdc, place, 0, \
		ppe->hIcon[ppe->markMouseClick& 0x04 ? ((ppe->markMouse & 0x02) ? 5 : 4) :((ppe->markMouse & 0x02) ? 3 : 2)], \
		TITLE_SIZE, TITLE_SIZE, 0, NULL, DI_NORMAL);
	place -= (TITLE_SIZE + 5);
	DrawIconEx(ppe->hdc, place, 0, ppe->hIcon[(ppe->markMouse & 0x04) ? 7 : 6], TITLE_SIZE, TITLE_SIZE, 0, NULL, DI_NORMAL);
	ReleaseDC(hWnd, hdc);
}

BYTE ANDmaskIconClose[] = {
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XC0,0X7F,0XFF,0XFF,0XFE,0X03,0XFF,0XFF,0XC0,0X3F,0XFF,0XFF,0XFC,0X03,0XFF,
	0XFF,0XC0,0X1F,0XFF,0XFF,0XF8,0X03,0XFF,0XFF,0XC0,0X0F,0XFF,0XFF,0XF0,0X03,0XFF,
	0XFF,0XC0,0X07,0XFF,0XFF,0XE0,0X03,0XFF,0XFF,0XC0,0X03,0XFF,0XFF,0XC0,0X03,0XFF,
	0XFF,0XC0,0X01,0XFF,0XFF,0X80,0X07,0XFF,0XFF,0XE0,0X00,0XFF,0XFF,0X00,0X0F,0XFF,
	0XFF,0XF0,0X00,0X7F,0XFE,0X00,0X1F,0XFF,0XFF,0XF8,0X00,0X3F,0XFC,0X00,0X3F,0XFF,
	0XFF,0XFC,0X00,0X1F,0XF8,0X00,0X7F,0XFF,0XFF,0XFE,0X00,0X0F,0XF0,0X00,0XFF,0XFF,
	0XFF,0XFF,0X00,0X07,0XE0,0X01,0XFF,0XFF,0XFF,0XFF,0X80,0X03,0XC0,0X03,0XFF,0XFF,
	0XFF,0XFF,0XC0,0X01,0X80,0X03,0XFF,0XFF,0XFF,0XFF,0XE0,0X00,0X00,0X07,0XFF,0XFF,
	0XFF,0XFF,0XF0,0X00,0X00,0X0F,0XFF,0XFF,0XFF,0XFF,0XF8,0X00,0X00,0X1F,0XFF,0XFF,
	0XFF,0XFF,0XFC,0X00,0X00,0X3F,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X7F,0XFF,0XFF,
	0XFF,0XFF,0XFF,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X80,0X01,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0X80,0X01,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X00,0X00,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFE,0X00,0X00,0X7F,0XFF,0XFF,0XFF,0XFF,0XFC,0X00,0X00,0X3F,0XFF,0XFF,
	0XFF,0XFF,0XF8,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X0F,0XFF,0XFF,
	0XFF,0XFF,0XE0,0X00,0X00,0X07,0XFF,0XFF,0XFF,0XFF,0XC0,0X01,0X80,0X03,0XFF,0XFF,
	0XFF,0XFF,0X80,0X03,0XC0,0X01,0XFF,0XFF,0XFF,0XFF,0X00,0X07,0XE0,0X00,0XFF,0XFF,
	0XFF,0XFE,0X00,0X0F,0XF0,0X00,0X7F,0XFF,0XFF,0XFC,0X00,0X1F,0XF8,0X00,0X3F,0XFF,
	0XFF,0XF8,0X00,0X3F,0XFC,0X00,0X1F,0XFF,0XFF,0XF0,0X00,0X7F,0XFE,0X00,0X0F,0XFF,
	0XFF,0XE0,0X00,0XFF,0XFF,0X00,0X07,0XFF,0XFF,0XC0,0X01,0XFF,0XFF,0X80,0X03,0XFF,
	0XFF,0XC0,0X03,0XFF,0XFF,0XC0,0X03,0XFF,0XFF,0XC0,0X07,0XFF,0XFF,0XE0,0X03,0XFF,
	0XFF,0XC0,0X0F,0XFF,0XFF,0XF0,0X03,0XFF,0XFF,0XC0,0X1F,0XFF,0XFF,0XF8,0X03,0XFF,
	0XFF,0XC0,0X3F,0XFF,0XFF,0XFC,0X03,0XFF,0XFF,0XC0,0X7F,0XFF,0XFF,0XFE,0X03,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
};
BYTE ANDmaskIconDing1[] = {
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFC,0X7F,0XFF,0XFF,0X1F,0XFF,0XFF,0XFF,0XF8,0X3F,0XFF,0XFF,0X0F,0XFF,
	0XFF,0XFF,0XF9,0X3F,0XFF,0XFE,0X07,0XFF,0XFF,0XFF,0XF1,0X9F,0XFF,0XFC,0X67,0XFF,
	0XFF,0XFF,0XF3,0X9F,0XFF,0XFC,0XE3,0XFF,0XFF,0XFF,0XE3,0XCF,0XFF,0XFC,0XF3,0XFF,
	0XFF,0XFF,0XE7,0XCF,0XFF,0XF9,0XF3,0XFF,0XFF,0XFF,0XE7,0XC0,0X00,0X01,0XF1,0XFF,
	0XFF,0XFF,0XC7,0XE0,0X00,0X01,0XF9,0XFF,0XFF,0XFF,0XCF,0XFF,0XFF,0XFF,0XF9,0XFF,
	0XFF,0XC0,0X0F,0XFF,0XFF,0XFF,0XF9,0XFF,0XFE,0X00,0X0F,0XFF,0XFF,0XFF,0XF9,0XFF,
	0XFE,0X00,0X0F,0XFF,0XFF,0XFF,0XF9,0XFF,0XFF,0XC0,0X0F,0XFF,0XFF,0XFF,0XF9,0XFF,
	0XFF,0XFF,0XCF,0XFF,0XFF,0XFF,0XF9,0XFF,0XFF,0XFF,0XC7,0XE0,0X00,0X01,0XF9,0XFF,
	0XFF,0XFF,0XE7,0XC0,0X00,0X01,0XF1,0XFF,0XFF,0XFF,0XE7,0XCF,0XFF,0XF9,0XF3,0XFF,
	0XFF,0XFF,0XE3,0XCF,0XFF,0XFC,0XF3,0XFF,0XFF,0XFF,0XF3,0X9F,0XFF,0XFC,0XE3,0XFF,
	0XFF,0XFF,0XF1,0X9F,0XFF,0XFC,0X67,0XFF,0XFF,0XFF,0XF9,0X3F,0XFF,0XFE,0X07,0XFF,
	0XFF,0XFF,0XF8,0X3F,0XFF,0XFF,0X0F,0XFF,0XFF,0XFF,0XFC,0X7F,0XFF,0XFF,0X1F,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
};
BYTE ANDmaskIconDing2[] = {
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X07,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFC,0X00,0X00,0X3F,0XFF,0XFF,
	0XFF,0XFF,0XF8,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X0F,0XFF,0XFF,
	0XFF,0XFF,0XF0,0X00,0X00,0X0F,0XFF,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X0F,0XFF,0XFF,
	0XFF,0XFF,0XFC,0X00,0X00,0X3F,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X20,0X7F,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XC0,0X23,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X27,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0X80,0X21,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X7F,0XFF,0XFF,
	0XFF,0XFF,0XF8,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X0F,0XFF,0XFF,
	0XFF,0XFF,0XF0,0X00,0X00,0X0F,0XFF,0XFF,0XFF,0XFF,0XF0,0X00,0X00,0X0F,0XFF,0XFF,
	0XFF,0XFF,0XF8,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X7F,0XFF,0XFF,
	0XFF,0XFF,0XFF,0X80,0X01,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0X1F,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFC,0X3F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFC,0X3F,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFC,0X3F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFC,0X3F,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFC,0X3F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFC,0X3F,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFC,0X3F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFC,0X3F,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFE,0X7F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0X7F,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFE,0X7F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
};
BYTE ANDmaskIconDown[] = {
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X0F,0XFF,0XFF,0XF0,0X00,0X7F,
	0XFF,0X00,0X07,0XFF,0XFF,0XE0,0X00,0XFF,0XFF,0X80,0X03,0XFF,0XFF,0XC0,0X01,0XFF,
	0XFF,0XC0,0X01,0XFF,0XFF,0X80,0X03,0XFF,0XFF,0XE0,0X00,0XFF,0XFF,0X00,0X07,0XFF,
	0XFF,0XF0,0X00,0X7F,0XFE,0X00,0X0F,0XFF,0XFF,0XF8,0X00,0X3F,0XFC,0X00,0X1F,0XFF,
	0XFF,0XFC,0X00,0X1F,0XF8,0X00,0X3F,0XFF,0XFF,0XFE,0X00,0X0F,0XF0,0X00,0X7F,0XFF,
	0XFF,0XFF,0X00,0X07,0XE0,0X00,0XFF,0XFF,0XFF,0XFF,0X80,0X03,0XC0,0X01,0XFF,0XFF,
	0XFF,0XFF,0XC0,0X01,0X80,0X03,0XFF,0XFF,0XFF,0XFF,0XE0,0X00,0X00,0X07,0XFF,0XFF,
	0XFF,0XFF,0XF0,0X00,0X00,0X0F,0XFF,0XFF,0XFF,0XFF,0XF8,0X00,0X00,0X1F,0XFF,0XFF,
	0XFF,0XFF,0XFC,0X00,0X00,0X3F,0XFF,0XFF,0XFF,0XFF,0XFE,0X00,0X00,0X7F,0XFF,0XFF,
	0XFF,0XFF,0XFF,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X80,0X01,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XC0,0X03,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE0,0X07,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XF0,0X0F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0X1F,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFC,0X3F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0X7F,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
};
BYTE XORmaskIcon[] = {
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
};
BYTE XORmaskIconEmpty[] = {
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};
BOOL CALLBACK PaneExample_Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		RECT * rc = (RECT*)lParam;
		MoveWindow(hDlg, 0,0,rc->right,rc->bottom, TRUE);
		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			MessageBox(hDlg, _TEXT("abc"), _TEXT("cde"), MB_OK);
			break;
		case IDCANCEL:
			DestroyWindow(hDlg);
			InvalidateRect(GetParent(hDlg), NULL, TRUE); //强制父窗口重绘
			return TRUE;
		}
	}
	return FALSE;
}


//以下是名叫框架的窗口容器
typedef struct __FRAME_STRUCT__
{
	HINSTANCE			hInst; //进程实例句柄
	FRAME_ELEM*			fe; //指向FRAME_ELEM
	HWND				hParent;

}FRAME;
LRESULT  CALLBACK Frame_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND Frame(HWND hParent, int ID, RECT* rc)
{
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);
	TCHAR* szClassName = _TEXT("FRAME_CLASS");
	static ATOM sclass = 0;
	if (!sclass)
	{
		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Frame_Proc;
		wcex.hInstance = hInst;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		//wcex.hbrBackground = (HBRUSH)(COLOR_MENU + 1);
		wcex.lpszClassName = szClassName;
		sclass = RegisterClassExW(&wcex);
		if (sclass = RegisterClassExW(&wcex)) return NULL;
	}

	FRAME* fs = (FRAME*)malloc(sizeof(FRAME));
	if (fs == NULL) return NULL;
	memset(fs, 0, sizeof(FRAME));
	fs->hInst = hInst;
	fs->hParent = hParent;
	HWND hChild = CreateWindowEx(0, szClassName, NULL, WS_CHILD | WS_VISIBLE | WS_DLGFRAME, \
		rc->left, rc->top, \
		rc->right, rc->bottom, \
		hParent, HMENU(ID), hInst, (LPVOID)fs);
	if (!hChild) return NULL;
	return hChild;
}
LRESULT  CALLBACK Frame_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		FRAME* fs = (FRAME*)((LPCREATESTRUCT(lParam))->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(fs));
		return (INT_PTR)TRUE;
	}
	case WM_PAINT:
	{
		FRAME* fs = (FRAME*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rc = RECT{ 0,0,32,32 };
		DrawFrameControl(hdc, &rc, DFC_BUTTON, DFCS_BUTTONPUSH| DFCS_PUSHED);
		SetBkColor(hdc, RGB(255, 255, 0));
		DrawText(hdc, _TEXT("点我"), 2, &rc, DT_SINGLELINE | DT_VCENTER);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
	{
		FRAME* fs = (FRAME*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		break;
	}
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	return (INT_PTR)FALSE;
}



//以下是名叫框架的窗口容器
typedef struct __EDITOR_STRUCT__
{
	HINSTANCE			hInst; //进程实例句柄
	HWND				hParent;
	TCHAR               ptcFileName[MAX_PATH];
}EDITOR;
LRESULT  CALLBACK Editor_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND Editor(HWND hParent, int ID, TCHAR* ptcFileName, RECT* rc)
{
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hParent, GWL_HINSTANCE);
	TCHAR* szClassName = _TEXT("EDITOR_CLASS");
	static ATOM sclass = 0;
	if (!sclass)
	{
		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Editor_Proc;
		wcex.hInstance = hInst;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		//wcex.hbrBackground = (HBRUSH)(COLOR_MENU + 1);
		wcex.lpszClassName = szClassName;
		sclass = RegisterClassExW(&wcex);
		if (sclass = RegisterClassExW(&wcex)) return NULL;
	}

	EDITOR* es = (EDITOR*)malloc(sizeof(EDITOR));
	if (es == NULL) return NULL;
	memset(es, 0, sizeof(EDITOR));
	es->hInst = hInst;
	es->hParent = hParent;
	lstrcpyn(es->ptcFileName, ptcFileName, MAX_PATH);
	HWND hChild = CreateWindowEx(0, szClassName, NULL, WS_CHILD | WS_VISIBLE | WS_DLGFRAME, \
		rc->left, rc->top, \
		rc->right, rc->bottom, \
		hParent, HMENU(ID), hInst, (LPVOID)es);
	if (!hChild) return NULL;
	return hChild;
}
LRESULT  CALLBACK Editor_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		EDITOR* es = (EDITOR*)((LPCREATESTRUCT(lParam))->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(es));
		return (INT_PTR)TRUE;
	}
	case WM_PAINT:
	{
		EDITOR* es = (EDITOR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		RECT rc = RECT{ 0,0,32,32 };
		DrawFrameControl(hdc, &rc, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED);
		SetBkColor(hdc, RGB(255, 255, 0));
		DrawText(hdc, _TEXT("点我"), 2, &rc, DT_SINGLELINE | DT_VCENTER);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
	{
		EDITOR* es = (EDITOR*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		break;
	}
	default:
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}
	return (INT_PTR)FALSE;
}















int ByteToHex(char* byte, int byte_len, char* hex, int hex_len)
{
	char* HEX_TABLE = "0123456789ABCDEF";
	int b = 0, h = 0;
	hex_len -= (hex_len % 3);
	while (b < byte_len && h < hex_len) {
		hex[h++] = HEX_TABLE[(byte[b] & 0xF0) >> 4];
		hex[h++] = HEX_TABLE[byte[b] & 0x0F];
		hex[h++] = ' ';
		b++;
	}
	return h;
}

int HexToByte(char* hex, int hex_len, char* byte, int byte_len)
{
	char BYTE_TABLE[] = { '1', '2', '3', '4', '5', '6', '7', '8', '1', '2', '3', '4', '5', '6', '7', '8', '1', '2', '3', '4', '5', '6', '7', '8',
		'1', '2', '3', '4', '5', '6', '7', '8', '1', '2', '3', '4', '5', '6', '7', '8', '1', '2', '3', '4', '5', '6', '7', '8',
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ':', ';', '<', '=', '>', '?', '@', 10, 11, 12, 13, 14, 15 , 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
		'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V','W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_', '\'', 10, 11, 12, 13, 14, 15 };
	int b = 0, h = 0;
	while (h < hex_len && b < byte_len) {
		if (hex[h] == ' ' || hex[h] == '\t' || hex[h] == '\v' || hex[h] == '\r' || hex[h] == '\n') //跳过空白字符
		{
			h++;
			continue;
		}
		byte[b] = ((BYTE_TABLE[hex[h]]) << 4) | (BYTE_TABLE[hex[h + 1]]);
		h += 2; b++;
	}
	return b;
}



//子窗口居中于父窗口
void Dialog_center(HWND hDlg)
{
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0); //获取可用桌面大小,保证子窗口能完全显示,
	int cx = rect.right - rect.left;
	int cy = rect.bottom - rect.top;

	RECT mainDialog; //主窗口的大小
	GetWindowRect(GetParent(hDlg), &mainDialog);
	RECT rectWindow;
	GetWindowRect(hDlg, &rectWindow);
	//获取可用桌面大小  

	POINT p1,p2; 
	p1.x = mainDialog.left + (mainDialog.right - mainDialog.left - (rectWindow.right - rectWindow.left)) / 2;
	p1.y = mainDialog.top + (mainDialog.bottom - mainDialog.top - (rectWindow.bottom - rectWindow.top)) / 2;
	p2.x = rectWindow.right - rectWindow.left;
	p2.y = rectWindow.bottom - rectWindow.top;

	if (p1.x < 0) p1.x = 0;
	if (p1.y < 0) p1.y = 0;
	if (p1.x + p2.x > cx) p1.x = cx - p2.x;
	if (p1.y + p2.y > cy) p1.y = cy - p2.y;
	MoveWindow(hDlg,p1.x,p1.y, p2.x, p2.y,TRUE);
}




