#ifndef __T_LOCK_H__
#define __T_LOCK_H__

class TLock_t
{
public:
	TLock_t(void)
	{
		InitializeCriticalSection(&m_CS);
	};

	virtual ~TLock_t(void)
	{
		DeleteCriticalSection(&m_CS);
	};

	virtual void Lock(void)
	{
		EnterCriticalSection(&m_CS);
	};

	virtual void UnLock(void)
	{
		LeaveCriticalSection(&m_CS);
	};

protected:
	CRITICAL_SECTION m_CS;
};

class ATLock_t
{
public:
	ATLock_t(TLock_t* pTLock)
	{
		m_pTLock = pTLock;

		m_pTLock->Lock();
	};

	ATLock_t(TLock_t& lock)
	{
		m_pTLock = &lock;

		m_pTLock->Lock();
	};


	virtual ~ATLock_t()
	{
		m_pTLock->UnLock();
	};

protected:

	TLock_t* m_pTLock;
};

#endif //__T_LOCK_H__