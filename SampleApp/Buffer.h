#pragma once

struct Buffer_t
{
	BYTE*	 m_pBuffer;
	UINT	 m_nBufferSize;

	BYTE*	 m_pData;
	UINT	 m_nDataSize;

	Buffer_t()
	{
		m_pBuffer = NULL;
		m_nBufferSize = 0;

		m_pData = NULL;
		m_nDataSize = 0;
	};

	virtual ~Buffer_t()
	{
		FreeBuffer();
	};

	virtual void FreeBuffer()
	{
		if (m_pBuffer == NULL)
			return;

		delete[] m_pBuffer;

		m_pBuffer = NULL;
		m_nBufferSize = 0;

		m_pData = NULL;
		m_nDataSize = 0;
	};

	virtual BOOL AllocateBuffer(UINT nBufferSize)
	{
		if ( nBufferSize == 0)
			return FALSE;

		FreeBuffer();

		m_pBuffer = new BYTE[nBufferSize];
		if ( m_pBuffer == NULL)
			return FALSE;
		m_nBufferSize = nBufferSize;

		m_pData = m_pBuffer;

		return true;
	};

	virtual BOOL ExtendBuffer(UINT nBufferSize)
	{
		if (nBufferSize <= m_nBufferSize) 
			return TRUE;

		return AllocateBuffer(nBufferSize);
	};

	virtual BOOL FillData(PBYTE pData, UINT nDataSize)
	{
		if (pData == NULL || nDataSize == 0) 
			return FALSE;

		if (m_nBufferSize < nDataSize)
			AllocateBuffer(nDataSize);

		memcpy(m_pBuffer, pData, nDataSize);
		m_nDataSize = nDataSize;

		return TRUE;
	};

	virtual BOOL AppendData(PBYTE pData, UINT nDataSize)
	{
		if (pData == NULL || nDataSize == 0) 
			return FALSE;

		if (!m_pBuffer && !m_nBufferSize)
			AllocateBuffer(nDataSize);

		if ( ( m_nBufferSize - ( m_pData - m_pBuffer)) < ( m_nDataSize + nDataSize))
		{
			Buffer_t bufferTemp;
			if (m_nDataSize)
			{
				if ( bufferTemp.FillData(m_pData, m_nDataSize) == FALSE)
					return FALSE;
			}

			PBYTE pBuffer = NULL;
			pBuffer = new BYTE[m_nDataSize + nDataSize];
			if ( pBuffer == NULL)
				return FALSE;

			if ( m_pBuffer != NULL)
				delete [] m_pBuffer;

			m_pBuffer = pBuffer;
			m_pData = pBuffer;

			m_nBufferSize = m_nDataSize + nDataSize;

			if (bufferTemp.m_nDataSize)
				memcpy( m_pData, bufferTemp.m_pData, bufferTemp.m_nDataSize);
			m_nDataSize = bufferTemp.m_nDataSize;
		}

		memcpy( m_pData + m_nDataSize, pData, nDataSize);
		m_nDataSize += nDataSize;

		return TRUE;
	};

	virtual void ClearData()
	{
		m_nDataSize = 0;
		m_pData = m_pBuffer;
	};
};