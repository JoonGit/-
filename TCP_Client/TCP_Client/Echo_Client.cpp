
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

// ������ ���� �޽����� �����ϰ� ȭ�鿡 ����ϴ� ������ �Լ�.
DWORD WINAPI ThreadReceive(LPVOID pParam)
{
	SOCKET hSocket = (SOCKET)pParam;
	char szBuffer[128] = { 0 };
	while (::recv(hSocket, szBuffer, sizeof(szBuffer), 0) > 0 )
	{
		printf("-> %s\n", szBuffer);
		memset(szBuffer, 0, sizeof(szBuffer));
	}
	puts("���� �����尡 �������ϴ�.");
	return 0;
}

DWORD WINAPI ThreadHertbeat(LPVOID pParam)
{
	SOCKET hSocket = (SOCKET)pParam;
	char szBuffer[128] = { 0 };
	char szText[128] = "����Ǿ� �ֽ��ϴ�";
	while (::recv(hSocket, szBuffer, sizeof(szBuffer), 0) > 0)
	{
		printf("*** %s\n", szBuffer);
		//::send(hSocket, szText, sizeof(szText), 0);
		memset(szBuffer, 0, sizeof(szBuffer));
	}
	//puts("���� Ȯ���� �������ϴ�.");
	return 1;
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

	// 1. ���Ӵ�� ���� ����
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: ������ ������ �� �����ϴ�.");
		return 0;
	}

	// 2. ��Ʈ ���ε� �� ����
	SOCKADDR_IN svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
//	svraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//InetPton(AF_INET, strIP, &ipv4addr)
	InetPton(AF_INET, L"127.0.0.1", &svraddr.sin_addr.s_addr);

	if (::connect(hSocket,
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
	{
		puts("ERROR: ������ ������ �� �����ϴ�.");
		return 0;
	}
	
	puts("������ ����Ǿ����ϴ�\n");

	//2.1
	int nOpt = 1;
	::setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY,
		(char*)&nOpt, sizeof(nOpt));

	DWORD dwThreadID = 0;
	HANDLE hThread = ::CreateThread(NULL,	//���ȼӼ� ���
		0,					//���� �޸𸮴� �⺻ũ��(1MB)
		ThreadReceive,		//������� ������ �Լ��̸�
		(LPVOID)hSocket,	//���� �ڵ��� �Ű������� �ѱ�
		0,					//���� �÷��״� �⺻�� ���
		&dwThreadID);		//������ ������ID�� ����� �����ּ�
	::CloseHandle(hThread);

	//HANDLE hHertbitsThread = ::CreateThread(NULL,	
	//	0,					
	//	ThreadHertbeat,		
	//	(LPVOID)hSocket,	
	//	0,					
	//	&dwThreadID);		
	//::CloseHandle(hHertbitsThread);

	// 3. ä�� �޽��� �� / ����
	char szBuffer[128] = { 0 };
	puts("ä���� �����մϴ�. �޽����� �Է��ϼ���.\n");
	while (1)
	{
		// ����ڷκ��� ���ڿ��� �Է� �޴´�.
		memset(szBuffer, 0, sizeof(szBuffer));
		gets_s(szBuffer);
		if (strcmp(szBuffer, "EXIT") == 0) break;

		// ����ڰ� �Է��� ���ڿ��� ������ �����Ѵ�.
		::send(hSocket, szBuffer, strlen(szBuffer) + 1, 0);
		memset(szBuffer, 0, sizeof(szBuffer));
		printf("From server: %s\n", szBuffer);
	}

	// 4. ������ �ݰ� ����
	//::shutdown(hSocket, SC_BOTH);
	::closesocket(hSocket);
	::Sleep(100);
	// ���� ����
	::WSACleanup();

	return 0;
}