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
		AfxMessageBox(_T("�ͻ��˴���SOCKETʧ��!"));
		return FALSE;
	}

	CString strServerIp;
	pChatRoom->GetDlgItemText(IDC_IP_ADDRESS,strServerIp);

	int iPort = pChatRoom->GetDlgItemInt(IDC_EDIT_SERVER_PORT);
	if(iPort <= 0 || iPort > 65535)
	{
		AfxMessageBox(_T("��ѡ����ʵĶ˿�: 0-65536"));
		goto __Error_End ;
	}

	char szIpAddress[16] = {0};
	USES_CONVERSION;
	strcpy_s(szIpAddress,16,T2A(strServerIp));//T2A:	T��ͨ�����ͣ���ǰ�ı��뷽ʽ��A:ANSI

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(szIpAddress);
	server.sin_port = htons(iPort);

	if(SOCKET_ERROR == connect(pChatRoom->m_ConnectSock,(sockaddr*)&server,sizeof(sockaddr)))
	{
		AfxMessageBox(_T("����ʧ�ܣ�������!"));
		goto __Error_End;
	}

	pChatRoom->m_bIsServer = FALSE;
	pChatRoom->EnableWindow(IDC_BTN_STOP_SERVER);
	pChatRoom->EnableWindow(IDC_BTN_CONNECT_SERVER,FALSE);

	pChatRoom->ShowMsg(_T("ϵͳ��ʾ�����ӷ������ɹ�!"));
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
				pChatRoom->ShowMsg(_T("�����ҷ������Ѿ�ֹͣ����ȴ�����������!"));
				break;
			}
		}
		Sleep(500);
	}

__Error_End:
	closesocket(pChatRoom->m_ConnectSock);
	return TRUE;
}