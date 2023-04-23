#include <iostream>
#include <Windows.h>

using namespace std;

//main() 함수와는 별도로 작동하는 작업자 스레드 함수.
DWORD WINAPI ThreadFunction(LPVOID pParam)
{
	puts("*** Begin Thread ****");
	for (int i = 0; i < 5; i++)
	{
		printf("[Worker thread] %d\n", i);
		::Sleep(1);
	}

	// 스레드가 끝나기 전에 이벤트를 세트한다.
	puts("종료 이벤트 세트 전");
	// 이 함수를 호출하면 main() 함수의
	// WaitForSingleObject() 함수가 반환 된다.
	::SetEvent((HANDLE)pParam);
	puts("종료 이벤트 세트 후");
	puts("*** End Thread ****");
	return 0;
}
int main()
{
	// 스레드 흐름을 확인하기 위한 신호 제어를 만들기 위한 이벤트 
	HANDLE hEvent = ::CreateEvent(
		NULL,	//디폴트 보안 속성 적용
		FALSE,	//자동으로 상태 전환
		FALSE,	//초기상태는 FALSE.
		NULL	//이름 없음
	);

	DWORD dwThreadID = 0;
	HANDLE hThread = ::CreateThread(
		NULL,	
		0,		
		ThreadFunction,	
		hEvent,	// 이벤트 핸들을 스레드 함수에 전달한다.
		0,		
		&dwThreadID);
	for (int i = 0; i < 5; i++)
	{
		printf("[Main thread] %d\n", i);
		if (i == 3 &&
			::WaitForSingleObject(hEvent, INFINITE) == WAIT_OBJECT_0)
		{
			puts("종료 이벤트를 감지했습니다!");
			::CloseHandle(hEvent);
		}
	}
	::CloseHandle(hEvent);
	return 0;
}