// SampleCaptureDlg.cpp : implementation file
//

#include "pch.h"
#include "MKCardSampleApp.h"
#include "SampleControlDlg.h"
#include "VideoViewDlg.h"
#include "SampleCaptureDlg.h"
#include "wavOut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum
{
	USER_MESSAGE_UPDATE_TIME_INFO = WM_USER + 100,
	USER_MESSAGE_SIGNAL_CHANGED,
};

#define AUDIO_BLOCK_BYTES 8192 ////1024 * 8

/////////////////////////////////////////////////////////////////////////////
// CSampleCaptureDlg dialog
CSampleCaptureDlg::CSampleCaptureDlg(const MKDevice_t& tDevice, int iCard, int iPort,
								   CWnd* pParent /*=NULL*/)
								   : CSampleControlDlg(tDevice, iCard, iPort, CSampleCaptureDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSampleCaptureDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bPlaySound = FALSE;
	m_hCapture = NULL;

	m_hEvent_Video = NULL;
	m_hEvent_Sound = NULL;

	m_pCaptureCacheAudio = NULL;

	m_pWavOut = nullptr;

	ZeroMemory(&m_CaptureData, sizeof(m_CaptureData));

}

void CSampleCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSampleCaptureDlg)
	DDX_Control(pDX, IDC_CHECK_PLAYAUDIO, m_btnPlaySound);
	DDX_Control(pDX, IDC_BUTTON_STOPCAPTURE, m_btn_StopCapture);
	DDX_Control(pDX, IDC_BUTTON_BEGINCAPTURE, m_btn_BeginCapture);
	DDX_Control(pDX, IDC_STATIC_FRAME_COUNTER, m_staticFrameCounter);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSampleCaptureDlg, CSampleControlDlg)
	//{{AFX_MSG_MAP(CSampleCaptureDlg)
	ON_BN_CLICKED(IDC_CHECK_PLAYAUDIO, OnCheck_PlaySound)
	ON_BN_CLICKED(IDC_BUTTON_GETMEDIAINFO, OnButton_GetMediaInfo)
	ON_BN_CLICKED(IDC_BUTTON_BEGINCAPTURE, OnButton_BeginCapture)
	ON_BN_CLICKED(IDC_BUTTON_STOPCAPTURE, OnButton_StopCapture)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleCaptureDlg message handlers

BOOL CSampleCaptureDlg::OnInitDialog() 
{
	__super::OnInitDialog();

	if (CWaveOut::GetDriverCount() == 0)
		m_btnPlaySound.EnableWindow(FALSE);

	const MKCardInfo_t* pCardInfo = &m_pDevice->m_DeviceInfo.card_info_list[m_iCard];
	const MKPortInfo_t* pPortInfo = &pCardInfo->port_info_list[m_iPort];
	CString sText;

	// 初始化对话框坐标

	sText.Format(_T("In Card:%lu Port:%lu"), m_iCard+1, m_iPort+1);
	SetWindowText(sText);

	sText.Format(_T("In View Card:%lu Port:%lu"), m_iCard+1, m_iPort+1);
	m_pVideoViewDlg->SetWindowText(sText);

	//////////////////////////////////////////////////////////////////////////
	OnButton_GetMediaInfo();

	MKCrossApi_SetSignalChangeCallBack(MKApi_SignalChangedCb, this, TRUE, m_hCard, m_iPort);

	m_VideoPool.CreatePool(BUFFER_COUNT);
	m_AudioPool.CreatePool(BUFFER_COUNT * 2);

	return TRUE;
}

BOOL CSampleCaptureDlg::DestroyWindow()
{
	MKCrossApi_SetSignalChangeCallBack(MKApi_SignalChangedCb, this, FALSE, m_hCard, m_iPort);

	OnButton_StopCapture();

	m_VideoPool.DestroyPool();
	m_AudioPool.DestroyPool();

	return __super::DestroyWindow();
}

LRESULT CSampleCaptureDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	case USER_MESSAGE_UPDATE_TIME_INFO:
		FrameToTimeCode(lParam, m_CaptureData.nHour, m_CaptureData.nMinute, m_CaptureData.nSecond,
			m_CaptureData.nFrame, m_CaptureData.fps);
		_stprintf_s(m_CaptureData.szTimeCode, 40, L"%02d:%02d:%02d:%02d", m_CaptureData.nHour, m_CaptureData.nMinute, 
			m_CaptureData.nSecond, m_CaptureData.nFrame);
		m_staticFrameCounter.SetWindowText(m_CaptureData.szTimeCode);
		break;
	case USER_MESSAGE_SIGNAL_CHANGED:
	{
		MKError	err;
		MKVideoInfoEx_t signal_video_info;
		MKAudioInfo_t signal_audio_info;
		err = MKCrossApi_GetMediaInfo_2(&signal_video_info, &signal_audio_info,
			m_hCard, m_iPort);
		if (err != MKError_No)
		{
			_tprintf(MKApi_GetErrorDescription(err));
			SetDlgItemText(IDC_EDIT_SIGNAL_INFO, L"error");
			break;
		}
		TCHAR szInfo[255];
		MKApi_FormatVideoInfoEx(szInfo, 255, &signal_video_info);
		SetDlgItemText(IDC_EDIT_SIGNAL_INFO, szInfo);

		if (m_hCapture)
		{
			if (signal_video_info.width != m_CaptureData.signal_video_info.width ||
				signal_video_info.height != m_CaptureData.signal_video_info.height ||
				signal_video_info.fps != m_CaptureData.signal_video_info.fps ||
				signal_video_info.bProgressiveFrame != m_CaptureData.signal_video_info.bProgressiveFrame)
			{
				OnButton_StopCapture();
				OnButton_BeginCapture();
			}
		}
	}
	break;
	}
	
	return __super::WindowProc(message, wParam, lParam);
}

void CSampleCaptureDlg::MKApi_SignalChanged(const MKCaptureStreamCb_t* p_cbdata)
{
	PostMessage(USER_MESSAGE_SIGNAL_CHANGED, 0, 0);
}

void CSampleCaptureDlg::OnButton_GetMediaInfo() 
{
	MKVideoInfoEx_t signal_video_info;
	MKAudioInfo_t signal_audio_info;
	MKError	err;
	err = MKCrossApi_GetMediaInfo_2(&signal_video_info, &signal_audio_info,
		m_hCard, m_iPort);
	if (err != MKError_No)
	{
		_tprintf(MKApi_GetErrorDescription(err));
		SetDlgItemText(IDC_EDIT_SIGNAL_INFO, L"error");
		return;
	}
	TCHAR szInfo[255];
	MKApi_FormatVideoInfoEx(szInfo, 255, &signal_video_info);
	SetDlgItemText(IDC_EDIT_SIGNAL_INFO, szInfo);
}

void CSampleCaptureDlg::OnCheck_PlaySound() 
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

void CSampleCaptureDlg::OnButton_BeginCapture() 
{
	if (m_hCapture)
		return;
	MKError	err;
	MKVideoInfoEx_t signal_video_info;
	MKAudioInfo_t signal_audio_info;
	err = MKCrossApi_GetMediaInfo_2(&signal_video_info, &signal_audio_info,
		m_hCard, m_iPort);
	if (err != MKError_No)
	{
		AfxMessageBox(L"信号获取失败");
		return;
	}
	if (signal_video_info.standard == MKVS_Unknow)
	{
		AfxMessageBox(L"没有信号不能开始采集");
		return;
	}

	m_CaptureData.signal_video_info = signal_video_info;
	m_CaptureData.signal_audio_info = signal_audio_info;
	m_CaptureData.width = signal_video_info.width;
	m_CaptureData.height = signal_video_info.height;
	m_CaptureData.pixel_format = MEDIA_PIX_FMT_UYVY;
	m_CaptureData.fps = signal_video_info.fps;

	// 分配缓冲区
	int video_frame_bytes = m_CaptureData.width * m_CaptureData.height * 2;
	int alloc_video_frame_bytes = video_frame_bytes + 1024 * 1024;

	m_VideoPool.ResetPool();
	std::list<Buffer_t*>* pList = m_VideoPool.LockEmptyPool();
	for (auto pBuf : *pList)
	{
		pBuf->ExtendBuffer(alloc_video_frame_bytes);
	}
	m_VideoPool.UnLockEmptyPool();

	m_AudioPool.ResetPool();
	pList = m_AudioPool.LockEmptyPool();
	for (auto pBuf : *pList)
	{
		pBuf->ExtendBuffer(AUDIO_BLOCK_BYTES);
	}
	m_AudioPool.UnLockEmptyPool();

	m_pCaptureCacheAudio = new Buffer_t;
	m_pCaptureCacheAudio->AllocateBuffer(AUDIO_BLOCK_BYTES);

	m_frame_counter_param[0].reset();
	m_frame_counter_param[1].reset();

	// 创建事件
	if (m_hEvent_Video == nullptr)
		m_hEvent_Video = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hEvent_Sound == nullptr)
		m_hEvent_Sound = CreateEvent(NULL, FALSE, FALSE, NULL);

	// 开始采集
	MKCaptureStreamParam_t cap_stream_param;
	ZeroMemory(&cap_stream_param, sizeof(cap_stream_param));
	cap_stream_param.p_video_user = this;
	cap_stream_param.p_capture_video_cb = MKApi_CaptureStreamCb;
	cap_stream_param.p_audio_user = this;
	cap_stream_param.p_capture_audio_cb = MKApi_CaptureStreamCb;

	err = MKCrossApi_BeginCapture(&m_hCapture, (MKCaptureStreamParam_t*)&cap_stream_param, sizeof(MKCaptureStreamParam_t), m_hCard, m_iPort);
	if (err != MKError_No)
	{
		m_hCapture = NULL;
		AfxMessageBox(MKApi_GetErrorDescription(err));
		return;
	}

	m_VideoViewThreadData.BeginThread(RunViewThread, this);
	m_PlayAudioThreadData.BeginThread(RunSoundThread, this);

	// 更新按钮状态
	m_btn_BeginCapture.EnableWindow(FALSE);
	m_btn_StopCapture.EnableWindow(TRUE);

	if (!m_pVideoViewDlg->IsWindowVisible())
		OnButton_View();
	
	printf("Begin capture!\nCard=%d Port=%d\n\n", m_iCard, m_iPort);
}

void CSampleCaptureDlg::OnButton_StopCapture() 
{
	// 停止采集
	if (m_hCapture)
	{
		MKCrossApi_StopCapture(m_hCapture);
		m_hCapture = NULL;	
	}

	m_VideoViewThreadData.QuitThread();
	m_PlayAudioThreadData.QuitThread();

	// 关闭句柄
	SAFE_CLOSE_HANDLE(m_hEvent_Video);
	SAFE_CLOSE_HANDLE(m_hEvent_Sound);
	
	// 释放缓冲区
	m_VideoPool.ResetPool();
	m_AudioPool.ResetPool();

	SAFE_DELETE(m_pCaptureCacheAudio);

	if (m_hWnd)
	{
		m_btn_BeginCapture.EnableWindow(TRUE);
		m_btn_StopCapture.EnableWindow(FALSE);
	}

	printf("Stop capture!\nCard=%d Port=%d\n\n", m_iCard, m_iPort);
}

//////////////////////////////////////////////////////////////////////////


void CSampleCaptureDlg::MKApi_CaptureStream(const MKCaptureStreamCb_t* p_cbdata)
{
	if(p_cbdata->channel != m_iPort) 
	{
		printf("======== Recv ch:%d, type:%d data\r\n", p_cbdata->channel, p_cbdata->stream_type);
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
		Buffer_t* pVideoBuff = m_VideoPool.PopEmptyData();
		if (!pVideoBuff)
		{
			printf("Video Pool fulled !\n");
			return;
		}
		pVideoBuff->FillData(p_cbdata->buffer, p_cbdata->length);

		m_VideoPool.PushPoolData(pVideoBuff);

		SetEvent(m_hEvent_Video);

		PostMessage(USER_MESSAGE_UPDATE_TIME_INFO, 0, p_cbdata->frame_counter);
	}
	else
	{
		if (p_cbdata->length == 1024)
		{
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
				}
				else
				{
					printf("======== Recv ch:%d, type:%d audio pool fulled, drop data\r\n", p_cbdata->channel, p_cbdata->stream_type);
				}
				m_pCaptureCacheAudio->ClearData();
			}
		}
	}
}

void CSampleCaptureDlg::VideoViewThread()
{
	Buffer_t* pVideoBuff;

	while (!m_VideoViewThreadData.bCancleThread)
	{
		// 处理视频事件
		// 获取数据缓存
		pVideoBuff = m_VideoPool.PopPoolData();
		if (pVideoBuff == nullptr)
		{
			WaitForSingleObject(m_hEvent_Video, 100);
		}
		else
		{
			// 显示视频数据
			if (pVideoBuff && pVideoBuff->m_nDataSize && m_pVideoViewDlg->IsWindowVisible())
			{
				m_pVideoViewDlg->DisplayVideo(pVideoBuff->m_pBuffer, m_CaptureData.width, m_CaptureData.height);
			}

			// 放回缓存
			m_VideoPool.PushEmptyPool(pVideoBuff);
			// 如果仍然还有视频数据,说明显示处理太慢，全部取出，跳过处理
			m_VideoPool.ResetPool();
		}
	}
}

void CSampleCaptureDlg::Thread_Audio()
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

