//#include <iostream>
//#include <Windows.h>
//
//using namespace std;
//
//main() �Լ��ʹ� ������ �۵��ϴ� �۾��� ������ �Լ�.
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
//		NULL,	//���ȼӼ� ���.
//		0,		//���� �޸𸮴� �⺻ũ��(1MB).
//		ThreadFunction,	//������� ������ �Լ��̸�.
//		NULL,	//�Լ��� ������ �Ű�����.
//		0,		//���� �÷��״� �⺻�� ���.
//		&dwThreadID);	//������ ������ID ����.
//	for (int i = 0; i < 10; i++)
//	{
//		printf("[Main thread] %d\n", i);
//	}
//
//	::CloseHandle(hThread);
//	//::Sleep(10);
//	return 0;
//}