// SampleAudioCaptureDlg.cpp : implementation file
//

#include "pch.h"
#include "MKCardSampleApp.h"
#include "SampleControlDlg.h"
#include "SampleAudioCaptureDlg.h"
#include "wavOut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum
{
	USER_MESSAGE_UPDATE_TIME_INFO = WM_USER + 100,
};

#define AUDIO_BLOCK_BYTES 8192 ////1024 * 8

/////////////////////////////////////////////////////////////////////////////
// CSampleAudioCaptureDlg dialog
CSampleAudioCaptureDlg::CSampleAudioCaptureDlg(const MKDevice_t& tDevice, int iCard, int iPort,
								   CWnd* pParent /*=NULL*/)
								   : CSampleControlDlg(tDevice, iCard, iPort, CSampleAudioCaptureDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSampleAudioCaptureDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bPlaySound = FALSE;
	m_hCapture = NULL;

	m_hEvent_Sound = NULL;
	m_pCaptureCacheAudio = NULL;

	m_pWavOut = nullptr;

	ZeroMemory(&m_CaptureData, sizeof(m_CaptureData));

}

void CSampleAudioCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSampleAudioCaptureDlg)
	DDX_Control(pDX, IDC_CHECK_PLAYAUDIO, m_btnPlaySound);
	DDX_Control(pDX, IDC_BUTTON_STOPCAPTURE, m_btn_StopCapture);
	DDX_Control(pDX, IDC_BUTTON_BEGINCAPTURE, m_btn_BeginCapture);
	DDX_Control(pDX, IDC_STATIC_FRAME_COUNTER, m_staticFrameCounter);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSampleAudioCaptureDlg, CSampleControlDlg)
	//{{AFX_MSG_MAP(CSampleAudioCaptureDlg)
	ON_BN_CLICKED(IDC_CHECK_PLAYAUDIO, OnCheck_PlaySound)
	ON_BN_CLICKED(IDC_BUTTON_BEGINCAPTURE, OnButton_BeginCapture)
	ON_BN_CLICKED(IDC_BUTTON_STOPCAPTURE, OnButton_StopCapture)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleAudioCaptureDlg message handlers

BOOL CSampleAudioCaptureDlg::OnInitDialog() 
{
	__super::OnInitDialog();

	if (CWaveOut::GetDriverCount() == 0)
		m_btnPlaySound.EnableWindow(FALSE);
	else
	{
		m_btnPlaySound.SetCheck(1);
		OnCheck_PlaySound();
	}

	const MKCardInfo_t* pCardInfo = &m_pDevice->m_DeviceInfo.card_info_list[m_iCard];
	const MKPortInfo_t* pPortInfo = &pCardInfo->port_info_list[m_iPort];
	CString sText;

	sText.Format(_T("In Card:%lu Port:%lu"), m_iCard+1, m_iPort+1);
	SetWindowText(sText);

	//////////////////////////////////////////////////////////////////////////
	m_AudioPool.CreatePool(400);

	return TRUE;
}

BOOL CSampleAudioCaptureDlg::DestroyWindow()
{
	OnButton_StopCapture();

	m_AudioPool.DestroyPool();

	return __super::DestroyWindow();
}

LRESULT CSampleAudioCaptureDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message)
	{
	case USER_MESSAGE_UPDATE_TIME_INFO:
	{
		double dTime = m_CaptureData.dTime;
		int nTime = (int)dTime;
		m_CaptureData.nHour = nTime / 3600;
		nTime -= m_CaptureData.nHour * 3600;
		m_CaptureData.nMinute = nTime / 60;
		m_CaptureData.nSecond = nTime - m_CaptureData.nMinute * 60;

		dTime -= nTime;
		nTime = int(dTime * 1000);
		_stprintf_s(m_CaptureData.szTimeCode, 40, L"%02d:%02d:%02d:%03d", m_CaptureData.nHour, m_CaptureData.nMinute,
			m_CaptureData.nSecond, nTime);
		m_staticFrameCounter.SetWindowText(m_CaptureData.szTimeCode);
	}
	break;
	}
	return __super::WindowProc(message, wParam, lParam);
}

void CSampleAudioCaptureDlg::OnCheck_PlaySound() 
{
	if (m_btnPlaySound.GetCheck() == BST_CHECKED)
	{
		SAFE_DELETE(m_pWavOut);

		WAVEFORMATEX wfx;
		wfx.wFormatTag = WAVE_FORMAT_PCM;
		wfx.nChannels = 2;
		wfx.wBitsPerSample = 16;
		wfx.nSamplesPerSec = 48000;
		wfx.nBlockAlign = wfx.nChannels * wfx.wBitsPerSample / 8;
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
		wfx.cbSize = 0;

		m_pWavOut = new CWaveOut;
		m_pWavOut->Initialize(&wfx, 5, AUDIO_BLOCK_BYTES);

		m_bPlaySound = TRUE;
	}
	else
	{
		m_bPlaySound = FALSE;
		Sleep(100);
		SAFE_DELETE(m_pWavOut);
	}
}


void CSampleAudioCaptureDlg::OnButton_BeginCapture() 
{
	if (m_hCapture)
		return;

	do 
	{
		m_AudioPool.ResetPool();
		auto pList = m_AudioPool.LockEmptyPool();
		for (auto pBuf : *pList)
		{
			pBuf->ExtendBuffer(AUDIO_BLOCK_BYTES);
		}
		m_AudioPool.UnLockEmptyPool();

		m_pCaptureCacheAudio = new Buffer_t;
		m_pCaptureCacheAudio->AllocateBuffer(AUDIO_BLOCK_BYTES);

		m_frame_counter_param[0].reset();
		m_frame_counter_param[1].reset();

		m_CaptureData.dTime = 0;

		// 创建事件
		m_hEvent_Sound = CreateEvent(NULL, FALSE, FALSE, NULL);

		// 开始采集
		MKCaptureStreamParam_t cap_stream_param;
		ZeroMemory(&cap_stream_param, sizeof(cap_stream_param));
		cap_stream_param.p_audio_user = this;
		cap_stream_param.p_capture_audio_cb = MKApi_CaptureStreamCb;

		MKError err = MKCrossApi_BeginCapture(&m_hCapture, (MKCaptureStreamParam_t*)&cap_stream_param, sizeof(MKCaptureStreamParam_t), m_hCard, m_iPort);
		if (err != MKError_No)
		{
			m_hCapture = NULL;
			AfxMessageBox(MKApi_GetErrorDescription(err));
			break;
		}

		m_PlayAudioThreadData.BeginThread(RunSoundThread, this);

		// 更新按钮状态
		m_btn_BeginCapture.EnableWindow(FALSE);
		m_btn_StopCapture.EnableWindow(TRUE);

		return;
	} while (0);

	SAFE_CLOSE_HANDLE(m_hEvent_Sound);
}

void CSampleAudioCaptureDlg::OnButton_StopCapture() 
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
	{
		if (msg.message=WM_PAINT) 
		{ 
			::DispatchMessage(&msg); 
		} 
	}
	// 停止采集
	if (m_hCapture)
	{
		MKCrossApi_StopCapture(m_hCapture);
		m_hCapture = NULL;	
	}

	m_PlayAudioThreadData.QuitThread();

	// 关闭句柄
	SAFE_CLOSE_HANDLE(m_hEvent_Sound);
	
	// 释放缓冲区
	m_AudioPool.ResetPool();

	SAFE_DELETE(m_pCaptureCacheAudio);

}


void CSampleAudioCaptureDlg::MKApi_CaptureStream(const MKCaptureStreamCb_t* p_cbdata)
{
	if(p_cbdata->channel != m_iPort) 
	{
		return ;
	}
	FRAME_COUNTER_PARAM* pParam = nullptr;
	if (p_cbdata->stream_type == MKStreamType_Video || p_cbdata->stream_type == MKStreamType_Audio)
	{
		pParam = &m_frame_counter_param[int(p_cbdata->stream_type) - 1];
		if (!pParam->bInit)
		{
			pParam->bInit = TRUE;
		}
		else
		{
			pParam->delta_counter = int(p_cbdata->frame_counter - pParam->last_frame_counter);
			if (pParam->delta_counter != 1)
				printf("=======Recv channel port:%d type:%d frameCounter:%I64d lost:%d ====\n", p_cbdata->channel, p_cbdata->stream_type, 
					p_cbdata->frame_counter, (pParam->delta_counter - 1));
		}
		pParam->last_frame_counter = p_cbdata->frame_counter;
	}

	if(p_cbdata->stream_type == MKStreamType_Video)
	{
	}
	else
	{
		if (p_cbdata->length == 1024)
		{
			m_CaptureData.dTime += p_cbdata->length / (double)192000;

			memcpy(m_pCaptureCacheAudio->m_pBuffer + m_pCaptureCacheAudio->m_nDataSize, p_cbdata->buffer, p_cbdata->length);
			m_pCaptureCacheAudio->m_nDataSize += p_cbdata->length;
			if (m_pCaptureCacheAudio->m_nDataSize >= AUDIO_BLOCK_BYTES)
			{
				Buffer_t* pAudioBuff = m_AudioPool.PopEmptyData();
				if (pAudioBuff)
				{
					m_AudioPool.PushPoolData(m_pCaptureCacheAudio);
					m_pCaptureCacheAudio = pAudioBuff;
					SetEvent(m_hEvent_Sound);

					//_stprintf_s(m_szDebugInfo, 1024, L"Recv ch:%d, data count = %d \n", p_cbdata->channel, m_AudioPool.GetPoolDataCount());
					//OutputDebugString(m_szDebugInfo);
				}
				else
				{
					//_stprintf_s(m_szDebugInfo, 1024, L"Recv ch:%d, type:%d audio pool fulled, drop data\r\n", p_cbdata->channel, p_cbdata->stream_type);
					//OutputDebugString(m_szDebugInfo);
				}
				m_pCaptureCacheAudio->ClearData();

				PostMessage(USER_MESSAGE_UPDATE_TIME_INFO, 0, m_CaptureData.dTime);
			}
		}
	}
}

void CSampleAudioCaptureDlg::Thread_Audio()
{
	Buffer_t* pAudioBuff;
	Buffer_t TempAudioBuff;
	TempAudioBuff.AllocateBuffer(AUDIO_BLOCK_BYTES);

	while (!m_PlayAudioThreadData.bCancleThread)
	{
		// 获取处理数据缓存
		pAudioBuff = m_AudioPool.PopPoolData();
		if (pAudioBuff == nullptr)
		{
			WaitForSingleObject(m_hEvent_Sound, 100);
		}
		else
		{
			if (m_bPlaySound && m_pWavOut)
			{
				m_pWavOut->Write(pAudioBuff->m_pData, pAudioBuff->m_nDataSize);
			}

			pAudioBuff->ClearData();
			// 放回缓存
			m_AudioPool.PushEmptyPool(pAudioBuff);
		}
	}

	TempAudioBuff.FreeBuffer();

}

