//OEMHost.h : main header file for the OEMHost application
//

#if !defined(AFX_OEMHost_H__D5EEA68A_FC9B_4BD4_85E9_5FBD0FE8271A__INCLUDED_)
#define AFX_OEMHost_H__D5EEA68A_FC9B_4BD4_85E9_5FBD0FE8271A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// COEMHostApp:
// See OEMHost.cpp for the implementation of this class
//

class COEMHostApp : public CWinApp
{
public:
	COEMHostApp();

	HANDLE m_hMutex;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COEMHostApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(COEMHostApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OEMHost_H__D5EEA68A_FC9B_4BD4_85E9_5FBD0FE8271A__INCLUDED_)
