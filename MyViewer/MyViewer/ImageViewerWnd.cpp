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
					//DrawXMark(pDC, m_ptEnd, RGB(0, 0, 255),); //������ǥ�� ����
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

//������ǥ�� ������ǥ�� ����
void CImageViewerWnd::SetStartEndPoints(CPoint ptStart, CPoint ptEnd)
{
	m_ptStart = ptStart;
	m_ptEnd = ptEnd;
	if (GetSafeHwnd()) { Invalidate(); }
}

//���� �߽ɰ� �������� �Է�
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

//�̹��� ������ ������� �׻� Circle�� ���̵��� BLACK+WHITE ���� �׸���
void CImageViewerWnd::DrawCircle(CDC *pDC, CPoint pt, int nRadius/* = 10*/)
{
	pDC->SelectStockObject(NULL_BRUSH);
	//int nOldMode = pDC->SetROP2(R2_NOT); //������ �����ص� �Ⱥ��̴� ���� �����Ͽ� �ּ�ó����.
	CRect rc(pt, pt);
	rc.InflateRect(nRadius, nRadius);
	pDC->SelectStockObject(WHITE_PEN);
	pDC->Ellipse(&rc);
	pDC->SelectStockObject(BLACK_PEN);
	rc.InflateRect(1, 1);
	pDC->Ellipse(&rc);
	//pDC->SetROP2(nOldMode);//R2_COPYPEN
}

//�̹��� ������ ������� �׻� �ؽ�Ʈ ������ ���̵��� ���
void CImageViewerWnd::DrawOutlineText(CDC *pDC, CString str, CRect rcText,
	UINT uFormat/* = DT_CENTER | DT_VCENTER | DT_SINGLELINE*/, COLORREF clrText/* = RGB(0, 0, 0)*/, 
	COLORREF clrOutline/* = RGB(255, 255, 255)*/)
{
	CPoint ptOffset[] = { {-1, -1}, {-1, 1}, {1, -1}, {1, 1}, {1, 0}, {0, 1}, {-1, 0}, {0, -1} };	
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(clrOutline);
	//8�������� �׵θ� �ؽ�Ʈ ���
	for (int i = 0; i < 8; i++) {
		CRect rc = rcText;
		rc.OffsetRect(ptOffset[i]);
		pDC->DrawText(str, &rc, uFormat);	
	}
	pDC->SetTextColor(clrText);
	pDC->DrawText(str, &rcText, uFormat);
}

//���� �߽ɿ� X ǥ�ø� �׸���, �߽���ǥ�� TEXT ����Ѵ�.
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

	//������ ������ ���� MemoryDC ���
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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

	//�����ڸ�忡���� DragDrop ���
	ChangeWindowMessageFilter(SPI_SETANIMATION, MSGFLT_ADD);
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	DragAcceptFiles(TRUE);

	return 0;
}


void CImageViewerWnd::PreSubclassWindow()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	ModifyStyleEx(0, WS_EX_ACCEPTFILES, 0);

	//�����ڸ�忡���� DragDrop ���
	ChangeWindowMessageFilter(SPI_SETANIMATION, MSGFLT_ADD);
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	DragAcceptFiles(TRUE);

	CWnd::PreSubclassWindow();
}
