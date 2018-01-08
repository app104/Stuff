#pragma once

#ifndef _HEAD_UTILS_
#define _HEAD_UTILS_
#include <tchar.h>

#include "Windows.h"
#include "Windowsx.h" // Edit_SetText

//����ʹ��������VB��ʹ�õ�InputBox
#define INPUT_STRING		0
#define INPUT_INT			1
#define INPUT_DOUBLE		2
#define INPUT_PASSWORD		3
#define INPUT_UPPERCASE		4
#define INPUT_LOWERCASE 	5
int InputBox(HWND hWnd, void* buf, int len, int Type = INPUT_STRING, TCHAR* lpCaption = _TEXT("������:"), int Width = 300, int Higth = 100);

//�����Ǵ��ڷָ���
#define TYPE_VSPLITER 0   //Vertical Spliter   ��ֱ�ָ�
#define TYPE_HSPLITER 1   //Horizontal Spliter ˮƽ�ָ�

#define SPLITER_WIDTH		5 //�ָ������
/*
����Spliter(): ����һ���ָ���
������
HWND hParent, �����ھ��
int ID, �ָ�����ID
int type, �ָ������� TYPE_VSPLITER Vertical Spliter   ��ֱ�ָ� �� TYPE_HSPLITER Horizontal Spliter ˮƽ�ָ�
int rangeA����������϶��ķ�Χ��������ϣ�Ϊ0���ǲ��ܳ��������ڱ߿�
int rangeB,��������϶��ķ�Χ���һ����£�Ϊ��1���ǲ��ܳ��������ڱ߿�
int posX, �ָ�������ڸ����ڵ�����ϵ�е�λ��X
int posY, �ָ�������ڸ����ڵ�����ϵ�е�λ��Y
int len, �ָ����ĳ���
����ֵ��
�������طָ����ľ����
ʧ�ܷ���NULL
*/
HWND Spliter(HWND hParent, int ID, int type, int rangeA, int rangeB, int posX, int posY, int len);

//������Ϣ,���ڸ������ڷ�����Ϣ
//�򸸴��ڷ�����Ϣʱ, �ṹΪ PostMessage(ss->hParent, MSG_VSPLITER, WPARAM(shift), (LPARAM)hWnd);
//4�������ֱ�Ϊ�����ھ��,��Ϣ, ����ƫ����, �ָ����ľ��
#define MSG_VSPLITER		(WM_USER+0x0888)  
#define MSG_HSPLITER		(WM_USER+0x0889)

#define MSG_SPLITERPOS		(WM_USER+0x0890)
#define MSG_SPLITERSTART	(WM_USER+0x0891)
#define MSG_SPLITEREND		(WM_USER+0x0892)
//���¶���ָ���λ��,����ϵΪ����������ϵ
#define  Spliter_pos(hWnd,start,end)  (SendMessage((hWnd),MSG_SPLITERPOS,WPARAM(start),LPARAM(end)))
//���¶���ָ�����ʼ��λ��,����ϵΪ����������ϵ
#define  Spliter_start(hWnd,new_end)  (SendMessage((hWnd),MSG_SPLITERSTART,WPARAM(new_end),(LPARAM)0))
//���¶���ָ���������λ��,����ϵΪ����������ϵ
#define  Spliter_end(hWnd,new_end)  (SendMessage((hWnd),MSG_SPLITEREND,WPARAM(new_end),(LPARAM)0))

//�����ǲ����SideBar ��д��SB
#define TYPE_SBLEFT		0 // ������
#define TYPE_SBRIGHT	1 // �Ҳ����
#define TYPE_SBTOP		2 // �ϲ����
#define TYPE_SBBOTTOM	3 // �²����
#define TYPE_SBVERTICAL		 TYPE_SBLEFT  //��ֱ�ı���
#define TYPE_SBHORIZONTAL	 TYPE_SBTOP   //ˮƽ�ı���


#define SIDEBAR_WIDTH 25  //�����������

typedef struct _SB_ELEM_ { //����������ť����Ϣ
	int		ID;       //��ťԪ��ID
	TCHAR	szName[32]; //��ťԪ�ص�����
	int		szLen;      //��ťԪ�����ֵĳ���,TCHAR,ֱ����0,���Լ������,
	HICON	icon;     //��ťԪ�ص�ͼ��ICO
	SIZE	szButton;         //��ťԪ�صĴ�С.cx,�ǰ�ť�ĳ���,cy,�ǰ�ť�Ŀ��
}SB_ELEM;
/*
����Sidebar(): ����һ���������������һ�Ѱ�ť�����Ըı���������
������
HWND hParent, �����ھ��
int ID, �������ID
int type, ��������� 
SB_ELEM* pse��������еİ�ťԪ������
int num, ������еİ�ťԪ�ص�����
int posX, ������ڸ����ڵ�����ϵ�е�λ��X
int posY, ������ڸ����ڵ�����ϵ�е�λ��Y
int len, ������ĳ���
����ֵ��
�������ز�����ľ����
ʧ�ܷ���NULL
*/
HWND Sidebar(HWND hParent, int ID, int type, SB_ELEM* pse, int num, int posX, int posY, int len);

//���������ϢMSG_SBBUTTON: WPARAMһֱΪ0, LPARAMΪ��ťID
#define MSG_SBBUTTON	(WM_USER+0x0900)

//���ò��������Ϣ���վ��
#define MSG_SBRECEIVE	(WM_USER+0x0901)
#define Sidebar_receive(hSidebar,hReceive) (SendMessage((hSidebar),MSG_SBRECEIVE,WPARAM(0),LPARAM(hReceive)))

//���ò��������Ϣ���վ��, timeoutΪ0ʱ��ʾ����ʱ, ��λ����
#define MSG_SBTIMEOUT	(WM_USER+0x0902)
#define Sidebar_timeout(hSidebar,timeout) (SendMessage((hSidebar),MSG_SBTIMEOUT,WPARAM(0),LPARAM(timeout)))

//�ı��Ӵ���λ��,�ṩ���Ͻ�λ��x��y����, ����ϵΪ�����ڵ�����ϵ
#define MSG_SBPOS		(WM_USER+0x0903)
#define Sidebar_pos(hSidebar,xPos, yPos) (SendMessage((hSidebar),MSG_SBPOS,WPARAM(xPos),LPARAM(yPos)))

//�ı��Ӵ��ڳ���
#define MSG_SBLEN		(WM_USER+0x0904)
#define Sidebar_len(hSidebar,len) (SendMessage((hSidebar),MSG_SBLEN,WPARAM(0),LPARAM(len)))

//���ò��������Ϣ���վ��, timeoutΪ0ʱ��ʾ����ʱ, ��λ����
#define MSG_SBTIMEOUT	(WM_USER+0x0902)
#define Sidebar_timeout(hSidebar,timeout) (SendMessage((hSidebar),MSG_SBTIMEOUT,WPARAM(0),LPARAM(timeout)))


//ͨ�ô�������
typedef struct _PANE_ELEM_ { //���������������Ӵ�������
	int		ID;       //�Ӵ���ID,
	HWND	hChild;   //�Ӵ��ھ��
	DLGPROC lpDialogFunc;//�Ӵ��ڻص�����.
	LPVOID	lParam;
	RECT rc; //�Ӵ��ڿ���ռ�õĴ�С
}PANE_ELEM;
/*
����Pane(): ����һ��ͨ���Ӵ�������
������
HWND hParent, �����ھ��
int ID, ͨ���Ӵ���������ID
PANE_ELEM* panee, ͨ���Ӵ����������Ӵ��ڵ�����
RECT* rc, ͨ���Ӵ��������Ĵ�С.
����ֵ��
�������ز�����ľ����
ʧ�ܷ���NULL
*/
HWND Pane(HWND hParent, int ID, PANE_ELEM* panee, int num, RECT* rc);
BOOL CALLBACK PaneExample_Proc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);//����

//�ַ���ת���ĺ꺯��
#define TcharToChar(tchar,tchar_len,_char,_char_len)\
	WideCharToMultiByte(CP_ACP, 0, (tchar), (tchar_len), (_char), (_char_len), NULL, NULL)
#define CharToTchar(_char,_char_len,tchar,tchar_len) \
	MultiByteToWideChar(CP_ACP, 0, (_char), (_char_len) + 1, tchar, tchar_len)


//�Ӵ��ھ��ڸ������м�
void Dialog_center(HWND hDlg);

#endif // _HEAD_UTILS_
