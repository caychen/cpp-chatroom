#include "stdafx.h"
#include "ChatRoomsDlg.h"
#include "ServerAndClient.h"

DWORD WINAPI ConnectThreadProc(LPVOID lpParameter)
{
	CChatRoomsDlg *pChatRoom = (CChatRoomsDlg*)lpParameter;
	ASSERT(pChatRoom != NULL);

	pChatRoom->m_ConnectSock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(INVALID_SOCKET == pChatRoom->m_ConnectSock)
	{
		AfxMessageBox(_T("客户端创建SOCKET失败!"));
		return FALSE;
	}

	CString strServerIp;
	pChatRoom->GetDlgItemText(IDC_IP_ADDRESS,strServerIp);

	int iPort = pChatRoom->GetDlgItemInt(IDC_EDIT_SERVER_PORT);
	if(iPort <= 0 || iPort > 65535)
	{
		AfxMessageBox(_T("请选择合适的端口: 0-65536"));
		goto __Error_End ;
	}

	char szIpAddress[16] = {0};
	USES_CONVERSION;
	strcpy_s(szIpAddress,16,T2A(strServerIp));//T2A:	T：通用类型，当前的编码方式，A:ANSI

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(szIpAddress);
	server.sin_port = htons(iPort);

	if(SOCKET_ERROR == connect(pChatRoom->m_ConnectSock,(sockaddr*)&server,sizeof(sockaddr)))
	{
		AfxMessageBox(_T("连接失败，请重试!"));
		goto __Error_End;
	}

	pChatRoom->m_bIsServer = FALSE;
	pChatRoom->EnableWindow(IDC_BTN_STOP_SERVER);
	pChatRoom->EnableWindow(IDC_BTN_CONNECT_SERVER,FALSE);

	pChatRoom->ShowMsg(_T("系统提示：连接服务器成功!"));
	pChatRoom->EnableWindow(IDC_IP_ADDRESS,FALSE);
	pChatRoom->EnableWindow(IDC_EDIT_SERVER_PORT,FALSE);


	while(TRUE && !(pChatRoom->m_bShutDown))
	{
		if(SOCKET_select(pChatRoom->m_ConnectSock))
		{
			TCHAR szBuf[MAX_BUF_SIZE] = {0};
			int iRet = recv(pChatRoom->m_ConnectSock,(char*)szBuf,MAX_BUF_SIZE,0);
			if(iRet > 0)
			{
				pChatRoom->ShowMsg(szBuf);
			}
			else
			{
				pChatRoom->ShowMsg(_T("聊天室服务器已经停止，请等待服务器开启!"));
				break;
			}
		}
		Sleep(500);
	}

__Error_End:
	closesocket(pChatRoom->m_ConnectSock);
	return TRUE;
}