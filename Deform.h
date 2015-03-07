// Deform.h : main header file for the DEFORM application
//

#if !defined(AFX_DEFORM_H__98693CA8_5C1A_46C8_B4BE_9A04EFE84EAD__INCLUDED_)
#define AFX_DEFORM_H__98693CA8_5C1A_46C8_B4BE_9A04EFE84EAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDeformApp:
// See Deform.cpp for the implementation of this class
//

class CDeformApp : public CWinApp
{
public:
	CDeformApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeformApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDeformApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFORM_H__98693CA8_5C1A_46C8_B4BE_9A04EFE84EAD__INCLUDED_)
