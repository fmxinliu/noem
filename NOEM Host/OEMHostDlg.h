#if !defined(AFX_OEMHOSTDLG_H__6E33728E_95DB_4BB0_B8AD_635DCD83FAD1__INCLUDED_)
#define AFX_OEMHOSTDLG_H__6E33728E_95DB_4BB0_B8AD_635DCD83FAD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OEMHostDlg.h : header file
//

#include "FingerWnd.h"
#include "Communication.h"
#include "afxwin.h"
#include "afxdtctl.h"

#include "crypt_user.h"

/////////////////////////////////////////////////////////////////////////////
// COEMHostDlg dialog

class COEMHostDlg : public CDialog
{
// Construction
public:
	COEMHostDlg(CWnd* pParent = NULL);   // standard constructor
	~COEMHostDlg();

	// Dialog Data
	//{{AFX_DATA(COEMHostDlg)
	enum { IDD = IDD_OEMHOST_DIALOG };
	//}}AFX_DATA

public:
	int				m_nUserID;
	int				m_nRange;
	BOOL			m_bCancel;
	BOOL			m_bFpImageExist;	
	BOOL			m_bTemplate1Exist;	
	BYTE			m_abyTemplate1[MAX_TEMPLATE_SIZE];
	unsigned int	m_nTemplateSize;
	CFont			m_Font1;
	CCommunication	m_clsCommu;

	CComboBox	m_cmbSetBaudrate;
	CComboBox	m_cmbUpImgType;
	CComboBox	m_cmbParamType;
	CFingerWnd	m_wndFinger;
	CComboBox	m_cmbConComPort;
	CComboBox	m_cmbConBaudRate;
	CString		m_strCmdResult;
	CString		m_strFpData;
	CString		m_strUserID;
	CString		m_strParamValue;
	CString		m_strModuleSN;
	int			m_nConMode;
	int			m_nLangType;
	UINT 		m_nLogImgWidth;
	UINT 		m_nLogImgHeight;
	int			m_nConDeviceID;
	CComboBox	m_cmbMaxFpCount;
	CButton		m_chkShowImage;
	CComboBox	m_cmbEnrollCount;
	BOOL		m_bStretchView;
	BOOL		m_bAdvLed;
	BOOL		m_bSecureComm;
	CDateTimeCtrl m_dtpDate;
	CDateTimeCtrl m_dtpTime;
	NOTIFYICONDATA m_nid;

public:
	BOOL	CheckUserID();
	BOOL	CheckModuleSN();
	BOOL	OpenImage(BYTE* pImageBuf, int* pnWidth, int* pnHeight);

	void	InitControl();	
	void	EnableControl(BOOL p_bEnable);
	void	SetLanguage(int nLanguageID);

	CString ConvertByteToPacketFormat(BYTE* p_pBuff, int p_nSize, int p_nUnit);
	CString ConvertByteToHex( BYTE* pBuff, int nSize, int nWidthSize );

	void	DoLedCtrl(unsigned char p_nCtrlCode);
	void    ShowTray();
	void    HideTray();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COEMHostDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(COEMHostDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnConnect();
	afx_msg void OnBtnDisconnect();
	afx_msg void OnBtnSaveFingerImage();
	afx_msg void OnBtnOpenTemplate();
	afx_msg void OnBtnSaveTemplate();
	afx_msg void OnBtnStop();
	afx_msg void OnBtnOpenFile1();
	afx_msg void OnBtnOpenFile2();
	afx_msg void OnBtnOpnFingerImage();
	afx_msg void OnBtnEnroll();
	afx_msg void OnBtnVerify();
	afx_msg void OnBtnIdentify();
	afx_msg void OnBtnIdentifyTemplate();
	afx_msg void OnBtnIdentifyImage();
	afx_msg void OnBtnDetectFinger();
	afx_msg void OnBtnGetUserCount();
	afx_msg void OnBtnDeleteId();
	afx_msg void OnBtnGetEmptyId();
	afx_msg void OnBtnGetStatus();
	afx_msg void OnBtnGetBrokenId();
	afx_msg void OnBtnUpTemplate();
	afx_msg void OnBtnDownTemplate();
	afx_msg void OnBtnGetDeviceInfo();
	afx_msg void OnBtnUpImage();
	afx_msg void OnBtnSetParam();
	afx_msg void OnBtnGetParam();
	afx_msg void OnBtnDeleteAll();
	afx_msg void OnBtnDownMultiTmpl();
	afx_msg void OnEditchangeCombo1();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnBtnVerifyWithDownTmpl();
	afx_msg void OnBtnSetModuleSn();
	afx_msg void OnBtnGetModuleSn();
	afx_msg void OnCbnSelchangeCmbMaxFpCount();
	afx_msg void OnBnClickedRadioChinese();
	afx_msg void OnBnClickedRadioEnglish();
	afx_msg void OnBnClickedBtnEnrollFromImage();
	afx_msg void OnBnClickedBtnGetEnrolledIdList();
	afx_msg void OnBnClickedBtnEnterStandby();
	afx_msg void OnBnClickedBtnAdjustSensor();
	afx_msg void OnBnClickedButtonUpgrade();
	afx_msg void OnBnClickedBtnUpMultiTmpl();
	afx_msg void OnBnClickedBtnSetRtc();
	afx_msg void OnBnClickedBtnGetRtc();
	afx_msg LRESULT OnShowTask(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OEMHOSTDLG_H__6E33728E_95DB_4BB0_B8AD_635DCD83FAD1__INCLUDED_)
