#include "qinqin_MusicPlayer.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ListBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
WNDPROC DefulatProc;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,PSTR szCmdLine, int iCmdShow)
{
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}

//�����ڻص�����
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
	case  WM_TIMER://��ʱ��
	{
		int pos =GetMusicPos(currentMusic)/1000;
		wsprintf(musicPos, L"%02d:%02d", pos / 60, pos % 60);
		SetWindowText(GetDlgItem(hDlg, ID_TIME), musicPos);
		SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_SETPOS, TRUE, pos);
		if (pos == musicDurationI)//����������
		{
			KillTimer(hDlg, TIMEID);
			lstrcpy(currentMusic, L"");
		}
	}
		break;
	case WM_HSCROLL://�������ƶ�
	{
		StopMusic(currentMusic);
		int pos = SendMessage(GetDlgItem(hDlg, IDC_SLIDER1), TBM_GETPOS, 0, 0);
		PlayMusicFromPause(currentMusic, pos * 1000);
	}
		break;
	case WM_CONTEXTMENU://�Ҽ��˵�������Ļ����Ϊ׼
	{
		if (wParam == (WPARAM)GetDlgItem(hDlg, IDC_LIST1))
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			TrackPopupMenu(hMenu, TPM_LEFTALIGN|TPM_TOPALIGN, xPos, yPos, 3, hDlg, NULL);//����Ϊ�˵����ϵ�
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

//��ʼ������
BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
	//����Ƥ����
	SkinH_Attach();
	//SkinH_AttachEx(
	//	(LPCTSTR)L"E:\\qinqin_MusicPlayer\\qinqin_MusicPlayer\\mmmmm.she",	//Ƥ���ļ�·��
	//	NULL		//Ƥ����Կ����Ѱ���logo
	//	);
	SkinH_DetachEx(GetDlgItem(hwnd, IDC_LIST1));//ж��skinsharp��listbox�Ļ���
	//���ر���
	HINSTANCE hinstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);
	hBmp = LoadBitmap(hinstance, MAKEINTRESOURCE(IDB_BITMAP1));
	//SetWindowLong  (���໯����)
	DefulatProc = (WNDPROC)SetWindowLong(GetDlgItem(hwnd, IDC_LIST1), GWL_WNDPROC, (LONG)ListBoxProc);

	//����ͼ��
	HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	SetClassLong(hwnd, GCL_HICON, (long)hIcon);  //SetWindowLong  (���໯���ڣ�

	//�����Ҽ��˵�
	hMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING, MENUADD, L"��Ӹ���");
	AppendMenu(hMenu, MF_STRING, MENUDEL, L"ɾ������");
	AppendMenu(hMenu, MF_STRING, MENUCLE, L"����б�");
	//��ini�ļ���ȡ�������б��·��
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
	case ID_START://�����ʼ
		Start(hwnd);
		break;
	case ID_PAUSE://�����ͣ
		Pause(hwnd);
		break;
	case IDC_LIST1://��Ӧ�б��ϵ�˫��
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
		if (MessageBox(hwnd, L"ȷ������б�?", L"����б�", MB_OKCANCEL) == IDOK)
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

	Button_GetText(GetDlgItem(hwnd, ID_PAUSE), buttonText, 4);	//��PAUSE��ť��ȡ�ַ�
	int ret = SendMessage(GetDlgItem(hwnd, IDC_LIST1), LB_GETCURSEL, 0, 0);
	if (ret != LB_ERR)
	{
		if (lstrlen(currentMusic) != 0)
		{
			StopMusic(currentMusic);
		}
		if (!lstrcmp(buttonText, L"����"))
		{
			Button_SetText(GetDlgItem(hwnd, ID_PAUSE), L"��ͣ");
		}
		//��listbox��ȡ������
		SendMessage(GetDlgItem(hwnd, IDC_LIST1), LB_GETTEXT, ret, (LPARAM)musicName);
		//��ini�ļ���ȡ·��
		GetPrivateProfileString(appName, musicName, NULL, musicLongPath, MAX_PATH, iniPath);
		GetShortPathName(musicLongPath, musicSortPath, MAX_PATH);
		PlayMusic(musicSortPath);
		lstrcpy(currentMusic, musicSortPath);
		musicDurationI = GetMusicLenth(currentMusic) / 1000;//��������
		wsprintf(musicLen, L"%02d:%02d", musicDurationI / 60, musicDurationI % 60);
		SetWindowText(GetDlgItem(hwnd, ID_LENTH), musicLen);
		SetTimer(hwnd, TIMEID, 500, NULL);//���ö�ʱ��
		SendMessage(GetDlgItem(hwnd, IDC_SLIDER1), TBM_SETRANGE, TRUE, MAKELONG(0, musicDurationI));//MAKELONG���κ꽫����������һ��
	}
	else
	{
		MessageBox(hwnd, L"��ѡ��Ҫ���ŵĸ���", L"", MB_OK);
	}
	//MessageBox(hwnd, "", "", 0);
}


void Pause(HWND hwnd)
{
	TCHAR buttonText[3] = L"";
	Button_GetText(GetDlgItem(hwnd, ID_PAUSE), buttonText, 4);	//��PAUSE��ť��ȡ�ַ�

	if (!lstrcmp(buttonText, L"��ͣ"))//��ȷ���0
	{
		PauseMusic(currentMusic);
		Button_SetText(GetDlgItem(hwnd, ID_PAUSE), L"����");
	}
	else if (!lstrcmp(buttonText, L"����"))
	{
		ResumeMusic(currentMusic);
		Button_SetText(GetDlgItem(hwnd, ID_PAUSE), L"��ͣ");
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

int GetMusicLenth(TCHAR* musicPath)//���غ���
{
	TCHAR order[MAX_PATH] = L"";
	TCHAR len[10] = L"";
	wsprintf(order, L"status %s length", musicPath);
	mciSendString(order, len, sizeof(len), NULL);
	return _wtoi(len);//�ַ���ת��Ϊ�������ݣ���Unicode��Ϊ_wtoi(),�ڶ��ֽ��ַ�����Ϊatoi()
}

int GetMusicPos(TCHAR* musicPath)//����λ��
{
	TCHAR order[MAX_PATH] = L"";
	TCHAR pos[10] = L"";
	wsprintf(order, L"status %s position", musicPath);
	mciSendString(order, pos, sizeof(pos), NULL);
	return _wtoi(pos);
}

//�������ļ�
void AddMusics(HWND hwnd)
{
	OPENFILENAME ofn;//�ļ��ṹ��
	TCHAR szFileName[MAX_PATH * 30];//��·����֧�ֶ�ѡ
	TCHAR musicDir[MAX_PATH] = L"";//�����ļ�Ŀ¼·��
	TCHAR musicName[50] = L"";//������

	//���
	ZeroMemory(&ofn, sizeof(ofn));
	ZeroMemory(&szFileName, sizeof(szFileName));

	ofn.lStructSize = sizeof(ofn);//��С
	ofn.hwndOwner = hwnd;//�����ھ��
	ofn.lpstrFilter = TEXT("All Files (*.*)\0*.*\0MP3�ļ� (*.mp3)\0*.mp3\0WAV�ļ� (*.wav)\0*.wav\0";);//ֻ�ܴ򿪵��ļ�����
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = sizeof(szFileName);
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT;//�����ѡ
	ofn.lpstrInitialDir = TEXT("C:\\qinqinMusicPlayerList\\");     //Ĭ�ϵĴ򿪵��ļ�·��

	TCHAR *p;
	if (GetOpenFileName(&ofn))
	{
		//lstrcat(musicDir, szFileName);//������Ŀ¼
		for (int i = 0; i < ofn.nFileOffset-1; i++)
			musicDir[i] = szFileName[i];
		p = szFileName + ofn.nFileOffset;//ָ��ָ��·������һ���ַ�
		lstrcat(musicDir, L"\\");//·��ĩβ��"\"
		while (*p)
		{
			TCHAR musicPath[MAX_PATH] = L"";
			lstrcpy(musicPath, musicDir);
			lstrcat(musicPath, p);
			lstrcpy(musicName, p);
			CutSuffix(musicName);
			SendMessage(GetDlgItem(hwnd, IDC_LIST1), LB_INSERTSTRING, -1, (LPARAM)musicName);
			//����Ӧ��������·������ini�ļ�
			WritePrivateProfileString(appName, musicName, musicPath, iniPath);
			p = p + lstrlen(p) + 1;
		}
	}
}
//ȥ���ַ������еĺ�׺��
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
//ɾ������
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
		MessageBox(hwnd, L"��ѡ��Ҫɾ���ĸ���", L"", MB_OK);
	}
}

void ClearMusiclist(HWND hwnd)
{
	SendMessage(GetDlgItem(hwnd, IDC_LIST1), LB_RESETCONTENT, 0, 0);
	WritePrivateProfileString(appName, NULL, NULL, iniPath);
}
