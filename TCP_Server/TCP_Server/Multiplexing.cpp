#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#include <Windows.h>
#include <list>
//#include <iterator>

/////////////////////////////////////////////////////////////////////////////////////////
SOCKET				g_hSocket;		// ������ ���� ����
std::list<SOCKET>	g_listClient;	// ����� Ŭ���̾�Ʈ ���� ����Ʈ.

/////////////////////////////////////////////////////////////////////////////////////////
void SendMessageAll(char* pszMessage, int nSize)
{
	std::list<SOCKET>::iterator it;
	for (it = g_listClient.begin(); it != g_listClient.end(); ++it)
		send(*it, pszMessage, nSize, 0);
}

/////////////////////////////////////////////////////////////////////////////////////////
void CloseAll(void)
{
	std::list<SOCKET>::iterator it;
	for (it = g_listClient.begin(); it != g_listClient.end(); ++it)
	{
		::shutdown(*it, SD_BOTH);
		::closesocket(*it);
	}
}


//// ���� ����� Ŭ���̾�Ʈ�� ������ ����Ʈ�� �����Ѵ�.
//BOOL AddUser(SOCKET hSocket)
//{
//
//	// *�� �ڵ�� ���� �� �����常 �����Ѵٴ� ���� ����ȴ�!
//	g_listClient.push_back(hSocket);
//
//
//	return TRUE;
//}

/////////////////////////////////////////////////////////////////////////////////////////
// ����� Ŭ���̾�Ʈ ��ο��� �޽����� �����Ѵ�.
//void SendChattingMessage(char* pszParam)
//{
//	int nLength = strlen(pszParam);
//	std::list<SOCKET>::iterator it;
//
//
//	for (it = g_listClient.begin(); it != g_listClient.end(); ++it)
//	{
//		::send(*it, pszParam, sizeof(char) * (nLength + 1), 0);
//	}
//
//}

/////////////////////////////////////////////////////////////////////////////////////////
//// Ctrl+c �̺�Ʈ�� �����ϰ� ���α׷��� �����Ѵ�.
BOOL CtrlHandeler(DWORD dwType)
{
	if (dwType == CTRL_C_EVENT)
	{
		::closesocket(g_hSocket);
		CloseAll();
		puts("��� Ŭ�󸮾�Ʈ ������ �����߽��ϴ�.");

		::WSACleanup();
		exit(0);
		return TRUE;
	}
	return FALSE;
}
//DWORD WINAPI ThreadFunction(LPVOID pParam)
//{
//	char szBuffer[128] = { 0 };
//	int nReceive = 0;
//	SOCKET hClient = (SOCKET)pParam;
//
//	printf("%d Ŭ���̾�Ʈ�� ����Ǿ����ϴ� \n", hClient);
//
//	// Ŭ���̾�Ʈ�κ��� ���ڿ��� �����Ѵ�
//	while ((nReceive = ::recv(hClient, szBuffer, sizeof(szBuffer), 0)) > 0)
//	{
//		puts(szBuffer);
//		SendChattingMessage(szBuffer);
//		memset(szBuffer, 0, sizeof(szBuffer));
//	}
//
//	printf("%d Ŭ���̾�Ʈ ������ ������ϴ� \n", hClient);
//	g_listClient.remove(hClient);
//
//	::closesocket(hClient);
//	return 0;
//}

int main()
{
	// ���� �ʱ�ȭ
	// �������� ���� ���̺귯�� �ʱ�ȭ
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: ������ �ʱ�ȭ �� �� �����ϴ�.");
		return 0;
	}


	// Ctrl+c Ű�� ������ �� �̸� �����ϰ� ó���� �Լ��� ����Ѵ�.
	if (::SetConsoleCtrlHandler(
		(PHANDLER_ROUTINE)CtrlHandeler, TRUE) == FALSE)
		puts("ERROR: Ctrl+c ó���⸦ ����� �� �����ϴ�.");

	// 1. ���� ��� �Ұ� ����
	g_hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (g_hSocket == INVALID_SOCKET)
	{
		puts("ERROR: ���� ��� ������ ������ �� �����ϴ�.");
		return 0;
	}

	// 2. ��Ʈ ���ε�
	SOCKADDR_IN svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (::bind(g_hSocket, (SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
	{
		puts("ERROR: ���Ͽ� IP�ּҿ� ��Ʈ�� ���ε� �� �� �����ϴ�.");
		return 0;
	}

	// 3. ���Ӵ�� ���·� ��ȯ
	// server�� Listen ���°� �ȴ�
	if (::listen(g_hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		puts("ERROR: ���� ���·� ��ȯ�� �� �����ϴ�.");
		return 0;
	}



	// 4. ����� Ŭ���̾�Ʈ ������ ���� ����Ʈ �����̳� ����
	g_listClient.push_back(g_hSocket);

	// 5. ������ ��ȭ�� ���ø� ���� �ݺ���
	UINT nCount;
	FD_SET fdRead;
	std::list<SOCKET>::iterator it;

	puts("*** I/O ��Ƽ�÷��� ä�� ������ �����մϴ�. ***");
	do
	{
		// 5-1. Ŭ���̾�Ʈ ���� �� �������� ��ȭ ���ü� �ʱ�ȭ
		FD_ZERO(&fdRead);
		for (it = g_listClient.begin(); it != g_listClient.end(); ++it)
			FD_SET(*it, &fdRead);

		// 5-2 ��ȭ�� �߻��� ������ ���
		::select(0, &fdRead, NULL, NULL, NULL);

		// 5-3 ��ȭ�� ������ ���� �� Ȯ��
		nCount = fdRead.fd_count;
		for (int nIndex = 0; nIndex < nCount; ++nIndex)
		{
			// ���Ͽ� ��ȭ ���� �����װ� SET �Ǿ��°�?
			if (!FD_ISSET(fdRead.fd_array[nIndex], &fdRead))
				continue;

			// 5-3-1. ������ listen ������ ��Ʈ�Ǿ��°�?
			// ��, ������ ������ �õ��ߴ°�?
			if (fdRead.fd_array[nIndex] == g_hSocket)
			{
				SOCKADDR_IN clientaddr = { 0 };
				int nAddrLen = sizeof(clientaddr);
				SOCKET hClient = ::accept(g_hSocket,
					(SOCKADDR*)&clientaddr, &nAddrLen);
				if (hClient != INVALID_SOCKET)
				{
					FD_SET(hClient, &fdRead);
					g_listClient.push_back(hClient);
				}
			}
			
			// 5-3-2 Ŭ���̾�Ʈ�� ������ �����Ͱ� �ִ� ���
			else
			{
				char szBuffer[1024] = { 0 };
				int nReceive = ::recv(fdRead.fd_array[nIndex],
					(char*)szBuffer, sizeof(szBuffer), 0);
				if (nReceive <= 0)
				{
					// ���� ����.
					::closesocket(fdRead.fd_array[nIndex]);
					FD_CLR(fdRead.fd_array[nIndex], &fdRead);
					g_listClient.remove(fdRead.fd_array[nIndex]);
					puts("Ŭ���̾�Ʈ�� ������ �������ϴ�.");
				}
				else
				{
					// ä�� �޽��� ����
					SendMessageAll(szBuffer, nReceive);
				}
			}
		}
	} while (g_hSocket != NULL);

	// 6. ��� ������ �ݰ� �����Ѵ�.
	CloseAll();

	puts("*** ä�ü����� �����մϴ�. ***");
	return 0;

	//// 4.1 Ŭ���̾�Ʈ ������ �޾Ƶ��̰� ���ο� ���� ����(����)
	//while ((hClient = ::accept(g_hSocket,
	//	(SOCKADDR*)&clientaddr, &nAddrLen)) != INVALID_SOCKET)
	//{
	//	if (AddUser(hClient) == FALSE)
	//	{
	//		puts("ERROR : �� �̻� Ŭ���̾�Ʈ ������ ó���� �� �����ϴ�.");
	//		CtrlHandeler(CTRL_C_EVENT);
	//		break;
	//	}
	//	// 4.2 �� Ŭ���̾�Ʈ�� ����ϱ� ���� ������ ����
	//	// Ŭ���̾�Ʈ ���� �����尡 �ϳ��� �����ȴ�
	//	hThread = ::CreateThread(
	//		NULL,				// ���ȼӼ� ���
	//		0,					// ���� �޸𸮴� �⺻ũ��(1MB)
	//		ThreadFunction,		// ������� ������ �Լ�
	//		(LPVOID)hClient,	// ���� ������ Ŭ���̾�Ʈ ����
	//		0,					// ���� �÷��״� �⺻�� ���
	//		&dwThreadID);		// ������ ������ ID�� ����� ���� �ּ�

	//	::CloseHandle(hThread);
	//}




}