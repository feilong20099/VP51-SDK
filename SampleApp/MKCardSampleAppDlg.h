
// MKCardSampleAppDlg.h: 头文件
//

#pragma once


// CMKCardSampleAppDlg 对话框
class CMKCardSampleAppDlg : public CDialogEx
{
// 构造
public:
	CMKCardSampleAppDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MKCardSampleApp_DIALOG };
#endif
	CButton		m_btnOpenCard;
	CButton		m_btnCloseCard;
	CButton		m_btnOpenSend;
	CButton		m_btnOpenCapture;
	CStatic		m_staticTemperature;
	CStatic		m_staticSoftwareInfo;
	CStatic		m_staticHardwareInfo;
	CListBox	m_ListBox_Port;
	CListBox	m_ListBox_Card;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	virtual BOOL DestroyWindow();

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSelchangeLISTCardList();
	afx_msg void OnButton_OpenCard();
	afx_msg void OnButton_CloseCard();
	afx_msg void OnClick_OpenPortSend();
	afx_msg void OnClick_OpenPortCapture();
	afx_msg void OnBnClickedButtonRegister();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnQueryEndSession();

private:
	void UIControl_Card(BOOL bOpen);
	void ResetPortList(UINT8 iCard);
	void CloseCard(int iCard);
	void GetTemperature();

public:
	MKDevice_t	m_SCDevice;
	int			m_iCardSel;
};
