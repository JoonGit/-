#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#include <Windows.h>
#include <list>
//#include <iterator>

/////////////////////////////////////////////////////////////////////////////////////////
SOCKET				g_hSocket;		
// 배열 요소에 대한 인덱스. 이 값은 WSA_MAXIMUM_WAIT_EVENTS보다 작다.
int					g_nListIndex;
// 소켓에 대한 이벤트 핸들의 배열
WSAEVENT			g_aListEvent[WSA_MAXIMUM_WAIT_EVENTS];
// 소켓의 배열. 이벤트 핸들 배열과 쌍을 이룬다
SOCKET				g_aListSocket[WSA_MAXIMUM_WAIT_EVENTS];

/////////////////////////////////////////////////////////////////////////////////////////
//void SendMessageAll(char* pszMessage, int nSize)
//{
//	std::list<SOCKET>::iterator it;
//	for (it = g_listClient.begin(); it != g_listClient.end(); ++it)
//		send(*it, pszMessage, nSize, 0);
//}

/////////////////////////////////////////////////////////////////////////////////////////
void CloseAll(void)
{
	for (int i = 0; i < g_nListIndex; i++)
	{
		::shutdown(g_aListSocket[i], SD_BOTH);
		::closesocket(g_aListSocket[i]);
		::WSACloseEvent(g_aListEvent[i]);
	}
}

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



	// 4. 통지 이벤트 수신을 위한 소켓 이벤트 등록.
	g_nListIndex = 0;
	g_aListSocket[g_nListIndex] = g_hSocket;
	g_aListEvent[g_nListIndex] = ::WSACreateEvent();

	// 서버 소켓 이므로 FD_ACCEPT 이벤트를 감시한다.
	if (::WSAEventSelect(g_hSocket, g_aListEvent[g_nListIndex],
		FD_ACCEPT) == SOCKET_ERROR)
	{
		puts("ERROR: WSAEventSelect()");
		return 0;
	}
	puts("*** 서버를 시작합니다. ***");

	// 5. 루프를 동명서 이벤트를 처리한다
	DWORD dwIndex;
	WSANETWORKEVENTS netEvent;
	while(TRUE)
	{
		// 소켓 이벤트를 기다린다.
		dwIndex = ::WSAWaitForMultipleEvents(
			g_nListIndex + 1,
			g_aListEvent,
			FALSE,
			100,
			FALSE);
		if (dwIndex == WSA_WAIT_FAILED)
			continue;

		// 이벤트가 발생한 소켓의 인덱스 및 이벤트 발생 이류를 확인한다
		if (::WSAEnumNetworkEvents(g_aListSocket[dwIndex],
			g_aListEvent[dwIndex], &netEvent) == SOCKET_ERROR)
			continue;

		// 5-1 클라이언트가 연결함.
		if (netEvent.lNetworkEvents & FD_ACCEPT)
		{
			if (netEvent.iErrorCode[FD_ACCEPT_BIT] != 0)
				continue;

			if (g_nListIndex >= WSA_MAXIMUM_WAIT_EVENTS)
			{
				puts("ERROR: 최대 접속상태!");
				continue;
			}

			SOCKADDR_IN clientaddr = { 0 };
			int nAddrLen = sizeof(clientaddr);
			SOCKET hClient = ::accept(g_hSocket,
				(SOCKADDR*)&clientaddr, &nAddrLen);
			if (hClient != INVALID_SOCKET)
			{
				g_nListIndex++;
				g_aListSocket[g_nListIndex] = hClient;
				g_aListEvent[g_nListIndex] = ::WSACreateEvent();
				puts("새 클라이언트가 연결됐습니다.");
			}
			::WSAEventSelect(hClient, g_aListEvent[g_nListIndex],
				FD_READ | FD_CLOSE);
		}
			
			else if (netEvent.lNetworkEvents & FD_CLOSE)
			{
				//클라이언트 소켓 및 이벤트 핸들을 닫는다.
				::WSACloseEvent(g_aListEvent[dwIndex]);
				::shutdown(g_aListSocket[dwIndex], SD_BOTH);
				::closesocket(g_aListSocket[dwIndex]);

				//닫은 소켓 및 핸들정보를 삭제하고 배열을 정리한다.
				for (DWORD i = dwIndex; i < g_nListIndex; ++i)
				{
					g_aListEvent[i] = g_aListEvent[i + 1];
					g_aListSocket[i] = g_aListSocket[i + 1];
				}

				g_nListIndex--;
				printf("클라이언트 연결 종료. 남은 수: %d.\n", g_nListIndex);
			}

			/////////////////////////////////////////////////////////////////
			//5-3. 클라이언트가 데이터를 전송함.
				else if (netEvent.lNetworkEvents & FD_READ)
			{
				TCHAR szBuffer[1024] = { 0 };
				int nRecv = ::recv(g_aListSocket[dwIndex],
					(char*)szBuffer, sizeof(szBuffer), 0);

				//연결된 모든 클라이언트에 같은 메시지를 전송한다.
				for (DWORD i = 1; i < g_nListIndex + 1; ++i)
					::send(g_aListSocket[i], (char*)szBuffer, nRecv, 0);
			}
			/////////////////////////////////////////////////////////////////
		}

		return 0;
	}