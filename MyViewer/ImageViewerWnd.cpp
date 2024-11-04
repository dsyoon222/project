#include "pch.h"
#include "ImageViewerWnd.h"

IMPLEMENT_DYNAMIC(CImageViewerWnd, CWnd)

CImageViewerWnd::CImageViewerWnd()
{
	m_ptCenter = CPoint(-1, -1);
	m_bShowCircle = m_bShowXMark = FALSE;
	m_nRadius = 30;
	CreateNewImage(m_image, 640, -480, 8, 128);
	DuplicateImage(m_image, m_imageOrigin);
}

BEGIN_MESSAGE_MAP(CImageViewerWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DROPFILES()
	ON_WM_CREATE()
END_MESSAGE_MAP()


void CImageViewerWnd::CreateNewImage(CImage &image, int nWidth, int nHeight, int nBPP, int nPixelValue)
{
	if (!image.IsNull())
		image.Destroy();
	image.Create(nWidth, nHeight, nBPP);

	//8비트 이미지의 경우 팔레트 반영
	if (nBPP == 8) {
		RGBQUAD *pRgb = new RGBQUAD[256];
		for (int i = 0; i < 256; i++) {
			pRgb[i].rgbBlue = pRgb[i].rgbGreen = pRgb[i].rgbRed = i;
			pRgb[i].rgbReserved = 0;
		}
		image.SetColorTable(0, 256, pRgb);
		delete[]pRgb;
	}
	//이미지 버퍼 초기값 설정(nHeight가 +- 값에 따라 Pitch 부호가 바뀜에 주의)
	int nPitch = image.GetPitch();
	if (nPitch > 0)
		memset(image.GetBits(), nPixelValue, abs(nHeight) * nPitch);
	else {
		BYTE *pBits = (BYTE*)image.GetBits();
		pBits = pBits + (abs(nHeight) - 1) * nPitch; //nPitch is minus 
		memset(pBits, nPixelValue, abs(nHeight) * abs(nPitch));
	}
}

//이미지 복사(Load한 칼라영상 Circle Draw 후 원복위해 복제이미지 생성 - 깊은복사)
void CImageViewerWnd::DuplicateImage(CImage &src, CImage &dst)
{
	if (src.IsNull())
		return;
	HBITMAP hBmpCopy = (HBITMAP)::CopyImage(src.operator HBITMAP(), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_CREATEDIBSECTION);
	BITMAP bm;
	CBitmap::FromHandle(src.operator HBITMAP())->GetBitmap(&bm);
	if (!dst.IsNull()) {
		dst.Destroy();
	}
	int nPitch = src.GetPitch();
	dst.Attach(hBmpCopy, nPitch < 0 ? CImage::DIBOR_BOTTOMUP : CImage::DIBOR_TOPDOWN); //Pitch +- 에 따른 메모리 순서를 알맞게 적용함.
}

BOOL CImageViewerWnd::OpenImage(LPCTSTR lpszFilePathName)
{
	BOOL bOK = TRUE;
	if (!m_image.IsNull())
		m_image.Destroy();
	m_image.Load(lpszFilePathName);
	if (m_image.IsNull()) {
		bOK = FALSE;
		CreateNewImage(m_image, 640, 480, 8);
	}
	DuplicateImage(m_image, m_imageOrigin);
	
	if (GetSafeHwnd())
		Invalidate();
	return bOK;
}

void CImageViewerWnd::SaveImage(LPCTSTR lpszFilePathName)
{	
	if (!m_image.IsNull())
		m_image.Save(lpszFilePathName);
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

BOOL CImageViewerWnd::PtInCircle(int x, int y, int nCenterX, int nCenterY, int nRadius)
{
	int deltaX = x - nCenterX;
	int deltaY = y - nCenterY;
	int nDistance = deltaX * deltaX + deltaY * deltaY;
	//double dblDist = sqrt(nDistance); //It will be a little slower to process.
	if (nDistance < (nRadius * nRadius))
		return TRUE;
	return FALSE;
}

//이미지 좌표를 벗어난 부분은 제외
//(주의) 출력 rectROI의 rectROI.bottom와 rectROI.right는 이미지 좌표를 벗어나기때문에 참조시 포함시키지 말것!!!
void CImageViewerWnd::OptimizeROI(CRect &rectROI, int nWidth, int nHeight)
{
	rectROI.NormalizeRect();//left, right, top, bottom 값 재정렬
	if (rectROI.left < 0) rectROI.left = 0;
	else if (rectROI.left > nWidth) rectROI.left = nWidth;

	if (rectROI.top < 0) rectROI.top = 0;
	else if (rectROI.top > nHeight) rectROI.top = nHeight;

	if (rectROI.right < 0) rectROI.right = 0;
	else if (rectROI.right > nWidth) rectROI.right = nWidth;

	if (rectROI.bottom < 0) rectROI.bottom = 0;
	else if (rectROI.bottom > nHeight) rectROI.bottom = nHeight;
}

void CImageViewerWnd::ClearCircle()
{
	if(m_ptCenter.x >= 0 && m_ptCenter.y >= 0)
		DrawCircle(m_ptCenter.x, m_ptCenter.y, m_nRadius, RESTORE_VALUE);
}

void CImageViewerWnd::RedrawCircle(int nGrayValue)
{
	if (m_ptCenter.x >= 0 && m_ptCenter.y >= 0)
		DrawCircle(m_ptCenter.x, m_ptCenter.y, m_nRadius, nGrayValue);
}

//이미지의 Circle영역에 nGrayValue 값을 넣는다.
//nGrayValue 가 RESTORE_VALUE(-1)이면 원본에서 이미지 픽셀값을 복구한다.
void CImageViewerWnd::DrawCircle(int nCenterX, int nCenterY, int nRadius, int nGrayValue/* = RESTORE_VALUE*/)
{
	if (!m_bShowCircle)
		return;
	if (m_image.IsNull())
		return;

	//Circle을 지우기위한 좌표와 Radius를 저장해 둔다.
	if (nGrayValue != RESTORE_VALUE) {
		m_ptCenter.x = nCenterX;
		m_ptCenter.y = nCenterY;
		m_nRadius = nRadius;
	}

	int nPitch = m_image.GetPitch();
	int nWidth = m_image.GetWidth();
	int nHeight = m_image.GetHeight();
	int nChannel = m_image.GetBPP() / 8;
	if (m_image.GetBPP() < 8) //1, 2, 4비트 이미지인 경우는 처리안하고 리턴.
		return;
	BYTE *pData = (BYTE*)m_image.GetBits();
	BYTE *pDataOrigin = (BYTE*)m_imageOrigin.GetBits();

	CRect rcROI(nCenterX - nRadius, nCenterY - nRadius, nCenterX + nRadius + 1, nCenterY + nRadius + 1);
	OptimizeROI(rcROI, nWidth, nHeight);//이미지 영역을 벗어나면 이미지영역내의 좌표만 적용

	for (int y = rcROI.top; y < rcROI.bottom; y++) {
		for (int x = rcROI.left; x < rcROI.right; x++) {
			if (PtInCircle(x, y, nCenterX, nCenterY, nRadius)) {
				//Load한 이미지가 RGB 칼라 이미지인 경우도 포함하여 처리
				int nIndex = y * nPitch + x * nChannel;
				if (nGrayValue == RESTORE_VALUE) //이미지 원복
					memcpy(&pData[nIndex], &pDataOrigin[nIndex], nChannel);
				else
					memset(&pData[nIndex], nGrayValue, nChannel);
			}
		}
	}
	if (GetSafeHwnd()) { Invalidate(); }
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
	pen.CreatePen(PS_SOLID, 1, clrLine);
	CPen *pOldPen = pDC->SelectObject(&pen);
	DrawLine(pDC, CPoint(pt.x - nXSize, pt.y - nXSize), CPoint(pt.x + nXSize, pt.y + nXSize));
	DrawLine(pDC, CPoint(pt.x - nXSize, pt.y + nXSize), CPoint(pt.x + nXSize, pt.y - nXSize));
	pDC->SelectObject(pOldPen);
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

	CRect rc;
	GetWindowRect(&rc);
	int w = rc.Width();
	memDC.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	memDC.SelectObject(&bitmap);

	memDC.FillSolidRect(&rcClient, clrBackgnd);

	if (!m_image.IsNull())
		m_image.Draw(memDC.m_hDC, 0, 0);
	
	if (m_bShowXMark && m_ptCenter.x > 0 && m_ptCenter.y > 0)
		DrawXMark(&memDC, m_ptCenter, RGB(255, 0, 0), TRUE);

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
	ModifyStyleEx(WS_BORDER, 0, 0);
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
	ModifyStyleEx(WS_BORDER, 0, 0);
	//관리자모드에서도 DragDrop 허용
	ChangeWindowMessageFilter(SPI_SETANIMATION, MSGFLT_ADD);
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	DragAcceptFiles(TRUE);

	CWnd::PreSubclassWindow();
}
