#pragma once


/////////////////////////////////////////////////////////////////////////////
// CSampleAudioCaptureDlg dialog

class CSampleAudioCaptureDlg : public CSampleControlDlg
{
// Construction
public:
	CSampleAudioCaptureDlg(const MKDevice_t& tDevice, int iCard, int iPort,
		CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_DIALOG_AUDIO_CAPTURE };
// Dialog Data
	//{{AFX_DATA(CSampleAudioCaptureDlg)
	CButton		m_btnPlaySound;
	CButton		m_btn_BeginCapture;
	CButton		m_btn_StopCapture;
	CStatic		m_staticFrameCounter;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleAudioCaptureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSampleAudioCaptureDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButton_BeginCapture();
	afx_msg void OnButton_StopCapture();
	afx_msg void OnCheck_PlaySound();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	static void MKApi_CaptureStreamCb(const MKCaptureStreamCb_t* p_cbdata, void* p_user){
		((CSampleAudioCaptureDlg*)p_user)->MKApi_CaptureStream(p_cbdata);
	};
	void MKApi_CaptureStream(const MKCaptureStreamCb_t* p_cbdata);

	static UINT WINAPI RunSoundThread(LPVOID pUserData){
		((CSampleAudioCaptureDlg*)pUserData)->Thread_Audio(); return 0;
	};
	void Thread_Audio();

	struct CAPTURE_DATA
	{
		TCHAR		szTimeCode[40];
		int			nHour, nMinute, nSecond, nFrame;

		double		dTime;
	};
	CAPTURE_DATA	m_CaptureData;

	CFifoList<Buffer_t>		m_AudioPool;
	Buffer_t*				m_pCaptureCacheAudio;

	BOOL		m_bPlaySound;
	class CWaveOut*		m_pWavOut;

	HANDLE	m_hCapture;

	EC_THREADDATA	m_PlayAudioThreadData;
	HANDLE	m_hEvent_Sound;

	struct FRAME_COUNTER_PARAM
	{
		BOOL bInit = FALSE;
		UINT64 last_frame_counter = 0;
		int delta_counter = 0;
		void reset()
		{
			bInit = FALSE;
			last_frame_counter = 0;
		}
	};
	FRAME_COUNTER_PARAM	m_frame_counter_param[2];

	TCHAR		m_szDebugInfo[1024];

};