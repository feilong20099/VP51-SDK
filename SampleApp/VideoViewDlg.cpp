// VideoViewDlg.cpp : implementation file
//

#include "pch.h"
#include "resource.h"
#include "VideoViewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// VideoViewDlg dialog


VideoViewDlg::VideoViewDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(VideoViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(VideoViewDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_bFullScreen = FALSE;

	m_hDC = nullptr;
	m_width = 0;
	m_height = 0;
	m_pBGRABuffer = nullptr;

	memset(&m_BMPInfo, 0, sizeof(BITMAPINFO));
	m_BMPInfo.bmiHeader.biSize = 40;
	m_BMPInfo.bmiHeader.biPlanes = 1;
	m_BMPInfo.bmiHeader.biCompression = BI_RGB;
	m_BMPInfo.bmiHeader.biBitCount = 32;
}


void VideoViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VideoViewDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_STATIC_IMAGEVIEW, m_ctrlImageView);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VideoViewDlg, CDialog)
	//{{AFX_MSG_MAP(VideoViewDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////


void VideoViewDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	if (m_ctrlImageView.m_hWnd)
	{
		if (nType == SIZE_MAXIMIZED)
		{
			ModifyStyle(WS_CAPTION, 0, SWP_FRAMECHANGED);
			ModifyStyleEx(WS_EX_DLGMODALFRAME, 0, SWP_FRAMECHANGED);
		}
		else
		{
			ModifyStyle(0, WS_CAPTION, SWP_FRAMECHANGED);
			ModifyStyleEx(0, WS_EX_DLGMODALFRAME, SWP_FRAMECHANGED);
		}

		CRect r;
		GetClientRect(r);
		
		m_ctrlImageView.MoveWindow(r);
	}
}

void VideoViewDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_bFullScreen)
	{
		ShowWindow(SW_NORMAL);
		m_bFullScreen = FALSE;
	}
	else
	{
		ShowWindow(SW_MAXIMIZE);
		m_bFullScreen = TRUE;
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}


void VideoViewDlg::OnOK()
{
	ShowWindow(SW_NORMAL);
	m_bFullScreen = FALSE;

	//CDialog::OnOK();
}


void VideoViewDlg::OnCancel()
{
	ShowWindow(SW_NORMAL);
	m_bFullScreen = FALSE;

	//CDialog::OnCancel();
}

void VideoViewDlg::OnClose()
{
	if (m_bFullScreen)
	{
		ShowWindow(SW_NORMAL);
		m_bFullScreen = FALSE;
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}

BOOL VideoViewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRect r;
	GetClientRect(r);

	m_ctrlImageView.MoveWindow(r);

	m_hDC = ::GetDC(m_ctrlImageView.m_hWnd);
	SetStretchBltMode(m_hDC, COLORONCOLOR);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void VideoViewDlg::OnDestroy()
{
	if (m_hDC)
	{
		::ReleaseDC(m_ctrlImageView.m_hWnd, m_hDC);
		m_hDC = nullptr;
	}
	SAFE_DELETE_ARRAY(m_pBGRABuffer);

	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

int VideoViewDlg::DisplayVideo(BYTE* pUYVY, int width, int height)
{
	if (m_pBGRABuffer)
	{
		if (m_width != width || m_height != height)
		{
			SAFE_DELETE_ARRAY(m_pBGRABuffer);
			m_pBGRABuffer = nullptr;
		}
	}
	if (nullptr == m_pBGRABuffer)
	{
		m_pBGRABuffer = new BYTE[width * height * 4];
	}
	convert_UYVY_BGRA(pUYVY, width * 2, width, height, m_pBGRABuffer, width * 4);

	m_BMPInfo.bmiHeader.biWidth = width;
	m_BMPInfo.bmiHeader.biHeight = -height;

	RECT rect;
	::GetClientRect(m_ctrlImageView.m_hWnd, &rect);

	::StretchDIBits(m_hDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top,
		0, 0, width, height,
		m_pBGRABuffer, &m_BMPInfo, DIB_RGB_COLORS, SRCCOPY);

	return 1;
}