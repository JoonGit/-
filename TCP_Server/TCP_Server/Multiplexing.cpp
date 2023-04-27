#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#include <Windows.h>
#include <list>
//#include <iterator>

/////////////////////////////////////////////////////////////////////////////////////////
SOCKET				g_hSocket;		// 서버의 리슨 소켓
std::list<SOCKET>	g_listClient;	// 연결된 클라이언트 소켓 리스트.

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


//// 새로 연결된 클라이언트의 소켓을 리스트에 저장한다.
//BOOL AddUser(SOCKET hSocket)
//{
//
//	// *이 코드는 오직 한 스레드만 수행한다는 것이 보장된다!
//	g_listClient.push_back(hSocket);
//
//
//	return TRUE;
//}

/////////////////////////////////////////////////////////////////////////////////////////
// 연결된 클라이언트 모두에게 메시지를 전송한다.
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
//// Ctrl+c 이벤트를 감지하고 프로그램을 종료한다.
BOOL CtrlHandeler(DWORD dwType)
{
	if (dwType == CTRL_C_EVENT)
	{
		::closesocket(g_hSocket);
		CloseAll();
		puts("모든 클라리언트 연결을 종료했습니다.");

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
//	printf("%d 클라이언트가 연결되었습니다 \n", hClient);
//
//	// 클라이언트로부터 문자열을 수신한다
//	while ((nReceive = ::recv(hClient, szBuffer, sizeof(szBuffer), 0)) > 0)
//	{
//		puts(szBuffer);
//		SendChattingMessage(szBuffer);
//		memset(szBuffer, 0, sizeof(szBuffer));
//	}
//
//	printf("%d 클라이언트 연결이 끊겼습니다 \n", hClient);
//	g_listClient.remove(hClient);
//
//	::closesocket(hClient);
//	return 0;
//}

int main()
{
	// 원속 초기화
	// 윈도우즈 소켓 라이브러리 초기화
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		puts("ERROR: 원속을 초기화 할 수 없습니다.");
		return 0;
	}


	// Ctrl+c 키를 눌렀을 때 이를 감지하고 처리할 함수를 등록한다.
	if (::SetConsoleCtrlHandler(
		(PHANDLER_ROUTINE)CtrlHandeler, TRUE) == FALSE)
		puts("ERROR: Ctrl+c 처리기를 등록할 수 없습니다.");

	// 1. 접속 대기 소겟 생성
	g_hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (g_hSocket == INVALID_SOCKET)
	{
		puts("ERROR: 접속 대기 소켓을 생성할 수 없습니다.");
		return 0;
	}

	// 2. 포트 바인딩
	SOCKADDR_IN svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
	svraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (::bind(g_hSocket, (SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
	{
		puts("ERROR: 소켓에 IP주소와 포트를 바인드 할 수 없습니다.");
		return 0;
	}

	// 3. 접속대기 상태로 전환
	// server는 Listen 상태가 된다
	if (::listen(g_hSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		puts("ERROR: 리슨 상태로 전환할 수 없습니다.");
		return 0;
	}



	// 4. 연결된 클라이언트 관리를 위한 리스트 컨테이너 생성
	g_listClient.push_back(g_hSocket);

	// 5. 소켓의 변화를 감시를 위한 반복문
	UINT nCount;
	FD_SET fdRead;
	std::list<SOCKET>::iterator it;

	puts("*** I/O 멀티플렉싱 채팅 서버를 시작합니다. ***");
	do
	{
		// 5-1. 클라이언트 접속 및 정보수신 변화 감시셋 초기화
		FD_ZERO(&fdRead);
		for (it = g_listClient.begin(); it != g_listClient.end(); ++it)
			FD_SET(*it, &fdRead);

		// 5-2 변화가 발생할 때까지 대기
		::select(0, &fdRead, NULL, NULL, NULL);

		// 5-3 변화가 감지된 소켓 셋 확인
		nCount = fdRead.fd_count;
		for (int nIndex = 0; nIndex < nCount; ++nIndex)
		{
			// 소켓에 변화 감시 프래그가 SET 되었는가?
			if (!FD_ISSET(fdRead.fd_array[nIndex], &fdRead))
				continue;

			// 5-3-1. 서버의 listen 소켓이 세트되었는가?
			// 즉, 누군가 연결을 시도했는가?
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
			
			// 5-3-2 클라이언트가 전송한 데이터가 있는 경우
			else
			{
				char szBuffer[1024] = { 0 };
				int nReceive = ::recv(fdRead.fd_array[nIndex],
					(char*)szBuffer, sizeof(szBuffer), 0);
				if (nReceive <= 0)
				{
					// 연결 종료.
					::closesocket(fdRead.fd_array[nIndex]);
					FD_CLR(fdRead.fd_array[nIndex], &fdRead);
					g_listClient.remove(fdRead.fd_array[nIndex]);
					puts("클라이언트가 연결을 끊었습니다.");
				}
				else
				{
					// 채팅 메시지 전송
					SendMessageAll(szBuffer, nReceive);
				}
			}
		}
	} while (g_hSocket != NULL);

	// 6. 모든 연결을 닫고 종료한다.
	CloseAll();

	puts("*** 채팅서버를 종료합니다. ***");
	return 0;

	//// 4.1 클라이언트 연결을 받아들이고 새로운 소켓 생성(개방)
	//while ((hClient = ::accept(g_hSocket,
	//	(SOCKADDR*)&clientaddr, &nAddrLen)) != INVALID_SOCKET)
	//{
	//	if (AddUser(hClient) == FALSE)
	//	{
	//		puts("ERROR : 더 이상 클라이언트 연결을 처리할 수 없습니다.");
	//		CtrlHandeler(CTRL_C_EVENT);
	//		break;
	//	}
	//	// 4.2 새 클라이언트와 통신하기 위한 스레드 생성
	//	// 클라이언트 마다 스레드가 하나씩 생성된다
	//	hThread = ::CreateThread(
	//		NULL,				// 보안속성 상속
	//		0,					// 스택 메모리는 기본크기(1MB)
	//		ThreadFunction,		// 스래드로 실행할 함수
	//		(LPVOID)hClient,	// 새로 생성된 클라이언트 소켓
	//		0,					// 생성 플래그는 기본값 사용
	//		&dwThreadID);		// 생성된 스레드 ID가 저장될 변수 주소

	//	::CloseHandle(hThread);
	//}




}