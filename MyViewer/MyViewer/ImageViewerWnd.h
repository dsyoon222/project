#pragma once
#include <afxwin.h>
/*
	�̹��� ������ ������ ȭ�鿡 ����ϰ�, ���� �׷��ִ� CWnd���� ��ӹ��� Ŭ����
*/

class CImageViewerWnd : public CWnd
{
	DECLARE_DYNAMIC(CImageViewerWnd)
public:
	CImageViewerWnd();

	//�̹��� ������ ���� �����쿡 ����մϴ�.
	BOOL OpenImage(LPCTSTR lpszFilePathName);

	//�̹��� ���Ϸ� �����մϴ�. bDrawOverlay�� TRUE �̸� ���� X��ǥ �ؽ�Ʈ�� �Բ� �׷� �����մϴ�.
	BOOL SaveImage(LPCTSTR lpszFilePathName, BOOL bDrawOveraly = TRUE);

	//������ǥ�� ������ǥ�� �����մϴ�.
	void SetStartEndPoints(CPoint ptStart, CPoint ptEnd);	

	//���� �߽ɰ� �������� �Է��մϴ�.
	void MoveCircle(CPoint ptCenter, int nRadius = 10);		

	//ȭ�鿡 ���� �����ְų� ����ϴ�.
	void ShowCircle(BOOL bShow = TRUE);						

	//���� �߽ɿ� X�� �����ְų� ����ϴ�.
	void ShowXMark(BOOL bShow = TRUE);						

protected:
	void DrawLine(CDC *pDC, CPoint pt1, CPoint pt2) { pDC->MoveTo(pt1); pDC->LineTo(pt2); }
	
	//�׵θ��� �ִ� �ؽ�Ʈ�� ����մϴ�.(��ǥ �ؽ�Ʈ�� �̹��� ������ ���� �������� �Ⱥ��̴� ��찡 �־� �׵θ��ؽ�Ʈ )
	void DrawOutlineText(CDC *pDC, CString str, CRect rcText, UINT uFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE,
		COLORREF clrText = RGB(0, 0, 0), COLORREF clrOutline = RGB(255, 255, 255));

	//DC�� pt��ġ�� nRadius�������� ���� �׸��ϴ�.
	void DrawCircle(CDC *pDC, CPoint pt, int nRadius = 20);

	//���߽ɿ� nXSize ũ���� X�� ǥ���ϰ�, bShowText�� TRUE�̸� ��ǥ �ؽ�Ʈ�� �Բ� ����մϴ�.
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

