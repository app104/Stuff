#pragma once

#ifndef _HEAD_UTILS_
#define _HEAD_UTILS_
#include <tchar.h>

#include "Windows.h"
#include "Windowsx.h" // Edit_SetText

//定义使用类似于VB下使用的InputBox
#define INPUT_STRING		0
#define INPUT_INT			1
#define INPUT_DOUBLE		2
#define INPUT_PASSWORD		3
#define INPUT_UPPERCASE		4
#define INPUT_LOWERCASE 	5
int InputBox(HWND hWnd, void* buf, int len, int Type = INPUT_STRING, TCHAR* lpCaption = _TEXT("请输入:"), int Width = 300, int Higth = 100);

//以下是窗口分隔条
#define TYPE_VSPLITER 0   //Vertical Spliter   垂直分隔
#define TYPE_HSPLITER 1   //Horizontal Spliter 水平分隔

#define SPLITER_WIDTH		3 //分隔条宽度
/*
函数Spliter(): 创建一个分隔条
参数：
HWND hParent, 父窗口句柄
int ID, 分隔条的ID
int type, 分隔条类型 TYPE_VSPLITER Vertical Spliter   垂直分隔 或 TYPE_HSPLITER Horizontal Spliter 水平分隔
int rangeA，可以鼠标拖动的范围最左或最上，为0就是不能超出父窗口边框, 父窗口坐标系
int rangeB,可以鼠标拖动的范围最右或最下，为－1就是不能超出父窗口边框, 父窗口坐标系
int posX, 分隔条左边在父窗口的坐标系中的位置X
int posY, 分隔条左边在父窗口的坐标系中的位置Y
int len, 分隔条的长度
返回值：
正常返回分隔条的句柄，
失败返回NULL
*/
HWND Spliter(HWND hParent, int ID, int type, int rangeA, int rangeB, int posX, int posY, int len);

//窗口消息,用于给父窗口发送消息
//向父窗口发送消息时, 结构为 PostMessage(ss->hParent, MSG_VSPLITER, WPARAM(shift), (LPARAM)hWnd);
//4个参数分别为父窗口句柄,消息, 鼠标的偏移量, 分隔条的句柄
//接收窗口改变窗口大小时一定要用GetWindowRect()/MapWindowPoints()来获取包括边框的大小,
#define MSG_VSPLITER		(WM_USER+0x0888)  
#define MSG_HSPLITER		(WM_USER+0x0889)

#define MSG_SPLITERPOS		(WM_USER+0x0890)
#define MSG_SPLITERSTART	(WM_USER+0x0891)
#define MSG_SPLITEREND		(WM_USER+0x0892)
//重新定义分隔条位置,坐标系为父窗口坐标系
#define  Spliter_pos(hWnd,start,end)  (SendMessage((hWnd),MSG_SPLITERPOS,WPARAM(start),LPARAM(end)))
//重新定义分隔条开始的位置,坐标系为父窗口坐标系
#define  Spliter_start(hWnd,new_end)  (SendMessage((hWnd),MSG_SPLITERSTART,WPARAM(new_end),(LPARAM)0))
//重新定义分隔条结束的位置,坐标系为父窗口坐标系
#define  Spliter_end(hWnd,new_end)  (SendMessage((hWnd),MSG_SPLITEREND,WPARAM(new_end),(LPARAM)0))

//以下是侧边栏SideBar 简写成SB
#define TYPE_SBLEFT		0 // 左侧边栏
#define TYPE_SBRIGHT	1 // 右侧边栏
#define TYPE_SBTOP		2 // 上侧边栏
#define TYPE_SBBOTTOM	3 // 下侧边栏
#define TYPE_SBVERTICAL		 TYPE_SBLEFT  //垂直的边栏
#define TYPE_SBHORIZONTAL	 TYPE_SBTOP   //水平的边栏


#define SIDEBAR_WIDTH 25  //定义侧边栏宽度

typedef struct _SB_ELEM_ { //保存侧边栏按钮的信息
	int		ID;       //按钮元素ID
	TCHAR	szName[32]; //按钮元素的名字
	int		szLen;      //按钮元素名字的长度,TCHAR,直接填0,会自己计算的,
	HICON	icon;     //按钮元素的图标ICO
	SIZE	szButton;         //按钮元素的大小.cx,是按钮的长度,cy,是按钮的宽度
}SB_ELEM;
/*
函数Sidebar(): 创建一个侧边栏，其中是一堆按钮，可以改变侧边栏类型
参数：
HWND hParent, 父窗口句柄
int ID, 侧边栏的ID
int type, 侧边栏类型 
SB_ELEM* pse，侧边栏中的按钮元素数据
int num, 侧边栏中的按钮元素的数量
int posX, 侧边栏在父窗口的坐标系中的位置X
int posY, 侧边栏在父窗口的坐标系中的位置Y
int len, 侧边栏的长度
返回值：
正常返回侧边栏的句柄，
失败返回NULL
*/
HWND Sidebar(HWND hParent, int ID, int type, SB_ELEM* pse, int num, int posX, int posY, int len);

//侧边栏的消息MSG_SBBUTTON: WPARAM为TRUE时为发生,为FALSE是为消失, LPARAM为按钮ID
#define MSG_SBBUTTON	(WM_USER+0x0900)

//以下消息不要在目标程序中出现
//设置侧边栏的消息接收句柄
#define MSG_SBRECEIVE	(WM_USER+0x0901)
#define Sidebar_receive(hSidebar,hReceive) (SendMessage((hSidebar),MSG_SBRECEIVE,WPARAM(0),LPARAM(hReceive)))

//设置侧边栏的消息接收句柄, timeout为0时表示不超时, 单位毫秒
#define MSG_SBTIMEOUT	(WM_USER+0x0902)
#define Sidebar_timeout(hSidebar,timeout) (SendMessage((hSidebar),MSG_SBTIMEOUT,WPARAM(0),LPARAM(timeout)))

//改变子窗口位置,提供左上角位置x和y坐标, 坐标系为父窗口的坐标系
#define MSG_SBPOS		(WM_USER+0x0903)
#define Sidebar_pos(hSidebar,xPos, yPos) (SendMessage((hSidebar),MSG_SBPOS,WPARAM(xPos),LPARAM(yPos)))

//改变子窗口长度
#define MSG_SBLEN		(WM_USER+0x0904)
#define Sidebar_len(hSidebar,len) (SendMessage((hSidebar),MSG_SBLEN,WPARAM(0),LPARAM(len)))

//设置侧边栏重置
#define MSG_SBRESET     (WM_USER+0x0906)
#define Sidebar_reset(hSidebar) (SendMessage((hSidebar),MSG_SBRESET,WPARAM(0),LPARAM(0)))

//通用窗口容器
typedef struct _PANE_ELEM_ { //窗口容器包含的子窗口数据
	TCHAR   szName[64]; //子窗口名称
	int		ID;       //子窗口ID,
	HWND	hChild;   //子窗口句柄
	DLGPROC lpDialogFunc;//子窗口回调函数.
	LPVOID	lParam;
	RECT rc; //子窗口可以占用的大小
}PANE_ELEM;
//下面同时只能有一个
#define PANE_SP_LEFT		1
#define PANE_SP_RIGHT		2
#define PANE_SP_TOP			4
#define PANE_SP_BOTTOM		8


#define PANE_MARK			0xFFFFFFF0
/*
函数Pane(): 创建一个通用子窗口容器
参数：
HWND hParent, 父窗口句柄
int ID, 通用子窗口容器的ID
PANE_ELEM* panee, 通用子窗口容器的子窗口的数据
RECT* rc, 通用子窗口容器的大小.
unsigned int mark, 按位表示,	Bit0: 是否有hSpliterLeft(左分隔条)
							Bit1: 是否有hSpliterRight(右分隔条)
							Bit2: 是否有hSpliterTop(上分隔条)
							Bit3: 是否有hSpliterBottom(下分隔条)
							16: 是否有hSidebar(侧边栏, 位于左边)
							32: 是否有hSidebar(侧边栏, 位于右边)
							48: 是否有hSidebar(侧边栏, 位于上边)
							64: 是否有hSidebar(侧边栏, 位于下边)
返回值：
正常返回侧边栏的句柄，
失败返回NULL
*/
HWND Pane(HWND hParent, int ID, PANE_ELEM* ppa, int num, RECT* rc, unsigned int mark);
BOOL CALLBACK PaneExample_Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);//测试


#define MSG_PANE_SB		(WM_USER+0x0950) //设置关联的侧边栏
#define Pane_setsb(hPane, hSidebar) (SendMessage((hPane),MSG_PANE_SB, WPARAM(0),LPARAM(hSidebar)))

																					  
//以下是名叫框架的窗口容器
typedef struct _FRAME_ELEM_ { //窗口容器包含的子窗口数据
	TCHAR   szName[64]; //子窗口名称
	int		ID;       //子窗口ID,
	HWND	hChild;   //子窗口句柄
	DLGPROC lpDialogFunc;//子窗口回调函数.
	LPVOID	lParam;
	RECT rc; //子窗口可以占用的大小
}FRAME_ELEM;
/*
函数Frame(): 创建一个名叫框架的窗口容器
参数：
HWND hParent, 父窗口句柄
int ID, 通用子窗口容器的ID
返回值：
正常返回框架的句柄，
失败返回NULL
*/
HWND Frame(HWND hParent, int ID, RECT* rc);


//以下是Editor
/*
函数Editor(): 创建一个名叫框架的窗口容器
参数：
HWND hParent, 父窗口句柄
int ID, 通用子窗口容器的ID
返回值：
正常返回框架的句柄，
失败返回NULL
*/
HWND Editor(HWND hParent, int ID, TCHAR* ptcFileName, RECT* rc);

//字符串转换的宏函数
#define TcharToChar(tchar,tchar_len,_char,_char_len)\
	WideCharToMultiByte(CP_ACP, 0, (tchar), (tchar_len), (_char), (_char_len), NULL, NULL)
#define CharToTchar(_char,_char_len,tchar,tchar_len) \
	MultiByteToWideChar(CP_ACP, 0, (_char), (_char_len) + 1, tchar, tchar_len)


//子窗口居于父窗口中间
void Dialog_center(HWND hDlg);



#endif // _HEAD_UTILS_


