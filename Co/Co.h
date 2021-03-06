#pragma once

#include "resource.h"

#define MAX_LOADSTRING 100

//Main_window.cpp
extern HINSTANCE hInst;                                // 当前实例
extern WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
extern WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

												// 此代码模块中包含的函数的前向声明: 
BOOL Main_init(HINSTANCE hInstance, int nCmdShow);

//Dialog_new.cpp
// “新建通道”框的消息处理程序。
INT_PTR CALLBACK Dialog_new_proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


//CTU.cpp
//显示TCP_UDP的控件,CTU就是 Controol TCP/UDP
HWND CTU(HWND hParent, int x, int y, int Width, int Higth);