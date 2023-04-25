#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")

int main()
{
	//���� �ʱ�ȭ
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: ������ �ʱ�ȭ �� �� �����ϴ�.");
		return 0;
	}

	// ���� ����
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: ������ ������ �� �����ϴ�.");
		return 0;
	}

	// ������ '�۽�' ������ ũ�⸦ Ȯ���ϰ� ����Ѵ�.
	int nBufSize = 0, nLen = sizeof(nBufSize);
	if (::getsockopt(hSocket, SOL_SOCKET,
		SO_SNDBUF, (char*)&nBufSize, &nLen) != SOCKET_ERROR)
		printf("Send buffer size: %d\n", nBufSize);

	// ������ '����' ������ ũ�⸦ Ȯ���ϰ� ����Ѵ�.
	nBufSize = 0;
	nLen = sizeof(nBufSize);
	if (::getsockopt(hSocket, SOL_SOCKET,
		SO_RCVBUF, (char*)&nBufSize, &nLen) != SOCKET_ERROR)
		printf("Receive buffer size: %d\n", nBufSize);

	// ������ �ݰ� ����.
	::closesocket(hSocket);
	::WSACleanup();
	return 0;
}