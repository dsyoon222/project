
// MyViewerDlg.h: 헤더 파일
//

#pragma once
#include "ImageViewerWnd.h"

// CMyViewerDlg 대화 상자
class CMyViewerDlg : public CDialogEx
{
// 생성입니다.
public:
	CMyViewerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	void UpdateImageViewerPoints(); //시작,종료좌표를 m_wndImageViewer에 업데이트
	CImageViewerWnd m_wndImageViewer;
	CString m_strExeFilePath;
	CToolTipCtrl m_ToolTip;

	void OnOK() {} //Enter키로 대화상자 종료되지 않도록 막음
	void OnCancel();
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYVIEWER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedDraw();
	afx_msg void OnBnClickedAction();
	afx_msg void OnBnClickedOpen();

protected:
	int m_x1;
	int m_y1;
	int m_x2;
	int m_y2;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChangeEditX1();
	afx_msg void OnEnChangeEditY1();
	afx_msg void OnEnChangeEditX2();
	afx_msg void OnEnChangeEditY2();
};
