//#include <iostream>
//#include <winsock2.h>
//#pragma comment(lib, "ws2_32")
//
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
//		// ������ ���ڿ��� �״�� ����(Echo)����.
//		::send(hClient, szBuffer, sizeof(szBuffer), 0);
//		puts(szBuffer); fflush(stdout);
//		memset(szBuffer, 0, sizeof(szBuffer));
//	}
//
//	printf("%d Ŭ���̾�Ʈ ������ ������ϴ� \n", hClient);
//	::closesocket(hClient);
//
//	return 0;
//}
//
//int main()
//{
//	// ���� �ʱ�ȭ
//	// �������� ���� ���̺귯�� �ʱ�ȭ
//	WSADATA wsa = { 0 };
//	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
//	{
//		puts("ERROR: ������ �ʱ�ȭ �� �� �����ϴ�.");
//		return 0;
//	}
//
//	// 1. ���� ��� �Ұ� ����
//	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
//	if (hSocket == INVALID_SOCKET)
//	{
//		puts("ERROR: ���� ��� ������ ������ �� �����ϴ�.");
//		return 0;
//	}
//	// * ���ε� ���� IP�ּҿ� ��Ʈ�� �����ϵ��� ���� �ɼ��� �����Ѵ�.
//	BOOL bOption = TRUE;
//	if (::setsockopt(hSocket, SOL_SOCKET,
//		SO_REUSEADDR, (char*)&bOption, sizeof(BOOL)) == SOCKET_ERROR)
//	{
//		puts("ERROR: ���� �ɼ��� ������ �� �����ϴ�.");
//	}
//
//	// 2. ��Ʈ ���ε�
//	SOCKADDR_IN svraddr = { 0 };
//	svraddr.sin_family = AF_INET;
//	svraddr.sin_port = htons(25000);
//	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
//	if (::bind(hSocket, (SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
//	{
//		puts("ERROR: ���� ���·� ��ȯ�� �� �����ϴ�.");
//		return 0;
//	}
//
//
//
//	// 3. ���Ӵ�� ���·� ��ȯ
//	// server�� Listen ���°� �ȴ�
//	if (::listen(hSocket, SOMAXCONN) == SOCKET_ERROR)
//	{
//		puts("ERROR: ���� ���·� ��ȯ�� �� �����ϴ�.");
//		return 0;
//	}
//	else
//	{
//		puts("Listen State");
//	}
//
//	// 4. Ŭ���̾�Ʈ ���� ó�� �� ����
//	SOCKADDR_IN clientaddr = { 0 };
//	int nAddrLen = sizeof(clientaddr);
//	SOCKET hClient = 0;
//	DWORD dwThreadID = 0;
//	HANDLE hThread;
//
//	// 4.1 Ŭ���̾�Ʈ ������ �޾Ƶ��̰� ���ο� ���� ����(����)
//	while ((hClient = ::accept(hSocket,
//		(SOCKADDR*)&clientaddr,
//		&nAddrLen)) != INVALID_SOCKET)
//	{
//		// 4.2 �� Ŭ���̾�Ʈ�� ����ϱ� ���� ������ ����
//		// Ŭ���̾�Ʈ ���� �����尡 �ϳ��� �����ȴ�
//		hThread = ::CreateThread(
//			NULL,				// ���ȼӼ� ���
//			0,					// ���� �޸𸮴� �⺻ũ��(1MB)
//			ThreadFunction,		// ������� ������ �Լ�
//			(LPVOID)hClient,	// ���� ������ Ŭ���̾�Ʈ ����
//			0,					// ���� �÷��״� �⺻�� ���
//			&dwThreadID);		// ������ ������ ID�� ����� ���� �ּ�
//
//		::CloseHandle(hThread);
//	}
//
//	// 5. ���� ���� �ݱ�
//	::closesocket(hSocket);
//
//	// ���� ����
//	::WSACleanup();
//	return 0;
//
//
//}