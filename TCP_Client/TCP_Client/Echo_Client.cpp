
#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

// 서버가 보낸 메시지를 수신하고 화면에 출력하는 스레드 함수.
DWORD WINAPI ThreadReceive(LPVOID pParam)
{
	SOCKET hSocket = (SOCKET)pParam;
	char szBuffer[128] = { 0 };
	while (::recv(hSocket, szBuffer, sizeof(szBuffer), 0) > 0 )
	{
		printf("-> %s\n", szBuffer);
		memset(szBuffer, 0, sizeof(szBuffer));
	}
	puts("수신 스레드가 끝났습니다.");
	return 0;
}

DWORD WINAPI ThreadHertbeat(LPVOID pParam)
{
	SOCKET hSocket = (SOCKET)pParam;
	char szBuffer[128] = { 0 };
	char szText[128] = "연결되어 있습니다";
	while (::recv(hSocket, szBuffer, sizeof(szBuffer), 0) > 0)
	{
		printf("*** %s\n", szBuffer);
		//::send(hSocket, szText, sizeof(szText), 0);
		memset(szBuffer, 0, sizeof(szBuffer));
	}
	//puts("상태 확인이 끝났습니다.");
	return 1;
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

	// 1. 접속대기 소켓 생성
	SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		puts("ERROR: 소켓을 생성할 수 없습니다.");
		return 0;
	}

	// 2. 포트 바인딩 및 연결
	SOCKADDR_IN svraddr = { 0 };
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(25000);
//	svraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//InetPton(AF_INET, strIP, &ipv4addr)
	InetPton(AF_INET, L"127.0.0.1", &svraddr.sin_addr.s_addr);

	if (::connect(hSocket,
		(SOCKADDR*)&svraddr, sizeof(svraddr)) == SOCKET_ERROR)
	{
		puts("ERROR: 서버에 연결할 수 없습니다.");
		return 0;
	}
	
	puts("서버에 연결되었습니다\n");

	//2.1
	int nOpt = 1;
	::setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY,
		(char*)&nOpt, sizeof(nOpt));

	DWORD dwThreadID = 0;
	HANDLE hThread = ::CreateThread(NULL,	//보안속성 상속
		0,					//스택 메모리는 기본크기(1MB)
		ThreadReceive,		//스래드로 실행할 함수이름
		(LPVOID)hSocket,	//소켓 핸들을 매개변수로 넘김
		0,					//생성 플래그는 기본값 사용
		&dwThreadID);		//생성된 스레드ID가 저장될 변수주소
	::CloseHandle(hThread);

	//HANDLE hHertbitsThread = ::CreateThread(NULL,	
	//	0,					
	//	ThreadHertbeat,		
	//	(LPVOID)hSocket,	
	//	0,					
	//	&dwThreadID);		
	//::CloseHandle(hHertbitsThread);

	// 3. 채팅 메시지 송 / 수신
	char szBuffer[128] = { 0 };
	puts("채팅을 시작합니다. 메시지를 입력하세요.\n");
	while (1)
	{
		// 사용자로부터 문자열을 입력 받는다.
		memset(szBuffer, 0, sizeof(szBuffer));
		gets_s(szBuffer);
		if (strcmp(szBuffer, "EXIT") == 0) break;

		// 사용자가 입력한 문자열을 서버에 전송한다.
		::send(hSocket, szBuffer, strlen(szBuffer) + 1, 0);
		memset(szBuffer, 0, sizeof(szBuffer));
		printf("From server: %s\n", szBuffer);
	}

	// 4. 소켓을 닫고 종료
	//::shutdown(hSocket, SC_BOTH);
	::closesocket(hSocket);
	::Sleep(100);
	// 윈속 해제
	::WSACleanup();

	return 0;
}