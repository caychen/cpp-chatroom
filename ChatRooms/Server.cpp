#include "stdafx.h"
#include "ChatRoomsDlg.h"

BOOL SOCKET_select(SOCKET hSocket,int nTimeOut,BOOL bRead)
{
	fd_set fdset;
	timeval tv;
	FD_ZERO(&fdset);
	FD_SET(hSocket,&fdset);

	nTimeOut = nTimeOut > 1000 ? 1000 : nTimeOut;

	tv.tv_sec = 0;
	tv.tv_usec = nTimeOut;

	int iRet = 0;
	if(bRead)
	{
		iRet = select(0,&fdset,NULL,NULL,&tv);
	}
	else
	{
		iRet = select(0,NULL,&fdset,NULL,&tv);
	}

	if(iRet <= 0)
		return FALSE;
	else if(FD_ISSET(hSocket,&fdset))
		return TRUE;

	return FALSE;

}

DWORD WINAPI ListenThreadProc(LPVOID lpParameter)
{
	CChatRoomsDlg *pChatRoom = (CChatRoomsDlg*)lpParameter;

	ASSERT(pChatRoom != NULL);

	//创建SOCKET
	pChatRoom->m_ListenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(INVALID_SOCKET == pChatRoom->m_ListenSocket)
	{
		AfxMessageBox(_T("服务端创建SOCKET失败!"));
		return FALSE;
	}

	UINT uPort = pChatRoom->GetDlgItemInt(IDC_LISTEN_PORT);
	if(uPort <= 0 || uPort > 65535)
	{
		AfxMessageBox(_T("请选择合适的端口: 0-65536"));
		goto __Error_End ;
	}

	//绑定端口号
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = INADDR_ANY;
	sockAddr.sin_port = htons(uPort);

	if(SOCKET_ERROR == bind(pChatRoom->m_ListenSocket,(sockaddr*)&sockAddr,sizeof(sockaddr_in)))
	{
		AfxMessageBox(_T("绑定端口失败!"));
		goto __Error_End ;
	}

	//监听端口
	if(SOCKET_ERROR == listen(pChatRoom->m_ListenSocket,5))
	{
		AfxMessageBox(_T("监听失败!"));
		goto __Error_End ;
	}

	pChatRoom->m_bIsServer = TRUE;
	pChatRoom->ShowMsg(_T("开启服务器成功!"));
	pChatRoom->EnableWindow(IDC_BTN_OPEN_SERVER,FALSE);
	pChatRoom->EnableWindow(IDC_LISTEN_PORT,FALSE);
	pChatRoom->EnableWindow(IDC_BTN_CLOSE_SERVER);

	while(TRUE && !(pChatRoom->m_bShutDown))
	{
		//接受客户端的请求，返回客户端信息
		//接下来客户端与服务器后使用的SOCKET为accSock
		if(SOCKET_select(pChatRoom->m_ListenSocket,100,TRUE))
		{
			sockaddr_in clientAddr;
			int iLen = sizeof(sockaddr_in);
			SOCKET accSock = accept(pChatRoom->m_ListenSocket,(sockaddr*)&clientAddr,&iLen);

			if(INVALID_SOCKET == accSock)
			{
				continue;
			}

			CClientItem tItem;
			tItem.m_Socket = accSock;
			tItem.m_strIp = inet_ntoa(clientAddr.sin_addr);
			tItem.m_pMainWnd = pChatRoom;
			INT_PTR idx = pChatRoom->m_ClientArray.Add(tItem);

			//dwFlags：CREATE_SUSPENDED：暂停run，直到调用ResumeThread函数 
			tItem.hThread = CreateThread(NULL,0,ClientThreadProc,&(pChatRoom->m_ClientArray.GetAt(idx)),CREATE_SUSPENDED,NULL);
			pChatRoom->m_ClientArray.GetAt(idx).hThread = tItem.hThread;
			ResumeThread(tItem.hThread);//恢复某个线程的句柄

			
			Sleep(100);
		}
	}

__Error_End:
	closesocket(pChatRoom->m_ListenSocket);
	return TRUE;
}


DWORD WINAPI ClientThreadProc(LPVOID lpParameter)
{
	CString strMsg;
	CClientItem m_ClientItem = *(CClientItem*)lpParameter;

	while(TRUE && !(m_ClientItem.m_pMainWnd->m_bShutDown))
	{
		if(SOCKET_select(m_ClientItem.m_Socket,100,TRUE))
		{
			TCHAR szBuf[MAX_BUF_SIZE] = {0};
			int iRet = recv(m_ClientItem.m_Socket,(char*)szBuf,MAX_BUF_SIZE,0);
			if(iRet > 0)
			{
				strMsg.Format(_T("%s"),szBuf);
				strMsg = _T("客户端：") + m_ClientItem.m_strIp + _T(">") + strMsg;
				m_ClientItem.m_pMainWnd->ShowMsg(strMsg);
				m_ClientItem.m_pMainWnd->SendClientsMsg(strMsg,&m_ClientItem);
			}
			else
			{
				strMsg = _T("客户端：") + m_ClientItem.m_strIp + _T("离开了聊天室");
				m_ClientItem.m_pMainWnd->ShowMsg(strMsg);

				m_ClientItem.m_pMainWnd->RemoveClientFromArray(m_ClientItem);
				break;
			}
		}
		Sleep(500);
	}
	return TRUE;
}