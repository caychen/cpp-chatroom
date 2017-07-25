// ChatRoomsDlg.h : header file
//

#pragma once
#include "resource.h"
#include "ServerAndClient.h"
#include "afxwin.h"

// CChatRoomsDlg dialog
class CChatRoomsDlg : public CDialog
{
// Construction
public:
	CChatRoomsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CHATROOMS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void DlgControlsInit();

	BOOL EnableWindow(UINT uID,BOOL bEnable = TRUE);

	afx_msg void OnBnClickedRadioServer();

	void ExtendDialog(BOOL bShwo = FALSE);

	afx_msg void OnBnClickedRadioClient();

	afx_msg void OnBnClickedNetSet();

	BOOL m_bShowAll;

	afx_msg void OnBnClickedOpenServer();

	SOCKET m_ListenSocket;

	HANDLE m_hListenThread;
	HANDLE m_hConnectThread;

	CArray <CClientItem,CClientItem> m_ClientArray;

	void ShowMsg(CString strMsg);

	CEdit m_MsgEdit;

	void RemoveClientFromArray(CClientItem in_Item);

	afx_msg void OnBnClickedSendMsg();

	afx_msg void OnBnClickedBtnConnectServer();

	SOCKET m_ConnectSock;

	void SendClientsMsg(CString strMsg,CClientItem *pNotSend = NULL);

	int m_bIsServer;
public:
	afx_msg void OnEnChangeInputMsg();

	void StopClient();
	void StopServer();
	
public:
	BOOL m_bShutDown;
public:
	afx_msg void OnBnClickedBtnStopServer();
public:
	afx_msg void OnBnClickedBtnCloseServer();
public:
	afx_msg void OnBnClickedCloseWindows();
public:
	afx_msg void OnBnClickedMoreFunctions();

	BOOL TrayMyIcon(BOOL bAdd = TRUE);
public:
	afx_msg void OnMenuTrayIcon();

	LRESULT OnTrayCallBackMsg(WPARAM wParam,LPARAM lParam);
public:
	afx_msg void OnMenuShow();
public:
	afx_msg void OnMenuQuit();
};
