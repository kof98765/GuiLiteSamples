
// HelloNoTouchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HelloNoTouch.h"
#include "HelloNoTouchDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CHelloNoTouchDlg dialog
CHelloNoTouchDlg* CHelloNoTouchDlg::m_the_dialog = NULL;
CRITICAL_SECTION CHelloNoTouchDlg::m_criSection;

CHelloNoTouchDlg::CHelloNoTouchDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HelloNoTouch_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_the_dialog = this;
	InitializeCriticalSection(&m_criSection);
}

void CHelloNoTouchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHelloNoTouchDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CHelloNoTouchDlg message handlers

BOOL CHelloNoTouchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	AfxBeginThread(CHelloNoTouchDlg::ThreadHelloNoTouch, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHelloNoTouchDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHelloNoTouchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHelloNoTouchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CHelloNoTouchDlg::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

#define DISPLAY_WIDTH	240
#define DISPLAY_HEIGHT	320

struct EXTERNAL_GFX_OP
{
	void(*draw_pixel)(int x, int y, unsigned int rgb);
	void(*fill_rect)(int x0, int y0, int x1, int y1, unsigned int rgb);
} my_gfx_op;

void CHelloNoTouchDlg::draw_pixel(int x, int y, unsigned int rgb)
{
	CDC* pDC = m_the_dialog->GetDC();
	if (pDC)
	{
		pDC->SetPixel(x, y, RGB(((((unsigned int)(rgb)) >> 16) & 0xFF), ((((unsigned int)(rgb)) >> 8) & 0xFF), (((unsigned int)(rgb)) & 0xFF)));
	}	
}

void CHelloNoTouchDlg::fill_rect(int x0, int y0, int x1, int y1, unsigned int rgb)
{
	CDC* pDC = m_the_dialog->GetDC();
	if (pDC)
	{
		CBrush brush;
		brush.CreateSolidBrush(RGB(((((unsigned int)(rgb)) >> 16) & 0xFF), ((((unsigned int)(rgb)) >> 8) & 0xFF), (((unsigned int)(rgb)) & 0xFF)));
		CRect rect(x0, y0, x1 + 1, y1 + 1);
		pDC->FillRect(&rect, &brush);
	}
}

UINT CHelloNoTouchDlg::ThreadHelloNoTouch(LPVOID pParam)
{
	Sleep(500);
	my_gfx_op.draw_pixel = draw_pixel;
	my_gfx_op.fill_rect = fill_rect;
	startHelloNoTouch(NULL, DISPLAY_WIDTH, DISPLAY_HEIGHT, 2, &my_gfx_op);
	return 0;
}

void CHelloNoTouchDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	sendTouch2HelloNoTouch(point.x, point.y, false);
}

void CHelloNoTouchDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	sendTouch2HelloNoTouch(point.x, point.y, true);
}

void CHelloNoTouchDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	unsigned int key = 2;
	switch (nChar)
	{
	case 68:
		key = 0;
		break;
	case 65:
		key = 1;
		break;
	}
	sendKey2HelloNoTouch(key);
}