#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#include <Windows.h>
#include <list>
//#include <iterator>

/////////////////////////////////////////////////////////////////////////////////////////
CRITICAL_SECTION	g_cs;			// ������ ����ȭ ��ü
SOCKET				g_hSocket;		// ������ ���� ����
std::list<SOCKET>	g_listClient;	// ����� Ŭ���̾�Ʈ ���� ����Ʈ.

/////////////////////////////////////////////////////////////////////////////////////////

// ���� ����� Ŭ���̾�Ʈ�� ������ ����Ʈ�� �����Ѵ�.
BOOL AddUser(SOCKET hSocket)
{
	::EnterCriticalSection(&g_cs);		// �Ӱ迵�� ����
	// *�� �ڵ�� ���� �� �����常 �����Ѵٴ� ���� ����ȴ�!
	g_listClient.push_back(hSocket);
	::LeaveCriticalSection(&g_cs);		// �Ӱ迵�� ��

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
// ����� Ŭ���̾�Ʈ ��ο��� �޽����� �����Ѵ�.
void SendChattingMessage(char* pszParam)
{
	int nLength = strlen(pszParam);
	std::list<SOCKET>::iterator it;

	::EnterCriticalSection(&g_cs);		// �Ӱ迵�� ����
	for (it = g_listClient.begin(); it != g_listClient.end(); ++it)
	{
		::send(*it, pszParam, sizeof(char) * (nLength + 1), 0);
	}::LeaveCriticalSection(&g_cs);		// �Ӱ迵�� ��
}

/////////////////////////////////////////////////////////////////////////////////////////
// Ctrl+c �̺�Ʈ�� �����ϰ� ���α׷��� �����Ѵ�.
BOOL CtrlHandeler(DWORD dwType)
{
	if (dwType == CTRL_C_EVENT)
	{
		std::list<SOCKET>::iterator it;

		//����� ��� Ŭ���̾�Ʈ �� ���� ������ �ݰ� ���α׷��� �����Ѵ�.
		::shutdown(g_hSocket, SD_BOTH);

		::EnterCriticalSection(&g_cs);		// �Ӱ迵�� ����
		for (it = g_listClient.begin(); it != g_listClient.end(); ++it)
		{
			std::cout << it._Getcont() << std::endl;
			// ���� ����Ʈ�� ��ϵ� ��� ������ �����Ѵ�.
			::closesocket(*it);
		}
		//
		g_listClient.clear();
		::LeaveCriticalSection(&g_cs);		// �Ӱ迵�� ��

		puts("��� Ŭ���̾�Ʈ ������ �����߽��ϴ�.");
		// Ŭ���̾�Ʈ�� ����ϴ� ��������� ����Ǳ⸦ ��ٸ���.
		::Sleep(100);
		::DeleteCriticalSection(&g_cs);
		::closesocket(g_hSocket);

		// ���� ����
		::WSACleanup();
		exit(0);
		return TRUE;
	}
	return FALSE;
}
DWORD WINAPI ThreadFunction(LPVOID pParam)
{
	char szBuffer[128] = { 0 };
	int nReceive = 0;
	SOCKET hClient = (SOCKET)pParam;

	printf("%d Ŭ���̾�Ʈ�� ����Ǿ����ϴ� \n", hClient);

	// Ŭ���̾�Ʈ�κ��� ���ڿ��� �����Ѵ�
	while ((nReceive = ::recv(hClient, szBuffer, sizeof(szBuffer), 0)) > 0)
	{
		// ������ ���ڿ��� �״�� ����(Echo)����.
		//::send(hClient, szBuffer, sizeof(szBuffer), 0);
		puts(szBuffer);
		SendChattingMessage(szBuffer);
		memset(szBuffer, 0, sizeof(szBuffer));
	}

	printf("%d Ŭ���̾�Ʈ ������ ������ϴ� \n", hClient);
	::EnterCriticalSection(&g_cs);
	g_listClient.remove(hClient);
	::LeaveCriticalSection(&g_cs);

	::closesocket(hClient);
	return 0;
}

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

	// �Ӱ迵�� ��ü�� �����Ѵ�.
	::InitializeCriticalSection(&g_cs);

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
	//// * ���ε� ���� IP�ּҿ� ��Ʈ�� �����ϵ��� ���� �ɼ��� �����Ѵ�.
	//BOOL bOption = TRUE;
	//if (::setsockopt(hSocket, SOL_SOCKET,
	//	SO_REUSEADDR, (char*)&bOption, sizeof(BOOL)) == SOCKET_ERROR)
	//{
	//	puts("ERROR: ���� �ɼ��� ������ �� �����ϴ�.");
	//}

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
	puts("*** ä�ü����� �����մϴ�. ***");


	// 4. Ŭ���̾�Ʈ ���� ó�� �� ����
	SOCKADDR_IN clientaddr = { 0 };
	int nAddrLen = sizeof(clientaddr);
	SOCKET hClient = 0;
	DWORD dwThreadID = 0;
	HANDLE hThread;

	// 4.1 Ŭ���̾�Ʈ ������ �޾Ƶ��̰� ���ο� ���� ����(����)
	while ((hClient = ::accept(g_hSocket,
		(SOCKADDR*)&clientaddr,&nAddrLen)) != INVALID_SOCKET)
	{
		if (AddUser(hClient) == FALSE)
		{
			puts("ERROR : �� �̻� Ŭ���̾�Ʈ ������ ó���� �� �����ϴ�.");
			CtrlHandeler(CTRL_C_EVENT);
			break;
		}
		// 4.2 �� Ŭ���̾�Ʈ�� ����ϱ� ���� ������ ����
		// Ŭ���̾�Ʈ ���� �����尡 �ϳ��� �����ȴ�
		hThread = ::CreateThread(
			NULL,				// ���ȼӼ� ���
			0,					// ���� �޸𸮴� �⺻ũ��(1MB)
			ThreadFunction,		// ������� ������ �Լ�
			(LPVOID)hClient,	// ���� ������ Ŭ���̾�Ʈ ����
			0,					// ���� �÷��״� �⺻�� ���
			&dwThreadID);		// ������ ������ ID�� ����� ���� �ּ�

		::CloseHandle(hThread);
	}

	puts("*** ä�ü����� �����մϴ�. ***");
	return 0;


}