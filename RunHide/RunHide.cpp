// RunHide.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "RunHide.h"
#include <stdio.h>
#include <direct.h>
#include "shellapi.h"
#pragma warning(disable: 4996 ) 
#define MAX_LOADSTRING 100

// 全局变量: 
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RUNHIDE));
	TCHAR curexename[512];
	GetModuleFileName(hInstance, curexename, 511);
	int iLength;
	//获取字节长度   
	iLength = WideCharToMultiByte(CP_ACP, 0, curexename, -1, NULL, 0, NULL, NULL);
	char* exename = (char*)malloc(iLength + 1);
	if (exename == NULL) return -1;
	WideCharToMultiByte(CP_ACP, 0, curexename, -1, exename, iLength, NULL, NULL);

	int fullstop = (int)strlen(exename);
	for (int i = fullstop - 1; i != 0; i--)
	{
		if ( exename[i] == '.')
		{
			fullstop = i;
			break;
		}
	}
	int backslash = fullstop;
	for (int i = backslash - 1; i != 0; i--)
	{
		if (exename[i] == '\\')
		{
			backslash = i;
			break;
		}
	}
	char name[MAX_PATH] = {};
	memcpy(name, exename + backslash+1, fullstop - backslash -1);
	char path[MAX_PATH] = {};
	memcpy(path, exename, backslash);
	free(exename);
	chdir(path);
	char ini[MAX_PATH] = {};
	sprintf(ini, "%s.ini", name);
	FILE* fp = fopen(ini, "r");
	if (fp == NULL)
	{
		char note[MAX_PATH];
		sprintf(note, "请在%s.exe文件夹下新建包含目标程序路径的%s.ini文件", name, name);
		MessageBoxA(NULL, note, "错误", MB_OK | MB_ICONERROR);
		return -1;
	}
	char cmd[MAX_PATH] = {};
	fread(cmd, 1, MAX_PATH, fp);

	fullstop = (int)strlen(cmd);
	backslash = fullstop;
	for (int i = backslash - 1; i != 0; i--)
	{
		if (cmd[i] == '\\')
		{
			backslash = i;
			break;
		}
	}
	if (backslash != fullstop)
	{
		strncpy(path, cmd, backslash);
		strcpy(name, cmd + backslash +1);
		chdir(path);
	}
	else
	{
		strcpy(name, cmd);
	}

	int retval = WinExec(name, SW_HIDE);

    return (int)retval;
}
