/**
 * WinPR: Windows Portable Runtime
 * Process Thread Functions
 *
 * Copyright 2012 Marc-Andre Moreau <marcandre.moreau@gmail.com>
 * Copyright 2015 Hewlett-Packard Development Company, L.P.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>

#include <uzi/handle.h>

#include <uzi/thread.h>

#ifndef _WIN32

#include <uzi/crt.h>
#include <uzi/platform.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_EVENTFD_H
#include <sys/eventfd.h>
#endif

#include <errno.h>
#include <fcntl.h>

#include <uzi/collections.h>

#include "thread.h"

#include "../handle/handle.h"

#define TAG "thread"

static wListDictionary* thread_list = NULL;

static BOOL ThreadCloseHandle(HANDLE handle);
static void cleanup_handle(void* obj);

static BOOL ThreadIsHandled(HANDLE handle)
{
	WINPR_THREAD* pThread = (WINPR_THREAD*) handle;

	if (!pThread || (pThread->Type != HANDLE_TYPE_THREAD))
	{
		SetLastError(ERROR_INVALID_HANDLE);
		return FALSE;
	}

	return TRUE;
}

static int ThreadGetFd(HANDLE handle)
{
	WINPR_THREAD* pThread = (WINPR_THREAD*) handle;

	if (!ThreadIsHandled(handle))
		return -1;

	return pThread->pipe_fd[0];
}

static DWORD ThreadCleanupHandle(HANDLE handle)
{
	WINPR_THREAD* thread = (WINPR_THREAD*) handle;

	if (!ThreadIsHandled(handle))
		return WAIT_FAILED;

	if (pthread_mutex_lock(&thread->mutex))
		return WAIT_FAILED;

	if (!thread->joined)
	{
		int status;
		status = pthread_join(thread->thread, NULL);

		if (status != 0)
		{
			pthread_mutex_unlock(&thread->mutex);
			return WAIT_FAILED;
		}
		else
		{
			thread->joined = TRUE;
		}
	}

	if (pthread_mutex_unlock(&thread->mutex))
		return WAIT_FAILED;

	return WAIT_OBJECT_0;
}

static HANDLE_OPS ops =
{
	ThreadIsHandled,
	ThreadCloseHandle,
	ThreadGetFd,
	ThreadCleanupHandle
};

/**
 * TODO: implement thread suspend/resume using pthreads
 * http://stackoverflow.com/questions/3140867/suspend-pthreads-without-using-condition
 */
static BOOL set_event(WINPR_THREAD* thread)
{
	int length;
	BOOL status = FALSE;
#ifdef HAVE_EVENTFD_H
	eventfd_t val = 1;

	do
	{
		length = eventfd_write(thread->pipe_fd[0], val);
	}
	while ((length < 0) && (errno == EINTR));

	status = (length == 0) ? TRUE : FALSE;
#else

	if (WaitForSingleObject(thread, 0) != WAIT_OBJECT_0)
	{
		length = write(thread->pipe_fd[1], "-", 1);

		if (length == 1)
			status = TRUE;
	}
	else
	{
		status = TRUE;
	}

#endif
	return status;
}

static BOOL reset_event(WINPR_THREAD* thread)
{
	int length;
	BOOL status = FALSE;
#ifdef HAVE_EVENTFD_H
	eventfd_t value;

	do
	{
		length = eventfd_read(thread->pipe_fd[0], &value);
	}
	while ((length < 0) && (errno == EINTR));

	if ((length > 0) && (!status))
		status = TRUE;

#else
	length = read(thread->pipe_fd[0], &length, 1);

	if ((length == 1) && (!status))
		status = TRUE;

#endif
	return status;
}

static BOOL thread_compare(void* a, void* b)
{
	pthread_t* p1 = a;
	pthread_t* p2 = b;
	BOOL rc = pthread_equal(*p1, *p2);
	return rc;
}

/* Thread launcher function responsible for registering
 * cleanup handlers and calling pthread_exit, if not done
 * in thread function. */
static void* thread_launcher(void* arg)
{
	void* rc = NULL;
	WINPR_THREAD* thread = (WINPR_THREAD*) arg;
	typedef void* (*fkt_t)(void*);
	fkt_t fkt;

	if (!thread)
	{
		goto exit;
	}

	if (!(fkt = (fkt_t)thread->lpStartAddress))
	{
		goto exit;
	}

	if (pthread_mutex_lock(&thread->threadIsReadyMutex))
		goto exit;

	if (!ListDictionary_Contains(thread_list, &thread->thread))
	{
		if (pthread_cond_wait(&thread->threadIsReady, &thread->threadIsReadyMutex) != 0)
		{
			pthread_mutex_unlock(&thread->threadIsReadyMutex);
			goto exit;
		}
	}

	if (pthread_mutex_unlock(&thread->threadIsReadyMutex))
		goto exit;

	assert(ListDictionary_Contains(thread_list, &thread->thread));
	rc = fkt(thread->lpParameter);
exit:

	if (thread)
	{
		if (!thread->exited)
			thread->dwExitCode = (DWORD)(size_t)rc;

		set_event(thread);

		if (thread->detached || !thread->started)
			cleanup_handle(thread);
	}

	return rc;
}

static BOOL winpr_StartThread(WINPR_THREAD* thread)
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	if (thread->dwStackSize > 0)
		pthread_attr_setstacksize(&attr, (size_t) thread->dwStackSize);

	thread->started = TRUE;
	reset_event(thread);

	if (pthread_create(&thread->thread, &attr, thread_launcher, thread))
		goto error;

	if (pthread_mutex_lock(&thread->threadIsReadyMutex))
		goto error;

	if (!ListDictionary_Add(thread_list, &thread->thread, thread))
	{
		pthread_mutex_unlock(&thread->threadIsReadyMutex);
		goto error;
	}

	if (pthread_cond_signal(&thread->threadIsReady) != 0)
	{
		pthread_mutex_unlock(&thread->threadIsReadyMutex);
		goto error;
	}

	if (pthread_mutex_unlock(&thread->threadIsReadyMutex))
		goto error;

	pthread_attr_destroy(&attr);
	return TRUE;
error:
	pthread_attr_destroy(&attr);
	return FALSE;
}

HANDLE CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes,
                    SIZE_T dwStackSize,
                    LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter,
                    DWORD dwCreationFlags, LPDWORD lpThreadId)
{
	HANDLE handle;
	WINPR_THREAD* thread;
	thread = (WINPR_THREAD*) calloc(1, sizeof(WINPR_THREAD));

	if (!thread)
		return NULL;

	thread->dwStackSize = dwStackSize;
	thread->lpParameter = lpParameter;
	thread->lpStartAddress = lpStartAddress;
	thread->lpThreadAttributes = lpThreadAttributes;
	thread->ops = &ops;
	thread->pipe_fd[0] = -1;
	thread->pipe_fd[1] = -1;
#ifdef HAVE_EVENTFD_H
	thread->pipe_fd[0] = eventfd(0, EFD_NONBLOCK);

	if (thread->pipe_fd[0] < 0)
	{
		goto error_pipefd0;
	}

#else

	if (pipe(thread->pipe_fd) < 0)
	{
		goto error_pipefd0;
	}

	{
		int flags = fcntl(thread->pipe_fd[0], F_GETFL);
		fcntl(thread->pipe_fd[0], F_SETFL, flags | O_NONBLOCK);
	}

#endif

	if (pthread_mutex_init(&thread->mutex, 0) != 0)
	{
		goto error_mutex;
	}

	if (pthread_mutex_init(&thread->threadIsReadyMutex, NULL) != 0)
	{
		goto error_thread_ready_mutex;
	}

	if (pthread_cond_init(&thread->threadIsReady, NULL) != 0)
	{
		goto error_thread_ready;
	}

	WINPR_HANDLE_SET_TYPE_AND_MODE(thread, HANDLE_TYPE_THREAD, WINPR_FD_READ);
	handle = (HANDLE) thread;

	if (!thread_list)
	{
		thread_list = ListDictionary_New(TRUE);

		if (!thread_list)
		{
			goto error_thread_list;
		}

		thread_list->objectKey.fnObjectEquals = thread_compare;
	}

	if (!(dwCreationFlags & CREATE_SUSPENDED))
	{
		if (!winpr_StartThread(thread))
			goto error_thread_list;
	}
	else
	{
		if (!set_event(thread))
			goto error_thread_list;
	}

	return handle;
error_thread_list:
	pthread_cond_destroy(&thread->threadIsReady);
error_thread_ready:
	pthread_mutex_destroy(&thread->threadIsReadyMutex);
error_thread_ready_mutex:
	pthread_mutex_destroy(&thread->mutex);
error_mutex:

	if (thread->pipe_fd[1] >= 0)
		close(thread->pipe_fd[1]);

	if (thread->pipe_fd[0] >= 0)
		close(thread->pipe_fd[0]);

error_pipefd0:
	free(thread);
	return NULL;
}

void cleanup_handle(void* obj)
{
	int rc;
	WINPR_THREAD* thread = (WINPR_THREAD*) obj;
	rc = pthread_cond_destroy(&thread->threadIsReady);

	rc = pthread_mutex_destroy(&thread->threadIsReadyMutex);

	rc = pthread_mutex_destroy(&thread->mutex);

	if (thread->pipe_fd[0] >= 0)
		close(thread->pipe_fd[0]);

	if (thread->pipe_fd[1] >= 0)
		close(thread->pipe_fd[1]);

	if (thread_list && ListDictionary_Contains(thread_list, &thread->thread))
		ListDictionary_Remove(thread_list, &thread->thread);

	free(thread);
}

BOOL ThreadCloseHandle(HANDLE handle)
{
	WINPR_THREAD* thread = (WINPR_THREAD*) handle;

	if (!thread_list)
	{

	}
	else if (!ListDictionary_Contains(thread_list, &thread->thread))
	{

	}
	else
	{
		ListDictionary_Lock(thread_list);

		if ((thread->started) && (WaitForSingleObject(thread, 0) != WAIT_OBJECT_0))
		{
			thread->detached = TRUE;
			pthread_detach(thread->thread);
		}
		else
		{
			cleanup_handle(thread);
		}

		ListDictionary_Unlock(thread_list);

		if (ListDictionary_Count(thread_list) < 1)
		{
			ListDictionary_Free(thread_list);
			thread_list = NULL;
		}
	}

	return TRUE;
}

VOID ExitThread(DWORD dwExitCode)
{
	DWORD rc;
	pthread_t tid = pthread_self();

	if (!thread_list)
	{
		pthread_exit(0);
	}
	else if (!ListDictionary_Contains(thread_list, &tid))
	{
		pthread_exit(0);
	}
	else
	{
		WINPR_THREAD* thread;
		ListDictionary_Lock(thread_list);
		thread = ListDictionary_GetItemValue(thread_list, &tid);
		assert(thread);
		thread->exited = TRUE;
		thread->dwExitCode = dwExitCode;

		ListDictionary_Unlock(thread_list);
		set_event(thread);
		rc = thread->dwExitCode;

		if (thread->detached || !thread->started)
			cleanup_handle(thread);

		pthread_exit((void*)(size_t) rc);
	}
}

BOOL GetExitCodeThread(HANDLE hThread, LPDWORD lpExitCode)
{
	ULONG Type;
	WINPR_HANDLE* Object;
	WINPR_THREAD* thread;

	if (!winpr_Handle_GetInfo(hThread, &Type, &Object))
		return FALSE;

	thread = (WINPR_THREAD*) Object;
	*lpExitCode = thread->dwExitCode;
	return TRUE;
}

HANDLE _GetCurrentThread(VOID)
{
	HANDLE hdl = NULL;
	pthread_t tid = pthread_self();

	if (!thread_list)
	{

	}
	else if (!ListDictionary_Contains(thread_list, &tid))
	{

	}
	else
	{
		hdl = ListDictionary_GetItemValue(thread_list, &tid);
	}

	return hdl;
}

DWORD GetCurrentThreadId(VOID)
{
	pthread_t tid;
	tid = pthread_self();
	/* Since pthread_t can be 64-bits on some systems, take just the    */
	/* lower 32-bits of it for the thread ID returned by this function. */
	return (DWORD)tid & 0xffffffffUL;
}

DWORD ResumeThread(HANDLE hThread)
{
	ULONG Type;
	WINPR_HANDLE* Object;
	WINPR_THREAD* thread;

	if (!winpr_Handle_GetInfo(hThread, &Type, &Object))
		return (DWORD) - 1;

	thread = (WINPR_THREAD*) Object;

	if (pthread_mutex_lock(&thread->mutex))
		return (DWORD) - 1;

	if (!thread->started)
	{
		if (!winpr_StartThread(thread))
		{
			pthread_mutex_unlock(&thread->mutex);
			return (DWORD) - 1;
		}
	}
	else
	{
		/* thread already started */
	}

	if (pthread_mutex_unlock(&thread->mutex))
		return (DWORD) - 1;

	return 0;
}

BOOL SwitchToThread(VOID)
{
	/**
	 * Note: on some operating systems sched_yield is a stub returning -1.
	 * usleep should at least trigger a context switch if any thread is waiting.
	 */
	if (!sched_yield())
		usleep(1);

	return TRUE;
}

BOOL TerminateThread(HANDLE hThread, DWORD dwExitCode)
{
	ULONG Type;
	WINPR_HANDLE* Object;
	WINPR_THREAD* thread;

	if (!winpr_Handle_GetInfo(hThread, &Type, &Object))
		return FALSE;

	thread = (WINPR_THREAD*) Object;
	thread->exited = TRUE;
	thread->dwExitCode = dwExitCode;

	if (pthread_mutex_lock(&thread->mutex))
		return FALSE;

#ifndef ANDROID
	pthread_cancel(thread->thread);
#else
	/* function not supported on this platform! */
#endif

	if (pthread_mutex_unlock(&thread->mutex))
		return FALSE;

	set_event(thread);
	return TRUE;
}

#endif

