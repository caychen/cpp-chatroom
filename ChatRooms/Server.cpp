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

	//����SOCKET
	pChatRoom->m_ListenSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(INVALID_SOCKET == pChatRoom->m_ListenSocket)
	{
		AfxMessageBox(_T("����˴���SOCKETʧ��!"));
		return FALSE;
	}

	UINT uPort = pChatRoom->GetDlgItemInt(IDC_LISTEN_PORT);
	if(uPort <= 0 || uPort > 65535)
	{
		AfxMessageBox(_T("��ѡ����ʵĶ˿�: 0-65536"));
		goto __Error_End ;
	}

	//�󶨶˿ں�
	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = INADDR_ANY;
	sockAddr.sin_port = htons(uPort);

	if(SOCKET_ERROR == bind(pChatRoom->m_ListenSocket,(sockaddr*)&sockAddr,sizeof(sockaddr_in)))
	{
		AfxMessageBox(_T("�󶨶˿�ʧ��!"));
		goto __Error_End ;
	}

	//�����˿�
	if(SOCKET_ERROR == listen(pChatRoom->m_ListenSocket,5))
	{
		AfxMessageBox(_T("����ʧ��!"));
		goto __Error_End ;
	}

	pChatRoom->m_bIsServer = TRUE;
	pChatRoom->ShowMsg(_T("�����������ɹ�!"));
	pChatRoom->EnableWindow(IDC_BTN_OPEN_SERVER,FALSE);
	pChatRoom->EnableWindow(IDC_LISTEN_PORT,FALSE);
	pChatRoom->EnableWindow(IDC_BTN_CLOSE_SERVER);

	while(TRUE && !(pChatRoom->m_bShutDown))
	{
		//���ܿͻ��˵����󣬷��ؿͻ�����Ϣ
		//�������ͻ������������ʹ�õ�SOCKETΪaccSock
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

			//dwFlags��CREATE_SUSPENDED����ͣrun��ֱ������ResumeThread���� 
			tItem.hThread = CreateThread(NULL,0,ClientThreadProc,&(pChatRoom->m_ClientArray.GetAt(idx)),CREATE_SUSPENDED,NULL);
			pChatRoom->m_ClientArray.GetAt(idx).hThread = tItem.hThread;
			ResumeThread(tItem.hThread);//�ָ�ĳ���̵߳ľ��

			
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
				strMsg = _T("�ͻ��ˣ�") + m_ClientItem.m_strIp + _T(">") + strMsg;
				m_ClientItem.m_pMainWnd->ShowMsg(strMsg);
				m_ClientItem.m_pMainWnd->SendClientsMsg(strMsg,&m_ClientItem);
			}
			else
			{
				strMsg = _T("�ͻ��ˣ�") + m_ClientItem.m_strIp + _T("�뿪��������");
				m_ClientItem.m_pMainWnd->ShowMsg(strMsg);

				m_ClientItem.m_pMainWnd->RemoveClientFromArray(m_ClientItem);
				break;
			}
		}
		Sleep(500);
	}
	return TRUE;
}