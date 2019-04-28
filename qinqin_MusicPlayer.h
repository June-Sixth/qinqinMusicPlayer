#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <Commctrl.h>
#include "SkinH.h"
#include "resource.h"
//��̬���ӿ�
#pragma comment(lib,"WINMM.LIB")
#pragma comment(lib,"SkinHu.lib")

#define TIMEID 10
#define MENUADD 101
#define MENUDEL 102
#define MENUCLE 103

//ȫ�ֱ���
TCHAR currentMusic[MAX_PATH] = L"";//��ǰ���ŵ�����·��
int musicDurationI = 0;//��ǰ���ֳ���(�����)
HMENU hMenu;//�Ҽ��˵�
HBITMAP hBmp;//λͼ
TCHAR appName[10] = L"music";
TCHAR iniPath[MAX_PATH] = L"D:\\qinqinMusicPlayerList.ini";

//��Ϣ������
BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void Cls_OnClose(HWND hwnd);
void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

//����������غ���
void Start(HWND hwnd);//��ʼ

void Pause(HWND hwnd);//��ͣ

void PlayMusic(TCHAR* musicPath);//���Ÿ���

void PlayMusicFromPause(TCHAR* musicPath, int pos);//��ĳ����ʼ����

void StopMusic(TCHAR* musicPath);//ֹͣ����

void PauseMusic(TCHAR* musicPath);//��ͣ����

void ResumeMusic(TCHAR* musicPath);//�ָ�����

int GetMusicLenth(TCHAR* musicPath);//���ظ���������

int GetMusicPos(TCHAR* musicPath);//��ȡ���Ž���

void AddMusics(HWND hwnd);//��Ӹ���

void CutSuffix(TCHAR *p);//ȥ����׺��

void DeleteMusics(HWND hwnd);

void ClearMusiclist(HWND hwnd);


