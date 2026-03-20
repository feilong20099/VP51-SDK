#pragma once


#define BUFFER_COUNT (30)

/////////////////////////////////////////////////////////////////////////////
// CSampleControlDlg dialog

class CSampleControlDlg : public CDialogEx
{
// Construction
public:
	CSampleControlDlg(const MKDevice_t& tDevice, int iCard, int iPort,
		UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSampleControlDlg)
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSampleControlDlg)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSampleControlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnButton_View();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	const MKDevice_t*	m_pDevice;
	int					m_iCard;
	int					m_iPort;
	HANDLE				m_hCard;

	class VideoViewDlg*		m_pVideoViewDlg;
};