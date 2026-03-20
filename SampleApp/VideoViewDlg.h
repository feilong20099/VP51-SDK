#pragma once


/////////////////////////////////////////////////////////////////////////////
// VideoViewDlg dialog

class VideoViewDlg : public CDialogEx
{
// Construction
public:
	VideoViewDlg(CWnd* pParent = NULL);   // standard constructor

	int DisplayVideo(BYTE* pUYVY, int width, int height);

// Dialog Data
	//{{AFX_DATA(VideoViewDlg)
	enum { IDD = IDD_DIALOG_VIDEOVIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VideoViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(VideoViewDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CStatic	m_ctrlImageView;

	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	BOOL m_bFullScreen;

	HDC					m_hDC;

	BITMAPINFO			m_BMPInfo;
	int					m_width, m_height;
	BYTE*				m_pBGRABuffer;

public:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
};