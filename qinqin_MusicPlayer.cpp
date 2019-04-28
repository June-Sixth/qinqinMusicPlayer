#include "qinqin_MusicPlayer.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ListBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
WNDPROC DefulatProc;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}

//主窗口回调函数
BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HICON hIcon;
	TCHAR musicPos[10] = L"";
	HDC hdcMem;
	RECT listBoxRect;

	switch (message)
	{
		HANDLE_MSG(hDlg, WM_INITDIALOG, Cls_OnInitDialog);
		HANDLE_MSG(hDlg, WM_COMMAND, Cls_OnCommand);
		HANDLE_MSG(hDlg, WM_CLOSE, Cls_OnClose);

	case WM_CTLCOLORLISTBOX:
	{
		GetClientRect((HWND)lParam, &listBoxRect);
		hdcMem = CreateCompatibleDC((HDC)wParam);
		SelectObject(hdcMem, hBmp);
		BitBlt((HDC)wParam, 0, 0, listBoxRect.right, listBoxRect.bottom, hdcMem, 0, 0, SRCCOPY);
		DeleteDC(hdcMem);

		SetTextColor((HDC)wParam, RGB(66, 88, 88));
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (BOOL)((HBRUSH)GetStockObject(NULL_BRUSH));
	}
		break;
	case  WM_TIMER://定时器
	{
		int pos =GetMusicPos(currentMusic)/1000;
		wsprintf(musicPos, L"%02d:%02d", pos / 60, pos % 60);
		SetWindowText(GetDlgItem(hDlg, ID_TIME), musicPos);
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_SETPOS, TRUE, pos);
		if (pos == musicDurationI)//歌曲播放完
		{
			KillTimer(hDlg, TIMEID);
			lstrcpy(currentMusic, L"");
		}
	}
		break;
	case WM_HSCROLL://进度条移动
	{
		StopMusic(currentMusic);
		int pos = SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_GETPOS, 0, 0);
		PlayMusicFromPause(currentMusic, pos * 1000);
	}
		break;
	case WM_CONTEXTMENU://右键菜单，以屏幕坐标为准
	{
		if (wParam == (WPARAM)GetDlgItem(hDlg, IDC_LIST1))
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			TrackPopupMenu(hMenu, TPM_LEFTALIGN|TPM_TOPALIGN, xPos, yPos, 3, hDlg, NULL);//鼠标点为菜单左上点
		}
	}
		break;
	//case WM_KEYDOWN:
	//	MessageBox(hDlg, L"", L"", MB_OK);
	//	break;
	}
	return FALSE;
}

LRESULT CALLBACK ListBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN:
	case WM_VSCROLL:
	case WM_MOUSEWHEEL:
		InvalidateRect(hwnd, NULL, FALSE);
		break;
	}
	return DefulatProc(hwnd, message, wParam, lParam);
}

//初始化窗口
BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	//加载皮肤库
	SkinH_Attach();
	//SkinH_AttachEx(
	//	(LPCTSTR)L"E:\\qinqin_MusicPlayer\\qinqin_MusicPlayer\\mmmmm.she",	//皮肤文件路径
	//	NULL		//皮肤密钥，免费版有logo
	//	);
	SkinH_DetachEx(GetDlgItem(hwnd, IDC_LIST1));//卸载skinsharp对listbox的换肤
	//加载背景
	HINSTANCE hinstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
	hBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BITMAP1));
	//SetWindowLong  (子类化窗口)
	DefulatProc = (WNDPROC)SetWindowLong(GetDlgItem(hwnd, IDC_LIST1), GWL_WNDPROC, (LONG)ListBoxProc);

	//加载图标
	HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	SetClassLong(hwnd, GCL_HICON, (long)hIcon);  //SetWindowLong  (子类化窗口）

	//创建右键菜单
	hMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING, MENUADD, L"添加歌曲");
	AppendMenu(hMenu, MF_STRING, MENUDEL, L"删除歌曲");
	AppendMenu(hMenu, MF_STRING, MENUCLE, L"清空列表");
	//从ini文件中取出歌曲列表和路径
	TCHAR musicNames[MAX_PATH] = L"";
	GetPrivateProfileString(appName, NULL, NULL, musicNames, MAX_PATH, iniPath);
	TCHAR *p = musicNames;
	while (*p)
	{
		SendMessage(GetDlgItem(hwnd, IDC_LIST1), LB_INSERTSTRING, -1, (LPARAM)p);
		p = p + lstrlen(p) + 1;
	}
	return TRUE;
}

void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case ID_START://点击开始
		Start(hwnd);
		break;
	case ID_PAUSE://点击暂停
		Pause(hwnd);
		break;
	case IDC_LIST1://响应列表上的双击
	{
		if (codeNotify == LBN_DBLCLK)
		Start(hwnd);
	}
		break;
	case MENUADD:
		AddMusics(hwnd);
		break;
	case MENUDEL:
		DeleteMusics(hwnd);
		break;
	case MENUCLE:
		if (MessageBox(hwnd, L"确定清空列表?", L"清空列表", MB_OKCANCEL) == IDOK)
			ClearMusiclist(hwnd);
		break;
	default:
		break;
	}
}

void Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

void Start(HWND hwnd)
{
	TCHAR musicLongPath[MAX_PATH] = L"";
	TCHAR musicSortPath[MAX_PATH] = L"";
	TCHAR buttonText[3] = L"";
	TCHAR musicLen[10] = L"";
	TCHAR musicName[50] = L"";

	Button_GetText(GetDlgItem(hwnd, ID_PAUSE), buttonText, 4);	//从PAUSE按钮上取字符
	int ret = SendMessage(GetDlgItem(hwnd, IDC_LIST1), LB_GETCURSEL, 0, 0);
	if (ret != LB_ERR)
	{
		if (lstrlen(currentMusic) != 0)
		{
			StopMusic(currentMusic);
		}
		if (!lstrcmp(buttonText, L"继续"))
		{
			Button_SetText(GetDlgItem(hwnd, ID_PAUSE), L"暂停");
		}
		//从listbox读取歌曲名
		SendMessage(GetDlgItem(hwnd, IDC_LIST1), LB_GETTEXT, ret, (LPARAM)musicName);
		//从ini文件读取路径
		GetPrivateProfileString(appName, musicName, NULL, musicLongPath, MAX_PATH, iniPath);
		GetShortPathName(musicLongPath, musicSortPath, MAX_PATH);
		PlayMusic(musicSortPath);
		lstrcpy(currentMusic, musicSortPath);
		musicDurationI = GetMusicLenth(currentMusic) / 1000;//歌曲秒数
		wsprintf(musicLen, L"%02d:%02d", musicDurationI / 60, musicDurationI % 60);
		SetWindowText(GetDlgItem(hwnd, ID_LENTH), musicLen);
		SetTimer(hwnd, TIMEID, 500, NULL);//设置定时器
		SendMessage(GetDlgItem(hwnd, IDC_SLIDER1), TBM_SETRANGE, TRUE, MAKELONG(0, musicDurationI));//MAKELONG带参宏将两个数连成一个
	}
	else
	{
		MessageBox(hwnd, L"请选择要播放的歌曲", L"", MB_OK);
	}
	//MessageBox(hwnd, "", "", 0);
}


void Pause(HWND hwnd)
{
	TCHAR buttonText[3] = L"";
	Button_GetText(GetDlgItem(hwnd, ID_PAUSE), buttonText, 4);	//从PAUSE按钮上取字符

	if (!lstrcmp(buttonText, L"暂停"))//相等返回0
	{
		PauseMusic(currentMusic);
		Button_SetText(GetDlgItem(hwnd, ID_PAUSE), L"继续");
	}
	else if (!lstrcmp(buttonText, L"继续"))
	{
		ResumeMusic(currentMusic);
		Button_SetText(GetDlgItem(hwnd, ID_PAUSE), L"暂停");
	}
}

void PlayMusic(TCHAR* musicPath)
{
	TCHAR order[MAX_PATH] = L"";
	wsprintf(order, L"open %s", musicPath);
	mciSendString(order, NULL, 0, NULL);
	wsprintf(order, L"play %s", musicPath);
	mciSendString(order, NULL, 0, NULL);
}

void PlayMusicFromPause(TCHAR* musicPath,int pos)
{
	TCHAR order[MAX_PATH] = L"";
	wsprintf(order, L"play %s from %d", musicPath, pos);
	mciSendString(order, NULL, 0, NULL);
}

void StopMusic(TCHAR* musicPath)
{
	TCHAR order[MAX_PATH] = L"";
	wsprintf(order, L"stop %s", musicPath);
	mciSendString(order, NULL, 0, NULL);
	wsprintf(order, L"close %s", musicPath);
	mciSendString(order, NULL, 0, NULL);
}

void PauseMusic(TCHAR* musicPath)
{
	TCHAR order[MAX_PATH] = L"";
	wsprintf(order, L"pause %s", musicPath);
	mciSendString(order, NULL, 0, NULL);
}

void ResumeMusic(TCHAR* musicPath)
{
	TCHAR order[MAX_PATH] = L"";
	wsprintf(order, L"resume %s", musicPath);
	mciSendString(order, NULL, 0, NULL);
}

int GetMusicLenth(TCHAR* musicPath)//返回毫秒
{
	TCHAR order[MAX_PATH] = L"";
	TCHAR len[10] = L"";
	wsprintf(order, L"status %s length", musicPath);
	mciSendString(order, len, sizeof(len), NULL);
	return _wtoi(len);//字符串转换为整形数据，在Unicode下为_wtoi(),在多字节字符集中为atoi()
}

int GetMusicPos(TCHAR* musicPath)//返回位置
{
	TCHAR order[MAX_PATH] = L"";
	TCHAR pos[10] = L"";
	wsprintf(order, L"status %s position", musicPath);
	mciSendString(order, pos, sizeof(pos), NULL);
	return _wtoi(pos);
}

//打开音乐文件
void AddMusics(HWND hwnd)
{
	OPENFILENAME ofn;//文件结构体
	TCHAR szFileName[MAX_PATH * 30];//总路径，支持多选
	TCHAR musicDir[MAX_PATH] = L"";//歌曲文件目录路径
	TCHAR musicName[50] = L"";//歌曲名

	//清空
	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory(&szFileName, sizeof(szFileName));

	ofn.lStructSize = sizeof(ofn);//大小
	ofn.hwndOwner = hwnd;//父窗口句柄
	ofn.lpstrFilter = TEXT("All Files (*.*)\0*.*\0MP3文件 (*.mp3)\0*.mp3\0WAV文件 (*.wav)\0*.wav\0";);//只能打开的文件类型
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT;//允许多选
	ofn.lpstrInitialDir = TEXT("C:\\qinqinMusicPlayerList\\");     //默认的打开的文件路径

	TCHAR *p;
	if (GetOpenFileName(&ofn))
	{
		//lstrcat(musicDir, szFileName);//拷贝出目录
		for (int i = 0; i < ofn.nFileOffset-1; i++)
			musicDir[i] = szFileName[i];
		p = szFileName + ofn.nFileOffset;//指针指向路径后面一个字符
		lstrcat(musicDir, L"\\");//路径末尾加"\"
		while (*p)
		{
			TCHAR musicPath[MAX_PATH] = L"";
			lstrcpy(musicPath, musicDir);
			lstrcat(musicPath, p);
			lstrcpy(musicName, p);
			CutSuffix(musicName);
			SendMessage(GetDlgItem(hwnd, IDC_LIST1), LB_INSERTSTRING, -1, (LPARAM)musicName);
			//将对应歌曲名的路径存入ini文件
			WritePrivateProfileString(appName, musicName, musicPath, iniPath);
			p = p + lstrlen(p) + 1;
		}
	}
}
//去除字符数组中的后缀名
void CutSuffix(TCHAR *pName)
{
	int i = lstrlen(pName);
	TCHAR* p = pName + i;
	while (p > pName)
	{
		if (*p == TEXT('.'))
		{
			*p = TEXT('\0');
			break;
		}
		p--;
	}
}
//删除音乐
void DeleteMusics(HWND hwnd)
{
	TCHAR musicName[50] = L"";
	int sel = SendMessage(GetDlgItem(hwnd, IDC_LIST1), LB_GETCURSEL, 0, 0);
	if (sel > -1)
	{
		SendMessage(GetDlgItem(hwnd, IDC_LIST1), LB_GETTEXT, (WPARAM)sel, (LPARAM)musicName);
		SendMessage(GetDlgItem(hwnd, IDC_LIST1), LB_DELETESTRING, sel, 0);
		WritePrivateProfileString(appName, musicName, NULL, iniPath);
	}
	else
	{
		MessageBox(hwnd, L"请选择要删除的歌曲", L"", MB_OK);
	}
}

void ClearMusiclist(HWND hwnd)
{
	SendMessage(GetDlgItem(hwnd, IDC_LIST1), LB_RESETCONTENT, 0, 0);
	WritePrivateProfileString(appName, NULL, NULL, iniPath);
}
