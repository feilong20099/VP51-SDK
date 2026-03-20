
#pragma once

#include <list>

template<typename T>
class CFifoList
{
public:
   CFifoList();
   virtual ~CFifoList();

   int		CreatePool(int nCount);
   int		ResizePool(int nCount);
   void		DestroyPool();

   int		PushEmptyPool(T* pPoolData);
   int		PushPoolData(T* pPoolData);

   T*	PopEmptyData(BOOL bPopFifoPool = FALSE);
   T*	PopPoolData();
   T*	PeekFrontPoolData();
   T*	PeekBackPoolData();

   void		ResetPool();
   std::list<T*>* LockEmptyPool();
   void		UnLockEmptyPool();
   std::list<T*>* LockDataPool();
   void		UnLockDataPool();
   int		GetEmptyDataCount();
   int		GetPoolDataCount();

   std::list<T*>* GetDataPool();

   int		m_nCount = 0;
private:
	CRITICAL_SECTION		m_CritSecDataList;
	CRITICAL_SECTION		m_CritSecEmptyList;
	std::list<T*>			m_DataList;
	std::list<T*>			m_EmptyList;
};

template<typename T>
CFifoList<T>::CFifoList()
{
	InitializeCriticalSection(&m_CritSecDataList);
	InitializeCriticalSection(&m_CritSecEmptyList);
}

template<typename T>
CFifoList<T>::~CFifoList()
{
	DestroyPool();
	DeleteCriticalSection(&m_CritSecDataList);
	DeleteCriticalSection(&m_CritSecEmptyList);
}

template<typename T>
int CFifoList<T>::CreatePool(int nCount)
{
	DestroyPool();

	T* pData;
	for (int i = 0; i < nCount; i++)
	{
		pData = new T;
		m_EmptyList.push_back(pData);
	}
	m_nCount = nCount;

	return nCount;
}

template<typename T>
int CFifoList<T>::ResizePool(int nCount)
{
	ResetPool();

	EnterCriticalSection(&m_CritSecEmptyList);
	T* pData;
	int nCur = (int)m_EmptyList.size();
	if (nCur > nCount)
	{
		while (m_EmptyList.size() > nCount)
		{
			pData = m_EmptyList.back();
			delete pData;
			m_EmptyList.pop_back();
		}
	}
	else if (nCur < nCount)
	{
		for (int i = nCur; i < nCount; i++)
		{
			pData = new T;
			m_EmptyList.push_back(pData);
		}
	}
	LeaveCriticalSection(&m_CritSecEmptyList);

	m_nCount = nCount;
	return nCount;
}

template<typename T>
void CFifoList<T>::DestroyPool()
{
	std::list<T*>::iterator iter;
	EnterCriticalSection(&m_CritSecDataList);
	EnterCriticalSection(&m_CritSecEmptyList);
	for (iter = m_DataList.begin(); iter != m_DataList.end(); iter++)
	{
		delete (*iter);
	}
	m_DataList.clear();
	for (iter = m_EmptyList.begin(); iter != m_EmptyList.end(); iter++)
	{
		delete (*iter);
	}
	m_EmptyList.clear();
	LeaveCriticalSection(&m_CritSecDataList);
	LeaveCriticalSection(&m_CritSecEmptyList);

	m_nCount = 0;
}

template<typename T>
int CFifoList<T>::PushEmptyPool(T* pPoolData)
{
	EnterCriticalSection(&m_CritSecEmptyList);
	m_EmptyList.push_back(pPoolData);
	LeaveCriticalSection(&m_CritSecEmptyList);
	return (int)m_EmptyList.size();
}

template<typename T>
int CFifoList<T>::PushPoolData(T* pPoolData)
{
	EnterCriticalSection(&m_CritSecDataList);
	m_DataList.push_back(pPoolData);
	LeaveCriticalSection(&m_CritSecDataList);
	return (int)m_DataList.size();
}

template<typename T>
T* CFifoList<T>::PopEmptyData(BOOL bPopFifoPool)
{
	T* pPoolData = NULL;
	EnterCriticalSection(&m_CritSecEmptyList);
	if (m_EmptyList.size() > 0)
	{
		pPoolData = m_EmptyList.front();
		m_EmptyList.pop_front();
	}
	if (bPopFifoPool && pPoolData == NULL)
	{
		EnterCriticalSection(&m_CritSecDataList);
		if (m_DataList.size() > 0)
		{
			pPoolData = m_DataList.front();
			m_DataList.pop_front();
		}
		LeaveCriticalSection(&m_CritSecDataList);
	}
	LeaveCriticalSection(&m_CritSecEmptyList);
	return pPoolData;
}

template<typename T>
T* CFifoList<T>::PopPoolData()
{
	T* pPoolData = NULL;
	EnterCriticalSection(&m_CritSecDataList);
	if (m_DataList.size() > 0)
	{
		pPoolData = m_DataList.front();
		m_DataList.pop_front();
	}
	LeaveCriticalSection(&m_CritSecDataList);
	return pPoolData;
}

template<typename T>
T* CFifoList<T>::PeekFrontPoolData()
{
	T* pPoolData = NULL;
	EnterCriticalSection(&m_CritSecDataList);
	if (m_DataList.size() > 0)
	{
		pPoolData = m_DataList.front();
	}
	LeaveCriticalSection(&m_CritSecDataList);
	return pPoolData;
}

template<typename T>
T* CFifoList<T>::PeekBackPoolData()
{
	T* pPoolData = NULL;
	EnterCriticalSection(&m_CritSecDataList);
	if (m_DataList.size() > 0)
	{
		pPoolData = m_DataList.back();
	}
	LeaveCriticalSection(&m_CritSecDataList);
	return pPoolData;
}

template<typename T>
void CFifoList<T>::ResetPool()
{
	std::list<T*>::iterator iter;
	EnterCriticalSection(&m_CritSecDataList);
	EnterCriticalSection(&m_CritSecEmptyList);

	for (iter = m_DataList.begin(); iter != m_DataList.end(); iter++)
	{
		m_EmptyList.push_back(*iter);
	}
	m_DataList.clear();
	LeaveCriticalSection(&m_CritSecDataList);
	LeaveCriticalSection(&m_CritSecEmptyList);
}

template<typename T>
std::list<T*>* CFifoList<T>::LockEmptyPool()
{
	EnterCriticalSection(&m_CritSecEmptyList);
	return &m_EmptyList;
}

template<typename T>
void CFifoList<T>::UnLockEmptyPool()
{
	LeaveCriticalSection(&m_CritSecEmptyList);
}

template<typename T>
std::list<T*>* CFifoList<T>::LockDataPool()
{
	EnterCriticalSection(&m_CritSecDataList);
	return &m_DataList;
}

template<typename T>
void CFifoList<T>::UnLockDataPool()
{
	LeaveCriticalSection(&m_CritSecDataList);
}

template<typename T>
int CFifoList<T>::GetEmptyDataCount()
{
	return (int)m_EmptyList.size();
}

template<typename T>
int CFifoList<T>::GetPoolDataCount()
{
	return (int)m_DataList.size();
}

template<typename T>
std::list<T*>* CFifoList<T>::GetDataPool()
{
	return &m_DataList;
}