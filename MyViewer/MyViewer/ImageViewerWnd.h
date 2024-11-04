#pragma once
#include <afxwin.h>
/*
	이미지 파일을 윈도우 화면에 출력하고, 원을 그려주는 CWnd에서 상속받은 클래스
*/
#define RESTORE_VALUE -1
class CImageViewerWnd : public CWnd
{
	DECLARE_DYNAMIC(CImageViewerWnd)
public:
	CImageViewerWnd();

	//이미지 파일을 열어 윈도우에 출력합니다.
	BOOL OpenImage(LPCTSTR lpszFilePathName);

	//이미지 파일로 저장합니다.
	void SaveImage(LPCTSTR lpszFilePathName);

	//화면에 원을 그릴지 설정한다.
	void ShowCircle(BOOL bShow = TRUE);						

	//원의 중심에 X를 Device Context 에 보여주거나 감춥니다.
	void ShowXMark(BOOL bShow = TRUE);

	int &GetRadius() { return m_nRadius; }
	CPoint &GetCenterPoint() { return m_ptCenter; }

	//Circle 영역내의 이미지버퍼에 nGrayValue값을 입력한다.
	void DrawCircle(int nCenterX, int nCenterY, int nRadius, int nGrayValue = RESTORE_VALUE);

	//이전에 그려진 Circle을 지운다(Original 이미지에서 픽셀값을 복원 적용)
	void ClearCircle();

	//같은 위치에 다시 그린다. (이미지 파일을 Load하여 이미지가 변경되었을 경우 호출)
	void RedrawCircle(int nGrayValue = 200); 

protected:
	void CreateNewImage(CImage &image, int nWidth = 640, int nHeight = 480, int nBPP = 8, int nPixelValue = 200);

	//src이미지와 동일한 새로운 dest이미지를 생성한다.
	void DuplicateImage(CImage &src, CImage &dest);
	void DrawLine(CDC *pDC, CPoint pt1, CPoint pt2) { pDC->MoveTo(pt1); pDC->LineTo(pt2); }

	//Center 좌표와 임의의 점(x, y)의 거리를 구하여 nRadius 이내에 있는지 체크한다.
	//Circle내에 있으면 TRUE를 리턴, 아니면 FALSE를 리턴한다.
	inline BOOL PtInCircle(int x, int y, int nCenterX, int nCenterY, int nRadius);
	
	//이미지 영역을 벗어난 부분을 제외시킨다.
	inline void OptimizeROI(CRect &rectROI, int nWidth, int nHeight);

	//테두리가 있는 텍스트를 출력합니다.(좌표 텍스트가 이미지 배경색과 같은 색상으로 안보이는 경우가 있어 테두리텍스트 )
	void DrawOutlineText(CDC *pDC, CString str, CRect rcText, UINT uFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE,
		COLORREF clrText = RGB(0, 0, 0), COLORREF clrOutline = RGB(255, 255, 255));

	//원중심에 nXSize 크기의 X를 표시하고, bShowText가 TRUE이면 좌표 텍스트도 함께 출력합니다.
	void DrawXMark(CDC *pDC, CPoint pt, COLORREF clrLine = RGB(255, 0, 0), BOOL bShowText = FALSE, int nXSize = 5);


	ATL::CImage m_image, m_imageOrigin;
	BOOL m_bShowCircle, m_bShowXMark;
	CPoint m_ptCenter;
	int m_nRadius;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void PreSubclassWindow();
};

