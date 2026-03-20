#pragma once


enum EC_THREAD_STATUS{EC_THREAD_QUIT = 0, EC_THREAD_SUSPEND = 1, EC_THREAD_RUNNING = 2};

struct EC_THREADDATA
{
	DWORD	dwStatus;//See EC_THREAD_STATUS
	HANDLE	hSuspendEvent;
	HANDLE	hResumeEvent;
	BOOL	bCancleThread;
	HANDLE  hThread;
	EC_THREADDATA()
	{
		dwStatus = 0;
		hThread = nullptr;

		hSuspendEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
		hResumeEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
		bCancleThread = FALSE;
	}
	~EC_THREADDATA()
	{
		::CloseHandle(hSuspendEvent);
		::CloseHandle(hResumeEvent);
	}
	__inline DWORD BeginThread(void* lpStartAddress, LPVOID lpParameter, int nPriority = THREAD_PRIORITY_NORMAL);
	__inline BOOL SetThreadPriority(int nPriority = THREAD_PRIORITY_NORMAL);
	__inline void ResumeThread();
	__inline void SuspendThread(BOOL bWaitState = TRUE);
	__inline BOOL IsSuspend();
	__inline BOOL NeedSleeping();
	__inline void QuitThread(unsigned int nTime = INFINITE, BOOL bMsgWait = FALSE);
};


DWORD EC_THREADDATA::BeginThread(void* lpStartAddress, LPVOID lpParameter, int nPriority)
{
	bCancleThread = FALSE;
	DWORD dwThreadId;
	dwStatus = EC_THREAD_SUSPEND;
	if (nPriority != THREAD_PRIORITY_NORMAL)
	{
		hThread = ::CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)lpStartAddress, lpParameter, CREATE_SUSPENDED, &dwThreadId);
		::SetThreadPriority(hThread, nPriority);
		::ResumeThread(hThread);
	}
	else
	{
		hThread = ::CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)lpStartAddress, lpParameter, 0, &dwThreadId);
	}
	if (hThread)
	{
		dwStatus = EC_THREAD_RUNNING;
	}
	else
		dwStatus = EC_THREAD_QUIT;
	return dwStatus;
}

BOOL EC_THREADDATA::SetThreadPriority(int nPriority)
{
	if (hThread)
	{
		return ::SetThreadPriority(hThread, nPriority);
	}
	return FALSE;
}

void EC_THREADDATA::ResumeThread()
{
	if (dwStatus == EC_THREAD_SUSPEND)
	{
		::ResetEvent(hResumeEvent);
		dwStatus = EC_THREAD_RUNNING;
		::WaitForSingleObject(hResumeEvent, INFINITE);
	}
}

void EC_THREADDATA::SuspendThread(BOOL bWaitState)
{
	if (dwStatus == EC_THREAD_RUNNING)
	{
		::ResetEvent(hSuspendEvent);
		::ResetEvent(hResumeEvent);
		dwStatus = EC_THREAD_SUSPEND;
		if (bWaitState)
			::WaitForSingleObject(hSuspendEvent, INFINITE);
	}
}

BOOL EC_THREADDATA::IsSuspend()
{
	if (dwStatus == EC_THREAD_SUSPEND)
	{
		::SetEvent(hSuspendEvent);
		return TRUE;
	}
	return FALSE;
}

BOOL EC_THREADDATA::NeedSleeping()
{
	if (dwStatus == EC_THREAD_SUSPEND)
	{
		::SetEvent(hSuspendEvent);
		return TRUE;
	}
	::SetEvent(hResumeEvent);
	return FALSE;
}

void EC_THREADDATA::QuitThread(unsigned int nTime, BOOL bMsgWait)
{
	if (dwStatus != EC_THREAD_QUIT && hThread)
	{
		ResumeThread();
		bCancleThread = TRUE;
		if (bMsgWait)
		{
			ULONGLONG ullBegin = GetTickCount64();
			BOOL bIsok = TRUE;
			while (bIsok)
			{
				DWORD dwTime = MsgWaitForMultipleObjects(1, &hThread, FALSE, 10, QS_ALLINPUT);
				//等到信号
				switch (dwTime)
				{
				case WAIT_OBJECT_0:
				{
					bIsok = FALSE;
				}
				break;

				case WAIT_OBJECT_0 + 1:
				{
					MSG msg;
					//有消息
					if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
				break;

				case WAIT_TIMEOUT:
				{
					//超时处理
				}
				break;
				}

				if ((GetTickCount64() - ullBegin > nTime) && bIsok)
				{
					TerminateThread(hThread, 0);
					break;
				}
			}
		}
		else
		{
			if (::WaitForSingleObject(hThread, nTime) != WAIT_OBJECT_0)
			{
				TerminateThread(hThread, 0);
			}
		}
	}
	dwStatus = EC_THREAD_QUIT;
	bCancleThread = FALSE;
	if (hThread)
	{
		::CloseHandle(hThread);
		hThread = nullptr;
	}
}