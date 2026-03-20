
// MKCardSampleAppDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MKCardSampleApp.h"
#include "MKCardSampleAppDlg.h"
#include "afxdialogex.h"
#include "SampleControlDlg.h"
#include "SampleCaptureDlg.h"
#include "SampleAudioCaptureDlg.h"
#include "SampleSendDlg.h"
#include "SampleRegisterDlg.h"

#pragma comment(lib,"version.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum
{
	TIMER_ID_GET_CARD_TEMPERATURE = 1
};


BOOL GetFileVer(LPCTSTR szFileName, WORD Version[4])
{
	DWORD handle = 0;
	int size = GetFileVersionInfoSize(szFileName, &handle);
	BYTE * versionInfo = new BYTE[size];
	VS_FIXEDFILEINFO * vsfi = NULL;
	if (GetFileVersionInfo(szFileName, handle, size, versionInfo))
	{
		UINT len = 0;
		VerQueryValue(versionInfo, L"\\", (void **)&vsfi, &len);
		if (vsfi)
		{
			Version[0] = HIWORD(vsfi->dwFileVersionMS);
			Version[1] = LOWORD(vsfi->dwFileVersionMS);
			Version[2] = HIWORD(vsfi->dwFileVersionLS);
			Version[3] = LOWORD(vsfi->dwFileVersionLS);
		}
	}
	delete[] versionInfo;
	return vsfi != nullptr;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMKCardSampleAppDlg 对话框


CMKCardSampleAppDlg::CMKCardSampleAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MKCARDSAMPLEAPP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ZeroMemory(&m_SCDevice, sizeof(MKDevice_t));
	m_iCardSel = -1;
}

void CMKCardSampleAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MKCARDDLG_BTN_OPENCARD, m_btnOpenCard);
	DDX_Control(pDX, IDC_MKCARDDLG_BTN_CLOSECARD, m_btnCloseCard);
	DDX_Control(pDX, IDC_MKCARDDLG_BTN_OPEN_PORTSEND, m_btnOpenSend);
	DDX_Control(pDX, IDC_MKCARDDLG_BTN_OPEN_PORTCAP, m_btnOpenCapture);
	DDX_Control(pDX, IDC_MKCARDDLG_STATIC_TEMPERATURE, m_staticTemperature);
	DDX_Control(pDX, IDC_MKCARDDLG_STATIC_SOFTWAREINFO, m_staticSoftwareInfo);
	DDX_Control(pDX, IDC_MKCARDDLG_STATIC_HARDWAREINFO, m_staticHardwareInfo);
	DDX_Control(pDX, IDC_MKCARDDLG_LIST_PORTLIST, m_ListBox_Port);
	DDX_Control(pDX, IDC_MKCARDDLG_LIST_CARDLIST, m_ListBox_Card);
}

BEGIN_MESSAGE_MAP(CMKCardSampleAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_LBN_SELCHANGE(IDC_MKCARDDLG_LIST_CARDLIST, OnSelchangeLISTCardList)
	ON_BN_CLICKED(IDC_MKCARDDLG_BTN_OPENCARD, OnButton_OpenCard)
	ON_BN_CLICKED(IDC_MKCARDDLG_BTN_CLOSECARD, OnButton_CloseCard)
	ON_BN_CLICKED(IDC_MKCARDDLG_BTN_OPEN_PORTSEND, OnClick_OpenPortSend)
	ON_BN_CLICKED(IDC_MKCARDDLG_BTN_OPEN_PORTCAP, OnClick_OpenPortCapture)
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_MKCARDDLG_BTN_REGISTER, &CMKCardSampleAppDlg::OnBnClickedButtonRegister)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CMKCardSampleAppDlg 消息处理程序

BOOL CMKCardSampleAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CString title = L"MKCardSampleApp_Cross";
#ifdef _WIN64
	title += _T(" x64");
#endif
	SetWindowText(title);

	WORD CardVersion[4] = { 0 };
	GetFileVer(L"libMKCardApi_x64.dll", CardVersion);

	WORD DemoVersion[4] = { 0 };
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(nullptr, szFileName, MAX_PATH);
	GetFileVer(szFileName, DemoVersion);

	CString info;
	info.Format(_T("Demo ver: %d.%d.%d.%d    MKCardApi ver: %d.%d.%d.%d"),
		DemoVersion[0], DemoVersion[1], DemoVersion[2], DemoVersion[3], CardVersion[0], CardVersion[1], CardVersion[2], CardVersion[3]);
	m_staticSoftwareInfo.SetWindowText(info);

	MKError err;
	err = MKCrossApi_GetDeviceinfo(&m_SCDevice.m_DeviceInfo, sizeof(MKDeviceInfo_t));
	if (err == MKError_No)
	{
		m_ListBox_Card.ResetContent();
		MKDeviceInfo_t* pDeviceInfo = &m_SCDevice.m_DeviceInfo;
		for (int iCard = 0; iCard < pDeviceInfo->card_count; iCard++)
		{
			MKCardInfo_t* pCardInfo = &pDeviceInfo->card_info_list[iCard];
			m_ListBox_Card.AddString(MKCrossApi_GetCardDescription(pCardInfo->type));
		}
		if (m_ListBox_Card.GetCount())
		{
			m_ListBox_Card.EnableWindow(TRUE);
			m_ListBox_Card.SetCurSel(0);
			OnSelchangeLISTCardList();
		}
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMKCardSampleAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMKCardSampleAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMKCardSampleAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


LRESULT CMKCardSampleAppDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_POWERBROADCAST:
		if ((UINT)wParam == PBT_APMQUERYSUSPEND || (UINT)wParam == PBT_APMQUERYSTANDBY)
			return BROADCAST_QUERY_DENY;
		break;
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}

BOOL CMKCardSampleAppDlg::DestroyWindow()
{
	for (int i = 0; i < m_SCDevice.m_DeviceInfo.card_count; i++)
	{
		CloseCard(i);
	}

	return CDialog::DestroyWindow();
}

BOOL CMKCardSampleAppDlg::OnQueryEndSession()
{
	if (!CDialog::OnQueryEndSession())
		return FALSE;

	DestroyWindow();

	return TRUE;
}

void CMKCardSampleAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ID_GET_CARD_TEMPERATURE)
	{
		GetTemperature();
	}

	CDialog::OnTimer(nIDEvent);
}

void CMKCardSampleAppDlg::OnButton_OpenCard()
{
	INT iCard = m_ListBox_Card.GetCurSel();
	if (iCard == LB_ERR)
		return;

	if (m_SCDevice.m_CardList[iCard].m_hCard)
		return;

	MKCardInfo_t* p_card_info = m_SCDevice.m_DeviceInfo.card_info_list + iCard;

	MKError err;
	err = MKCrossApi_OpenCard(&m_SCDevice.m_CardList[iCard].m_hCard, p_card_info);
	if (err != MKError_No)
	{
		AfxMessageBox(MKApi_GetErrorDescription(err));
		return;
	}
	UIControl_Card(TRUE);

	printf("Open Card!\nCard=%d\n\n", iCard);
}

void CMKCardSampleAppDlg::OnButton_CloseCard()
{
	INT iCard = m_ListBox_Card.GetCurSel();
	if (iCard == LB_ERR)
		return;

	CloseCard(iCard);

	UIControl_Card(FALSE);

	printf("Close Card!\nCard=%d\n\n", iCard);
}

void CMKCardSampleAppDlg::OnSelchangeLISTCardList()
{
	int iCard = m_ListBox_Card.GetCurSel();
	if (iCard == LB_ERR)
		return;

	if (iCard == m_iCardSel)
		return;
	m_iCardSel = iCard;

	CString sVer;
	INT version, ver1, ver2, ver3, ver4;
	version = m_SCDevice.m_DeviceInfo.card_info_list[m_iCardSel].hardware_version;
	ver1 = (version & 0xF000) >> 12;
	ver2 = (version & 0xF00) >> 8;
	ver3 = (version & 0xF0) >> 4;
	ver4 = (version & 0xF);

	sVer.Format(_T("Card hardware ver: %ld.%ld.%ld.%ld"),
		ver1, ver2, ver3, ver4);
	m_staticHardwareInfo.SetWindowText(sVer);
	UIControl_Card(m_SCDevice.m_CardList[m_iCardSel].m_hCard ? TRUE : FALSE);
}

void CMKCardSampleAppDlg::OnClick_OpenPortSend()
{
	int iCard = m_ListBox_Card.GetCurSel();
	if (iCard == LB_ERR)
		return;

	int iPort = m_ListBox_Port.GetCurSel();
	if (iPort == LB_ERR)
		return;

	MKPortInfo_t* pPortInfo = &m_SCDevice.m_DeviceInfo.card_info_list[iCard].port_info_list[iPort];
	if(pPortInfo->send_data_spport.all == 0)
	{
		AfxMessageBox(_T("This port can't Send Data!"));
		return;
	}

	CDialogEx* pDlg = m_SCDevice.m_CardList[iCard].m_pPortDlg[iPort];
	if (pDlg &&	pDlg->m_hWnd)
	{
		AfxMessageBox(_T("This port is opended!"));
		return;
	}

	if (pDlg)
		delete pDlg;
	pDlg = new CSampleSendDlg(m_SCDevice, iCard, iPort);
	pDlg->Create(CSampleSendDlg::IDD);
	pDlg->ShowWindow(SW_SHOW);
	m_SCDevice.m_CardList[iCard].m_pPortDlg[iPort] = pDlg;
}

void CMKCardSampleAppDlg::OnClick_OpenPortCapture()
{
	int iCard = m_ListBox_Card.GetCurSel();
	if (iCard == LB_ERR)
		return;

	int iPort = m_ListBox_Port.GetCurSel();
	if (iPort == LB_ERR)
		return;

	MKPortInfo_t* pPortInfo = &m_SCDevice.m_DeviceInfo.card_info_list[iCard].port_info_list[iPort];
	if (pPortInfo->capture_data_spport.all == 0)
	{
		AfxMessageBox(_T("This port can't Capture!"));
		return;
	}

	CDialogEx* pDlg = m_SCDevice.m_CardList[iCard].m_pPortDlg[iPort];
	if (pDlg &&	pDlg->m_hWnd)
	{
		AfxMessageBox(_T("This capture port is opended!"));
		return;
	}

	if (pDlg)
		delete pDlg;
	if (pPortInfo->capture_data_spport.video > 0)
	{
		pDlg = new CSampleCaptureDlg(m_SCDevice, iCard, iPort);
		pDlg->Create(CSampleCaptureDlg::IDD);
	}
	else
	{
		pDlg = new CSampleAudioCaptureDlg(m_SCDevice, iCard, iPort);
		pDlg->Create(CSampleAudioCaptureDlg::IDD);
	}

	pDlg->ShowWindow(SW_SHOW);
	m_SCDevice.m_CardList[iCard].m_pPortDlg[iPort] = pDlg;
}

void CMKCardSampleAppDlg::CloseCard(int iCard)
{
	if (iCard >= m_SCDevice.m_DeviceInfo.card_count)
		return;

	MKCard_t* pCard = &m_SCDevice.m_CardList[iCard];
	if (pCard->m_hCard == NULL)
		return;
	int port_count = m_SCDevice.m_DeviceInfo.card_info_list[iCard].port_count;
	for (int iPort = 0; iPort < port_count; iPort++)
	{
		CDialogEx* pCapDlg = pCard->m_pPortDlg[iPort];
		if (pCapDlg)
		{
			if (pCapDlg->m_hWnd)
				pCapDlg->DestroyWindow();
			delete pCapDlg;
			pCard->m_pPortDlg[iPort] = NULL;
		}
	}

	MKCrossApi_CloseCard(pCard->m_hCard);
	pCard->m_hCard = NULL;
}

void CMKCardSampleAppDlg::GetTemperature()
{
	MKError err;
	MKCard_t* p_card = &m_SCDevice.m_CardList[m_iCardSel];
	if (p_card == nullptr || p_card->m_hCard == nullptr)
		return;

	{
		INT32 temperature = 0;
		CString info;

		err = MKCrossApi_GetControl(&temperature, sizeof(INT32), MKCT_GetTemperature, p_card->m_hCard, 0);
		if (err == MKError_No)
		{
			info.Format(_T("  Temperature: %lu℃"), temperature);
			m_staticTemperature.SetWindowText(info);
		}
	}
}

void CMKCardSampleAppDlg::ResetPortList(UINT8 iCard)
{
	MKCardInfo_t* pCardInfo = &m_SCDevice.m_DeviceInfo.card_info_list[iCard];
	MKPortInfo_t* pPortInfo;

	CString desc;
	int idx = 0;
	m_ListBox_Port.ResetContent();
	for (int iPort = 0; iPort < pCardInfo->port_count; iPort++)
	{
		desc.Format(_T("Port %lu"), iPort + 1);

		pPortInfo = &pCardInfo->port_info_list[iPort];

		if (pPortInfo->capture_data_spport.all > 0)
		{
			desc += _T(" In");
			if (pPortInfo->capture_data_spport.video)
				desc += _T("V");
			if (pPortInfo->capture_data_spport.audio)
				desc += _T("A");
		}
		if (pPortInfo->send_data_spport.all > 0)
		{
			desc += _T(" Out");
			if (pPortInfo->send_data_spport.video)
				desc += _T("V");
			if (pPortInfo->send_data_spport.audio)
				desc += _T("A");
		}

		idx = m_ListBox_Port.AddString(desc);
		m_ListBox_Port.SetItemData(idx, iPort);
	}

	if (m_ListBox_Port.GetCount())
		m_ListBox_Port.SetCurSel(0);
}

void CMKCardSampleAppDlg::UIControl_Card(BOOL bOpen)
{
	ResetPortList(m_iCardSel);

	KillTimer(TIMER_ID_GET_CARD_TEMPERATURE);
	if (bOpen)
	{
		INT32 temperature = 0;
		MKError err = MKCrossApi_GetControl(&temperature, sizeof(INT32), MKCT_GetTemperature,
			m_SCDevice.m_CardList[m_iCardSel].m_hCard, 0);
		if (err == MKError_No)
			SetTimer(TIMER_ID_GET_CARD_TEMPERATURE, 1000, NULL);
	}
	else
	{
		m_staticTemperature.SetWindowText(_T(""));
	}

	m_btnOpenCard.EnableWindow(!bOpen);
	m_btnCloseCard.EnableWindow(bOpen);

	m_ListBox_Port.EnableWindow(bOpen);
	m_btnOpenCapture.EnableWindow(bOpen);
	m_btnOpenSend.EnableWindow(bOpen);
}

void CMKCardSampleAppDlg::OnBnClickedButtonRegister()
{
	INT iCard = m_ListBox_Card.GetCurSel();
	if (iCard == LB_ERR)
		return;

	if (!m_SCDevice.m_CardList[iCard].m_hCard)
		return;

	CSampleRegisterDlg dlg(m_SCDevice.m_CardList[iCard].m_hCard);
	dlg.DoModal();
}
