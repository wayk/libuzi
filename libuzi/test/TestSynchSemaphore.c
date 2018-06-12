
#include <uzi/crt.h>
#include <uzi/synch.h>

int TestSynchSemaphore(int argc, char* argv[])
{
	HANDLE semaphore;

	semaphore = CreateSemaphoreA(NULL, 0, 1, NULL);

	if (!semaphore)
	{
		printf("CreateSemaphore failure\n");
		return -1;
	}

	CloseHandle(semaphore);

	return 0;
}

