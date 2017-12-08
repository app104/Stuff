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

//定义窗口分隔条
#define TYPE_VSPLITER 0   //Vertical Spliter   垂直分隔
#define TYPE_HSPLITER 1   //Horizontal Spliter 水平分隔
//窗口消息,用于给父窗口发送消息
//向父窗口发送消息时, 结构为 PostMessage(ss->hParent, MSG_VSPLITER, WPARAM(shift), (LPARAM)hWnd);
//4个参数分别为父窗口句柄,消息, 鼠标的偏移量, 分隔条的句柄
#define MSG_VSPLITER		(WM_USER+0x0888)  
#define MSG_HSPLITER		(WM_USER+0x0889)

HWND Spliter(HWND hParent, int type, int x, int y, int Width, int Higth);
#define MSG_SPLITERSTART	(WM_USER+0x0890)
#define MSG_SPLITEREND		(WM_USER+0x0891)
#define  Spliter_start(hWnd,new_end)  (SendMessage((hWnd),MSG_SPLITERSTART,WPARAM(new_end),(LPARAM)0))
#define  Spliter_end(hWnd,new_end)  (SendMessage((hWnd),MSG_SPLITEREND,WPARAM(new_end),(LPARAM)0))
#endif // _HEAD_UTILS_
