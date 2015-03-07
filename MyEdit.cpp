// MyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "Deform.h"
#include "MyEdit.h"
#include "DeformView.h"
#include "DeformDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyEdit

CMyEdit::CMyEdit()
{
}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	//{{AFX_MSG_MAP(CMyEdit)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEdit message handlers

BOOL CMyEdit::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
		CDeformView *pView = (CDeformView *)GetParent();
		if( pView->m_ShowEdit )
		{
		
			CDeformDoc *pDoc = (CDeformDoc *)pView->GetDocument();
			CString str;
			unsigned short *wcsCode = new unsigned short[32];

			GetWindowText(str);
			str.TrimLeft();
			str.TrimRight();
			if( str.GetLength() <= 0 ) return CEdit::PreTranslateMessage(pMsg);

			int size = str.GetLength();
			if( size > 2) size = 2;

			MultiByteToWideChar(
				CP_ACP,         // code page
				MB_PRECOMPOSED,         // character-type options
				str.GetBuffer(32),		// address of string to map
				size,      // number of bytes in string
				wcsCode,  // address of wide-character buffer
				32        // size of buffer
			);
			
			pDoc->LoadFont((int)wcsCode[0]);
			ShowWindow(SW_HIDE);
			EnableWindow(FALSE);
			pView->m_ShowEdit = FALSE;
			pView->SetFocus();
			
			delete [] wcsCode;
		}
		else
		{
			SetWindowText("");
			EnableWindow(TRUE);
			ShowWindow(SW_SHOW);
			pView->m_ShowEdit = TRUE;
		}
    }else if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE )
	{
		CDeformView *pView = (CDeformView *)GetParent();
		EnableWindow(FALSE);
		ShowWindow(SW_HIDE);
		pView->m_ShowEdit = FALSE;
		pView->SetFocus();
	}
    
	return CEdit::PreTranslateMessage(pMsg);
}
