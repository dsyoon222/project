
// MyViewerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MyViewer.h"
#include "MyViewerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CMyViewerDlg 대화 상자



CMyViewerDlg::CMyViewerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MYVIEWER_DIALOG, pParent)
	, m_x1(150)
	, m_y1(150)
	, m_x2(400)
	, m_y2(400)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	srand((unsigned)time(NULL));//rand 함수 seed 생성
}

void CMyViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VIEWER_STATIC, m_wndImageViewer);
	DDX_Text(pDX, IDC_EDIT_X1, m_x1);
	DDX_Text(pDX, IDC_EDIT_Y1, m_y1);
	DDX_Text(pDX, IDC_EDIT_X2, m_x2);
	DDX_Text(pDX, IDC_EDIT_Y2, m_y2);
}

BEGIN_MESSAGE_MAP(CMyViewerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DRAW, &CMyViewerDlg::OnBnClickedDraw)
	ON_BN_CLICKED(IDC_ACTION, &CMyViewerDlg::OnBnClickedAction)
	ON_BN_CLICKED(IDC_OPEN, &CMyViewerDlg::OnBnClickedOpen)
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT_X1, &CMyViewerDlg::OnEnChangeEditX1)
	ON_EN_CHANGE(IDC_EDIT_Y1, &CMyViewerDlg::OnEnChangeEditY1)
	ON_EN_CHANGE(IDC_EDIT_X2, &CMyViewerDlg::OnEnChangeEditX2)
	ON_EN_CHANGE(IDC_EDIT_Y2, &CMyViewerDlg::OnEnChangeEditY2)
END_MESSAGE_MAP()


// CMyViewerDlg 메시지 처리기

BOOL CMyViewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	//====실행파일 경로 획득(하위image폴더가 없으면 생성함.)
	TCHAR strExeFilePath[_MAX_PATH] = {};
	::GetModuleFileName(NULL, strExeFilePath, _MAX_PATH);
	TCHAR szDrive[_MAX_PATH] = {};
	TCHAR szPath[_MAX_PATH] = {};
	TCHAR szFileName[_MAX_PATH] = {};
	TCHAR szExt[_MAX_PATH] = {};
	_tsplitpath_s(strExeFilePath, szDrive, _MAX_PATH,
		szPath, _MAX_PATH, szFileName, _MAX_PATH, szExt, _MAX_PATH);
	m_strExeFilePath = szPath; // (주의) 맨뒤 '\\' 포함됨.
	if (::GetFileAttributes(m_strExeFilePath + _T("image")) == INVALID_FILE_ATTRIBUTES)
		::CreateDirectory(m_strExeFilePath + _T("image"), NULL);

	//====이미지 보기 컨트롤 초기 위치설정
	if (m_wndImageViewer.GetSafeHwnd()) {
		const int topMargin = 70;
		CRect rcClient;
		GetClientRect(&rcClient);
		m_wndImageViewer.SetWindowPos(NULL, 0, topMargin, rcClient.Width(), rcClient.Height() - topMargin, SWP_NOZORDER);
	}
	UpdateImageViewerPoints(); //시작좌표와 종료좌표를 업데이트

	//좌표입력 스핀버튼의 Range설정
	int nMinValue = 0, nMaxValue = 2000;
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_X1))->SetRange32(nMinValue, nMaxValue);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_Y1))->SetRange32(nMinValue, nMaxValue);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_X2))->SetRange32(nMinValue, nMaxValue);
	((CSpinButtonCtrl*)GetDlgItem(IDC_SPIN_Y2))->SetRange32(nMinValue, nMaxValue);

	//====버튼위 도움말 툴팁표시
	if (m_ToolTip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX)) {
		m_ToolTip.AddTool(GetDlgItem(IDC_DRAW), _T("시작좌표에서 랜덤한 크기의 원을 그립니다."));
		m_ToolTip.AddTool(GetDlgItem(IDC_ACTION), _T("시작좌표에서 종료좌표 방향으로 원을 움직인 후 이미지파일로 저장합니다."));
		m_ToolTip.AddTool(GetDlgItem(IDC_OPEN), _T("윈도우 탐색기 파일열기 대화상자에서 이미지 파일을 불러옵니다."));
		m_ToolTip.SetDelayTime(TTDT_INITIAL, 0);
		m_ToolTip.Activate(TRUE);
	}
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMyViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMyViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMyViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//이미지 보기창에 Random한 크기의 원을 출력한다.
void CMyViewerDlg::OnBnClickedDraw()
{
	UpdateData(TRUE);
	int nRadius = (rand() % 300) + 20; //최소=20, 최대=319 크기
	m_wndImageViewer.MoveCircle(CPoint(m_x1, m_y1), nRadius);
	m_wndImageViewer.ShowCircle(TRUE);
}

void CMyViewerDlg::OnBnClickedAction()
{
	UpdateData(TRUE);

	const int step = 10;//move 10 pixels per 'Action' Button click
	if (m_x2 > m_x1)
		m_x1 += step;
	else
		m_x1 -= step;

	if (m_y2 > m_y1)
		m_y1 += step;
	else
		m_y1 -= step;

	int dx = abs(m_x2 - m_x1);
	int dy = abs(m_y2 - m_y1);

	if (dx <= step)
		m_x1 = m_x2;
	if (dy <= step)
		m_y1 = m_y2;
	UpdateData(FALSE);

	m_wndImageViewer.MoveCircle(CPoint(m_x1, m_y1), 0);
	m_wndImageViewer.SetStartEndPoints(CPoint(m_x1, m_y1), CPoint(m_x2, m_y2));
	//m_wndImageViewer.ShowXMark(TRUE);
	m_wndImageViewer.ShowCircle(TRUE);
	
	SYSTEMTIME cur_time;
	::GetLocalTime(&cur_time);
	CString strFileName;
	strFileName.Format(_T("%04d-%02d-%02d_%02d%02d%02d_(%d, %d).jpg"),
		cur_time.wYear, cur_time.wMonth, cur_time.wDay,
		cur_time.wHour, cur_time.wMinute, cur_time.wSecond, m_x1, m_y1);// cur_time.wMilliseconds);
	m_wndImageViewer.SaveImage(m_strExeFilePath + _T("image\\") + strFileName);
}

//탐색기 파일열기 대화상자
void CMyViewerDlg::OnBnClickedOpen()
{
	TCHAR szFilter[] = _T("Image Files(*.bmp, *.jpg, *.png) | *.bmp;*.jpg;*.png|All Files(*.*)|*.*||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (dlg.DoModal() == IDOK)
	{
		if (m_wndImageViewer.OpenImage(dlg.GetPathName()) == FALSE) {
			MessageBox(_T("지원되는 이미지 파일형식이 아닙니다."), _T("파일오류"), MB_ICONWARNING | MB_OK);
			m_wndImageViewer.ShowXMark(FALSE);
			return;
		}
		UpdateData(TRUE);
		m_wndImageViewer.MoveCircle(CPoint(m_x1, m_y1), 0);
		m_wndImageViewer.ShowCircle(TRUE);
		m_wndImageViewer.SetStartEndPoints(CPoint(m_x1, m_y1), CPoint(m_x2, m_y2));
		m_wndImageViewer.ShowXMark(TRUE);
	}
}

void CMyViewerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_wndImageViewer.GetSafeHwnd()) {
		//화면상단의 컨트롤 위치 영역 확보
		const int topMargin = 70;
		m_wndImageViewer.SetWindowPos(NULL, 0, topMargin, cx, cy - topMargin, SWP_NOZORDER);
	}
}


BOOL CMyViewerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//버튼3개의 툴팁 업데이트
	if(m_ToolTip.GetSafeHwnd()) 
		m_ToolTip.RelayEvent(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMyViewerDlg::OnCancel()
{
	int nMess = MessageBox(_T("정말 종료하시겠습니까?"), _T("종료 확인"), MB_ICONQUESTION | MB_YESNO);
	if (nMess == IDYES)
		CDialog::OnCancel();
}

HBRUSH CMyViewerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.
	if (pWnd) {
		switch (pWnd->GetDlgCtrlID()) {
		case IDC_STATIC_START:
			pDC->SetTextColor(RGB(255, 0, 0));
			break;
		case IDC_STATIC_END:
			pDC->SetTextColor(RGB(0, 0, 255));
			break;
		}
	}
	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}

//시작좌표, 종료좌표 UI값 변경시 업데이트 한다.
void CMyViewerDlg::UpdateImageViewerPoints()
{
	if (m_wndImageViewer.GetSafeHwnd()) {
		UpdateData(TRUE);
		m_wndImageViewer.SetStartEndPoints(CPoint(m_x1, m_y1), CPoint(m_x2, m_y2));
	}
}

void CMyViewerDlg::OnEnChangeEditX1()
{
	UpdateImageViewerPoints();
}

void CMyViewerDlg::OnEnChangeEditY1()
{
	UpdateImageViewerPoints();
}

void CMyViewerDlg::OnEnChangeEditX2()
{
	UpdateImageViewerPoints();
}

void CMyViewerDlg::OnEnChangeEditY2()
{
	UpdateImageViewerPoints();
}
