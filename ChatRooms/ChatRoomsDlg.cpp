// ChatRoomsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ChatRooms.h"
#include "ChatRoomsDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMenuShow();
public:
	afx_msg void OnMenuQuit();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)

END_MESSAGE_MAP()


// CChatRoomsDlg dialog




CChatRoomsDlg::CChatRoomsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChatRoomsDlg::IDD, pParent)
	, m_bShowAll(FALSE)
	, m_hListenThread(NULL)
	, m_hConnectThread(NULL)
	, m_ListenSocket(INVALID_SOCKET)
	, m_ConnectSock(INVALID_SOCKET)
	, m_bIsServer(-1)
	, m_bShutDown(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatRoomsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSG_RECORD, m_MsgEdit);
}

BEGIN_MESSAGE_MAP(CChatRoomsDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RADIO_SERVER, &CChatRoomsDlg::OnBnClickedRadioServer)
	ON_BN_CLICKED(IDC_RADIO_CLIENT, &CChatRoomsDlg::OnBnClickedRadioClient)
	ON_BN_CLICKED(IDC_NET_SET, &CChatRoomsDlg::OnBnClickedNetSet)
	ON_BN_CLICKED(IDC_BTN_OPEN_SERVER, &CChatRoomsDlg::OnBnClickedOpenServer)
	ON_BN_CLICKED(IDC_SEND_MSG, &CChatRoomsDlg::OnBnClickedSendMsg)
	ON_BN_CLICKED(IDC_BTN_CONNECT_SERVER, &CChatRoomsDlg::OnBnClickedBtnConnectServer)
	ON_EN_CHANGE(IDC_INPUT_MSG, &CChatRoomsDlg::OnEnChangeInputMsg)
	ON_BN_CLICKED(IDC_BTN_STOP_SERVER, &CChatRoomsDlg::OnBnClickedBtnStopServer)
	ON_BN_CLICKED(IDC_BTN_CLOSE_SERVER, &CChatRoomsDlg::OnBnClickedBtnCloseServer)
	ON_BN_CLICKED(IDC_CLOSE_WINDOWS, &CChatRoomsDlg::OnBnClickedCloseWindows)
	ON_BN_CLICKED(IDC_MORE_FUNCTIONS, &CChatRoomsDlg::OnBnClickedMoreFunctions)
	ON_COMMAND(ID_MENUTRAYICON, &CChatRoomsDlg::OnMenuTrayIcon)
	ON_MESSAGE(WM_TRAYICON_MSG,OnTrayCallBackMsg)
	ON_COMMAND(ID_MENU_SHOW, &CChatRoomsDlg::OnMenuShow)
	ON_COMMAND(ID_MENU_QUIT, &CChatRoomsDlg::OnMenuQuit)
END_MESSAGE_MAP()


// CChatRoomsDlg message handlers

BOOL CChatRoomsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	DlgControlsInit();
	ExtendDialog(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChatRoomsDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChatRoomsDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CChatRoomsDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CChatRoomsDlg::DlgControlsInit()
{
	//初始化默认选择客户端选项
	CheckRadioButton(IDC_RADIO_CLIENT,IDC_RADIO_SERVER,IDC_RADIO_CLIENT);

	//初始化编辑框值
	SetDlgItemText(IDC_IP_ADDRESS,_T("127.0.0.1"));
	SetDlgItemText(IDC_EDIT_SERVER_PORT,_T("5566"));
	SetDlgItemText(IDC_EDIT_LOCAL_PORT,_T("5566"));

	/*
	GetDlgItem(IDC_BTN_STOP_SERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_LOCAL_PORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_LOCAL_PORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_OPEN_SERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CLOSE_SERVER)->EnableWindow(FALSE);*/

	EnableWindow(IDC_BTN_STOP_SERVER,FALSE);
	EnableWindow(IDC_STATIC_LOCAL_PORT,FALSE);
	EnableWindow(IDC_EDIT_LOCAL_PORT,FALSE);
	EnableWindow(IDC_BTN_OPEN_SERVER,FALSE);
	EnableWindow(IDC_BTN_CLOSE_SERVER,FALSE);
	EnableWindow(IDC_SEND_MSG,FALSE);

	ShowMsg(_T("程序启动成功!"));
}

BOOL CChatRoomsDlg::EnableWindow(UINT uID,BOOL bEnable /* = TRUE */)
{
	return ::EnableWindow(GetDlgItem(uID)->GetSafeHwnd(),bEnable);
	//return	GetDlgItem(uID)->EnableWindow(bEnable);
}

void CChatRoomsDlg::OnBnClickedRadioServer()
{
	// TODO: Add your control notification handler code here
	EnableWindow(IDC_STATIC_SERVER_IP,FALSE);
	EnableWindow(IDC_IP_ADDRESS,FALSE);
	EnableWindow(IDC_STATIC_SERVER_PORT,FALSE);
	EnableWindow(IDC_EDIT_SERVER_PORT,FALSE);
	EnableWindow(IDC_BTN_CONNECT_SERVER,FALSE);
	EnableWindow(IDC_BTN_STOP_SERVER,FALSE);

	EnableWindow(IDC_STATIC_LOCAL_PORT);
	EnableWindow(IDC_EDIT_LOCAL_PORT);
	EnableWindow(IDC_BTN_OPEN_SERVER);
	EnableWindow(IDC_BTN_CLOSE_SERVER,FALSE);
}

void CChatRoomsDlg::OnBnClickedRadioClient()
{
	// TODO: Add your control notification handler code here
	EnableWindow(IDC_STATIC_SERVER_IP);
	EnableWindow(IDC_IP_ADDRESS);
	EnableWindow(IDC_STATIC_SERVER_PORT);
	EnableWindow(IDC_EDIT_SERVER_PORT);
	EnableWindow(IDC_BTN_CONNECT_SERVER);
	EnableWindow(IDC_BTN_STOP_SERVER);

	EnableWindow(IDC_BTN_STOP_SERVER,FALSE);
	EnableWindow(IDC_STATIC_LOCAL_PORT,FALSE);
	EnableWindow(IDC_EDIT_LOCAL_PORT,FALSE);
	EnableWindow(IDC_BTN_OPEN_SERVER,FALSE);
	EnableWindow(IDC_BTN_CLOSE_SERVER,FALSE);
}

//扩展
void CChatRoomsDlg::ExtendDialog(BOOL bShwo /* = FALSE */)
{
	static CRect m_DlgRectLarge(0,0,0,0);
	static CRect m_DlgRectSmall(0,0,0,0);

	static CRect m_groupRectLarge(0,0,0,0);
	static CRect m_groupRectSmall(0,0,0,0);

	if(m_DlgRectLarge.IsRectNull())//判断矩形的四点坐标是否为0
	{
		GetWindowRect(&m_DlgRectLarge);
		m_DlgRectSmall = m_DlgRectLarge;
		m_DlgRectSmall.right -= 270;

		::GetWindowRect(GetDlgItem(IDC_FRAME)->GetSafeHwnd(),&m_groupRectLarge);
		m_groupRectSmall = m_groupRectLarge;
		m_groupRectSmall.right -= 270;
	}

	if(bShwo)//扩展右半部分,用大矩形设置
	{
		m_bShowAll = TRUE;
		SetWindowPos(NULL,0,0,m_DlgRectLarge.Width(),m_DlgRectLarge.Height(),SWP_NOMOVE | SWP_NOZORDER);

		//方法1：同上设置；方法2：使用api函数
		::SetWindowPos(GetDlgItem(IDC_FRAME)->GetSafeHwnd(),NULL,0,0,m_groupRectLarge.Width(),m_groupRectLarge.Height(),SWP_NOMOVE | SWP_NOZORDER);
	}
	else//隐藏右半部分，用小矩形设置
	{
		m_bShowAll = FALSE;
		SetWindowPos(NULL,0,0,m_DlgRectSmall.Width(),m_DlgRectSmall.Height(),SWP_NOMOVE | SWP_NOZORDER);

		::SetWindowPos(GetDlgItem(IDC_FRAME)->GetSafeHwnd(),NULL,0,0,m_groupRectSmall.Width(),m_groupRectSmall.Height(),SWP_NOMOVE | SWP_NOZORDER);

	}
}
void CChatRoomsDlg::OnBnClickedNetSet()
{
	// TODO: Add your control notification handler code here

	ExtendDialog(!m_bShowAll);
}

void CChatRoomsDlg::OnBnClickedOpenServer()
{
	// TODO: Add your control notification handler code here

	m_hListenThread = CreateThread(NULL,0,ListenThreadProc,this,0,NULL);

}


void CChatRoomsDlg::ShowMsg(CString strMsg)
{
	m_MsgEdit.SetSel(-1,-1);
	m_MsgEdit.ReplaceSel(strMsg + _T("\r\n"));
}

//将退出的客户端从队列中删除
void CChatRoomsDlg::RemoveClientFromArray(CClientItem in_Item)
{
	for(int idx = 0;idx < m_ClientArray.GetCount();++idx)
	{
		CClientItem tItem = m_ClientArray.GetAt(idx);
		if(tItem.m_strIp == in_Item.m_strIp &&
			tItem.m_Socket == in_Item.m_Socket && 
			tItem.hThread == in_Item.hThread)
			m_ClientArray.RemoveAt(idx);
	}
}

void CChatRoomsDlg::OnBnClickedSendMsg()
{
	// TODO: Add your control notification handler code here

	CString strMsg;
	GetDlgItemText(IDC_INPUT_MSG,strMsg);

	if(m_bIsServer == TRUE)
	{
		strMsg = _T("服务器：>") + strMsg;
		ShowMsg(strMsg);
		SendClientsMsg(strMsg);
	}
	else if(m_bIsServer == FALSE)
	{
		CString strTmp = _T("本地客户端：>") + strMsg;
		ShowMsg(strTmp);

		int iSend =	send(m_ConnectSock,(char*)strMsg.GetBuffer(),strMsg.GetLength() * sizeof(TCHAR),0);
		strMsg.ReleaseBuffer();
	}
	else
		AfxMessageBox(_T("请先进入聊天室"));


	SetDlgItemText(IDC_INPUT_MSG,_T(""));
}

void CChatRoomsDlg::OnBnClickedBtnConnectServer()
{
	// TODO: Add your control notification handler code here
	m_hConnectThread = CreateThread(NULL,0,ConnectThreadProc,this,0,NULL);
		
}

void CChatRoomsDlg::SendClientsMsg(CString strMsg,CClientItem *pNotSend)
{
	TCHAR szBuf[MAX_BUF_SIZE] = {0};
	_tcscpy_s(szBuf,MAX_BUF_SIZE,strMsg);

	for(INT_PTR idx = 0;idx < m_ClientArray.GetCount();++idx)
	{
		if(!pNotSend || pNotSend->hThread != m_ClientArray.GetAt(idx).hThread
			|| pNotSend->m_Socket != m_ClientArray.GetAt(idx).m_Socket 
			|| pNotSend->m_strIp != m_ClientArray.GetAt(idx).m_strIp)

			send(m_ClientArray.GetAt(idx).m_Socket,(char*)szBuf,_tcslen(szBuf) * sizeof(TCHAR),0);
	}
}
void CChatRoomsDlg::OnEnChangeInputMsg()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	CString strMsg;
	GetDlgItemText(IDC_INPUT_MSG,strMsg);

	if(strMsg.IsEmpty())
		EnableWindow(IDC_SEND_MSG,FALSE);
	else
		EnableWindow(IDC_SEND_MSG);
}

void CChatRoomsDlg::StopClient()
{
	m_bShutDown = TRUE;
	DWORD dwRet = WaitForSingleObject(m_hConnectThread,1000);

	if(WAIT_OBJECT_0 != dwRet)
	{
		TerminateThread(m_hConnectThread,-1);
		closesocket(m_ConnectSock);
	}

	m_hConnectThread = NULL;
	m_ConnectSock = INVALID_SOCKET;
	
	m_bIsServer = -1;
	m_bShutDown = FALSE;
}

void CChatRoomsDlg::StopServer()
{
	UINT nCount = m_ClientArray.GetCount();

	HANDLE *pHandles = new HANDLE[nCount + 1];
	pHandles[0] = m_hListenThread;

	for(int idx = 0;idx < nCount;++idx)
	{
		pHandles[idx + 1] = m_ClientArray.GetAt(idx).hThread;
	}

	m_bShutDown = TRUE;
	DWORD dwRet = WaitForMultipleObjects(nCount + 1,pHandles,TRUE,1000);
	if(WAIT_OBJECT_0 != dwRet)
	{
		for(INT_PTR i = 0;i < m_ClientArray.GetCount();++i)
		{
			TerminateThread(m_ClientArray.GetAt(i).hThread,-1);
			closesocket(m_ClientArray.GetAt(i).m_Socket);
		}
		TerminateThread(m_hListenThread,-1);
		closesocket(m_ListenSocket);
	}

	delete []pHandles;
	m_hListenThread = NULL;
	m_ListenSocket = INVALID_SOCKET;
	m_bShutDown = FALSE;
	m_bIsServer = -1;
}

void CChatRoomsDlg::OnBnClickedBtnStopServer()
{
	// TODO: Add your control notification handler code here

	if(IDYES == MessageBox(_T("确定关闭客户端"),0,MB_YESNO))
	{
		StopClient();
		ShowMsg(_T("客户端已成功停止!"));

		EnableWindow(IDC_BTN_CONNECT_SERVER);
		EnableWindow(IDC_BTN_STOP_SERVER,FALSE);

		EnableWindow(IDC_IP_ADDRESS);
		EnableWindow(IDC_EDIT_SERVER_PORT);
	}
}

void CChatRoomsDlg::OnBnClickedBtnCloseServer()
{
	// TODO: Add your control notification handler code here
	StopServer();
	EnableWindow(IDC_BTN_OPEN_SERVER);
	EnableWindow(IDC_LISTEN_PORT);
	EnableWindow(IDC_BTN_CLOSE_SERVER,FALSE);
}

void CChatRoomsDlg::OnBnClickedCloseWindows()
{
	// TODO: Add your control notification handler code here
	if(m_bIsServer == TRUE)
		StopServer();
	else if(m_bIsServer == FALSE)
		StopClient();

	OnCancel();
}

void CChatRoomsDlg::OnBnClickedMoreFunctions()
{
	// TODO: Add your control notification handler code here
	CPoint pt;
	CRect mRect;
	CMenu mMenu,*pMenu = NULL;

	GetDlgItem(IDC_MORE_FUNCTIONS)->GetWindowRect(&mRect);
	pt = mRect.BottomRight();
	
	mMenu.LoadMenu(IDR_MENU);

	pMenu = mMenu.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,pt.x,pt.y,this);
}

BOOL CChatRoomsDlg::TrayMyIcon(BOOL bAdd /* = TRUE */)
{
	BOOL bRet = FALSE;

	NOTIFYICONDATA tnd;
	tnd.cbSize = sizeof(PNOTIFYICONDATA);
	tnd.hWnd = m_hWnd;//GetSafeHwnd();
	tnd.uID = IDR_MAINFRAME;

	if(bAdd)
	{
		tnd.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
		tnd.uCallbackMessage = WM_TRAYICON_MSG;
		tnd.hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));

		_tcscpy_s(tnd.szTip,sizeof(tnd.szTip),_T("聊天室v1.0"));
		ShowWindow(SW_MINIMIZE);
		ShowWindow(SW_HIDE);
		bRet = Shell_NotifyIcon(NIM_ADD,&tnd);

	}
	else
	{
		ShowWindow(SW_SHOWNA);
		SetForegroundWindow();
		bRet = Shell_NotifyIcon(NIM_DELETE,&tnd);
	}
	return bRet;
}


void CChatRoomsDlg::OnMenuTrayIcon()
{
	// TODO: Add your command handler code here
	TrayMyIcon();
}

LRESULT CChatRoomsDlg::OnTrayCallBackMsg(WPARAM wParam,LPARAM lParam)
{
	CMenu mMenu,*pMenu = NULL;
	CPoint pt;
	switch(lParam)
	{
	case WM_RBUTTONUP:
		
		mMenu.LoadMenu(IDR_MENU1);
		pMenu = mMenu.GetSubMenu(0);
		GetCursorPos(&pt);

		SetForegroundWindow();
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTALIGN,pt.x,pt.y,this);
		break;

	case WM_LBUTTONDBLCLK:
		ShowWindow(SW_RESTORE);
		SetForegroundWindow();
		TrayMyIcon(FALSE);
		break;

	default:
		break;

	}
	return NULL;
}

void CChatRoomsDlg::OnMenuShow()
{
	// TODO: Add your command handler code here
	ShowWindow(SW_RESTORE);
	TrayMyIcon(FALSE);
}

void CChatRoomsDlg::OnMenuQuit()
{
	// TODO: Add your command handler code here
	TrayMyIcon(FALSE);
	OnBnClickedCloseWindows();
}