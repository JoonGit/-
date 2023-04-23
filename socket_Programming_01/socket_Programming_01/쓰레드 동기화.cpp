#include <iostream>
#include <Windows.h>

using namespace std;

//main() �Լ��ʹ� ������ �۵��ϴ� �۾��� ������ �Լ�.
DWORD WINAPI ThreadFunction(LPVOID pParam)
{
	puts("*** Begin Thread ****");
	for (int i = 0; i < 5; i++)
	{
		printf("[Worker thread] %d\n", i);
		::Sleep(1);
	}

	// �����尡 ������ ���� �̺�Ʈ�� ��Ʈ�Ѵ�.
	puts("���� �̺�Ʈ ��Ʈ ��");
	// �� �Լ��� ȣ���ϸ� main() �Լ���
	// WaitForSingleObject() �Լ��� ��ȯ �ȴ�.
	::SetEvent((HANDLE)pParam);
	puts("���� �̺�Ʈ ��Ʈ ��");
	puts("*** End Thread ****");
	return 0;
}
int main()
{
	// ������ �帧�� Ȯ���ϱ� ���� ��ȣ ��� ����� ���� �̺�Ʈ 
	HANDLE hEvent = ::CreateEvent(
		NULL,	//����Ʈ ���� �Ӽ� ����
		FALSE,	//�ڵ����� ���� ��ȯ
		FALSE,	//�ʱ���´� FALSE.
		NULL	//�̸� ����
	);

	DWORD dwThreadID = 0;
	HANDLE hThread = ::CreateThread(
		NULL,	
		0,		
		ThreadFunction,	
		hEvent,	// �̺�Ʈ �ڵ��� ������ �Լ��� �����Ѵ�.
		0,		
		&dwThreadID);
	for (int i = 0; i < 5; i++)
	{
		printf("[Main thread] %d\n", i);
		if (i == 3 &&
			::WaitForSingleObject(hEvent, INFINITE) == WAIT_OBJECT_0)
		{
			puts("���� �̺�Ʈ�� �����߽��ϴ�!");
			::CloseHandle(hEvent);
		}
	}
	::CloseHandle(hEvent);
	return 0;
}