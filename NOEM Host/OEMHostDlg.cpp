// OEMHostDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OEMHost.h"
#include "OEMHostDlg.h"
#include "Serial.h"
#include "FolderDlg.h"
#include "MyString.h"
#include "ImReader/ImReader.h"
#include <mmsystem.h>

#define WM_SHOWTASK WM_USER + 100 // 托盘点击消息

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BYTE		g_FpImageBuf[100000]; // max size : 300*300
BYTE		g_FpImageBuf2[100000]; // max size : 300*300
int			g_nImageWidth;
int			g_nImageHeight;
int			g_nMaxFpCount = 20;
int			g_anMaxFpCount[17] = {2, 10, 20, 40, 50, 60, 80, 100, 170, 200, 300, 400, 500, 1000, 1700, 2000, 3000};
BOOL		g_bLangEng = TRUE;

int		g_nMultiTemplateCount;

HWND	g_hMainWnd = NULL;
void	DoEvents();
BOOL	IsNumber(CString str)
{
	int		i, count;
	TCHAR	temp;
	
	if (str.GetLength() > 10)
		return FALSE;
	
	if (str.GetLength() == 10 && str > _T("4294967295"))
		return FALSE;

	count = str.GetLength();

	if (count == 0)
		return FALSE;

	for(i=0; i < count; i++)
	{ 
		temp = str.GetAt(i); 
						
		if(temp >= '0' && temp <= '9') 
			continue; 
		else break; 
	}

	if (i != count)
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Led Control
typedef enum _E_LED_COLOR {
	E_LED_COLOR_OK				= 0,	//蓝色
	E_LED_COLOR_NG				= 1,	//红色
	E_LED_COLOR_3				= 2,	//预留
	E_LED_COLOR_4				= 3,	//预留
	E_LED_COLOR_5				= 4,	//预留
	E_LED_COLOR_6				= 5,	//预留
	E_LED_COLOR_7				= 6,	//预留
}E_LED_COLOR;

typedef enum _E_LED_STATUS {
	E_LED_STATUS_OFF			= 0,	//关
	E_LED_STATUS_ON				= 1,	//开
	E_LED_STATUS_BREATH			= 2,	//呼吸
	E_LED_STATUS_BLINK_SLOW		= 3,	//慢闪烁
	E_LED_STATUS_BLINK_QUICK	= 4,	//快闪烁
}E_LED_STATUS;

typedef enum _E_LED_CTRL_CODE {
	E_LED_CTRL_CONNECT			= 0,	//打开
	E_LED_CTRL_DISCONNECT		= 1,	//断开
	E_LED_CTRL_WAIT				= 2,	//等着
	E_LED_CTRL_CAPTURE			= 3,	//采集
	E_LED_CTRL_DETECTED			= 4,	//采集完成
	E_LED_CTRL_IDENTIFY_OK		= 5,	//识别通过
	E_LED_CTRL_IDENTIFY_NG		= 6,	//识别失败
};


// Led Control Value
#define LED_VAL(color, status)			(unsigned short)(((color & 0xFF) << 8) + (status & 0xFF))
//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// COEMHostDlg dialog


COEMHostDlg::COEMHostDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COEMHostDlg::IDD, pParent)
	, m_nConDeviceID(0)
	, m_nLogImgWidth(256)
	, m_nLogImgHeight(360)
	, m_bStretchView(TRUE)
	, m_bAdvLed(FALSE)
	, m_bSecureComm(FALSE)
{
	//{{AFX_DATA_INIT(COEMHostDlg)
	m_strCmdResult = _T("");
	m_strFpData = _T("");
	m_strUserID = _T("");
	m_strParamValue = _T("");
	m_strModuleSN = _T("");
	m_nConMode = 0;
	m_nLangType = 0;
	//}}AFX_DATA_INIT

	m_bTemplate1Exist = FALSE;
	m_bFpImageExist = FALSE;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

COEMHostDlg::~COEMHostDlg()
{
}

void COEMHostDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COEMHostDlg)
	DDX_Control(pDX, IDC_CMB_SET_BAUDRATE, m_cmbSetBaudrate);
	DDX_Control(pDX, IDC_COMBO2, m_cmbUpImgType);
	DDX_Control(pDX, IDC_COMBO1, m_cmbParamType);
	DDX_Control(pDX, IDC_WND_FINGER, m_wndFinger);
	DDX_Control(pDX, IDC_CMB_COMPORT, m_cmbConComPort);
	DDX_Control(pDX, IDC_CMB_BAUDRATE, m_cmbConBaudRate);
	DDX_Text(pDX, IDC_EDIT_RESULT, m_strCmdResult);
	DDX_Text(pDX, IDC_EDIT_FP_DATA, m_strFpData);
	DDX_Text(pDX, IDC_EDIT_USER_ID, m_strUserID);
	DDX_Text(pDX, IDC_EDIT1, m_strParamValue);
	DDX_Text(pDX, IDC_EDIT_MODULE_SN, m_strModuleSN);
	DDX_Radio(pDX, IDC_RADIO_COMM_SERIAL, m_nConMode);
	DDX_Radio(pDX, IDC_RADIO_CHINESE, m_nLangType);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_DEVICE_ID, m_nConDeviceID);
	DDX_Control(pDX, IDC_CMB_MAX_FP_COUNT, m_cmbMaxFpCount);
	DDX_Control(pDX, IDC_CHK_SHOW_IMAGE, m_chkShowImage);
	DDX_Control(pDX, IDC_CMB_ENROLL_COUNT, m_cmbEnrollCount);
	DDX_Text(pDX, IDC_EDT_IMG_WIDTH, m_nLogImgWidth);
	DDX_Text(pDX, IDC_EDT_IMG_HEIGHT, m_nLogImgHeight);
	DDX_Check(pDX, IDC_CHK_IMG_STRETCH, m_bStretchView);
	DDX_Check(pDX, IDC_CHK_ADVANCED_LED, m_bAdvLed);
	DDX_Check(pDX, IDC_CHK_SECURE_COMM, m_bSecureComm);
	DDX_Control(pDX, IDC_DTP_DATE, m_dtpDate);
	DDX_Control(pDX, IDC_DTP_TIME, m_dtpTime);
}


BEGIN_MESSAGE_MAP(COEMHostDlg, CDialog)
	//{{AFX_MSG_MAP(COEMHostDlg)
	ON_BN_CLICKED(IDC_BTN_CONNECT, OnBtnConnect)
	ON_BN_CLICKED(IDC_BTN_DISCONNECT, OnBtnDisconnect)
	ON_BN_CLICKED(IDC_BTN_SAVE_FINGER_IMAGE, OnBtnSaveFingerImage)
	ON_BN_CLICKED(IDC_BTN_OPEN_TEMPLATE, OnBtnOpenTemplate)
	ON_BN_CLICKED(IDC_BTN_SAVE_TEMPLATE, OnBtnSaveTemplate)
	ON_BN_CLICKED(IDC_BTN_STOP, OnBtnStop)
	ON_BN_CLICKED(IDC_BTN_OPEN_FILE1, OnBtnOpenFile1)
	ON_BN_CLICKED(IDC_BTN_OPEN_FILE2, OnBtnOpenFile2)
	ON_BN_CLICKED(IDC_BTN_OPN_FINGER_IMAGE, OnBtnOpnFingerImage)
	ON_BN_CLICKED(IDC_BTN_ENROLL, OnBtnEnroll)
	ON_BN_CLICKED(IDC_BTN_VERIFY, OnBtnVerify)
	ON_BN_CLICKED(IDC_BTN_IDENTIFY, OnBtnIdentify)
	ON_BN_CLICKED(IDC_BTN_IDENTIFY_TEMPLATE, OnBtnIdentifyTemplate)
	ON_BN_CLICKED(IDC_BTN_IDENTIFY_IMAGE, OnBtnIdentifyImage)
	ON_BN_CLICKED(IDC_BTN_DETECT_FINGER, OnBtnDetectFinger)
	ON_BN_CLICKED(IDC_BTN_GET_USER_COUNT, OnBtnGetUserCount)
	ON_BN_CLICKED(IDC_BTN_DELETE_ID, OnBtnDeleteId)
	ON_BN_CLICKED(IDC_BTN_GET_EMPTY_ID, OnBtnGetEmptyId)
	ON_BN_CLICKED(IDC_BTN_GET_STATUS, OnBtnGetStatus)
	ON_BN_CLICKED(IDC_BTN_GET_BROKEN_ID, OnBtnGetBrokenId)
	ON_BN_CLICKED(IDC_BTN_UP_TEMPLATE, OnBtnUpTemplate)
	ON_BN_CLICKED(IDC_BTN_DOWN_TEMPLATE, OnBtnDownTemplate)
	ON_BN_CLICKED(IDC_BTN_GET_DEVICE_INFO, OnBtnGetDeviceInfo)
	ON_BN_CLICKED(IDC_BTN_UP_IMAGE, OnBtnUpImage)
	ON_BN_CLICKED(IDC_BTN_SET_PARAM, OnBtnSetParam)
	ON_BN_CLICKED(IDC_BTN_GET_PARAM, OnBtnGetParam)
	ON_BN_CLICKED(IDC_BTN_DELETE_ALL, OnBtnDeleteAll)
	ON_BN_CLICKED(IDC_BTN_DOWN_MULTI_TMPL, OnBtnDownMultiTmpl)
	ON_CBN_EDITCHANGE(IDC_COMBO1, OnEditchangeCombo1)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BTN_VERIFY_WITH_DOWN_TMPL, OnBtnVerifyWithDownTmpl)
	ON_BN_CLICKED(IDC_BTN_SET_MODULE_SN, OnBtnSetModuleSn)
	ON_BN_CLICKED(IDC_BTN_GET_MODULE_SN, OnBtnGetModuleSn)
	ON_CBN_SELCHANGE(IDC_CMB_MAX_FP_COUNT, &COEMHostDlg::OnCbnSelchangeCmbMaxFpCount)
	ON_BN_CLICKED(IDC_RADIO_CHINESE, &COEMHostDlg::OnBnClickedRadioChinese)
	ON_BN_CLICKED(IDC_RADIO_ENGLISH, &COEMHostDlg::OnBnClickedRadioEnglish)
	ON_BN_CLICKED(IDC_BTN_ENROLL_FROM_IMAGE, &COEMHostDlg::OnBnClickedBtnEnrollFromImage)
	ON_BN_CLICKED(IDC_BTN_GET_ENROLLED_ID_LIST, &COEMHostDlg::OnBnClickedBtnGetEnrolledIdList)
	ON_BN_CLICKED(IDC_BTN_ENTER_STANDBY, &COEMHostDlg::OnBnClickedBtnEnterStandby)
	ON_BN_CLICKED(IDC_BTN_ADJUST_SENSOR, &COEMHostDlg::OnBnClickedBtnAdjustSensor)
	ON_BN_CLICKED(IDC_BUTTON_UPGRADE, &COEMHostDlg::OnBnClickedButtonUpgrade)
	ON_BN_CLICKED(IDC_BTN_UP_MULTI_TMPL, &COEMHostDlg::OnBnClickedBtnUpMultiTmpl)
	ON_BN_CLICKED(IDC_BTN_SET_RTC, &COEMHostDlg::OnBnClickedBtnSetRtc)
	ON_BN_CLICKED(IDC_BTN_GET_RTC, &COEMHostDlg::OnBnClickedBtnGetRtc)
    ON_MESSAGE(WM_SHOWTASK, &COEMHostDlg::OnShowTask)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COEMHostDlg message handlers

BOOL COEMHostDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	HideTray();
	return CDialog::DestroyWindow();
}

BOOL COEMHostDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
    ShowTray();

	// TODO: Add extra initialization here
	InitControl();	

	g_hMainWnd = m_hWnd;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/***************************************************************************/
/***************************************************************************/
BOOL COEMHostDlg::CheckUserID()
{
	DWORD	w_dwParam;
	CString	w_strMsg;

	//. Check FP No
	if (!IsNumber(m_strUserID))
	{
		w_strMsg.Format(_T("Please input correct user id(%d~%d)"), 1, g_nMaxFpCount);
		AfxMessageBox(w_strMsg);
		return FALSE;
	}
	w_dwParam = _ttoi(m_strUserID);
	if (w_dwParam < 1 || w_dwParam > (DWORD)g_nMaxFpCount)
	{
		w_strMsg.Format(_T("Please input correct user id(%d~%d)"), 1, g_nMaxFpCount);
		AfxMessageBox(w_strMsg);
		return FALSE;
	}

	m_nUserID = w_dwParam;

	return TRUE;
}
/***************************************************************************/
/***************************************************************************/
BOOL COEMHostDlg::CheckModuleSN()
{
	CString str;
	
	UpdateData(TRUE);

	if (m_strModuleSN.GetLength() > MODULE_SN_LEN)
	{
		str.Format(_T("Please input SN less than %d letters!"), MODULE_SN_LEN);
		AfxMessageBox(str);
		return FALSE;
	}
	
	return TRUE;
}
/***************************************************************************/
/***************************************************************************/
BOOL COEMHostDlg::OpenImage(BYTE* pImageBuf, int* pnWidth, int* pnHeight)
{
	int			w_nWidth, w_nHeight;
	char		w_szTemp[256];
	BYTE*		w_pBuf;
	BOOL		w_bRet;
	CString		strFile;
	CString		sFilter;

	sFilter= _T("BMP File(*.bmp)|*.bmp|All File(*.*)|*.*||");
	CFileDialog fo(TRUE, _T("bmp"), _T("Finger.bmp"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFilter,NULL);

	if (fo.DoModal() != IDOK)
		return FALSE;

	strFile = fo.GetPathName();

	memset(w_szTemp, 0, sizeof(w_szTemp));
	::WideCharToMultiByte(CP_ACP, 0, strFile.GetBuffer(0), -1, w_szTemp, 256, NULL, NULL);

	if (FCLoadImage(w_szTemp, &w_pBuf, &w_nWidth, &w_nHeight, 0) != 0)
	{
		AfxMessageBox(_T("Load Fail!!!"));
		return FALSE;
	}

// 	if (w_nWidth != 202 && w_nWidth != 242 && w_nWidth != 192)
// 	{
// 		w_bRet = FALSE;
// 		AfxMessageBox(_T("Image size is not correct!"));
// 		goto l_exit;
// 	}
// 
// 	if (w_nHeight != 258 && w_nHeight != 266 && w_nHeight != 192)
// 	{
// 		w_bRet = FALSE;
// 		AfxMessageBox(_T("Image size is not correct!"));
// 		goto l_exit;
// 	}

	memcpy(pImageBuf, w_pBuf, w_nWidth*w_nHeight);

	*pnWidth	= w_nWidth;
	*pnHeight	= w_nHeight;
	
	w_bRet = TRUE;

// l_exit:

	delete[] w_pBuf;

	return w_bRet;
}
/***************************************************************************/
/***************************************************************************/
void COEMHostDlg::InitControl()
{	
	int		i;
	CString	w_strTmp;
	
	m_strUserID = _T("1");

	SetLanguage(0);

	//. Search Comport and Add to combo box
	for (i=1; i<=16; i++)
	{
		w_strTmp.Format(_T("COM%d"), i);

		m_cmbConComPort.AddString(w_strTmp);
	}
	
  	m_Font1.CreateStockObject(SYSTEM_FIXED_FONT);

	GetDlgItem(IDC_EDIT_RESULT)->SetFont(&m_Font1);
	GetDlgItem(IDC_EDIT_FP_DATA)->SetFont(&m_Font1);
		
	m_cmbConBaudRate.AddString(_T("9600"));
	m_cmbConBaudRate.AddString(_T("19200"));
	m_cmbConBaudRate.AddString(_T("38400"));
	m_cmbConBaudRate.AddString(_T("57600"));
	m_cmbConBaudRate.AddString(_T("115200"));
	m_cmbConBaudRate.AddString(_T("230400"));
	m_cmbConBaudRate.AddString(_T("460800"));
	m_cmbConBaudRate.AddString(_T("921600"));
	m_cmbConComPort.SetCurSel(0);
	m_cmbConBaudRate.SetCurSel(BAUD115200 - 1);

	m_cmbSetBaudrate.AddString(_T("9600"));
	m_cmbSetBaudrate.AddString(_T("19200"));
	m_cmbSetBaudrate.AddString(_T("38400"));
	m_cmbSetBaudrate.AddString(_T("57600"));
	m_cmbSetBaudrate.AddString(_T("115200"));
	m_cmbSetBaudrate.AddString(_T("230400"));
	m_cmbSetBaudrate.AddString(_T("460800"));
	m_cmbSetBaudrate.AddString(_T("921600"));

	m_cmbSetBaudrate.SetCurSel(0);

	m_cmbParamType.AddString(_T("Device ID"));
	m_cmbParamType.AddString(_T("Security Level"));
	m_cmbParamType.AddString(_T("Duplication Check"));
	m_cmbParamType.AddString(_T("Baudrate"));
	m_cmbParamType.AddString(_T("Auto Learn"));
	m_cmbParamType.AddString(_T("Device Password"));
	m_cmbParamType.SetCurSel(0);

	m_cmbUpImgType.AddString(_T("Full"));
	m_cmbUpImgType.AddString(_T("Quarter"));
	m_cmbUpImgType.SetCurSel(0);

	for (i=0; i<sizeof(g_anMaxFpCount) / sizeof(int); i++)
	{
		w_strTmp.Format(_T("%d"), g_anMaxFpCount[i]);
		m_cmbMaxFpCount.AddString(w_strTmp);
	}	
	m_cmbMaxFpCount.SetCurSel(12);
	g_nMaxFpCount = g_anMaxFpCount[12];

	for (i = 0; i < 10; i ++)
	{
		w_strTmp.Format(_T("%d"), i + 1);
		m_cmbEnrollCount.AddString(w_strTmp);
	}
	m_cmbEnrollCount.SetCurSel(2);

	GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);
	EnableControl(FALSE);	

	GetDlgItem(IDC_CMB_SET_BAUDRATE)->ShowWindow(FALSE);
	GetDlgItem(IDC_EDIT1)->ShowWindow(TRUE);

	UpdateData(FALSE);
}
/***************************************************************************/
/***************************************************************************/
void COEMHostDlg::EnableControl(BOOL p_bEnable)
{
	GetDlgItem(IDC_BTN_SET_PARAM)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_GET_PARAM)->EnableWindow(p_bEnable);

	GetDlgItem(IDC_BTN_ENROLL)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_VERIFY)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_IDENTIFY)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_IDENTIFY_TEMPLATE)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_IDENTIFY_IMAGE)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_DETECT_FINGER)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_GET_USER_COUNT)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_DELETE_ID)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_DELETE_ALL)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_GET_EMPTY_ID)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_GET_STATUS)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_GET_BROKEN_ID)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_UP_TEMPLATE)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_DOWN_TEMPLATE)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_DOWN_MULTI_TMPL)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_GET_DEVICE_INFO)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_UP_IMAGE)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_VERIFY_WITH_DOWN_TMPL)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_ENROLL_FROM_IMAGE)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_SET_MODULE_SN)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_GET_MODULE_SN)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_OPN_FINGER_IMAGE)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_SAVE_FINGER_IMAGE)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_CHK_IMG_STRETCH)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_OPEN_TEMPLATE)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_SAVE_TEMPLATE)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_GET_ENROLLED_ID_LIST)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_ENTER_STANDBY)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_ADJUST_SENSOR)->EnableWindow(p_bEnable);	
	GetDlgItem(IDC_CHK_SHOW_IMAGE)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BUTTON_UPGRADE)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_UP_MULTI_TMPL)->EnableWindow(p_bEnable);
// 	GetDlgItem(IDC_CHK_ADVANCED_LED)->EnableWindow(p_bEnable);
//	GetDlgItem(IDC_CHK_SECURE_COMM)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_DTP_DATE)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_DTP_TIME)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_SET_RTC)->EnableWindow(p_bEnable);
	GetDlgItem(IDC_BTN_GET_RTC)->EnableWindow(p_bEnable);
}
/***************************************************************************/
/***************************************************************************/
void COEMHostDlg::SetLanguage(int nLanguageID)
{
	if (nLanguageID == 0)
	{
		GetDlgItem(IDC_BTN_CONNECT)->SetWindowText(IDS_CN_CONNECT);
		GetDlgItem(IDC_BTN_DISCONNECT)->SetWindowText(IDS_CN_DISCONNECT);
		GetDlgItem(IDC_BTN_SET_PARAM)->SetWindowText(IDS_CN_SET_PARAM);
		GetDlgItem(IDC_BTN_GET_PARAM)->SetWindowText(IDS_CN_GET_PARAM);
		GetDlgItem(IDC_BTN_ENROLL)->SetWindowText(IDS_CN_ENROLL);
		GetDlgItem(IDC_BTN_VERIFY)->SetWindowText(IDS_CN_VERIFY);
		GetDlgItem(IDC_BTN_IDENTIFY)->SetWindowText(IDS_CN_IDENTIFY);
		GetDlgItem(IDC_BTN_IDENTIFY_TEMPLATE)->SetWindowText(IDS_CN_IDENTIFY_TEMPLATE);
		GetDlgItem(IDC_BTN_IDENTIFY_IMAGE)->SetWindowText(IDS_CN_IDENTIFY_IMAGE);
		GetDlgItem(IDC_BTN_DETECT_FINGER)->SetWindowText(IDS_CN_DETECT_FINGER);
		GetDlgItem(IDC_BTN_GET_USER_COUNT)->SetWindowText(IDS_CN_GET_USER_COUNT);
		GetDlgItem(IDC_BTN_DELETE_ID)->SetWindowText(IDS_CN_DELETE_ID);
		GetDlgItem(IDC_BTN_DELETE_ALL)->SetWindowText(IDS_CN_DELETE_ALL);
		GetDlgItem(IDC_BTN_GET_EMPTY_ID)->SetWindowText(IDS_CN_GET_EMPTY_ID);
		GetDlgItem(IDC_BTN_GET_STATUS)->SetWindowText(IDS_CN_GET_STATUS);
		GetDlgItem(IDC_BTN_GET_BROKEN_ID)->SetWindowText(IDS_CN_GET_BROKEN_ID);
		GetDlgItem(IDC_BTN_UP_TEMPLATE)->SetWindowText(IDS_CN_UP_TEMPLATE);
		GetDlgItem(IDC_BTN_DOWN_TEMPLATE)->SetWindowText(IDS_CN_DOWN_TEMPLATE);
		GetDlgItem(IDC_BTN_DOWN_MULTI_TMPL)->SetWindowText(IDS_CN_DOWN_MULTI_TEMPLATE);
		GetDlgItem(IDC_BTN_GET_DEVICE_INFO)->SetWindowText(IDS_CN_GET_DEVICE_INFO);
		GetDlgItem(IDC_BTN_UP_IMAGE)->SetWindowText(IDS_CN_UP_IMAGE);
		GetDlgItem(IDC_BTN_VERIFY_WITH_DOWN_TMPL)->SetWindowText(IDS_CN_VERIFY_WITH_DOWN_TMPL);
		GetDlgItem(IDC_BTN_STOP)->SetWindowText(IDS_CN_STOP);
		GetDlgItem(IDC_BTN_SET_MODULE_SN)->SetWindowText(IDS_CN_SET_MODULE_SN);
		GetDlgItem(IDC_BTN_GET_MODULE_SN)->SetWindowText(IDS_CN_GET_MODULE_SN);
		GetDlgItem(IDC_BTN_OPN_FINGER_IMAGE)->SetWindowText(IDS_CN_OPEN);
		GetDlgItem(IDC_BTN_SAVE_FINGER_IMAGE)->SetWindowText(IDS_CN_SAVE);
		GetDlgItem(IDC_CHK_IMG_STRETCH)->SetWindowText(IDS_CN_STRETCH);
		GetDlgItem(IDC_BTN_OPEN_TEMPLATE)->SetWindowText(IDS_CN_OPEN);
		GetDlgItem(IDC_BTN_SAVE_TEMPLATE)->SetWindowText(IDS_CN_SAVE);
		GetDlgItem(IDC_BTN_ENROLL_FROM_IMAGE)->SetWindowText(IDS_CN_ENROLL_FROM_IMAGE);
		GetDlgItem(IDC_BTN_GET_ENROLLED_ID_LIST)->SetWindowText(IDS_CN_GET_ENROLLED_ID_LIST);
		GetDlgItem(IDC_BTN_ENTER_STANDBY)->SetWindowText(IDS_CN_ENTER_STANDBY);
		GetDlgItem(IDC_BTN_ADJUST_SENSOR)->SetWindowText(IDS_CN_ADJUST_SENSOR);	

		GetDlgItem(IDC_LB_PARAM_VALUE)->SetWindowText(IDS_CN_PARAM_VALUE);
		GetDlgItem(IDC_LB_PARAM_TYPE)->SetWindowText(IDS_CN_PARAM_TYPE);
		GetDlgItem(IDC_GROUP_RECEIVED_RESULT)->SetWindowText(IDS_CN_RECEIVED_RESULT);
		GetDlgItem(IDC_GROUP_FINGER_IMAGE)->SetWindowText(IDS_CN_FINGER_IMAGE);
		GetDlgItem(IDC_GROUP_TEMPLATE)->SetWindowText(IDS_CN_TEMPLATE);
		GetDlgItem(IDC_GROUP_COMM_INFO)->SetWindowText(IDS_CN_COMM_INFO);
		GetDlgItem(IDC_GROUP_CON_MODE)->SetWindowText(IDS_CN_CON_MODE);
		GetDlgItem(IDC_LB_COMPORT)->SetWindowText(IDS_CN_COMPORT);
		GetDlgItem(IDC_LB_BAUDRATE)->SetWindowText(IDS_CN_BAUDRATE);
		GetDlgItem(IDC_LB_DST_DEVICE_ID)->SetWindowText(IDS_CN_DST_DEVICE_ID);
		GetDlgItem(IDC_LB_FP_COUNT)->SetWindowText(IDS_CN_FP_COUNT);
		GetDlgItem(IDC_GROUP_LANGUAGE)->SetWindowText(IDS_CN_LANGUAGE);		
		GetDlgItem(IDC_LB_IMAGE_TYPE)->SetWindowText(IDS_CN_IMAGE_TYPE);
		GetDlgItem(IDC_CHK_SHOW_IMAGE)->SetWindowText(IDS_CN_SHOW_IMAGE);
		GetDlgItem(IDC_LB_ENROLL_COUNT)->SetWindowText(IDS_CN_ENROLL_COUNT);
		GetDlgItem(IDC_BUTTON_UPGRADE)->SetWindowText(IDS_CN_UPGRADE_FIRMWARE);
		GetDlgItem(IDC_BTN_UP_MULTI_TMPL)->SetWindowText(IDS_CN_UP_MULTI_TEMPLATE);
		GetDlgItem(IDC_CHK_ADVANCED_LED)->SetWindowText(IDS_CN_ADVANCED_LED);
		GetDlgItem(IDC_CHK_SECURE_COMM)->SetWindowText(IDS_CN_SECURE_COMM);
		GetDlgItem(IDC_GROUP_RTC)->SetWindowText(IDS_CN_GROUP_RTC);
		GetDlgItem(IDC_BTN_SET_RTC)->SetWindowText(IDS_CN_SET_RTC);
		GetDlgItem(IDC_BTN_GET_RTC)->SetWindowText(IDS_CN_GET_RTC);

		if (m_cmbParamType.GetCurSel() == DP_DEV_PASS)
		{
			GetDlgItem(IDC_BTN_SET_PARAM)->SetWindowText(IDS_CN_SET_PARAM_PASS);
			GetDlgItem(IDC_BTN_GET_PARAM)->SetWindowText(IDS_CN_VERIFY_PARAM_PASS);
		}
		else
		{
			GetDlgItem(IDC_BTN_SET_PARAM)->SetWindowText(IDS_CN_SET_PARAM);
			GetDlgItem(IDC_BTN_GET_PARAM)->SetWindowText(IDS_CN_GET_PARAM);
		}

		g_bLangEng = FALSE;
	}
	else
	{
		GetDlgItem(IDC_BTN_CONNECT)->SetWindowText(IDS_EN_CONNECT);
		GetDlgItem(IDC_BTN_DISCONNECT)->SetWindowText(IDS_EN_DISCONNECT);
		GetDlgItem(IDC_BTN_SET_PARAM)->SetWindowText(IDS_EN_SET_PARAM);
		GetDlgItem(IDC_BTN_GET_PARAM)->SetWindowText(IDS_EN_GET_PARAM);
		GetDlgItem(IDC_BTN_ENROLL)->SetWindowText(IDS_EN_ENROLL);
		GetDlgItem(IDC_BTN_VERIFY)->SetWindowText(IDS_EN_VERIFY);
		GetDlgItem(IDC_BTN_IDENTIFY)->SetWindowText(IDS_EN_IDENTIFY);
		GetDlgItem(IDC_BTN_IDENTIFY_TEMPLATE)->SetWindowText(IDS_EN_IDENTIFY_TEMPLATE);
		GetDlgItem(IDC_BTN_IDENTIFY_IMAGE)->SetWindowText(IDS_EN_IDENTIFY_IMAGE);
		GetDlgItem(IDC_BTN_DETECT_FINGER)->SetWindowText(IDS_EN_DETECT_FINGER);
		GetDlgItem(IDC_BTN_GET_USER_COUNT)->SetWindowText(IDS_EN_GET_USER_COUNT);
		GetDlgItem(IDC_BTN_DELETE_ID)->SetWindowText(IDS_EN_DELETE_ID);
		GetDlgItem(IDC_BTN_DELETE_ALL)->SetWindowText(IDS_EN_DELETE_ALL);
		GetDlgItem(IDC_BTN_GET_EMPTY_ID)->SetWindowText(IDS_EN_GET_EMPTY_ID);
		GetDlgItem(IDC_BTN_GET_STATUS)->SetWindowText(IDS_EN_GET_STATUS);
		GetDlgItem(IDC_BTN_GET_BROKEN_ID)->SetWindowText(IDS_EN_GET_BROKEN_ID);
		GetDlgItem(IDC_BTN_UP_TEMPLATE)->SetWindowText(IDS_EN_UP_TEMPLATE);
		GetDlgItem(IDC_BTN_DOWN_TEMPLATE)->SetWindowText(IDS_EN_DOWN_TEMPLATE);
		GetDlgItem(IDC_BTN_DOWN_MULTI_TMPL)->SetWindowText(IDS_EN_DOWN_MULTI_TEMPLATE);
		GetDlgItem(IDC_BTN_GET_DEVICE_INFO)->SetWindowText(IDS_EN_GET_DEVICE_INFO);
		GetDlgItem(IDC_BTN_UP_IMAGE)->SetWindowText(IDS_EN_UP_IMAGE);
		GetDlgItem(IDC_BTN_VERIFY_WITH_DOWN_TMPL)->SetWindowText(IDS_EN_VERIFY_WITH_DOWN_TMPL);
		GetDlgItem(IDC_BTN_STOP)->SetWindowText(IDS_EN_STOP);
		GetDlgItem(IDC_BTN_SET_MODULE_SN)->SetWindowText(IDS_EN_SET_MODULE_SN);
		GetDlgItem(IDC_BTN_GET_MODULE_SN)->SetWindowText(IDS_EN_GET_MODULE_SN);
		GetDlgItem(IDC_BTN_OPN_FINGER_IMAGE)->SetWindowText(IDS_EN_OPEN);
		GetDlgItem(IDC_BTN_SAVE_FINGER_IMAGE)->SetWindowText(IDS_EN_SAVE);
		GetDlgItem(IDC_CHK_IMG_STRETCH)->SetWindowText(IDS_EN_STRETCH);
		GetDlgItem(IDC_BTN_OPEN_TEMPLATE)->SetWindowText(IDS_EN_OPEN);
		GetDlgItem(IDC_BTN_SAVE_TEMPLATE)->SetWindowText(IDS_EN_SAVE);
		GetDlgItem(IDC_BTN_ENROLL_FROM_IMAGE)->SetWindowText(IDS_EN_ENROLL_FROM_IMAGE);
		GetDlgItem(IDC_BTN_GET_ENROLLED_ID_LIST)->SetWindowText(IDS_EN_GET_ENROLLED_ID_LIST);
		GetDlgItem(IDC_BTN_ENTER_STANDBY)->SetWindowText(IDS_EN_ENTER_STANDBY);
		GetDlgItem(IDC_BTN_ADJUST_SENSOR)->SetWindowText(IDS_EN_ADJUST_SENSOR);		

		GetDlgItem(IDC_LB_PARAM_VALUE)->SetWindowText(IDS_EN_PARAM_VALUE);
		GetDlgItem(IDC_LB_PARAM_TYPE)->SetWindowText(IDS_EN_PARAM_TYPE);
		GetDlgItem(IDC_GROUP_RECEIVED_RESULT)->SetWindowText(IDS_EN_RECEIVED_RESULT);
		GetDlgItem(IDC_GROUP_FINGER_IMAGE)->SetWindowText(IDS_EN_FINGER_IMAGE);
		GetDlgItem(IDC_GROUP_TEMPLATE)->SetWindowText(IDS_EN_TEMPLATE);
		GetDlgItem(IDC_GROUP_COMM_INFO)->SetWindowText(IDS_EN_COMM_INFO);
		GetDlgItem(IDC_GROUP_CON_MODE)->SetWindowText(IDS_EN_CON_MODE);
		GetDlgItem(IDC_LB_COMPORT)->SetWindowText(IDS_EN_COMPORT);
		GetDlgItem(IDC_LB_BAUDRATE)->SetWindowText(IDS_EN_BAUDRATE);
		GetDlgItem(IDC_LB_DST_DEVICE_ID)->SetWindowText(IDS_EN_DST_DEVICE_ID);
		GetDlgItem(IDC_LB_FP_COUNT)->SetWindowText(IDS_EN_FP_COUNT);
		GetDlgItem(IDC_GROUP_LANGUAGE)->SetWindowText(IDS_EN_LANGUAGE);
		GetDlgItem(IDC_LB_IMAGE_TYPE)->SetWindowText(IDS_EN_IMAGE_TYPE);
		GetDlgItem(IDC_CHK_SHOW_IMAGE)->SetWindowText(IDS_EN_SHOW_IMAGE);
		GetDlgItem(IDC_LB_ENROLL_COUNT)->SetWindowText(IDS_EN_ENROLL_COUNT);
		GetDlgItem(IDC_BUTTON_UPGRADE)->SetWindowText(IDS_EN_UPGRADE_FIRMWARE);
		GetDlgItem(IDC_BTN_UP_MULTI_TMPL)->SetWindowText(IDS_EN_UP_MULTI_TEMPLATE);
		GetDlgItem(IDC_CHK_ADVANCED_LED)->SetWindowText(IDS_EN_ADVANCED_LED);
		GetDlgItem(IDC_CHK_SECURE_COMM)->SetWindowText(IDS_EN_SECURE_COMM);
		GetDlgItem(IDC_GROUP_RTC)->SetWindowText(IDS_EN_GROUP_RTC);
		GetDlgItem(IDC_BTN_SET_RTC)->SetWindowText(IDS_EN_SET_RTC);
		GetDlgItem(IDC_BTN_GET_RTC)->SetWindowText(IDS_EN_GET_RTC);

		if (m_cmbParamType.GetCurSel() == DP_DEV_PASS)
		{
			GetDlgItem(IDC_BTN_SET_PARAM)->SetWindowText(IDS_EN_SET_PARAM_PASS);
			GetDlgItem(IDC_BTN_GET_PARAM)->SetWindowText(IDS_EN_VERIFY_PARAM_PASS);
		}
		else
		{
			GetDlgItem(IDC_BTN_SET_PARAM)->SetWindowText(IDS_EN_SET_PARAM);
			GetDlgItem(IDC_BTN_GET_PARAM)->SetWindowText(IDS_EN_GET_PARAM);
		}

		g_bLangEng = TRUE;
	}
}
/***************************************************************************/
/***************************************************************************/
CString COEMHostDlg::ConvertByteToPacketFormat(BYTE* p_pBuff, int p_nSize, int p_nUnit)
{
	CString w_strTemp, w_strData;
	
	for( int i = 0; i< p_nSize; i++ )
	{		
		w_strTemp.Format(_T("0x%02X "), p_pBuff[i]);
		w_strData += w_strTemp;
		
		if ( (i+1) % p_nUnit == 0)
		{
			w_strData = w_strData + _T("\r\n");
		}
	}
	return w_strData;
}
/***************************************************************************/
/***************************************************************************/
CString COEMHostDlg::ConvertByteToHex( BYTE* pBuff, int nSize, int nWidthSize )
{
	CString w_strTemp, w_strData;
	
	for( int i = 0; i< nSize; i++ ){
		if(!(i%nWidthSize) && i){
			w_strData += _T( "\r\n" );
		}
		w_strTemp.Format(_T("%02X "), pBuff[i]);
		w_strData += w_strTemp;
	}
	return w_strData;
}
/***************************************************************************/
/***************************************************************************/
void COEMHostDlg::DoLedCtrl(unsigned char p_nCtrlCode)
{
	unsigned char w_nColor = 0;

	if (m_bAdvLed)	// Advanced
	{
		switch (p_nCtrlCode)
		{
		case E_LED_CTRL_CONNECT:
			{
				// Blue Led Breath
				w_nColor = 0x80 + (1 << E_LED_COLOR_OK);
				m_clsCommu.Run_SLEDControl(LED_VAL(w_nColor, E_LED_STATUS_BREATH));
				break;
			}
		case E_LED_CTRL_DISCONNECT:
			{
				// All Led Off
				w_nColor = 0x80 + (1 << E_LED_COLOR_OK) + (1 << E_LED_COLOR_NG);
				m_clsCommu.Run_SLEDControl(LED_VAL(w_nColor, E_LED_STATUS_OFF));
				break;
			}
		case E_LED_CTRL_WAIT:
			{
				// Blue Led Breath
				w_nColor = 0x80 + (1 << E_LED_COLOR_OK);
				m_clsCommu.Run_SLEDControl(LED_VAL(w_nColor, E_LED_STATUS_BREATH));
				break;
			}
		case E_LED_CTRL_CAPTURE:
			{
				// Blue Led Blink Slow
				w_nColor = 0x80 + (1 << E_LED_COLOR_OK);
				m_clsCommu.Run_SLEDControl(LED_VAL(w_nColor, E_LED_STATUS_BLINK_SLOW));
				break;
			}
		case E_LED_CTRL_DETECTED:
			{
				// All Led Off
				w_nColor = 0x80 + (1 << E_LED_COLOR_OK) + (1 << E_LED_COLOR_NG);
				m_clsCommu.Run_SLEDControl(LED_VAL(w_nColor, E_LED_STATUS_OFF));
				break;
			}
		case E_LED_CTRL_IDENTIFY_OK:
			{
				UpdateData(FALSE);
				DoEvents();

				// Blue Led On 1s
				w_nColor = 0x80 + (1 << E_LED_COLOR_OK);
				m_clsCommu.Run_SLEDControl(LED_VAL(w_nColor, E_LED_STATUS_ON));
				Sleep(1000);
				m_clsCommu.Run_SLEDControl(LED_VAL(w_nColor, E_LED_STATUS_OFF));
				break;
			}
		case E_LED_CTRL_IDENTIFY_NG:
			{
				UpdateData(FALSE);
				DoEvents();

				// Red Led On 1s
				w_nColor = 0x80 + (1 << E_LED_COLOR_NG);
				m_clsCommu.Run_SLEDControl(LED_VAL(w_nColor, E_LED_STATUS_ON));
				Sleep(1000);
				m_clsCommu.Run_SLEDControl(LED_VAL(w_nColor, E_LED_STATUS_OFF));
				break;
			}
		default:
			{
				break;
			}
		}
	}
	else	// Original
	{
		switch (p_nCtrlCode)
		{
		case E_LED_CTRL_WAIT:
			{
				// Led Off
				m_clsCommu.Run_SLEDControl(0);
				break;
			}
		case E_LED_CTRL_CAPTURE:
			{
				// Led On
				m_clsCommu.Run_SLEDControl(1);
				break;
			}
		case E_LED_CTRL_DETECTED:
			{
				// Led Off
				m_clsCommu.Run_SLEDControl(0);
				break;
			}
		case E_LED_CTRL_CONNECT:
		case E_LED_CTRL_DISCONNECT:
		case E_LED_CTRL_IDENTIFY_OK:
		case E_LED_CTRL_IDENTIFY_NG:
		default:
			{
				break;
			}
		}
	}
}
/***************************************************************************/
/***************************************************************************/
void COEMHostDlg::OnBtnConnect()
{
	int		w_nComPortIndex, w_nBaudRateIndex=0, w_nCommMode;
	CString	w_strComPort;
	CString	w_strMsg;
	int		w_nRet;
	
	UpdateData(TRUE);
	
	if (m_nConMode == 0)
	{
		w_nComPortIndex = m_cmbConComPort.GetCurSel();
		w_nBaudRateIndex = m_cmbConBaudRate.GetCurSel();
		m_cmbConComPort.GetLBText(w_nComPortIndex, w_strComPort);
	}

	if (m_nConMode == 0)
		w_nCommMode = SERIAL_CON_MODE;
	else if (m_nConMode == 1)
		w_nCommMode = USB_CON_MODE;

	// Check Device ID
	if (m_nConDeviceID < 0 || m_nConDeviceID > 255)
	{
		AfxMessageBox(_T("Please input correct device id(%d~%d)"), 0, MAX_DEVICE_ID);
		return;
	}

	//. Init Comport
	if( m_clsCommu.InitConnection(	
		w_nCommMode, 
		w_strComPort, 
		w_nBaudRateIndex, 
		0,
		m_nConDeviceID) != CONNECTION_SUCCESS)
		return;
	
	m_clsCommu.SetCallbackWnd(m_hWnd);

	//. Test Connection with device
	if( m_clsCommu.Run_TestConnection() != ERR_SUCCESS)
	{
		AfxMessageBox(_T("Fail in communicating with the device !"));
		OnBtnDisconnect();
		return;
	}

	// run security key exchange
	if (m_bSecureComm)
	{
		int w_nKeySize = 0;

		SetCryptState(0);

		do
		{
			// generate key
			memset(g_pCryptN, 0x00, CRYPT_DEF_RSA_BYTE);
			memset(g_pCryptE, 0x00, CRYPT_DEF_RSA_BYTE);
			memset(g_pCryptD, 0x00, CRYPT_DEF_RSA_BYTE);
			memset(g_pCryptRemoteN, 0x00, CRYPT_DEF_RSA_BYTE);
			memset(g_pCryptRemoteE, 0x00, CRYPT_DEF_RSA_BYTE);
			w_nRet = crt_user_RSAKeyGen(g_pCryptN, g_pCryptE, g_pCryptD);
			if (w_nRet != CRT_SUCCESS)
			{
				m_bSecureComm = FALSE;
				break;
			}

			// send host key to device
			if (m_clsCommu.Run_SetHostRSAPubKey(g_pCryptN, g_pCryptE, CRYPT_DEF_RSA_BYTE) != ERR_SUCCESS)
			{
				m_bSecureComm = FALSE;
				break;
			}

			// get device key
			if (m_clsCommu.Run_GetOEMRSAPubKey(g_pCryptRemoteN, g_pCryptRemoteE, &w_nKeySize) != ERR_SUCCESS)
			{
				m_bSecureComm = FALSE;
				break;
			}

			// success
			SetCryptState(1);
		} while (FALSE);
	}

	DoLedCtrl(E_LED_CTRL_CONNECT);
	
	//. Enable Control
	EnableControl(TRUE);
	GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_CMB_COMPORT)->EnableWindow(FALSE);
	GetDlgItem(IDC_CMB_BAUDRATE)->EnableWindow(FALSE);

	GetDlgItem(IDC_CHK_SECURE_COMM)->EnableWindow(FALSE);
	
	UpdateData(FALSE);
}

void COEMHostDlg::OnBtnDisconnect()
{
	OnBtnStop();

	DoLedCtrl(E_LED_CTRL_DISCONNECT);
	
	//m_nListType = 0;
	m_bTemplate1Exist = FALSE;
	m_bFpImageExist = FALSE;
	m_clsCommu.CloseConnection();
	m_wndFinger.ClearFpImage();
	m_strCmdResult.Empty();
	m_strFpData.Empty();

	EnableControl(FALSE);	
	GetDlgItem(IDC_BTN_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);
	
	GetDlgItem(IDC_CMB_COMPORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_CMB_BAUDRATE)->EnableWindow(TRUE);

	// success
	SetCryptState(0);
	GetDlgItem(IDC_CHK_SECURE_COMM)->EnableWindow(TRUE);

	UpdateData(FALSE);
}

int     
SaveBMP8(
		 const char*		filename, 
		 int				p_nWidth,
		 int				p_nHeight, 
		 unsigned char*	p_pbImageData
		 )
{	
	unsigned char head[1078]={
		/***************************/
		//file header
		0x42,0x4d,//file type 
			//0x36,0x6c,0x01,0x00, //file size***
			0x0,0x0,0x0,0x00, //file size***
			0x00,0x00, //reserved
			0x00,0x00,//reserved
			0x36,0x4,0x00,0x00,//head byte***
			/***************************/
			//infoheader
			0x28,0x00,0x00,0x00,//struct size
			
			//0x00,0x01,0x00,0x00,//map width*** 
			0x00,0x00,0x0,0x00,//map width*** 
			//0x68,0x01,0x00,0x00,//map height***
			0x00,0x00,0x00,0x00,//map height***
			
			0x01,0x00,//must be 1
			0x08,0x00,//color count***
			0x00,0x00,0x00,0x00, //compression
			//0x00,0x68,0x01,0x00,//data size***
			0x00,0x00,0x00,0x00,//data size***
			0x00,0x00,0x00,0x00, //dpix
			0x00,0x00,0x00,0x00, //dpiy
			0x00,0x00,0x00,0x00,//color used
			0x00,0x00,0x00,0x00,//color important
	};	
	FILE	*fh;
	int		i,j, iImageStep;
	long	num;
	unsigned char	*p1;
	unsigned char	w_bTemp[4];
	
	if (p_nWidth & 0x03){
		iImageStep = p_nWidth + (4 - (p_nWidth & 0x03));
	}
	else{
		iImageStep = p_nWidth;
	}
	
	num=p_nWidth; head[18]= num & 0xFF;
	num=num>>8;  head[19]= num & 0xFF;
	num=num>>8;  head[20]= num & 0xFF;
	num=num>>8;  head[21]= num & 0xFF;
	
	num=p_nHeight; head[22]= num & 0xFF;
	num=num>>8;  head[23]= num & 0xFF;
	num=num>>8;  head[24]= num & 0xFF;
	num=num>>8;  head[25]= num & 0xFF;
	
	j=0;
	for (i=54;i<1078;i=i+4)
	{
		head[i]=head[i+1]=head[i+2]=j; 
		head[i+3]=0;
		j++;
	}
	
	memset( w_bTemp, 0, sizeof(w_bTemp) );
	
	if((fopen_s(&fh, filename, "wb")) != 0)
		return 0;
	
    fwrite(head,sizeof(char),1078,fh);
	
	if (iImageStep == p_nWidth){
		p1 = p_pbImageData + (p_nHeight - 1) * p_nWidth;
		for( i = 0; i < p_nHeight; i ++){	
			fwrite( p1, 1, p_nWidth, fh );
			p1 -= p_nWidth;
		}	
	}
	else{
		iImageStep -= p_nWidth;
		p1 = p_pbImageData + (p_nHeight - 1) * p_nWidth;
		for( i = 0; i < p_nHeight; i ++){	
			fwrite( p1, 1, p_nWidth, fh );
			fwrite( w_bTemp, 1, iImageStep, fh );
			p1 -= p_nWidth;
		}
	}
	fclose(fh);
	
	return 1;
}
void COEMHostDlg::OnBtnSaveFingerImage() 
{
	char		w_szFileName[MAX_PATH];
	CFileDialog dlg(FALSE, _T("Dlg"), NULL, OFN_HIDEREADONLY , _T("Image File(*.bmp)|*.bmp|"));
	
	if ( dlg.DoModal() == IDOK )
	{
		if ( !dlg.GetPathName().IsEmpty() )	
		{
			memset(w_szFileName, 0, MAX_PATH);
			
			::WideCharToMultiByte(CP_ACP, 0, dlg.GetPathName().GetBuffer(0), -1, w_szFileName, MAX_PATH, NULL, NULL);
			
			SaveBMP8(w_szFileName, g_nImageWidth, g_nImageHeight, g_FpImageBuf);
			AfxMessageBox(_T("Save Success"));
		}
	}	
}

void COEMHostDlg::OnBtnOpenTemplate() 
{
	CFileDialog	w_pDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Template File(*.fpt)|*.fpt|"));
	
	UpdateData(TRUE);

	if (w_pDlg.DoModal() == IDOK )
	{
		if (!w_pDlg.GetPathName().IsEmpty())	
		{
			CFile	w_clsFile;
			
			if (w_clsFile.Open(w_pDlg.GetPathName(), CFile::modeRead | CFile::typeBinary, NULL))
			{
				m_nTemplateSize = (unsigned int)w_clsFile.GetLength();
				w_clsFile.Read(m_abyTemplate1, m_nTemplateSize);
				w_clsFile.Close();
				
				m_strFpData = ConvertByteToHex(m_abyTemplate1, m_nTemplateSize, 8);
				
				m_bTemplate1Exist = TRUE;
				
				UpdateData(FALSE);
			}
			else
			{
				AfxMessageBox(_T("Error opening template file!"));
			}
		}
	}
}

void COEMHostDlg::OnBtnSaveTemplate() 
{
	if (!m_bTemplate1Exist)
	{
		AfxMessageBox(_T("template is not exist!"));
		return;
	}
	
	CFile		w_clsFile;
	CString		w_strFileName;
	CFileDialog	w_pDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, _T("Template File(*.fpt)|*.fpt|"));
	
	if (w_pDlg.DoModal() == IDOK )
	{
		if (!w_pDlg.GetPathName().IsEmpty())	
		{			
			w_strFileName = w_pDlg.GetPathName();
			
			if (w_strFileName.Find(_T(".fpt")) < 0)
			{
				w_strFileName = w_strFileName + _T(".fpt");
			}
			
			if (w_clsFile.Open(w_strFileName, CFile::modeReadWrite | CFile::modeCreate | CFile::typeBinary, NULL))
			{
				w_clsFile.Write(m_abyTemplate1, m_nTemplateSize);
				w_clsFile.Close();
				
				AfxMessageBox(_T("Success!"));
			}
			else
			{
				AfxMessageBox(_T("Error creating template file!"));
			}
		}
	}	
}

void COEMHostDlg::OnBtnStop() 
{
	m_bCancel = TRUE;	
}

LRESULT COEMHostDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	CString	w_strTmp;

	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case WM_UP_IMAGE_PROGRESS:		
		w_strTmp.Format(_T("%d%%..."), wParam);
		//m_strCmdResult = m_strCmdResult + w_strTmp;		
		if (lParam == 0)
			m_strCmdResult = _T("Downloading image...\r\n") + w_strTmp;
		else if(lParam == 1)
			m_strCmdResult = _T("Uploading image...\r\n") + w_strTmp;
		else if(lParam == 2)
			m_strCmdResult = _T("Downloading firmware...\r\n") + w_strTmp;
		else
			m_strCmdResult = _T("Downloading ...\r\n") + w_strTmp;
		UpdateData(FALSE);
		DoEvents();
		break;
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}

void COEMHostDlg::OnBtnOpenFile1() 
{

}
#define MAX_OPEN_FP_DATA_COUNT		7
void COEMHostDlg::OnBtnOpenFile2() 
{

}

#define	DEFAULT_IMG_WIDTH			242
#define DEFAULT_IMG_HEIGHT			266
void COEMHostDlg::OnBtnOpnFingerImage() 
{
	int			w_nWidth, w_nHeight;
	char		w_szTemp[256];
	BYTE*		w_pBuf;
	CString strFile;
	CString sFilter;

	UpdateData(TRUE);
	
    sFilter= _T("BMP File(*.bmp)|*.bmp|RAW File(*.raw)|*.raw|All File(*.*)|*.*||");
	CFileDialog fo(TRUE, _T("bmp"), _T("Finger.bmp"),OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFilter,NULL);
	
	if (fo.DoModal() != IDOK)
		return;
	
	strFile = fo.GetPathName();

	if (fo.GetFileExt().Compare(_T("raw")) == 0)
	{
		HANDLE w_hFile = NULL;
		DWORD w_nSize = 0;
		DWORD w_nReadLen = 0;
		unsigned char *w_pTmpBuf;
		unsigned int w_nCropX = 0;
		unsigned int w_nCropY = 0;
		unsigned int w_nCropW = 0;
		unsigned int w_nCropH = 0;
		unsigned int w_nGapW = 0;
		unsigned int w_nGapH = 0;
		unsigned int i;

		// open file
		w_hFile = CreateFile(strFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if ((w_hFile == INVALID_HANDLE_VALUE) || (w_hFile == NULL))
		{
			MessageBox(_T("Can't open image file. Please reopen it."), _T("Load Image"), MB_ICONEXCLAMATION | MB_OK);
			return;
		}

		// get file size
		w_nSize = GetFileSize(w_hFile, &w_nReadLen);

		// check image size
		if (w_nSize != m_nLogImgWidth * m_nLogImgHeight)
		{
			MessageBox(_T("Image size is not correct. Reset the image size."), _T("Load Image"), MB_ICONEXCLAMATION | MB_OK);
			CloseHandle(w_hFile);
			return;
		}

		// allocate memory
		w_pTmpBuf = new BYTE[w_nSize];
		w_pBuf = new BYTE[DEFAULT_IMG_WIDTH * DEFAULT_IMG_HEIGHT];

		// read file
		ReadFile(w_hFile, w_pTmpBuf, w_nSize, &w_nReadLen, NULL);

		// check crop option
		if (m_nLogImgWidth >= DEFAULT_IMG_WIDTH)
		{
			w_nCropX = (m_nLogImgWidth - DEFAULT_IMG_WIDTH) / 2;
			w_nCropW = DEFAULT_IMG_WIDTH;
			w_nGapW = 0;
		}
		else
		{
			w_nCropX = 0;
			w_nCropW = m_nLogImgWidth;
			w_nGapW = DEFAULT_IMG_WIDTH - m_nLogImgWidth;
		}
		if (m_nLogImgHeight >= DEFAULT_IMG_HEIGHT)
		{
			w_nCropY = (m_nLogImgHeight - DEFAULT_IMG_HEIGHT) / 2;
			w_nCropH = DEFAULT_IMG_HEIGHT;
			w_nGapH = 0;
		}
		else
		{
			w_nCropY = 0;
			w_nCropH = m_nLogImgHeight;
			w_nGapH = DEFAULT_IMG_HEIGHT - m_nLogImgHeight;
		}

		// resize image
		for (i = 0; i < w_nCropH; i++)
		{
			memcpy(&w_pBuf[i * DEFAULT_IMG_WIDTH], &w_pTmpBuf[(i + w_nCropY) * m_nLogImgWidth + w_nCropX], w_nCropW);
			if (w_nGapW > 0)
				memset(&w_pBuf[i * DEFAULT_IMG_WIDTH + w_nCropW], 0xFF, w_nGapW);
		}
		if (w_nGapH > 0)
			memset(&w_pBuf[i * DEFAULT_IMG_WIDTH], 0xFF, w_nGapH * DEFAULT_IMG_WIDTH);

		// free memory
		delete[] w_pTmpBuf;

		w_nWidth = DEFAULT_IMG_WIDTH;
		w_nHeight = DEFAULT_IMG_HEIGHT;

		// close file
		CloseHandle(w_hFile);
	}
	else
	{
		memset(w_szTemp, 0, sizeof(w_szTemp));
		::WideCharToMultiByte(CP_ACP, 0, strFile.GetBuffer(0), -1, w_szTemp, 256, NULL, NULL);

		if (FCLoadImage(w_szTemp, &w_pBuf, &w_nWidth, &w_nHeight, 0) != 0)
		{
			AfxMessageBox(_T("Load Fail!!!"));
			return;
		}
	}
	
// 	if (w_nWidth != 202 && w_nWidth != 242 && w_nWidth != 192)
// 	{
// 		AfxMessageBox(_T("Image size is not correct!"));
// 		goto l_exit;
// 	}
// 
// 	if (w_nHeight != 258 && w_nHeight != 266 && w_nHeight != 192)
// 	{
// 		AfxMessageBox(_T("Image size is not correct!"));
// 		goto l_exit;
// 	}
	
	memcpy(g_FpImageBuf, w_pBuf, w_nWidth*w_nHeight);
	
	g_nImageWidth = w_nWidth;
	g_nImageHeight = w_nHeight;
	
	m_bFpImageExist = TRUE;

	m_wndFinger.SetImage(g_FpImageBuf, g_nImageWidth, g_nImageHeight, m_bStretchView);
	
// l_exit:
	
	delete[] w_pBuf;	
}

void COEMHostDlg::OnBtnEnroll() 
{
	int		w_nRet, w_nFPNo, w_nEnrollStep = 0, w_nGenCount, w_nStatus, w_nDupFpNo;
	DWORD	w_dwTime;
	CString w_str, w_strResultMsg;
	int		w_nDetect;
	
	UpdateData(TRUE);

	if (!CheckUserID())
		return;
	
	EnableControl(FALSE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);

	m_bCancel = FALSE;

	w_nFPNo = m_nUserID;
	w_nGenCount = m_cmbEnrollCount.GetCurSel() + 1; //3
	
	//. Check if fp is exist
	w_nRet = m_clsCommu.Run_GetStatus(w_nFPNo, &w_nStatus);
	
	if ( w_nRet != ERR_SUCCESS )
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		goto l_exit;
	}
	
	if( w_nRet == ERR_SUCCESS && w_nStatus == GD_TEMPLATE_NOT_EMPTY )
	{
		m_strCmdResult.Format(_T("Template is already exist"));
		goto l_exit;
	}	
	
	DoLedCtrl(E_LED_CTRL_CAPTURE);

	m_strCmdResult = _T("Input your finger");
	UpdateData(FALSE);

	w_nEnrollStep = 0;
// 	while(w_nEnrollStep < w_nGenCount)
	do
	{
		w_dwTime = GetTickCount();

		//. Get Image
		while (1)
		{
			DoEvents();

			if(m_bCancel)
			{
				m_strCmdResult = _T("Operation canceled");
				goto l_exit;
			}

			w_nRet = m_clsCommu.Run_GetImage();
			
			if(w_nRet == ERR_SUCCESS)
				break;
			
			if(w_nRet == ERR_CONNECTION)
			{
				m_strCmdResult.Format(_T("Error Connection"));
				goto l_exit;
			}			
		}
		
		//. Up Image
		if (m_chkShowImage.GetCheck())
		{
			m_strCmdResult = _T("Uploading image...");
			UpdateData(FALSE);
			DoEvents();

			w_nRet = m_clsCommu.Run_UpImage(0, g_FpImageBuf, &g_nImageWidth, &g_nImageHeight);

			if(w_nRet != ERR_SUCCESS)
			{
				m_strCmdResult = GetErrorMsg(w_nRet);
				goto l_exit;
			}	

			m_wndFinger.SetImage(g_FpImageBuf, g_nImageWidth, g_nImageHeight, m_bStretchView);

			m_strCmdResult = _T("Uploading image is successed.");
			UpdateData(FALSE);
			DoEvents();
		}

		//. Create Template From Captured Image
		w_nRet = m_clsCommu.Run_Generate(w_nEnrollStep);
		
		if (w_nRet != ERR_SUCCESS)
		{
			if (w_nRet == ERR_BAD_QUALITY)
			{
				m_strCmdResult = _T("Bad Quality. Try Again!");
				UpdateData(FALSE);
				DoEvents();
				continue;
			}
			else if (w_nRet == ERR_INPUT_CORRECT_FP)
			{
				m_strCmdResult = _T("Finger rolled too much");
				UpdateData(FALSE);
				DoEvents();
				continue;
			}
			else if (w_nRet == ERR_MOVE_FINGER)
			{
				m_strCmdResult = _T("Press other side of finger");
				UpdateData(FALSE);
				DoEvents();
				continue;
			}
			else if (w_nRet == ERR_INVALID_BUFFER_ID)
			{
				w_nEnrollStep = w_nGenCount;
			}
			else
			{
				m_strCmdResult = GetErrorMsg(w_nRet);
				goto l_exit;
			}
		}

		// Check Finger
		do 
		{
			w_nRet = m_clsCommu.Run_FingerDetect(&w_nDetect);
			if (w_nRet != ERR_SUCCESS)
			{
				m_strCmdResult.Format(_T("Error Connection"));
				goto l_exit;
			}
			if (w_nDetect == 1)
			{
				m_strCmdResult = _T("Release your finger");
				UpdateData(FALSE);
				DoEvents();
			}
		} while (w_nDetect == 1);

		if ((w_nEnrollStep != MAX_ENROLL_COUNT) && (w_nGenCount > w_nEnrollStep + 1))
			m_strCmdResult.Format(_T("Input %d More Finger"), w_nGenCount - w_nEnrollStep - 1);
		else
			m_strCmdResult = _T("Input More Finger");
		UpdateData(FALSE);

		w_nEnrollStep++;
		if (w_nEnrollStep >= w_nGenCount)
		{
			if (w_nGenCount != 1)
			{
				//. Merge Template
				w_nRet = m_clsCommu.Run_Merge(0, w_nGenCount);

				if (w_nRet == ERR_SUCCESS)
				{
					m_strCmdResult = _T("Release Finger");
					UpdateData(FALSE);
					break;
				}
				else if (w_nRet == ERR_LEAK_TEMPLATE_INFO)
				{
					w_nEnrollStep = MAX_ENROLL_COUNT;
					continue;
				}
				else
				{
					m_strCmdResult = GetErrorMsg(w_nRet);
					goto l_exit;
				}
			}
			else
			{
				break;
			}
		}
	} while (1);
	
	//. Store template
	w_nRet = m_clsCommu.Run_StoreChar(w_nFPNo, 0, &w_nDupFpNo);
	
	if( w_nRet != ERR_SUCCESS )
	{
		if(w_nRet == ERR_DUPLICATION_ID)
		{
			m_strCmdResult.Format(_T("Result : Fail\r\nDuplication ID = %u"), w_nDupFpNo);
		}
		else
		{
			m_strCmdResult = GetErrorMsg(w_nRet);
		}
	}
	else
	{
		m_strCmdResult.Format(_T("Result : Success\r\nTemplate No : %d"), w_nFPNo);
	}

l_exit:
	DoLedCtrl(E_LED_CTRL_WAIT);
	UpdateData(FALSE);
	EnableControl(TRUE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(TRUE);	
}

void COEMHostDlg::OnBtnVerify() 
{
	int		w_nRet, w_nTmplNo, w_nFpStatus, w_nLearned;
	DWORD	w_dwTime;
	CString	w_strTmp;

	UpdateData(TRUE);

	if (!CheckUserID())
		return;

	EnableControl(FALSE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
	
	m_bCancel = FALSE;

	w_nTmplNo = m_nUserID;

	//. Check if fp is exist
	w_nRet = m_clsCommu.Run_GetStatus(w_nTmplNo, &w_nFpStatus);
	
	if( w_nRet != ERR_SUCCESS )
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		goto l_exit;
	}
	
	if( w_nRet == ERR_SUCCESS && w_nFpStatus == GD_TEMPLATE_EMPTY )
	{
		m_strCmdResult = _T("Template is empty");
		goto l_exit;
	}
	
	DoLedCtrl(E_LED_CTRL_CAPTURE);

	m_strCmdResult = _T("Input your finger.");
	UpdateData(FALSE);
	
	w_dwTime = GetTickCount();
	
	//. Get Image
	while (1)
	{
		DoEvents();

		if(m_bCancel)
		{
			m_strCmdResult = _T("Operation Canceled");
			goto l_exit;
		}

		//. Get Image
		w_nRet = m_clsCommu.Run_GetImage();
		
		if(w_nRet == ERR_SUCCESS)
			break;
		else if(w_nRet == ERR_CONNECTION)
		{
			m_strCmdResult = GetErrorMsg(w_nRet);
			goto l_exit;
		}					
	}

	DoLedCtrl(E_LED_CTRL_DETECTED);
	
	m_strCmdResult = "Release your finger";
	UpdateData(FALSE);
	DoEvents();

	//. Up Image
	if (m_chkShowImage.GetCheck())
	{
		m_strCmdResult = _T("Uploading image...");
		UpdateData(FALSE);
		DoEvents();

		w_nRet = m_clsCommu.Run_UpImage(0, g_FpImageBuf, &g_nImageWidth, &g_nImageHeight);

		if(w_nRet != ERR_SUCCESS)
		{
			m_strCmdResult = GetErrorMsg(w_nRet);
			goto l_exit;
		}	

		m_wndFinger.SetImage(g_FpImageBuf, g_nImageWidth, g_nImageHeight, m_bStretchView);

		m_strCmdResult = _T("Uploading image is successed.");
		UpdateData(FALSE);
		DoEvents();
	}

	w_dwTime = GetTickCount();

	//. Create Template
	w_nRet = m_clsCommu.Run_Generate(0);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		goto l_exit;
	}

	//. Verify 
	w_nRet = m_clsCommu.Run_Verify(w_nTmplNo, 0, &w_nLearned);
	
	w_dwTime = GetTickCount() - w_dwTime;

	if (w_nRet == ERR_SUCCESS)
	{
		m_strCmdResult.Format(_T("Result : Success\r\nTemplate No : %d, Learn Result : %d\r\nMatch Time : %dms"), w_nTmplNo, w_nLearned, w_dwTime );
		DoLedCtrl(E_LED_CTRL_IDENTIFY_OK);
	}
	else
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		DoLedCtrl(E_LED_CTRL_IDENTIFY_NG);
	}
	
l_exit:
	DoLedCtrl(E_LED_CTRL_WAIT);
	UpdateData(FALSE);
	EnableControl(TRUE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(TRUE);
}

void COEMHostDlg::OnBtnIdentify() 
{
	int		w_nRet, w_nTmplNo, w_nLearned;
	DWORD	w_dwTime;
	
	UpdateData(TRUE);
	
	EnableControl(FALSE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
	
	m_bCancel = FALSE;		

	m_strCmdResult.Empty();

	while (1)
	{
		DoLedCtrl(E_LED_CTRL_CAPTURE);

		DoEvents();

		if (m_strCmdResult.IsEmpty())
			m_strCmdResult = _T("Input your finger.");
		else
			m_strCmdResult = m_strCmdResult + _T("\r\nInput your finger.");

		UpdateData(FALSE);

		w_dwTime = GetTickCount();
		
		//. Get Image
		while (1)
		{
			DoEvents();

			if(m_bCancel)
			{
				m_strCmdResult = _T("Operation Canceled");
				goto l_exit;
			}
			
			//. Get Image
			w_nRet = m_clsCommu.Run_GetImage();
			
			if (w_nRet == ERR_SUCCESS)
				break;
			else if ((w_nRet == ERR_CONNECTION) || (w_nRet == ERR_NOT_AUTHORIZED))
			{
				m_strCmdResult = GetErrorMsg(w_nRet);
				goto l_exit;
			}
		}

		DoLedCtrl(E_LED_CTRL_DETECTED);
		
		m_strCmdResult = _T("Release your finger");
		UpdateData(FALSE);
		
		DoEvents();

		//. Up Image
		if (m_chkShowImage.GetCheck())
		{
			m_strCmdResult = _T("Uploading image...");
			UpdateData(FALSE);
			DoEvents();

			w_nRet = m_clsCommu.Run_UpImage(0, g_FpImageBuf, &g_nImageWidth, &g_nImageHeight);

			if(w_nRet != ERR_SUCCESS)
			{
				m_strCmdResult = GetErrorMsg(w_nRet);
				goto l_exit;
			}

			m_wndFinger.SetImage(g_FpImageBuf, g_nImageWidth, g_nImageHeight, m_bStretchView);

			m_strCmdResult = _T("Uploading image is successed.");
			UpdateData(FALSE);
			DoEvents();
		}

		w_dwTime = GetTickCount();

		//. Create Template
		w_nRet = m_clsCommu.Run_Generate(0);

		if (w_nRet != ERR_SUCCESS)
		{
			m_strCmdResult = GetErrorMsg(w_nRet);

			if (w_nRet == ERR_CONNECTION)
				goto l_exit;
			else
			{
				Sleep(1000);
				continue;
			}
		}

		//. Identify
		w_nRet = m_clsCommu.Run_Search(0, 1, g_nMaxFpCount, &w_nTmplNo, &w_nLearned);
		
		w_dwTime = GetTickCount() - w_dwTime;

		if (w_nRet == ERR_SUCCESS)
		{
			m_strCmdResult.Format(_T("Result : Success\r\nTemplate No : %d, Learn Result : %d\r\nMatch Time : %dms"), w_nTmplNo, w_nLearned, w_dwTime);
			DoLedCtrl(E_LED_CTRL_IDENTIFY_OK);
		}
		else
		{
 			m_strCmdResult.Format(_T("\r\nMatch Time : %dms"), w_dwTime);
 			m_strCmdResult = GetErrorMsg(w_nRet) + m_strCmdResult;
			DoLedCtrl(E_LED_CTRL_IDENTIFY_NG);
		}

		UpdateData(FALSE);
		DoEvents();
	}
	
l_exit:
	DoLedCtrl(E_LED_CTRL_WAIT);
	UpdateData(FALSE);	
	EnableControl(TRUE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(TRUE);
}

void COEMHostDlg::OnBtnIdentifyTemplate() 
{
	int		w_nRet, w_nTmplNo, w_nLearned;
	DWORD	w_dwTime;
	
	UpdateData(TRUE);

	//. Check if template is exist
	if (!m_bTemplate1Exist)
	{
		AfxMessageBox(_T("Please open template file to verify!"));
		return;
	}	

	if (!CheckUserID())
		return;

	EnableControl(FALSE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
	
	w_dwTime = GetTickCount();
	
	// Download Template to Buffer0
	w_nRet = m_clsCommu.Run_DownChar(0, m_abyTemplate1, m_nTemplateSize);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		goto l_exit;
	}
	
	//. Identify
	w_nRet = m_clsCommu.Run_Search(0, 1, g_nMaxFpCount, &w_nTmplNo, &w_nLearned);
	
	w_dwTime = GetTickCount() - w_dwTime;
	
	if (w_nRet == ERR_SUCCESS)
	{
		m_strCmdResult.Format(_T("Result : Success\r\nTemplate No : %d, Learn Result : %d\r\nMatch Time : %dms"), w_nTmplNo, w_nLearned, w_dwTime );
	}
	else
	{
		m_strCmdResult.Format(_T("\r\nMatch Time : %dms"), w_dwTime);
		m_strCmdResult = GetErrorMsg(w_nRet) + m_strCmdResult;
	}
	
l_exit:
	
	UpdateData(FALSE);
	
	EnableControl(TRUE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(TRUE);	
}

void COEMHostDlg::OnBtnIdentifyImage() 
{
	int		w_nRet, w_nTmplNo, w_nLearned;
	DWORD	w_dwTime;
	
	UpdateData(TRUE);
	
	//. Check if template is exist
	if (!m_bFpImageExist)
	{
		AfxMessageBox(_T("Please open image file to verify!"));
		return;
	}	
	
	if (!CheckUserID())
		return;
	
	EnableControl(FALSE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);	
	
	m_strCmdResult = _T("Downloading image...");
	UpdateData(FALSE);
	DoEvents();

	// Download Template to Buffer0
	w_nRet = m_clsCommu.Run_DownImage(g_FpImageBuf, g_nImageWidth, g_nImageHeight);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		goto l_exit;
	}
	
	w_dwTime = GetTickCount();

	//. Create template
	w_nRet = m_clsCommu.Run_Generate(0);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		goto l_exit;
	}

	//. Identify
	w_nRet = m_clsCommu.Run_Search(0, 1, g_nMaxFpCount, &w_nTmplNo, &w_nLearned);
	
	w_dwTime = GetTickCount() - w_dwTime;
	
	if (w_nRet == ERR_SUCCESS)
	{
		m_strCmdResult.Format(_T("Result : Success\r\nTemplate No : %d, Learn Result : %d\r\nMatch Time : %dms"), w_nTmplNo, w_nLearned, w_dwTime );
	}
	else
	{
		m_strCmdResult.Format(_T("\r\nMatch Time : %dms"), w_dwTime);
		m_strCmdResult = GetErrorMsg(w_nRet) + m_strCmdResult;
	}
	
l_exit:
	
	UpdateData(FALSE);
	
	EnableControl(TRUE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(TRUE);	
	
}

void COEMHostDlg::OnBtnDetectFinger() 
{
	int		w_nRet, w_nDetect;

	DoLedCtrl(E_LED_CTRL_CAPTURE);
	
	Sleep(50);

	w_nRet = m_clsCommu.Run_FingerDetect(&w_nDetect);

	DoLedCtrl(E_LED_CTRL_DETECTED);

	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}

	if (w_nDetect == 0)
		m_strCmdResult.Format(_T("Result : Success\r\nFinger is not detected"));
	else
		m_strCmdResult.Format(_T("Result : Success\r\nFinger is detected"));
	
	UpdateData(FALSE);

	DoLedCtrl(E_LED_CTRL_WAIT);
}

void COEMHostDlg::OnBtnGetUserCount() 
{
	int		w_nRet, w_nEnrollCount;

	UpdateData(TRUE);

	if (!CheckUserID())
		return;

	w_nRet = m_clsCommu.Run_GetEnrollCount(1, g_nMaxFpCount, &w_nEnrollCount);

	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}

	m_strCmdResult.Format(_T("Result : Success\r\nEnroll Count = %d"), w_nEnrollCount);	
	UpdateData(FALSE);
}

void COEMHostDlg::OnBtnDeleteId() 
{
	int		w_nRet;
	
	UpdateData(TRUE);
	
	if (!CheckUserID())
		return;
	
	w_nRet = m_clsCommu.Run_DelChar(m_nUserID, m_nUserID);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}
	
	m_strCmdResult.Format(_T("Result : Success"));
	UpdateData(FALSE);	
}

void COEMHostDlg::OnBtnGetEmptyId() 
{
	int		w_nRet, w_nEmptyID;
	
	UpdateData(TRUE);
	
	if (!CheckUserID())
		return;
	
	
	w_nRet = m_clsCommu.Run_GetEmptyID(1, g_nMaxFpCount, &w_nEmptyID);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}
	
	m_strCmdResult.Format(_T("Result : Success\r\nEmpty ID = %d"), w_nEmptyID);
	UpdateData(FALSE);	
}

void COEMHostDlg::OnBtnGetStatus() 
{
	int		w_nRet, w_nStatus;
	
	UpdateData(TRUE);
	
	if (!CheckUserID())
		return;
	
	w_nRet = m_clsCommu.Run_GetStatus(m_nUserID, &w_nStatus);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}
	
	if (w_nStatus == 0)
		m_strCmdResult.Format(_T("Result : Success\r\nStatus : Empty"));
	else
		m_strCmdResult.Format(_T("Result : Success\r\nStatus : Not Empty"));

	UpdateData(FALSE);
}

void COEMHostDlg::OnBtnGetBrokenId()
{
	int		w_nRet, w_nCount, w_nFirstID;
	
	UpdateData(TRUE);
	
	if (!CheckUserID())
		return;
	
	w_nRet = m_clsCommu.Run_GetBrokenID(1, g_nMaxFpCount, &w_nCount, &w_nFirstID);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}
	
	m_strCmdResult.Format(_T("Result : Success\r\nBroken FP Count : %d\r\nFirst Broken FP No : %d"), w_nCount, w_nFirstID);
	UpdateData(FALSE);
}

void COEMHostDlg::OnBtnUpTemplate() 
{
	int		w_nRet;
	
	UpdateData(TRUE);

	if (!CheckUserID())
		return;

	EnableControl(FALSE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
	
	// Load Template to Buffer
	w_nRet = m_clsCommu.Run_LoadChar(m_nUserID, 0);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		goto l_exit;
	}
	
	// Up template
	w_nRet = m_clsCommu.Run_UpChar(0, m_abyTemplate1, &m_nTemplateSize);
	
	if( w_nRet != ERR_SUCCESS )
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		goto l_exit;
	}
	
	m_strCmdResult.Format(_T("Result : Success"));
	m_strFpData = ConvertByteToHex(m_abyTemplate1, m_nTemplateSize, 8);	
	m_bTemplate1Exist = TRUE;
	
l_exit:
	
	UpdateData(FALSE);
	
	EnableControl(TRUE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(TRUE);	
}

void COEMHostDlg::OnBtnDownTemplate() 
{
	int		w_nRet, w_nDupTmplNo;
	
	UpdateData(TRUE);

	if (!CheckUserID())
		return;

	if (!m_bTemplate1Exist)
	{
		AfxMessageBox(_T("Please open template!"));
		return;
	}

	EnableControl(FALSE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
	
	// Download Template to Buffer
	w_nRet = m_clsCommu.Run_DownChar(0, m_abyTemplate1, m_nTemplateSize);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		goto l_exit;
	}
	
	//. Store template
	w_nRet = m_clsCommu.Run_StoreChar(m_nUserID, 0, &w_nDupTmplNo);
	
	if( w_nRet != ERR_SUCCESS )
	{
		if(w_nRet == ERR_DUPLICATION_ID)
		{
			m_strCmdResult.Format(_T("Result : Fail\r\nDuplication ID = %u"), w_nDupTmplNo);
		}
		else
		{
			m_strCmdResult = GetErrorMsg(w_nRet);
		}
	}
	else
	{
		m_strCmdResult.Format(_T("Result : Success"));
	}
	
l_exit:
	
	UpdateData(FALSE);
	
	EnableControl(TRUE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(TRUE);	
}

void COEMHostDlg::OnBtnGetDeviceInfo() 
{
	int		w_nRet;
	char	w_szDeviceInfo[256];

	w_nRet = m_clsCommu.Run_GetDeviceInfo(w_szDeviceInfo);

	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}

	int		w_nLen;
	WCHAR* w_pwszTmp = NULL;
	CString w_strTmp;

	w_nLen = ::MultiByteToWideChar(CP_ACP, 0, w_szDeviceInfo, -1, w_pwszTmp, 0);
	w_pwszTmp = new wchar_t[w_nLen+1];
	::MultiByteToWideChar(CP_ACP, 0, w_szDeviceInfo, -1, w_pwszTmp, w_nLen);			
	w_strTmp = _T("Info : ");
	w_strTmp = w_strTmp + w_pwszTmp;
	m_strCmdResult = _T("Result : Success\r\n") + w_strTmp;
	delete[] w_pwszTmp;

	UpdateData(FALSE);
	
}

void COEMHostDlg::OnBtnUpImage() 
{
	int		w_nRet, w_nType;

	UpdateData(TRUE);

	w_nType = m_cmbUpImgType.GetCurSel();

	if (w_nType == CB_ERR)
	{
		AfxMessageBox(_T("Please select image type!"));
		return;
	}


	EnableControl(FALSE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
	DoLedCtrl(E_LED_CTRL_CAPTURE);

	m_bCancel = FALSE;
	
	m_strCmdResult = _T("Input your finger.");
	UpdateData(FALSE);
	
	Sleep(50);
	
	//. Get Image
	while (1)
	{
		DoEvents();
		
		if(m_bCancel)
		{
			m_strCmdResult = _T("Operation Canceled");
			goto l_exit;
		}
		
		//. Get Image
		w_nRet = m_clsCommu.Run_GetImage();
		
		if (w_nRet == ERR_SUCCESS)
			break;
		else if ((w_nRet == ERR_CONNECTION) || (w_nRet == ERR_NOT_AUTHORIZED))
		{
			m_strCmdResult = GetErrorMsg(w_nRet);
			goto l_exit;
		}				
	}

	DoLedCtrl(E_LED_CTRL_DETECTED);
	
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(FALSE);
	
	m_strCmdResult = _T("Uploading finger image...");
	UpdateData(FALSE);
	DoEvents();
	
	//. Up Image
	w_nRet = m_clsCommu.Run_UpImage(w_nType, g_FpImageBuf, &g_nImageWidth, &g_nImageHeight);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		goto l_exit;
	}	
	
	m_strCmdResult = _T("Uploading finger image is success.");
	UpdateData(FALSE);

	if (w_nType == 1)
	{
		g_nImageWidth = g_nImageWidth * 2;
		g_nImageHeight = g_nImageHeight * 2;

		// expand image
		int i, j;
		PBYTE	p_buf1 = g_FpImageBuf;
		PBYTE	p_buf = g_FpImageBuf2;//+IMAGE_WIDTH*(IMAGE_HEIGHT-1);
		
		for ( i = 0; i < g_nImageHeight/2; i++)
		{
			for ( j = 0; j < g_nImageWidth/2; j++)
			{
				p_buf[0] = p_buf1[0];
				p_buf[1] = p_buf1[0];
				p_buf[g_nImageWidth] = p_buf1[0];
				p_buf[g_nImageWidth+1] = p_buf1[0];
				p_buf1 ++;
				p_buf += 2;
			}
			p_buf += g_nImageWidth;
		}
		
		memcpy(g_FpImageBuf, g_FpImageBuf2, g_nImageWidth*g_nImageHeight);
	}

	m_bFpImageExist = TRUE;
	m_wndFinger.SetImage(g_FpImageBuf, g_nImageWidth, g_nImageHeight, m_bStretchView);
	
l_exit:
	DoLedCtrl(E_LED_CTRL_WAIT);
	UpdateData(FALSE);
	EnableControl(TRUE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(TRUE);
}

void COEMHostDlg::OnBtnSetParam() 
{
	int		w_nRet, w_nParamType, w_nParamValue;
	CString	w_strMsg;

	UpdateData(TRUE);

	w_nParamType = m_cmbParamType.GetCurSel();
	
	if (w_nParamType == CB_ERR)
	{
		AfxMessageBox(_T("Please select parameter type!"));
		return;
	}
	
	if (w_nParamType == DP_BAUDRATE)
	{
		w_nParamValue = m_cmbSetBaudrate.GetCurSel() + 1;
	}
	else if (w_nParamType != DP_DEV_PASS)
	{
		//. Check Value
		if (!IsNumber(m_strParamValue))
		{
			w_strMsg.Format(_T("Please input correct value!"), 1, g_nMaxFpCount);
			AfxMessageBox(w_strMsg);
			return;
		}
		w_nParamValue = _ttoi(m_strParamValue);
	}

	switch(w_nParamType)
	{
	case DP_DEVICE_ID:
		if (w_nParamValue < MIN_DEVICE_ID || w_nParamValue > MAX_DEVICE_ID)
		{
			w_strMsg.Format(_T("Please input correct device id(%d~%d)"), MIN_DEVICE_ID, MAX_DEVICE_ID);
			AfxMessageBox(w_strMsg);
			return;
		}
		break;
	case DP_SECURITY_LEVEL:
		if (w_nParamValue < MIN_SECURITY_LEVEL || w_nParamValue > MAX_SECURITY_LEVEL)
		{
			w_strMsg.Format(_T("Please input correct security level(%d~%d)"), MIN_SECURITY_LEVEL, MAX_SECURITY_LEVEL);
			AfxMessageBox(w_strMsg);
			return;
		}
		break;
	case DP_DUP_CHECK:
		if (w_nParamValue != 0 && w_nParamValue != 1)
		{
			w_strMsg.Format(_T("Please input correct duplication check(0, 1)"));
			AfxMessageBox(w_strMsg);
			return;
		}
		break;
	case DP_BAUDRATE:
		if (w_nParamValue < MIN_BAUDRATE_IDX || w_nParamValue > MAX_BAUDRATE_IDX)
		{
			w_strMsg.Format(_T("Please input correct baudrate(%d~%d)"), MIN_BAUDRATE_IDX, MAX_BAUDRATE_IDX);
			AfxMessageBox(w_strMsg);
			return;
		}
		break;
	case DP_AUTO_LEARN:
		if (w_nParamValue != 0 && w_nParamValue != 1)
		{
			w_strMsg.Format(_T("Please input correct auto learn(0, 1)"));
			AfxMessageBox(w_strMsg);
			return;
		}
		break;
	case DP_DEV_PASS:
		{
			char w_szDevPass[MAX_DEVPASS_LEN + 1];
			int w_nLen;

			memset(w_szDevPass, 0, sizeof(w_szDevPass));

			w_nLen = m_strParamValue.GetLength();
			if (w_nLen > MAX_DEVPASS_LEN)
				w_nLen = MAX_DEVPASS_LEN;

			::WideCharToMultiByte(CP_ACP, 0, m_strParamValue.GetBuffer(0), -1, w_szDevPass, m_strParamValue.GetLength(), NULL, NULL);

			w_nRet = m_clsCommu.Run_SetDevPass((BYTE*)w_szDevPass);

			if (w_nRet != ERR_SUCCESS)
			{
				m_strCmdResult = GetErrorMsg(w_nRet);
				UpdateData(FALSE);
				return;
			}

			m_strCmdResult.Format(_T("Result : Success"));

		}
		break;
	}
	
	if (w_nParamType == DP_DEV_PASS)
	{
		UpdateData(FALSE);
		return;
	}

	w_nRet = m_clsCommu.Run_SetParam(w_nParamType, w_nParamValue);

	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}
	
	m_strParamValue.Format(_T("%d"), w_nParamValue);
	m_strCmdResult.Format(_T("Result : Success"));

	if (w_nParamType == DP_BAUDRATE && m_nConMode == SERIAL_CON_MODE)
	{
		int		w_nComPortIndex;
		CString	w_strComPort;

		w_nComPortIndex = m_cmbConComPort.GetCurSel();
	
		m_cmbConComPort.GetLBText(w_nComPortIndex, w_strComPort);

		//. Init Comport
		m_clsCommu.InitConnection(	
			SERIAL_CON_MODE, 
			w_strComPort, 
			w_nParamValue - 1, 
			0,
			0);
	}
	UpdateData(FALSE);
}

void COEMHostDlg::OnBtnGetParam() 
{
	int		w_nRet, w_nParamType, w_nParamValue;
	int		w_nLen;

	UpdateData(TRUE);

	w_nParamType = m_cmbParamType.GetCurSel();

	if (w_nParamType == CB_ERR)
	{
		AfxMessageBox(_T("Please select parameter type!"));
		return;
	}

	if (w_nParamType == DP_DEV_PASS)
	{
		char	w_szDevPass[MAX_DEVPASS_LEN+1];

		memset(w_szDevPass, 0, sizeof(w_szDevPass));

		w_nLen = m_strParamValue.GetLength();
		if (w_nLen > MAX_DEVPASS_LEN)
			w_nLen = MAX_DEVPASS_LEN;

		::WideCharToMultiByte(CP_ACP, 0, m_strParamValue.GetBuffer(0), -1, w_szDevPass, w_nLen, NULL, NULL);

		w_nRet = m_clsCommu.Run_VerfiyDevPass((BYTE*)w_szDevPass);

		if (w_nRet != ERR_SUCCESS)
		{
			m_strCmdResult = GetErrorMsg(w_nRet);
			UpdateData(FALSE);
			return;
		}

		m_strCmdResult.Format(_T("Result : Success"));

		UpdateData(FALSE);
		return;
	}

	w_nRet = m_clsCommu.Run_GetParam(w_nParamType, &w_nParamValue);

	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}

	if (w_nParamType == DP_BAUDRATE)
		m_cmbSetBaudrate.SetCurSel(w_nParamValue - 1);
	else
		m_strParamValue.Format(_T("%d"), w_nParamValue);

	m_strCmdResult.Format(_T("Result : Success"));
	UpdateData(FALSE);		
}

void COEMHostDlg::OnBtnDeleteAll() 
{
	int		w_nRet;
	
	UpdateData(TRUE);
	
	if (!CheckUserID())
		return;
	
	w_nRet = m_clsCommu.Run_DelChar(1, g_nMaxFpCount);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}
	
	m_strCmdResult.Format(_T("Result : Success"));
	UpdateData(FALSE);		
}

void COEMHostDlg::OnBtnDownMultiTmpl() 
{
	int				w_nRet, w_nFpNo = 1, w_nDupTmplNo, w_nValue;
	BOOL			w_bWorking;
	BYTE			w_TemplateBuf[MAX_TEMPLATE_SIZE];
	CFile			w_clsTempFile;
	CString			w_strFindCond, w_strFileName;
	CFileFind		w_clsFinder;
	LPCTSTR			lpszTitle = _T( "Select the folder to open templates" );
	UINT			uFlags	  = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	CFolderDialog	OpenFolderDlg( lpszTitle, _T(""), this, uFlags );
	DWORD			w_nTickCount;
	unsigned int	w_nTmplSize = 0;
	
	if(OpenFolderDlg.DoModal() == IDOK)
	{
		w_strFindCond = OpenFolderDlg.GetFolderPath();
		w_strFindCond = w_strFindCond + _T("\\*.fpt");
		w_bWorking = w_clsFinder.FindFile(w_strFindCond);
		
		EnableControl(FALSE);		
		GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
		m_bCancel = FALSE;
		
		// Get duplication check
		w_nRet = m_clsCommu.Run_GetParam(DP_DUP_CHECK, &w_nValue);

		if (w_nRet != ERR_SUCCESS)
		{
			m_strCmdResult = GetErrorMsg(w_nRet);
			UpdateData(FALSE);
			goto l_exit;
		}
		
		// Set duplication check
		w_nRet = m_clsCommu.Run_SetParam(DP_DUP_CHECK, 0);
		
		if (w_nRet != ERR_SUCCESS)
		{
			m_strCmdResult = GetErrorMsg(w_nRet);
			UpdateData(FALSE);
			goto l_exit;
		}

		w_nTickCount = GetTickCount();

		while (w_bWorking)
		{
			DoEvents();
			
			w_bWorking = w_clsFinder.FindNextFile();
			
			w_strFileName = w_clsFinder.GetFileName();
			
			if (!w_clsTempFile.Open(w_clsFinder.GetFilePath(), CFile::modeRead | CFile::typeBinary, NULL))
				continue;

			w_nTmplSize = (unsigned int)w_clsTempFile.GetLength();
			
			if(w_clsTempFile.Read(w_TemplateBuf, w_nTmplSize) != w_nTmplSize)
				continue;
			
			w_clsTempFile.Close();

			// Download Template to Buffer
			w_nRet = m_clsCommu.Run_DownChar(0, w_TemplateBuf, w_nTmplSize);
			
			if (w_nRet != ERR_SUCCESS)
			{
				m_strCmdResult = GetErrorMsg(w_nRet);
				UpdateData(FALSE);
				goto l_exit;
			}
			
			//. Store template
			w_nRet = m_clsCommu.Run_StoreChar(w_nFpNo, 0, &w_nDupTmplNo);
			
			if( w_nRet != ERR_SUCCESS )
			{
				m_strCmdResult = GetErrorMsg(w_nRet);
				UpdateData(FALSE);
				goto l_exit;
			}
			
			m_strCmdResult.Format(_T("Downloaded Template Count : %d"), w_nFpNo);
			UpdateData(FALSE);
			DoEvents();
			
			w_nFpNo++;
			
			if (m_bCancel == TRUE)
			{
				goto l_exit;
			}
			
			if (w_nFpNo == g_nMaxFpCount+1)
			{
				goto l_exit;
			}
		}
		
l_exit:
		
		// restore duplication check
		m_clsCommu.Run_SetParam(DP_DUP_CHECK, w_nValue);

		m_strCmdResult.Format(_T("Total Time : %dms"), GetTickCount() - w_nTickCount);

		UpdateData(FALSE);
		
		EnableControl(TRUE);
	}
	
}

void COEMHostDlg::OnEditchangeCombo1() 
{
// 	int		w_nCurSel;	
// 
// 	w_nCurSel = m_cmbParamType.GetCurSel();
// 
// 	if (w_nCurSel == DP_BAUDRATE)
// 	{
// 		GetDlgItem(IDC_CMB_SET_BAUDRATE)->ShowWindow(TRUE);
// 		GetDlgItem(IDC_EDIT1)->ShowWindow(FALSE);
// 	}
// 	else
// 	{
// 		GetDlgItem(IDC_CMB_SET_BAUDRATE)->ShowWindow(FALSE);
// 		GetDlgItem(IDC_EDIT1)->ShowWindow(TRUE);
// 	}
}

void COEMHostDlg::OnSelchangeCombo1() 
{
	int		w_nCurSel;

	UpdateData(TRUE);
	
	w_nCurSel = m_cmbParamType.GetCurSel();
	
	if (w_nCurSel == DP_BAUDRATE)
	{
		GetDlgItem(IDC_CMB_SET_BAUDRATE)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT1)->ShowWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_CMB_SET_BAUDRATE)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT1)->ShowWindow(TRUE);
	}

	if (w_nCurSel == DP_DEV_PASS)
	{
		if (g_bLangEng == TRUE)
		{
			GetDlgItem(IDC_BTN_SET_PARAM)->SetWindowText(IDS_EN_SET_PARAM_PASS);
			GetDlgItem(IDC_BTN_GET_PARAM)->SetWindowText(IDS_EN_VERIFY_PARAM_PASS);
		}
		else
		{
			GetDlgItem(IDC_BTN_SET_PARAM)->SetWindowText(IDS_CN_SET_PARAM_PASS);
			GetDlgItem(IDC_BTN_GET_PARAM)->SetWindowText(IDS_CN_VERIFY_PARAM_PASS);
		}
		((CEdit*)GetDlgItem(IDC_EDIT1))->SetPasswordChar(_T('*'));
	}
	else
	{
		if (g_bLangEng == TRUE)
		{
			GetDlgItem(IDC_BTN_SET_PARAM)->SetWindowText(IDS_EN_SET_PARAM);
			GetDlgItem(IDC_BTN_GET_PARAM)->SetWindowText(IDS_EN_GET_PARAM);
		}
		else
		{
			GetDlgItem(IDC_BTN_SET_PARAM)->SetWindowText(IDS_CN_SET_PARAM);
			GetDlgItem(IDC_BTN_GET_PARAM)->SetWindowText(IDS_CN_GET_PARAM);
		}
		((CEdit*)GetDlgItem(IDC_EDIT1))->SetPasswordChar(NULL);
	}

	m_strParamValue.Empty();
	UpdateData(FALSE);
}

void COEMHostDlg::OnBtnVerifyWithDownTmpl() 
{
	int		w_nRet, w_nLearned;
	DWORD	w_dwTime;
	
	//. Check if template is exist
	if (!m_bTemplate1Exist)
	{
		AfxMessageBox(_T("Please open template file to verify!"));
		return;
	}
	
	EnableControl(FALSE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
	m_bCancel = FALSE;

	//. Download template1
	w_nRet = m_clsCommu.Run_DownChar(1, m_abyTemplate1, m_nTemplateSize);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		goto l_exit;
	}
	
	DoLedCtrl(E_LED_CTRL_CAPTURE);
	m_strCmdResult = "Input your finger.";
	UpdateData(FALSE);
	
	w_dwTime = GetTickCount();
	
	//. Get Image
	while (1)
	{
		DoEvents();
		
		if(m_bCancel)
		{
			m_strCmdResult = _T("Operation canceled!");
			goto l_exit;
		}
		
		//. Get Image
		w_nRet = m_clsCommu.Run_GetImage();
		
		if(w_nRet == ERR_SUCCESS)
			break;
		else if(w_nRet == ERR_CONNECTION)
		{
			m_strCmdResult = GetErrorMsg(w_nRet);
			goto l_exit;
		}		
	}

	DoLedCtrl(E_LED_CTRL_DETECTED);

	m_strCmdResult = _T("Release your finger");
	UpdateData(FALSE);	
	DoEvents();

	// Create template from image
	w_nRet = m_clsCommu.Run_Generate(0);
	
	if(w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		goto l_exit;
	}
	
	// Verify created template and download template
	w_nRet = m_clsCommu.Run_Match(0, 1, &w_nLearned);

	if(w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		DoLedCtrl(E_LED_CTRL_IDENTIFY_NG);
		goto l_exit;
	}

	m_strCmdResult.Format(_T("Result : Verify Success"));
	DoLedCtrl(E_LED_CTRL_IDENTIFY_OK);
	
l_exit:
	DoLedCtrl(E_LED_CTRL_WAIT);
	EnableControl(TRUE);	
	UpdateData(FALSE);
	GetDlgItem(IDC_BTN_DISCONNECT)->EnableWindow(TRUE);		
}

void COEMHostDlg::OnBtnSetModuleSn() 
{
	int		w_nRet;
	char	w_szModuleSN[MODULE_SN_LEN+1];
	
	UpdateData(TRUE);
		
	if (!CheckModuleSN())
		return;
	
	memset(w_szModuleSN, 0, sizeof(w_szModuleSN));
	
	::WideCharToMultiByte(CP_ACP, 0, m_strModuleSN.GetBuffer(0), -1, w_szModuleSN, MODULE_SN_LEN+1, NULL, NULL);
	
	w_nRet = m_clsCommu.Run_SetModuleSN((BYTE*)w_szModuleSN);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}
	
	m_strCmdResult.Format(_T("Result : Success"));
	UpdateData(FALSE);	
}

void COEMHostDlg::OnBtnGetModuleSn() 
{
	int		w_nRet;
	char	w_szModuleSN[MODULE_SN_LEN+1];
	
	UpdateData(TRUE);
	
	memset(w_szModuleSN, 0, sizeof(w_szModuleSN));
	
	w_nRet = m_clsCommu.Run_GetModuleSN((BYTE*)w_szModuleSN);
	
	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}
	
	int		w_nLen;
	WCHAR* w_pwszTmp = NULL;
	CString w_strTmp;
	
	w_nLen = ::MultiByteToWideChar(CP_ACP, 0, w_szModuleSN, -1, w_pwszTmp, 0);
	w_pwszTmp = new wchar_t[w_nLen+1];
	::MultiByteToWideChar(CP_ACP, 0, w_szModuleSN, -1, w_pwszTmp, w_nLen);			
	w_strTmp = w_pwszTmp;
	m_strModuleSN = w_strTmp;
	m_strCmdResult = _T("Result : Success\r\n");
	delete[] w_pwszTmp;
	
	UpdateData(FALSE);		
}

void COEMHostDlg::OnCbnSelchangeCmbMaxFpCount()
{
	g_nMaxFpCount = g_anMaxFpCount[m_cmbMaxFpCount.GetCurSel()];
}

void COEMHostDlg::OnBnClickedRadioChinese()
{
	SetLanguage(0);
}

void COEMHostDlg::OnBnClickedRadioEnglish()
{
	SetLanguage(1);
}
#define MAX_IMAGE_SIZE	(300*300)
void COEMHostDlg::OnBnClickedBtnEnrollFromImage()
{
	int		i, w_nRet, w_nFpNo, w_nDupFpNo, w_nStatus, w_nEnrollCount;
	int		w_anWidth[MAX_ENROLL_COUNT], w_anHeight[MAX_ENROLL_COUNT];
	BYTE*	w_pImage = NULL;

	UpdateData(TRUE);

	if (!CheckUserID())
		return;

	w_nFpNo = m_nUserID;
	w_nEnrollCount = m_cmbEnrollCount.GetCurSel() + 1;

	if (w_nEnrollCount < 0)
	{
		AfxMessageBox(_T("Please select enroll count!"));
		return;
	}

	//. Check if fp is exist
	w_nRet = m_clsCommu.Run_GetStatus(w_nFpNo, &w_nStatus);

	if ( w_nRet != ERR_SUCCESS )
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}

	if( w_nRet == ERR_SUCCESS && w_nStatus == GD_TEMPLATE_NOT_EMPTY )
	{
		m_strCmdResult.Format(_T("Template is already exist"));
		UpdateData(FALSE);
		return;
	}	

	w_pImage = new BYTE[MAX_IMAGE_SIZE * MAX_ENROLL_COUNT];

	for (i=0; i<w_nEnrollCount; i++)
	{
		if (!OpenImage(&w_pImage[i*MAX_IMAGE_SIZE], &w_anWidth[i], &w_anHeight[i]))
		{
			goto l_exit;
		}
	}

	for (i=0; i<w_nEnrollCount; i++)
	{
		// Download Template to Buffer0
		w_nRet = m_clsCommu.Run_DownImage(	&w_pImage[i*MAX_IMAGE_SIZE], 
											w_anWidth[i], w_anHeight[i]);

		if (w_nRet != ERR_SUCCESS)
		{
			m_strCmdResult = GetErrorMsg(w_nRet);
			UpdateData(FALSE);
			goto l_exit;
		}

		//. Create Template From Captured Image
		w_nRet = m_clsCommu.Run_Generate(i);

		if (w_nRet != ERR_SUCCESS)
		{
			m_strCmdResult = GetErrorMsg(w_nRet);
			UpdateData(FALSE);
			goto l_exit;
		}
	}

	if (w_nEnrollCount != 1)
	{
		//. Merge Template
		w_nRet = m_clsCommu.Run_Merge(0, w_nEnrollCount);

		if (w_nRet != ERR_SUCCESS)
		{
			m_strCmdResult = GetErrorMsg(w_nRet);
			UpdateData(FALSE);
			goto l_exit;
		}
	}

	//. Store template
	w_nRet = m_clsCommu.Run_StoreChar(w_nFpNo, 0, &w_nDupFpNo);

	if (w_nRet != ERR_SUCCESS)
	{
		if(w_nRet == ERR_DUPLICATION_ID)
		{
			m_strCmdResult.Format(_T("Result : Fail\r\nDuplication ID = %u"), w_nDupFpNo);
		}
		else
		{
			m_strCmdResult = GetErrorMsg(w_nRet);
		}
	}
	else
	{
		m_strCmdResult.Format(_T("Result : Success\r\nTemplate No : %d"), w_nFpNo);
	}
	UpdateData(FALSE);

l_exit:

	delete[] w_pImage;
}

void COEMHostDlg::OnBnClickedBtnGetEnrolledIdList()
{
	int		i, w_nRet, w_nEnrollCount, *w_pnIDs;
	CString	w_strTmp;

	w_pnIDs = new int[5000];

	w_nRet = m_clsCommu.Run_GetEnrolledIDList(&w_nEnrollCount, w_pnIDs);

	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		goto l_exit;
	}
	
	m_strCmdResult.Format(_T("Result : Success\r\nEnroll Count = %d\r\nID = \r\n"), w_nEnrollCount);

	for (i=0; i<w_nEnrollCount; i++)
	{
		w_strTmp.Format(_T("%d\r\n"), w_pnIDs[i]);
		m_strCmdResult = m_strCmdResult + w_strTmp;
	}

	UpdateData(FALSE);

l_exit:

	delete[] w_pnIDs;
}

void COEMHostDlg::OnBnClickedBtnEnterStandby()
{
	int		w_nRet;

	UpdateData(TRUE);

	w_nRet = m_clsCommu.Run_EnterStandbyState();

	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}

	m_strCmdResult.Format(_T("Result : Success"));
	UpdateData(FALSE);	
}

void COEMHostDlg::OnBnClickedBtnAdjustSensor()
{
	int		w_nRet;

	m_strCmdResult.Format(_T("Please screen off sensor.\r\nAdjusting sensor..."));
	UpdateData(FALSE);	
	DoEvents();

	w_nRet = m_clsCommu.Run_AdjustSensor();

	if (w_nRet != ERR_SUCCESS)
	{
		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);
		return;
	}

	m_strCmdResult.Format(_T("Adjust OK!"));
	UpdateData(FALSE);
}


void COEMHostDlg::OnBnClickedButtonUpgrade()
{
	// TODO: Add your control notification handler code here
	BYTE*		w_pBuf;
	CString strFile;
	CString sFilter;
	CFile		w_clsFile;
	int			i, w_nRet;

	sFilter= _T("BINARY File(*.bin)|*.bin|All File(*.*)|*.*||");
	CFileDialog fo(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,sFilter,NULL);

	if (fo.DoModal() != IDOK)
		return;

	strFile = fo.GetPathName();
	
	if(w_clsFile.Open(strFile, CFile::modeRead | CFile::typeBinary, NULL)){
		i = (int)w_clsFile.GetLength();
		w_pBuf = new unsigned char[i];
		w_clsFile.Read(w_pBuf, i);

		w_nRet = m_clsCommu.Run_UpgradeFirmware(w_pBuf, i);
		delete []w_pBuf;

		m_strCmdResult = GetErrorMsg(w_nRet);
		UpdateData(FALSE);

	}else{
		MessageBox(_T("Error opening firmware file!"));
	}
}


void COEMHostDlg::OnBnClickedBtnUpMultiTmpl()
{
	int				w_nRet, w_nFpNo = 1, w_nFpCount;
	BYTE			w_TemplateBuf[MAX_TEMPLATE_SIZE];
	CFile			w_clsTempFile;
	CString			w_strSavePath, w_strFileName;
	LPCTSTR			lpszTitle = _T( "Select the folder to save templates" );
	UINT			uFlags	  = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	CFolderDialog	OpenFolderDlg( lpszTitle, _T(""), this, uFlags );
	DWORD			w_nTickCount;
	unsigned int	w_nTmplSize = 0;

	UpdateData(TRUE);

	w_nFpCount = 0;

	if(OpenFolderDlg.DoModal() == IDOK)
	{
		w_strSavePath = OpenFolderDlg.GetFolderPath();
		
		EnableControl(FALSE);		
		GetDlgItem(IDC_BTN_STOP)->EnableWindow(TRUE);
		m_bCancel = FALSE;

		w_nTickCount = GetTickCount();

		while (w_nFpNo <= g_nMaxFpCount)
		{
			DoEvents();

			m_strCmdResult.Format(_T("Upload Template : %d"), w_nFpNo);
			UpdateData(FALSE);

			w_nRet = m_clsCommu.Run_LoadChar(w_nFpNo, 0);
			if (w_nRet != ERR_SUCCESS)
			{
				w_nFpNo ++;
				if (m_bCancel == TRUE)
				{
					goto l_exit;
				}
				continue;
			}

			w_nRet = m_clsCommu.Run_UpChar(0, w_TemplateBuf, &w_nTmplSize);
			if( w_nRet != ERR_SUCCESS )
			{
				m_strCmdResult = GetErrorMsg(w_nRet);
				goto l_exit;
			}

			w_strFileName.Format(_T("%s\\%04d.fpt"), w_strSavePath, w_nFpNo);
			if (!w_clsTempFile.Open(w_strFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary, NULL))
				continue;
			w_clsTempFile.Write(w_TemplateBuf, w_nTmplSize);
			w_clsTempFile.Close();

			DoEvents();

			w_nFpNo ++;
			w_nFpCount ++;

			if (m_bCancel == TRUE)
			{
				goto l_exit;
			}
		}

l_exit:
		m_strCmdResult.Format(_T("Uploaded Template Count : %d\r\nTotal Time : %dms"), w_nFpCount, GetTickCount() - w_nTickCount);
		UpdateData(FALSE);

		EnableControl(TRUE);
	}
}


void COEMHostDlg::OnBnClickedBtnSetRtc()
{
	ST_RTC_TYPE w_stRTCTime;
	SYSTEMTIME	w_sysTime;
	int	w_nRet = 0;

	// update data from ui
	UpdateData(TRUE);

	// get date
	m_dtpDate.GetTime(&w_sysTime);
	w_stRTCTime.m_nYear = (unsigned short)w_sysTime.wYear;
	w_stRTCTime.m_nMonth = (unsigned char)w_sysTime.wMonth;
	w_stRTCTime.m_nDay = (unsigned char)w_sysTime.wDay;

	// get time
	m_dtpTime.GetTime(&w_sysTime);
	w_stRTCTime.m_nHour = (unsigned char)w_sysTime.wHour;
	w_stRTCTime.m_nMinute = (unsigned char)w_sysTime.wMinute;
	w_stRTCTime.m_nSecond = (unsigned char)w_sysTime.wSecond;

	// run set RTC
	w_nRet = m_clsCommu.Run_SetRTC(w_stRTCTime);
	if( w_nRet != ERR_SUCCESS )
		m_strCmdResult = GetErrorMsg(w_nRet);

	m_strCmdResult.Format(_T("Set RTC Success"));

	// update data to ui
	UpdateData(FALSE);
}


void COEMHostDlg::OnBnClickedBtnGetRtc()
{
	ST_RTC_TYPE w_stRTCTime;
	SYSTEMTIME	w_sysTime;
	int	w_nRet = 0;

	// update data from ui
	UpdateData(TRUE);

	// run set RTC
	w_nRet = m_clsCommu.Run_GetRTC(&w_stRTCTime);
	if( w_nRet != ERR_SUCCESS )
		m_strCmdResult = GetErrorMsg(w_nRet);

	// set date
	memset(&w_sysTime, 0x00, sizeof(SYSTEMTIME));
	w_sysTime.wYear = w_stRTCTime.m_nYear;
	w_sysTime.wMonth = w_stRTCTime.m_nMonth;
	w_sysTime.wDay = w_stRTCTime.m_nDay;
	m_dtpDate.SetTime(&w_sysTime);

	// set time
	w_sysTime.wHour = w_stRTCTime.m_nHour;
	w_sysTime.wMinute = w_stRTCTime.m_nMinute;
	w_sysTime.wSecond = w_stRTCTime.m_nSecond;
	m_dtpTime.SetTime(&w_sysTime);

	m_strCmdResult.Format(_T("Get RTC Success"));

	// update data to ui
	UpdateData(FALSE);
}

// wParam 接收的是图标的ID，lParam 接收的是鼠标的行为
afx_msg LRESULT COEMHostDlg::OnShowTask(WPARAM wParam, LPARAM lParam)
{
    if(wParam != IDR_MAINFRAME)
        return 1;

    switch (lParam)
    {
    case WM_RBUTTONUP: // 右键弹出快捷菜单
        {
            CPoint pt;
            GetCursorPos(&pt); // 得到鼠标位置
            CMenu menu;
            //menu.LoadMenu(IDR_MENU1);
            //CMenu *pPopUp = menu.GetSubMenu(0);
            //pPopUp->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
            menu.CreatePopupMenu(); // 声明一个弹出式菜单
            // 增加菜单项“关闭”，点击则发送消息 WM_DESTROY 给主窗口（已隐藏），将程序结束
            menu.AppendMenu(MF_STRING, WM_DESTROY, _T("退出"));
            SetForegroundWindow();
            // 确定弹出式菜单的位置
            menu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, this);
            // 资源回收
            HMENU hmenu = menu.Detach();
            menu.DestroyMenu();
        }
        break;
    case WM_LBUTTONDBLCLK: // 双击左键的处理
        {
            if (IsIconic())
                ShowWindow(SW_RESTORE); // 最小化时，恢复显示
            else if (IsZoomed())
                ShowWindow(SW_SHOWNORMAL);  // 最大化时，恢复正常尺寸
            SetForegroundWindow(); // 置为前台窗口
        }
        break;
    }
    return 0;
}

void COEMHostDlg::ShowTray()
{
    m_nid.cbSize  = (DWORD)sizeof(NOTIFYICONDATA);
    m_nid.hWnd    = this->m_hWnd;
    m_nid.uID     = IDR_MAINFRAME;
    m_nid.uFlags  = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = WM_SHOWTASK;           // 点击托盘图标，发送的消息
    m_nid.hIcon   = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
    _tcscpy_s(m_nid.szTip, _T("NOEM Host"));        // 鼠标悬停显示的内容
    Shell_NotifyIcon(NIM_ADD, &m_nid);              // 在托盘区添加图标
}

void COEMHostDlg::HideTray()
{
    Shell_NotifyIcon(NIM_DELETE, &m_nid);
}
