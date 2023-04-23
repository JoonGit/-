//#include <iostream>
//#include <Windows.h>
//
//using namespace std;
//
//main() 함수와는 별도로 작동하는 작업자 스레드 함수.
//DWORD WINAPI ThreadFunction(LPVOID pParam)
//{
//	puts("*** Begin Thread ****");
//
//	for (int i = 0; i < 5; i++)
//	{
//		printf("[Worker thread] %d\n", i);
//	}
//	puts("*** End Thread ****");
//
//	return 0;
//}
//
//int main()
//{
//
//	DWORD dwThreadID = 0;
//	HANDLE hThread = ::CreateThread(
//		NULL,	//보안속성 상속.
//		0,		//스택 메모리는 기본크기(1MB).
//		ThreadFunction,	//스래드로 실행할 함수이름.
//		NULL,	//함수에 전달할 매개변수.
//		0,		//생성 플래그는 기본값 사용.
//		&dwThreadID);	//생성된 스레드ID 저장.
//	for (int i = 0; i < 10; i++)
//	{
//		printf("[Main thread] %d\n", i);
//	}
//
//	::CloseHandle(hThread);
//	//::Sleep(10);
//	return 0;
//}