#ifndef  _WAV_OUT_DEVICE_H
#define  _WAV_OUT_DEVICE_H

#include <windows.h>
#include <mmsystem.h>
 
class CWaveBuffer
{
public:
	CWaveBuffer();
	virtual ~CWaveBuffer();
	BOOL Init(HWAVEOUT hWave, int Size);
	BOOL Write(PBYTE pData, int nBytes, int& BytesWritten, DWORD_PTR dwUserData = 0);
	BOOL WritePlannar(PBYTE pCh0, PBYTE pCh1, int nSamples, int nSampleBytes, int& nSamplesWritten, DWORD_PTR dwUserData = 0);
	void Flush();
	void Reset();
	int	 GetPendingDataSize(){return m_nBytes;}
	
private:
	WAVEHDR      m_Hdr;
	HWAVEOUT     m_hWave;
	int          m_nBytes;
};

class CWaveOut
{
public:
	CWaveOut();
	virtual ~CWaveOut();
	static int GetDriverCount();
	BOOL Initialize(LPCWAVEFORMATEX Format, int nBuffers, int BufferSize, UINT uDeviceID = WAVE_MAPPER);
	void Uninitialize();
	void Write(PBYTE Data, int nBytes, DWORD_PTR dwUserData = 0);
	void WritePlannar(PBYTE pCh0, PBYTE pCh1, int nSamples, DWORD_PTR dwUserData = 0);
	void Flush();
	void Wait();
	void Reset();
	int	 GetCurrentPendingDataSize();

	void AudioDataSendCB(LPWAVEHDR pWH);
	
	DWORD_PTR	m_dwPrevProcessedUserData;
	HANDLE		m_hWaveSent;
private:
	static void CALLBACK WaveCallback(HWAVEOUT hwo, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

	WAVEFORMATEX	m_Format;

	HANDLE			m_hSem;
	int				m_nBuffers;
	int				m_CurrentBuffer;
	BOOL			m_NoBuffer;
	CWaveBuffer		*m_Hdrs;
	HWAVEOUT		m_hWave;

};

#endif