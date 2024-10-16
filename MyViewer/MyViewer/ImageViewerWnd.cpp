#include "pch.h"
#include "ImageViewerWnd.h"

IMPLEMENT_DYNAMIC(CImageViewerWnd, CWnd)

CImageViewerWnd::CImageViewerWnd()
{
	m_ptStart = m_ptEnd = CPoint(0, 0);
	m_bShowCircle = m_bShowXMark = FALSE;
	m_nCircleRadius = 80;
	m_image.Create(800, 600, 24);//default empty image
}

BEGIN_MESSAGE_MAP(CImageViewerWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DROPFILES()
	ON_WM_CREATE()
END_MESSAGE_MAP()

BOOL CImageViewerWnd::OpenImage(LPCTSTR lpszFilePathName)
{
	BOOL bOK = TRUE;
	if (!m_image.IsNull())
		m_image.Destroy();
	m_image.Load(lpszFilePathName);
	if (m_image.IsNull()) {
		bOK = FALSE;
		m_image.Create(800, 600, 24); //default empty image
	}
	if (GetSafeHwnd())
		Invalidate();
	return bOK;
}

BOOL CImageViewerWnd::SaveImage(LPCTSTR lpszFilePathName, BOOL bDrawOveraly/* = TRUE*/)
{	
	CString str(lpszFilePathName);
	if (str.GetLength() > 4)
	{
		GUID guidImageFormat;
		if (str.Right(4).CompareNoCase(_T(".bmp")) == 0)
			guidImageFormat = Gdiplus::ImageFormatBMP;
		else if (str.Right(4).CompareNoCase(_T(".jpg")) == 0)
			guidImageFormat = Gdiplus::ImageFormatJPEG;
		else if (str.Right(4).CompareNoCase(_T(".png")) == 0)
			guidImageFormat = Gdiplus::ImageFormatPNG;
		else if (str.Right(4).CompareNoCase(_T(".gif")) == 0)
			guidImageFormat = Gdiplus::ImageFormatGIF;
		else if (str.Right(4).CompareNoCase(_T(".tif")) == 0)
			guidImageFormat = Gdiplus::ImageFormatTIFF;
		else
			return FALSE;

		if (!m_image.IsNull()) {
			if (bDrawOveraly == FALSE)
				m_image.Save(lpszFilePathName, guidImageFormat);
			else 
			{
				CImage image;
				BOOL bOK = image.Create(m_image.GetWidth(), m_image.GetHeight(), 24);
				if (bOK) {
					CDC *pDC = CDC::FromHandle(image.GetDC());
					m_image.Draw(pDC->m_hDC, 0, 0);
					DrawCircle(pDC, m_ptStart, m_nCircleRadius);
					DrawXMark(pDC, m_ptStart, RGB(255, 0, 0), TRUE);
					//DrawXMark(pDC, m_ptEnd, RGB(0, 0, 255),); //종료좌표는 제외
					image.ReleaseDC();
					image.Save(lpszFilePathName, guidImageFormat);
				}
				else
					return FALSE;
			}
			return TRUE;
		}
	}
	return FALSE;
}

//시작좌표와 종료좌표를 설정
void CImageViewerWnd::SetStartEndPoints(CPoint ptStart, CPoint ptEnd)
{
	m_ptStart = ptStart;
	m_ptEnd = ptEnd;
	if (GetSafeHwnd()) { Invalidate(); }
}

//원의 중심과 반지름을 입력
void CImageViewerWnd::MoveCircle(CPoint ptCenter, int nRadius/* = 10*/)
{
	m_ptStart = ptCenter;
	if (nRadius > 0)
		m_nCircleRadius = nRadius;
	if (GetSafeHwnd()) { Invalidate(); }
}

void CImageViewerWnd::ShowCircle(BOOL bShow/* = TRUE*/)
{
	m_bShowCircle = bShow;
	if (GetSafeHwnd()) { Invalidate(); }
}

void CImageViewerWnd::ShowXMark(BOOL bShow/* = TRUE*/)
{
	m_bShowXMark = bShow;
	if (GetSafeHwnd()) { Invalidate(); }
}

//이미지 배경색에 영향없이 항상 Circle이 보이도록 BLACK+WHITE 이중 그리기
void CImageViewerWnd::DrawCircle(CDC *pDC, CPoint pt, int nRadius/* = 10*/)
{
	pDC->SelectStockObject(NULL_BRUSH);
	//int nOldMode = pDC->SetROP2(R2_NOT); //배경색과 반전해도 안보이는 색이 존재하여 주석처리함.
	CRect rc(pt, pt);
	rc.InflateRect(nRadius, nRadius);
	pDC->SelectStockObject(WHITE_PEN);
	pDC->Ellipse(&rc);
	pDC->SelectStockObject(BLACK_PEN);
	rc.InflateRect(1, 1);
	pDC->Ellipse(&rc);
	//pDC->SetROP2(nOldMode);//R2_COPYPEN
}

//이미지 배경색에 영향없이 항상 텍스트 색상이 보이도록 출력
void CImageViewerWnd::DrawOutlineText(CDC *pDC, CString str, CRect rcText,
	UINT uFormat/* = DT_CENTER | DT_VCENTER | DT_SINGLELINE*/, COLORREF clrText/* = RGB(0, 0, 0)*/, 
	COLORREF clrOutline/* = RGB(255, 255, 255)*/)
{
	CPoint ptOffset[] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1}, {1, 0}, {0, 1}, {-1, 0}, {0, -1} };	
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(clrOutline);
	//8방향으로 테두리 텍스트 출력
	for (int i = 0; i < 8; i++) {
		CRect rc = rcText;
		rc.OffsetRect(ptOffset[i]);
		pDC->DrawText(str, &rc, uFormat);	
	}
	pDC->SetTextColor(clrText);
	pDC->DrawText(str, &rcText, uFormat);
}

//원의 중심에 X 표시를 그리고, 중심좌표를 TEXT 출력한다.
void CImageViewerWnd::DrawXMark(CDC *pDC, CPoint pt, COLORREF clrLine/* = RGB(255, 0, 0)*/, 
	BOOL bShowText/* = FALSE*/, int nXSize/* = 5*/)
{
	CPen pen;
	pen.CreatePen(PS_SOLID, 2, RGB(255, 255, 255));	
	pDC->SelectObject(&pen);
	DrawLine(pDC, CPoint(pt.x - nXSize - 1, pt.y - nXSize - 1), CPoint(pt.x + nXSize, pt.y + nXSize));
	DrawLine(pDC, CPoint(pt.x - nXSize - 1, pt.y + nXSize + 1), CPoint(pt.x + nXSize, pt.y - nXSize));
	pen.DeleteObject();
	pen.CreatePen(PS_SOLID, 1, clrLine);
	pDC->SelectObject(&pen);
	DrawLine(pDC, CPoint(pt.x - nXSize, pt.y - nXSize), CPoint(pt.x + nXSize, pt.y + nXSize));
	DrawLine(pDC, CPoint(pt.x - nXSize, pt.y + nXSize), CPoint(pt.x + nXSize, pt.y - nXSize));

	if (bShowText) {
		CRect rcText(pt, pt);
		rcText.InflateRect(500, 100);
		rcText.OffsetRect(25, 20);

		CFont font;
		font.CreatePointFont(100, _T("Arial"));
		pDC->SelectObject(&font);
		CString strText;
		strText.Format(_T("( %d, %d )"), pt.x, pt.y);

		DrawOutlineText(pDC, strText, rcText);
	}
}

void CImageViewerWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	//깜빡임 방지를 위해 MemoryDC 사용
	CRect rcClient;
	CDC memDC;
	CBitmap bitmap;
	COLORREF clrBackgnd = RGB(0, 0, 0);

	GetClientRect(&rcClient);
	memDC.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	memDC.SelectObject(&bitmap);
	memDC.FillSolidRect(&rcClient, clrBackgnd);
	if (!m_image.IsNull())
		m_image.Draw(memDC.m_hDC, 0, 0);

	if (m_bShowCircle)
		DrawCircle(&memDC, m_ptStart, m_nCircleRadius);

	if (m_bShowXMark) 
	{
		DrawXMark(&memDC, m_ptEnd, RGB(0, 0, 255), FALSE);
		DrawXMark(&memDC, m_ptStart, RGB(255, 0, 0), TRUE);
	}

	dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &memDC, 0, 0, SRCCOPY);
}


BOOL CImageViewerWnd::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}


void CImageViewerWnd::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int NumDropFiles = 0;
	TCHAR szFilePath[MAX_PATH];
	NumDropFiles = DragQueryFile(hDropInfo, 0xFFFFFFFF, szFilePath, MAX_PATH);
	if(NumDropFiles) {
		DragQueryFile(hDropInfo, 0, szFilePath, MAX_PATH);
		if (!::PathIsDirectory(szFilePath)) {
			if (OpenImage(szFilePath)) {
				ShowCircle(TRUE);
				ShowXMark(TRUE);
			}
		}
	}
	CWnd::OnDropFiles(hDropInfo);
}


int CImageViewerWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	ModifyStyleEx(0, WS_EX_ACCEPTFILES, 0);

	//관리자모드에서도 DragDrop 허용
	ChangeWindowMessageFilter(SPI_SETANIMATION, MSGFLT_ADD);
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	DragAcceptFiles(TRUE);

	return 0;
}


void CImageViewerWnd::PreSubclassWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	ModifyStyleEx(0, WS_EX_ACCEPTFILES, 0);

	//관리자모드에서도 DragDrop 허용
	ChangeWindowMessageFilter(SPI_SETANIMATION, MSGFLT_ADD);
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	DragAcceptFiles(TRUE);

	CWnd::PreSubclassWindow();
}
