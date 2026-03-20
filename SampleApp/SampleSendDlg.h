#pragma once

// CSampleSendDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CSampleSendDlg dialog

class CSampleSendDlg : public CSampleControlDlg
{
// Construction
public:
	CSampleSendDlg(const MKDevice_t& tDevice, int iCard, int iPort,
		CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSampleSendDlg)
	enum { IDD = IDD_DIALOG_SCPORTSEND };
	CButton	m_checkTimeCode;
	CComboBox	m_ComboVideoStandard;
	CButton	m_btnBeginSend;
	CButton	m_btnStopSend;
	CStatic	m_staticTimeInfo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleSendDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSampleSendDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButton_BeginSend();
	afx_msg void OnButton_StopSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	static void MKSendDataCb(const MKSendCb_t* p_cbdata, void* p_user);

	static UINT __stdcall FillDataThreadCB(LPVOID pUserData){
		((CSampleSendDlg*)pUserData)->FillDataThread(); return 0;
	};
	
	static UINT __stdcall SendDataThreadCB(LPVOID pUserData){
		((CSampleSendDlg*)pUserData)->SendDataThread(); return 0;
	};

	static UINT __stdcall VideoViewThreadCB(LPVOID pUserData){
		((CSampleSendDlg*)pUserData)->VideoViewThread(); return 0;
	};
	void	FillDataThread();
	void	SendDataThread();
	void	VideoViewThread();

	void	ClearVideoViewList();

	HANDLE			m_hSend;
	
	HANDLE			m_hFillDataEvent;
	EC_THREADDATA	m_FillDataThreadData;

	HANDLE			m_hSendDataEvent;
	EC_THREADDATA	m_SendDataThreadData;

	HANDLE			m_hVideoViewEvent;
	EC_THREADDATA	m_VideoViewThreadData;

private:
	enum SEND_STATE
	{
		SEND_STATE_STOP = 0,
		SEND_STATE_PLAY,
		SEND_STATE_SEEK,
		SEND_STATE_PUASE,
	};
	int		m_Send_State;

	unsigned __int64	m_llSendCounter;

	CFifoList<Buffer_t> m_SendVideoPool;				// ≤•≥ˆ ”∆µª∫≥Â¡¥±Ì
	CFifoList<AUDIO_FRAME_SAMPLE> m_SendAudioPool;		// ≤•≥ˆ“Ù∆µª∫≥Â¡¥±Ì

	TLock_t	m_Critic_ViewVideoList;
	std::list<Buffer_t*> m_ViewVideoDataList;// ‘§º‡ª∫≥Â¡¥±Ì

	int		m_nVideoWidth;
	int		m_nVideoHeight;
	int		m_nVideoTimeNum;
	int		m_nVideoTimeScale;
	double	m_dVideoFps;
	int		m_nVideoFrameBytes;

	BOOL	m_bTimeCode;
	TCHAR	m_szTimeCode[40];
};
