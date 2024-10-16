#pragma once
#include <afxwin.h>
/*
	이미지 파일을 윈도우 화면에 출력하고, 원을 그려주는 CWnd에서 상속받은 클래스
*/

class CImageViewerWnd : public CWnd
{
	DECLARE_DYNAMIC(CImageViewerWnd)
public:
	CImageViewerWnd();

	//이미지 파일을 열어 윈도우에 출력합니다.
	BOOL OpenImage(LPCTSTR lpszFilePathName);

	//이미지 파일로 저장합니다. bDrawOverlay가 TRUE 이면 원과 X좌표 텍스트도 함께 그려 저장합니다.
	BOOL SaveImage(LPCTSTR lpszFilePathName, BOOL bDrawOveraly = TRUE);

	//시작좌표와 종료좌표를 설정합니다.
	void SetStartEndPoints(CPoint ptStart, CPoint ptEnd);	

	//원의 중심과 반지름을 입력합니다.
	void MoveCircle(CPoint ptCenter, int nRadius = 10);		

	//화면에 원을 보여주거나 감춥니다.
	void ShowCircle(BOOL bShow = TRUE);						

	//원의 중심에 X를 보여주거나 감춥니다.
	void ShowXMark(BOOL bShow = TRUE);						

protected:
	void DrawLine(CDC *pDC, CPoint pt1, CPoint pt2) { pDC->MoveTo(pt1); pDC->LineTo(pt2); }
	
	//테두리가 있는 텍스트를 출력합니다.(좌표 텍스트가 이미지 배경색과 같은 색상으로 안보이는 경우가 있어 테두리텍스트 )
	void DrawOutlineText(CDC *pDC, CString str, CRect rcText, UINT uFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE,
		COLORREF clrText = RGB(0, 0, 0), COLORREF clrOutline = RGB(255, 255, 255));

	//DC의 pt위치에 nRadius반지름의 원을 그립니다.
	void DrawCircle(CDC *pDC, CPoint pt, int nRadius = 20);

	//원중심에 nXSize 크기의 X를 표시하고, bShowText가 TRUE이면 좌표 텍스트도 함께 출력합니다.
	void DrawXMark(CDC *pDC, CPoint pt, COLORREF clrLine = RGB(255, 0, 0), BOOL bShowText = FALSE, int nXSize = 5);

	ATL::CImage m_image;
	BOOL m_bShowCircle, m_bShowXMark;
	CPoint m_ptStart, m_ptEnd;
	int m_nCircleRadius;

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void PreSubclassWindow();
};

