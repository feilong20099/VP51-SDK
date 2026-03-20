// CSampleSendDlg.cpp : implementation file
//

#include "pch.h"
#include "MKCardSampleApp.h"
#include "SampleControlDlg.h"
#include "VideoViewDlg.h"
#include "SampleSendDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum
{
	USER_MESSAGE_UPDATE_TIME_INFO = WM_USER + 100,
};
static const int SEEK_AUDIO_FRAME_COUNT = 5;

#define M_PI       3.14159265358979323846   // pi

static void FillImageBuff_ColorBar_uyvy(BYTE *pData, int nWidth, int nHeight)
{
	//						 White		  Yellow	  Cyan		  Green		  Magenta	  Red		  Blue		  Black
	const DWORD uyvy_ColorBar_Color[] = { 0xEB80EB80, 0xDB8ADB10, 0xBC10BC99, 0xAC1AAC29, 0x4EE54ED6, 0x3EF03E66, 0x1F751fF0, 0x10801080 };

	int nBarLen = nWidth / 16;

	DWORD *pDst = (DWORD *)pData;

	for (int i_line = 0; i_line < nHeight; i_line++)
	{
		for (int i_bar = 0; i_bar < 8; i_bar++)
		{
			for (int i = 0; i < nBarLen; i++)
			{
				*pDst++ = uyvy_ColorBar_Color[i_bar];
			}
		}
	}
}

static void FillWaveBuffer_Sine_16(BYTE* pBuff,
	UINT32 nBuffSize,
	UINT32 nSampleFrequency,
	UINT32 nMargin,
	UINT32 nCycleFrequency,
	UINT32& nSampleOffset)
{
	UINT32 sample_count_in_cycle = nSampleFrequency / nCycleFrequency;

	short* p_buff = (short*)pBuff;
	short* p_buff_end = (short*)(pBuff + nBuffSize);
	while (p_buff < p_buff_end)
	{
		*p_buff++ = (short)(SHRT_MAX * sin(2 * nSampleOffset * M_PI / sample_count_in_cycle));

		nSampleOffset++;
		if (nSampleOffset >= sample_count_in_cycle)
			nSampleOffset = 0;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSampleSendDlg dialog

CSampleSendDlg::CSampleSendDlg(const MKDevice_t& tDevice, int iCard, int iPort, 
							 CWnd* pParent /*=NULL*/)
	: CSampleControlDlg(tDevice, iCard, iPort, CSampleSendDlg::IDD, pParent)
{
	m_hSend = NULL;
	
	m_hFillDataEvent = NULL;
	m_hSendDataEvent = NULL;
	m_hVideoViewEvent = NULL;

	m_nVideoWidth = 0;
	m_nVideoHeight = 0;
	m_nVideoTimeNum = 0;
	m_nVideoTimeScale = 0;

	m_Send_State = SEND_STATE_STOP;

	m_bTimeCode = TRUE;
}

void CSampleSendDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSampleSendDlg)
	DDX_Control(pDX, IDC_SENDDLG_COMBO_STANDARD, m_ComboVideoStandard);
	DDX_Control(pDX, IDC_SENDDLG_BTN_BEGINSEND, m_btnBeginSend);
	DDX_Control(pDX, IDC_SENDDLG_BTN_STOPSEND, m_btnStopSend);
	DDX_Control(pDX, IDC_SENDDLG_STATIC_TIMEINFO, m_staticTimeInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSampleSendDlg, CSampleControlDlg)
	//{{AFX_MSG_MAP(CSampleSendDlg)
	ON_BN_CLICKED(IDC_SENDDLG_BTN_BEGINSEND, OnButton_BeginSend)
	ON_BN_CLICKED(IDC_SENDDLG_BTN_STOPSEND, OnButton_StopSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleSendDlg message handlers

BOOL CSampleSendDlg::OnInitDialog() 
{
	__super::OnInitDialog();

	const MKCardInfo_t* pCardInfo = &m_pDevice->m_DeviceInfo.card_info_list[m_iCard];
	const MKPortInfo_t* pPortInfo = &pCardInfo->port_info_list[m_iPort];

	CString sText;
	sText.Format(_T("Out Card:%lu Port:%lu"), m_iCard+1, m_iPort+1);
	SetWindowText(sText);
	
	sText.Format(_T("Out View Card:%lu Port:%lu"), m_iCard+1, m_iPort+1);
	m_pVideoViewDlg->SetWindowText(sText);

	MKVideoStandard* p_vstd_array;
	int vstd_count;
	if (MKCrossApi_GetSendVSTDArray(pCardInfo, nullptr, 0, &vstd_count))
	{
		p_vstd_array = new MKVideoStandard[vstd_count];
		MKCrossApi_GetSendVSTDArray(pCardInfo, p_vstd_array, vstd_count, nullptr);
		int nIndex;
		for (int i = 0; i < vstd_count; i++)
		{
			nIndex = m_ComboVideoStandard.AddString(MKApi_GetVideoStandardDesc(p_vstd_array[i]));
			m_ComboVideoStandard.SetItemData(nIndex, p_vstd_array[i]);
		}
		m_ComboVideoStandard.SelectString(0, MKApi_GetVideoStandardDesc(MKVS_HD_1080I_50));

		delete[] p_vstd_array;
	}

	m_checkTimeCode.SetCheck(BST_CHECKED);

	m_SendVideoPool.CreatePool(BUFFER_COUNT);
	m_SendAudioPool.CreatePool(BUFFER_COUNT);

	// 创建事件
	m_hFillDataEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hSendDataEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hVideoViewEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return TRUE;
}

BOOL CSampleSendDlg::DestroyWindow()
{
	OnButton_StopSend();

	m_SendVideoPool.DestroyPool();
	m_SendAudioPool.DestroyPool();

	SAFE_CLOSE_HANDLE(m_hFillDataEvent);
	SAFE_CLOSE_HANDLE(m_hSendDataEvent);
	SAFE_CLOSE_HANDLE(m_hVideoViewEvent);

	return __super::DestroyWindow();
}

LRESULT CSampleSendDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == USER_MESSAGE_UPDATE_TIME_INFO)
	{
		struct TIMECODE
		{
			BYTE frame;
			BYTE second;
			BYTE minute;
			BYTE hour;
		};
		TIMECODE* pTimeCode = (TIMECODE*)&lParam;

		_stprintf_s(m_szTimeCode, 40, L"%02d:%02d:%02d:%02d", pTimeCode->hour, pTimeCode->minute,
			pTimeCode->second, pTimeCode->frame);
		m_staticTimeInfo.SetWindowText(m_szTimeCode);
	}
	
	return __super::WindowProc(message, wParam, lParam);
}

void CSampleSendDlg::OnButton_BeginSend() 
{
	if (m_hSend)
		return;

	MKVideoStandard	standard;

	int nSel = m_ComboVideoStandard.GetCurSel();
	if (nSel == -1)
	{
		AfxMessageBox(L"please select output video stardard");
		return;
	}
	standard = (MKVideoStandard)m_ComboVideoStandard.GetItemData(nSel);

	// 获取制式信息
	m_nVideoWidth = MKApi_GetVideoWidth(standard);
	m_nVideoHeight = MKApi_GetVideoHeight(standard);
	m_nVideoTimeNum = MKApi_GetVideoTimeNum(standard);
	m_nVideoTimeScale = MKApi_GetVideoTimeScale(standard);
	m_dVideoFps = m_nVideoTimeNum / (double)m_nVideoTimeScale;

	// 分配缓冲区
	m_nVideoFrameBytes = m_nVideoWidth * m_nVideoHeight * 2;

	int video_alloc_bytes = m_nVideoFrameBytes + 1024 * 1024;

	m_SendVideoPool.ResetPool();
	auto pVideoPool = m_SendVideoPool.LockEmptyPool();
	for (auto pBuf : *pVideoPool)
	{
		pBuf->ExtendBuffer(video_alloc_bytes);
	}
	m_SendVideoPool.UnLockEmptyPool();

	int nSampleCount;
	if (m_nVideoTimeScale == 1)
		nSampleCount = 48000 / m_nVideoTimeNum;
	else
		nSampleCount = 48000 * m_nVideoTimeScale / m_nVideoTimeNum + 1;
	nSampleCount++;

	int planarSize = nSampleCount * 2;
	int totalSize = planarSize * MKAudio_CHANNEL_COUNT_MAX;

	m_SendAudioPool.CreatePool(BUFFER_COUNT);
	std::list<AUDIO_FRAME_SAMPLE*>* pAudioList = m_SendAudioPool.LockEmptyPool();
	for (auto pAudioSample : *pAudioList)
	{
		AllocAudioFrame(pAudioSample, totalSize, planarSize);
		memset(pAudioSample->pAllData, 0, totalSize);
	}
	m_SendAudioPool.UnLockEmptyPool();

	m_bTimeCode = m_checkTimeCode.GetCheck();

	ResetEvent(m_hFillDataEvent);
	ResetEvent(m_hSendDataEvent);
	ResetEvent(m_hVideoViewEvent);

	// 开始播出
	MKError err;
	MKSendParam_t send_param;
	ZeroMemory(&send_param, sizeof(send_param));

	send_param.p_send_cb = MKSendDataCb;
	send_param.p_user = this;
	send_param.video.standard = standard;
	send_param.video.sample_bits = 8;
	send_param.video.format = MEDIA_PIX_FMT_UYVY;
	//end
	err = MKCrossApi_BeginSend(&m_hSend, &send_param, sizeof(MKSendParam_t), m_hCard, m_iPort);
	if (err != MKError_No)
	{
		m_hSend = NULL;
		AfxMessageBox(MKApi_GetErrorDescription(err));
		return;
	}

	m_Send_State = SEND_STATE_PLAY;
	m_llSendCounter = 0;

	// 创建数据生成、预监、播出线程
	m_FillDataThreadData.BeginThread(FillDataThreadCB, this);
	m_VideoViewThreadData.BeginThread(VideoViewThreadCB, this);
	m_SendDataThreadData.BeginThread(SendDataThreadCB, this, THREAD_PRIORITY_HIGHEST);

	m_ComboVideoStandard.EnableWindow(FALSE);
	m_btnBeginSend.EnableWindow(FALSE);
	m_btnStopSend.EnableWindow(TRUE);

	if (!m_pVideoViewDlg->IsWindowVisible())
		OnButton_View();
}

void CSampleSendDlg::OnButton_StopSend() 
{
	if (m_hSend)
	{
		MKCrossApi_StopSend(m_hSend);
		m_hSend = NULL;
	}

	m_FillDataThreadData.QuitThread();
	m_SendDataThreadData.QuitThread();
	m_VideoViewThreadData.QuitThread();

	ClearVideoViewList();

	if (m_hWnd)
	{
		m_ComboVideoStandard.EnableWindow(TRUE);
		m_btnBeginSend.EnableWindow(TRUE);
		m_btnStopSend.EnableWindow(FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////
// 处理各类播出事件
void CSampleSendDlg::MKSendDataCb(const MKSendCb_t* p_cbdata, void* p_user)
{
	CSampleSendDlg* pThis = (CSampleSendDlg*)p_user;
	switch (p_cbdata->cb_type)
	{
	case MKCbType_SetData:
		SetEvent(pThis->m_hSendDataEvent);
		break;
	}
}

// 数据线程不停填充生成图像与音频数据
void CSampleSendDlg::FillDataThread()
{
	UINT64 nFrameCounter = 0;

	Buffer_t* pVideoBuff = NULL;
	AUDIO_FRAME_SAMPLE* pAudioSample = NULL;

	UINT32 nSampleOffset = 0;

	while (!m_FillDataThreadData.bCancleThread)
	{
		// 取缓存
		pVideoBuff = m_SendVideoPool.PopEmptyData();
		if (nullptr == pVideoBuff)
		{
			WaitForSingleObject(m_hFillDataEvent, 80);
			continue;
		}

		pAudioSample = m_SendAudioPool.PopEmptyData();

		// 使用播出计数和开始时间产生文本
		nFrameCounter++;

		// 填充缓存
		FillImageBuff_ColorBar_uyvy(pVideoBuff->m_pBuffer, m_nVideoWidth, m_nVideoHeight);
		pVideoBuff->m_nDataSize = m_nVideoFrameBytes;

		pAudioSample->frame_counter = nFrameCounter;
		pAudioSample->nSampleCount = MKApi_GetAudioFrameSampleCount(nFrameCounter, m_nVideoTimeNum, m_nVideoTimeScale);

		for(int i = 0; i < MKAudio_CHANNEL_COUNT_MAX; i++)
			pAudioSample->data_length[i] = pAudioSample->nSampleCount * 2;
		FillWaveBuffer_Sine_16(pAudioSample->buffer[0], pAudioSample->data_length[0],
			48000, SHRT_MAX, 1000, nSampleOffset);

		m_SendVideoPool.PushPoolData(pVideoBuff);
		m_SendAudioPool.PushPoolData(pAudioSample);

	}
}

// 播出线程响应播出事件，如果没有数据可播，就播出一帧空数据，并打印警告信息
void CSampleSendDlg::SendDataThread()
{
	DWORD nWait;
	int i;
	Buffer_t* pVideoBuff = NULL;
	AUDIO_FRAME_SAMPLE* pAudioFrame = NULL;

	unsigned __int64 llFrameCounter = 0;
	// 时码信息
	int nFrame = 0;
	int nSec = 0;
	int nMin = 0;
	int nHour = 0;
	INT64 nTimeCode = 0;

	MKError err;
	MKMediaData_t send_data;

	// 等待数据线程准备足够数据
	while (!m_SendDataThreadData.bCancleThread)
	{
		if (m_SendVideoPool.GetPoolDataCount() >= 8)
		{
			break;
		}
		Sleep(20);
	}
	// 启动播出
	SetEvent(m_hSendDataEvent);

	while (!m_SendDataThreadData.bCancleThread)
	{
		nWait = WaitForSingleObject(m_hSendDataEvent, 100);
		if (nWait == WAIT_OBJECT_0)
		{
			// 取数据缓存
			pVideoBuff = m_SendVideoPool.PopPoolData();
			if (!pVideoBuff)
			{
				// 获取缓存失败，机器速度过慢？ 
				printf("Send buffer is empty! The computer is too slow?\n\n");
				continue;
			}
			pAudioFrame = m_SendAudioPool.PopPoolData();

			llFrameCounter = pAudioFrame->frame_counter;
			// 计算时码信息
			FrameToTimeCode(llFrameCounter, nHour, nMin, nSec, nFrame, m_dVideoFps);
			nTimeCode = ( ((INT64)nHour) << 24) | (nMin << 16) | (nSec << 8)| (nFrame);

			// 设置播出参数
			ZeroMemory(&send_data, sizeof(MKMediaData_t));

			// 视频数据
			send_data.video_data.buffer = pVideoBuff->m_pData;
			send_data.video_data.data_length = pVideoBuff->m_nDataSize;
			send_data.video_data.buffer_alloc_length = pVideoBuff->m_nBufferSize;

			// 音频数据
			for (i = 0; i < MKAudio_CHANNEL_COUNT_MAX; i++)
			{
				send_data.audio_data.buffer[i] = pAudioFrame->buffer[i];
				send_data.audio_data.data_length[i] = pAudioFrame->data_length[i];
				send_data.audio_data.buffer_alloc_length[i] = pAudioFrame->buffer_alloc_length[i];
			}
			
			// 播出数据
			err = MKCrossApi_SetData(&send_data, sizeof(send_data), m_hSend);
			if (err != MKError_No)
			{
				auto pVideoPool = m_SendVideoPool.LockDataPool();
				pVideoPool->push_front(pVideoBuff);
				m_SendVideoPool.UnLockDataPool();

				auto pAudioPool = m_SendAudioPool.LockDataPool();
				pAudioPool->push_front(pAudioFrame);
				m_SendAudioPool.UnLockDataPool();
				continue;
			}
			m_llSendCounter = llFrameCounter;

			// 音频数据缓存放回音频缓冲链表备用
			m_SendAudioPool.PushEmptyPool(pAudioFrame);

			m_Critic_ViewVideoList.Lock();
			m_ViewVideoDataList.push_back(pVideoBuff);
			m_Critic_ViewVideoList.UnLock();

			// 通知产生数据
			SetEvent(m_hFillDataEvent);

			// 通知预监线程处理视频数据
			SetEvent(m_hVideoViewEvent);
			// 通知主界面绘制时码信息
			PostMessage(USER_MESSAGE_UPDATE_TIME_INFO, 0, nTimeCode);
		}
		else
		{
			if (m_Send_State == SEND_STATE_PLAY)
			{
				//printf("Wait SendData Event timeout \n");
			}
		}
	}
}

// 预监线程显示已播出数据的画面，并负责将视频缓存放回缓冲区。如果有过多数据来不及显示，直接跳过。
void CSampleSendDlg::VideoViewThread()
{
	UINT nWait;
	Buffer_t* pVideoBuff;

	while (!m_VideoViewThreadData.bCancleThread)
	{
		nWait = WaitForSingleObject(m_hVideoViewEvent, 100);
		if (nWait == WAIT_OBJECT_0)
		{
			m_Critic_ViewVideoList.Lock();
			if (m_ViewVideoDataList.size())
			{
				pVideoBuff = m_ViewVideoDataList.front();
				m_ViewVideoDataList.pop_front();
			}
			else
				pVideoBuff = nullptr;
			m_Critic_ViewVideoList.UnLock();

			if (!pVideoBuff)
				continue;

			// 显示播出画面
			if (m_pVideoViewDlg->IsWindowVisible())
			{
				m_pVideoViewDlg->DisplayVideo(pVideoBuff->m_pBuffer, m_nVideoWidth, m_nVideoHeight);
			}

			// 将已显示的缓存放回缓冲区
			m_SendVideoPool.PushEmptyPool(pVideoBuff);

			// 跳过来不及显示的视频数据
			ClearVideoViewList();
		}
	}
}

void CSampleSendDlg::ClearVideoViewList()
{
	m_Critic_ViewVideoList.Lock();
	if (m_ViewVideoDataList.size() > 0)
	{
		auto pPool = m_SendVideoPool.LockEmptyPool();
		for (auto pPoolData : m_ViewVideoDataList)
		{
			pPool->push_back(pPoolData);
		}
		m_SendVideoPool.UnLockEmptyPool();
		m_ViewVideoDataList.clear();
	}
	m_Critic_ViewVideoList.UnLock();
}

