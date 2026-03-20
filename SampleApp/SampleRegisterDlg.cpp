// SampleRegisterDlg.cpp : 实现文件
//

#include "pch.h"
#include "MKCardSampleApp.h"
#include "SampleRegisterDlg.h"
#include "afxdialogex.h"


// CSampleRegisterDlg 对话框

IMPLEMENT_DYNAMIC(CSampleRegisterDlg, CDialogEx)

CSampleRegisterDlg::CSampleRegisterDlg(HANDLE hCard, CWnd* pParent /*=NULL*/)
	: CDialogEx(CSampleRegisterDlg::IDD, pParent),
	m_hCard(hCard)
{

}

CSampleRegisterDlg::~CSampleRegisterDlg()
{
}

void CSampleRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REGISTERDLG_EDIT_BAR, m_ctrlBarIdx);
	DDX_Control(pDX, IDC_REGISTERDLG_EDIT_ADDRESS, m_ctrlAddress);
	DDX_Control(pDX, IDC_REGISTERDLG_EDIT_VALUE, m_ctrlValue);
}


BEGIN_MESSAGE_MAP(CSampleRegisterDlg, CDialogEx)
	ON_BN_CLICKED(IDC_REGISTERDLG_BTN_READ, &CSampleRegisterDlg::OnBnClickedButtonRead)
	ON_BN_CLICKED(IDC_REGISTERDLG_BTN_WRITE, &CSampleRegisterDlg::OnBnClickedButtonWrite)
END_MESSAGE_MAP()


// CSampleRegisterDlg 消息处理程序


void CSampleRegisterDlg::OnBnClickedButtonRead()
{
	CString text;
	MKError err;
	MKControl_Regsiter_t reg;

	m_ctrlAddress.GetWindowText(text);

	TCHAR szTmp[32] = { 0 };
	_tcscpy_s(szTmp, 32, _T("0x"));
	_tcscat_s(szTmp, text);
	TCHAR *endptr;
	reg.address = _tcstol(szTmp, &endptr, 16);
	reg.value = 0xFFFFFFFF;
	m_ctrlBarIdx.GetWindowText(text);
	reg.bar_idx = _ttoi(text);
	err = MKCrossApi_GetControl(&reg, sizeof(reg), MKCT_GetReigster, m_hCard, 0);
	if (err != MKError_No)
	{
		_tprintf(L"%s\n", MKApi_GetErrorDescription(err));
		return;
	}

	text.Format(_T("0x%x"), reg.value);
	m_ctrlValue.SetWindowText(text);
}

void CSampleRegisterDlg::OnBnClickedButtonWrite()
{
	CString text;
	MKError err;
	MKControl_Regsiter_t reg;

	m_ctrlAddress.GetWindowText(text);

	TCHAR szTmp[32] = { 0 };
	_tcscpy_s(szTmp, 32, _T("0x"));
	_tcscat_s(szTmp, text);
	TCHAR *endptr;
	reg.address = _tcstol(szTmp, &endptr, 16);

	m_ctrlValue.GetWindowText(text);

	_tcscpy_s(szTmp, 32, _T("0x"));
	_tcscat_s(szTmp, text);
	reg.value = _tcstol(szTmp, &endptr, 16);

	m_ctrlBarIdx.GetWindowText(text);
	reg.bar_idx = _ttoi(text);
	err = MKCrossApi_SetControl(&reg, sizeof(reg), MKCT_SetReigster, m_hCard, 0);
	if (err != MKError_No)
	{
		_tprintf(L"%s\n", MKApi_GetErrorDescription(err));
		return;
	}
}

BOOL CSampleRegisterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ctrlBarIdx.SetWindowText(_T("0"));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
