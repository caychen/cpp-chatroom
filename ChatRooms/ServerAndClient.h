#pragma  once
#include "resource.h"

#define  MAX_BUF_SIZE	1024
#define  WM_TRAYICON_MSG (WM_USER + 1)

class CChatRoomsDlg;

class CClientItem
{
public:
	CString m_strIp;
	SOCKET m_Socket;
	HANDLE hThread;
	CChatRoomsDlg *m_pMainWnd;

	CClientItem()
	{
		m_pMainWnd = NULL;
		m_Socket = INVALID_SOCKET;
		hThread = NULL;
	}
};

BOOL SOCKET_select(SOCKET hSocket,int nTimeOut = 100,BOOL bRead = FALSE);

DWORD WINAPI ListenThreadProc(LPVOID lpParameter);

DWORD WINAPI ClientThreadProc(LPVOID lpParameter);

DWORD WINAPI ConnectThreadProc(LPVOID lpParameter);