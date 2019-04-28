#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <Commctrl.h>
#include "SkinH.h"
#include "resource.h"
//静态链接库
#pragma comment(lib,"WINMM.LIB")
#pragma comment(lib,"SkinHu.lib")

#define TIMEID 10
#define MENUADD 101
#define MENUDEL 102
#define MENUCLE 103

//全局变量
TCHAR currentMusic[MAX_PATH] = L"";//当前播放的音乐路径
int musicDurationI = 0;//当前音乐长度(毫秒计)
HMENU hMenu;//右键菜单
HBITMAP hBmp;//位图
TCHAR appName[10] = L"music";
TCHAR iniPath[MAX_PATH] = L"D:\\qinqinMusicPlayerList.ini";

//消息分流器
BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
void Cls_OnClose(HWND hwnd);
void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

//播放音乐相关函数
void Start(HWND hwnd);//开始

void Pause(HWND hwnd);//暂停

void PlayMusic(TCHAR* musicPath);//播放歌曲

void PlayMusicFromPause(TCHAR* musicPath, int pos);//从某处开始播放

void StopMusic(TCHAR* musicPath);//停止播放

void PauseMusic(TCHAR* musicPath);//暂停播放

void ResumeMusic(TCHAR* musicPath);//恢复播放

int GetMusicLenth(TCHAR* musicPath);//返回歌曲毫秒数

int GetMusicPos(TCHAR* musicPath);//获取播放进度

void AddMusics(HWND hwnd);//添加歌曲

void CutSuffix(TCHAR *p);//去除后缀名

void DeleteMusics(HWND hwnd);

void ClearMusiclist(HWND hwnd);


