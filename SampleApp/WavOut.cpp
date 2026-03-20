#include "pch.h"
#include "wavOut.h"

#pragma comment(lib, "winmm.lib")

CWaveBuffer::CWaveBuffer()
{
	m_hWave = NULL;
	memset(&m_Hdr, 0, sizeof(m_Hdr));	
}

CWaveBuffer::~CWaveBuffer() 
{
    if (m_Hdr.lpData) 
	{
        waveOutUnprepareHeader(m_hWave, &m_Hdr, sizeof(WAVEHDR));
        LocalFree(m_Hdr.lpData);
    }
}

BOOL CWaveBuffer::Init(HWAVEOUT hWave, int Size)
{
    m_hWave  = hWave;
    m_nBytes = 0;

    m_Hdr.lpData = (LPSTR)LocalAlloc(LMEM_FIXED, Size);
    if (m_Hdr.lpData == NULL) 
	{
        return FALSE;
    }
	memset(m_Hdr.lpData, 0, Size);
	
    m_Hdr.dwBufferLength  = Size;
    m_Hdr.dwBytesRecorded = 0;
    m_Hdr.dwUser = 0;
    m_Hdr.dwFlags = 0;
    m_Hdr.dwLoops = 0;
    m_Hdr.lpNext = 0;
    m_Hdr.reserved = 0;

    waveOutPrepareHeader(hWave, &m_Hdr, sizeof(WAVEHDR));

    return TRUE;
}

void CWaveBuffer::Flush()
{
    m_nBytes = 0;
    waveOutWrite(m_hWave, &m_Hdr, sizeof(WAVEHDR));
}

void CWaveBuffer::Reset()
{
	m_nBytes = 0;
    m_Hdr.dwBytesRecorded = 0;
    m_Hdr.dwUser = 0;
}

BOOL CWaveBuffer::Write(PBYTE pData, int nBytes, int& BytesWritten, DWORD_PTR dwUserData)
{
	m_Hdr.dwUser = dwUserData;

    BytesWritten = min((int)m_Hdr.dwBufferLength - m_nBytes, nBytes);
    CopyMemory((PVOID)(m_Hdr.lpData + m_nBytes), (PVOID)pData, BytesWritten);
    m_nBytes += BytesWritten;
    if (m_nBytes == (int)m_Hdr.dwBufferLength) 
	{
        m_nBytes = 0;
        waveOutWrite(m_hWave, &m_Hdr, sizeof(WAVEHDR));
        return TRUE;
    }
    return FALSE;
}

BOOL CWaveBuffer::WritePlannar(PBYTE pCh0, PBYTE pCh1, int nSamples, int nSampleBytes, int& nSamplesWritten, DWORD_PTR dwUserData)
{
	m_Hdr.dwUser = dwUserData;

	nSamplesWritten = min(((int)m_Hdr.dwBufferLength - m_nBytes) / nSampleBytes / 2, nSamples);
	PBYTE pDst = (PBYTE)(m_Hdr.lpData + m_nBytes);
	for (int i = 0; i < nSamplesWritten; i++)
	{
		memcpy(pDst, pCh0, nSampleBytes);
		pCh0 += nSampleBytes;
		pDst += nSampleBytes;
		memcpy(pDst, pCh1, nSampleBytes);
		pCh1 += nSampleBytes;
		pDst += nSampleBytes;
	}
	m_nBytes += nSamplesWritten * nSampleBytes * 2;
	if (m_nBytes == (int)m_Hdr.dwBufferLength)
	{
		m_nBytes = 0;
		waveOutWrite(m_hWave, &m_Hdr, sizeof(WAVEHDR));
		return TRUE;
	}
	return FALSE;
}

void CALLBACK CWaveOut::WaveCallback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    if (uMsg == WOM_DONE) 
	{
		CWaveOut* pThis = (CWaveOut*)dwInstance;
		LPWAVEHDR pWH = (LPWAVEHDR)dwParam1;
		pThis->AudioDataSendCB(pWH);
    }
}

int CWaveOut::GetDriverCount()
{
	return waveOutGetNumDevs();
}

CWaveOut::CWaveOut() :
    m_nBuffers(0),
    m_CurrentBuffer(0),
    m_NoBuffer(TRUE),
	m_hSem(NULL),
	m_Hdrs(NULL),
    m_hWave(NULL)
{
	m_hWaveSent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

CWaveOut::~CWaveOut()
{
	Uninitialize();

	CloseHandle(m_hWaveSent);
}

BOOL CWaveOut::Initialize(LPCWAVEFORMATEX Format, int nBuffers, int BufferSize, UINT uDeviceID)
{
	Uninitialize();

	if(waveOutGetNumDevs() == 0)
		return FALSE;

	waveOutOpen(&m_hWave,
		uDeviceID,
		Format,
		(DWORD_PTR)WaveCallback,
		(DWORD_PTR)this,
		CALLBACK_FUNCTION);
	if(m_hWave == NULL)
		return FALSE;

	m_Format = *Format;
	m_Hdrs = new CWaveBuffer[nBuffers];
	m_nBuffers = nBuffers;

	for (int i = 0; i < nBuffers; i++) 
	{
		m_Hdrs[i].Init(m_hWave, BufferSize);
	}
	m_hSem = CreateSemaphore(NULL, nBuffers, nBuffers, NULL);
	return TRUE;
}

void CWaveOut::Uninitialize()
{
	if(m_hWave)
	{
		waveOutReset(m_hWave);
	}

	if(m_Hdrs)
	{
		delete [] m_Hdrs;
		m_Hdrs = NULL;
	}

	if(m_hWave)
	{
		waveOutClose(m_hWave);
		m_hWave = NULL;
	}
	if(m_hSem)
	{
		CloseHandle(m_hSem);
		m_hSem = NULL;
	}

}

void CWaveOut::Flush()
{
    if (!m_NoBuffer)
	{
        m_Hdrs[m_CurrentBuffer].Flush();
        m_NoBuffer = TRUE;
        m_CurrentBuffer = (m_CurrentBuffer + 1) % m_nBuffers;
    }
}

void CWaveOut::Reset()
{
	if(m_hWave == NULL)
		return;
    waveOutReset(m_hWave);
	for(int i = 0; i < m_nBuffers; i++)
	{
		m_Hdrs[i].Reset();
	}
	m_NoBuffer = TRUE;

	::ResetEvent(m_hWaveSent);

	LONG lPrevCount = 0;
	while(ReleaseSemaphore(m_hSem, 1, &lPrevCount))
	{
	}
}

void CWaveOut::Write(PBYTE pData, int nBytes, DWORD_PTR dwUserData)
{
	if(m_hWave == NULL)
		return;
    while (nBytes != 0)
	{
        if(m_NoBuffer) 
		{
            //WaitForSingleObject(m_hSem, INFINITE);
			if (WaitForSingleObject(m_hSem, 1000) == WAIT_TIMEOUT)
			{
				break;
			}
			
            m_NoBuffer = FALSE;
        }

        int nWritten;
        if (m_Hdrs[m_CurrentBuffer].Write(pData, nBytes, nWritten, dwUserData))
		{
            m_NoBuffer = TRUE;
            m_CurrentBuffer = (m_CurrentBuffer + 1) % m_nBuffers;
            nBytes -= nWritten;
            pData += nWritten;
        } 
		else
		{
            break;
        }
    }
}

void CWaveOut::WritePlannar(PBYTE pCh0, PBYTE pCh1, int nSamples, DWORD_PTR dwUserData)
{
	if (m_hWave == NULL || m_Format.nChannels != 2)
		return;
	while (nSamples != 0)
	{
		if (m_NoBuffer)
		{
			//WaitForSingleObject(m_hSem, INFINITE);
			if (WaitForSingleObject(m_hSem, 1000) == WAIT_TIMEOUT)
			{
				break;
			}

			m_NoBuffer = FALSE;
		}

		int nSampleBytes = m_Format.wBitsPerSample >> 3;
		int nWritten;
		if (m_Hdrs[m_CurrentBuffer].WritePlannar(pCh0, pCh1, nSamples, nSampleBytes, nWritten, dwUserData))
		{
			m_NoBuffer = TRUE;
			m_CurrentBuffer = (m_CurrentBuffer + 1) % m_nBuffers;
			nSamples -= nWritten;
			pCh0 += nWritten * nSampleBytes;
			pCh1 += nWritten * nSampleBytes;
		}
		else
		{
			break;
		}
	}
}

void CWaveOut::Wait()
{
	if(m_hWave == NULL)
		return;
    Flush();

    for (int i = 0; i < m_nBuffers; i++)
	{
        WaitForSingleObject(m_hSem, INFINITE);
    }
    LONG lPrevCount;
    ReleaseSemaphore(m_hSem, m_nBuffers, &lPrevCount);
}

void CWaveOut::AudioDataSendCB(LPWAVEHDR pWH)
{
	ReleaseSemaphore(m_hSem, 1, NULL);

	m_dwPrevProcessedUserData = pWH->dwUser;

	::SetEvent(m_hWaveSent);
}

int	 CWaveOut::GetCurrentPendingDataSize()
{
	return m_Hdrs[m_CurrentBuffer].GetPendingDataSize();
}