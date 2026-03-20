#pragma once


// CSampleRegisterDlg 对话框

class CSampleRegisterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSampleRegisterDlg)

public:
	CSampleRegisterDlg(HANDLE hCard, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSampleRegisterDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_REGISTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRead();
	afx_msg void OnBnClickedButtonWrite();

	CEdit	m_ctrlAddress;
	CEdit	m_ctrlValue;
	CEdit	m_ctrlBarIdx;

	HANDLE	m_hCard;
	virtual BOOL OnInitDialog();
};
