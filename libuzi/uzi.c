
#include <uzi/synch.h>

#include <uzi/uzi.h>

UZI_HANDLE UziCreateEvent(bool manualReset, bool initialState)
{
	UZI_HANDLE handle;
	BOOL bManualReset = manualReset ? TRUE : FALSE;
	BOOL bInitialState = initialState ? TRUE : FALSE;
	handle = (UZI_HANDLE) CreateEventA(NULL, bManualReset, bInitialState, NULL);
	return handle;
}

bool UziSetEvent(UZI_HANDLE handle)
{
	return SetEvent((HANDLE) handle) ? true : false;
}

bool UziResetEvent(UZI_HANDLE handle)
{
	return ResetEvent((HANDLE) handle) ? true : false;
}

bool UziEventIsSet(UZI_HANDLE handle)
{
	return (UziWaitSingle(handle, 0) == UZI_WAIT_OBJECT_0) ? true : false;
}

bool UziCloseHandle(UZI_HANDLE handle)
{
	return CloseHandle((HANDLE) handle) ? true : false;
}

uint32_t UziWaitSingle(UZI_HANDLE handle, uint32_t timeout)
{
	uint32_t waitStatus;
	waitStatus = WaitForSingleObject((HANDLE) handle, timeout);
	return waitStatus;
}

uint32_t UziWaitMulti(uint32_t nCount, const UZI_HANDLE* handles, bool waitAll, uint32_t timeout)
{
	uint32_t waitStatus;
	BOOL bWaitAll = waitAll ? TRUE : FALSE;
	waitStatus = WaitForMultipleObjects(nCount, (const HANDLE*) handles, bWaitAll, timeout);
	return waitStatus;
}

bool UziInitCS(UZI_CS* cs, uint32_t spinCount, uint32_t flags)
{
	if (!spinCount)
		spinCount = 4000;

	return InitializeCriticalSectionEx((CRITICAL_SECTION*) cs, spinCount, flags) ? true : false;
}

void UziEnterCS(UZI_CS* cs)
{
	EnterCriticalSection((CRITICAL_SECTION*) cs);
}

bool UziTryEnterCS(UZI_CS* cs)
{
	return TryEnterCriticalSection((CRITICAL_SECTION*) cs) ? true : false;
}

void UziLeaveCS(UZI_CS* cs)
{
	LeaveCriticalSection((CRITICAL_SECTION*) cs);
}

void UziDeleteCS(UZI_CS* cs)
{
	DeleteCriticalSection((CRITICAL_SECTION*) cs);
}
