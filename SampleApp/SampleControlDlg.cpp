// SampleControlDlg.cpp : implementation file
//

#include "pch.h"
#include "MKCardSampleApp.h"
#include "SampleControlDlg.h"
#include "VideoViewDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSampleControlDlg dialog


CSampleControlDlg::CSampleControlDlg(const MKDevice_t& tDevice,int iCard, int iPort, 
					 UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialogEx(nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(CSampleControlDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pDevice = &tDevice;
	m_iCard = iCard;
	m_iPort = iPort;
	m_hCard = m_pDevice->m_CardList[m_iCard].m_hCard;

	m_pVideoViewDlg = NULL;
}

void CSampleControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSampleControlDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSampleControlDlg, CDialog)
	//{{AFX_MSG_MAP(CSampleControlDlg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_VIEW, OnButton_View)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSampleControlDlg message handlers

BOOL CSampleControlDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	// 初始化视频预览窗口
	m_pVideoViewDlg = new VideoViewDlg();
	if (!m_pVideoViewDlg->Create(VideoViewDlg::IDD, NULL))
	{
		delete m_pVideoViewDlg;
		return FALSE;
	}

	return TRUE;
}

BOOL CSampleControlDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if((pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
			return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CSampleControlDlg::OnButton_View()
{
	if (m_pVideoViewDlg->IsWindowVisible())
		m_pVideoViewDlg->ShowWindow(SW_HIDE);
	else
		m_pVideoViewDlg->ShowWindow(SW_SHOW);
}


void CSampleControlDlg::OnClose() 
{
	CDialog::OnClose();
	
	DestroyWindow();
}

void CSampleControlDlg::OnDestroy() 
{
	m_pVideoViewDlg->DestroyWindow();
	SAFE_DELETE(m_pVideoViewDlg);

	CDialog::OnDestroy();
}
