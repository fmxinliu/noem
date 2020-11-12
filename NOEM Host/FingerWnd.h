#if !defined(AFX_FINGERWND_H__7B416256_B532_4C46_ABB7_781448782C1F__INCLUDED_)
#define AFX_FINGERWND_H__7B416256_B532_4C46_ABB7_781448782C1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FingerWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFingerWnd window

class CFingerWnd : public CStatic
{
// Construction
public:
	CFingerWnd();

// Attributes
public:
	PBYTE	m_pImg;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFingerWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFingerWnd();
	void DrawImageBuffer(HDC p_hdc, BYTE* p_pImageBuffer, int p_w, int p_h);
	void DrawImage(BOOL p_bStretch = TRUE);
	void DrawImageArea(BOOL p_bDrawImageArea);
	void ClearFpImage();	
	BOOL IsEmpty();
	BOOL SetImage(PBYTE p_pImg, int p_nWidth, int p_nHeight, BOOL p_bStretch = TRUE);
	
	int		m_nImageWidth;
	int		m_nImageHeight;
	BOOL	m_bDrawImageArea;
	// Generated message map functions
protected:
	//{{AFX_MSG(CFingerWnd)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINGERWND_H__7B416256_B532_4C46_ABB7_781448782C1F__INCLUDED_)
