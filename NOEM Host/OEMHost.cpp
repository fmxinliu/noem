// OEMHost.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OEMHost.h"
#include "OEMHostDlg.h"

#pragma warning( disable : 4996 )

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void DoEvents()
{
	MSG	msg;
	
	while (PeekMessage (&msg, NULL, 0, 0,PM_REMOVE))
	{
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}
}

/////////////////////////////////////////////////////////////////////////////
// COEMHostApp

BEGIN_MESSAGE_MAP(COEMHostApp, CWinApp)
	//{{AFX_MSG_MAP(COEMHostApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COEMHostApp construction

COEMHostApp::COEMHostApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only COEMHostApp object

COEMHostApp theApp;

/////////////////////////////////////////////////////////////////////////////
// COEMHostApp initialization

BOOL COEMHostApp::InitInstance()
{

	AfxEnableControlContainer();
	m_hMutex = CreateMutex(NULL,TRUE, _T("NOEM Host"));
	if( GetLastError() == ERROR_ALREADY_EXISTS ){
		AfxMessageBox(_T("Program already is running."));
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	COEMHostDlg dlg;
 	
 	m_pMainWnd = &dlg;
 	int nResponse = dlg.DoModal();
 	if (nResponse == IDOK)
 	{
 		// TODO: Place code here to handle when the dialog is
 		//  dismissed with OK
 	}
 	else if (nResponse == IDCANCEL)
 	{
 		// TODO: Place code here to handle when the dialog is
 		//  dismissed with Cancel
 	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
int COEMHostApp::ExitInstance()
{
	CloseHandle(m_hMutex);
	return 1;
}
