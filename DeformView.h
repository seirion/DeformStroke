// DeformView.h : interface of the CDeformView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFORMVIEW_H__861B0A05_893C_427D_A96F_D15EB4E9A46D__INCLUDED_)
#define AFX_DEFORMVIEW_H__861B0A05_893C_427D_A96F_D15EB4E9A46D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
// inlcude files
#include <ft2build.h>
#include FT_FREETYPE_H
//////////////////////////////////////////////////////////////////////////

#include "DeformDoc.h"
#include "MyEdit.h"

class CMyEdit;

class CDeformView : public CFormView
{
protected: // create from serialization only
	CDeformView();
	DECLARE_DYNCREATE(CDeformView)

public:
	//{{AFX_DATA(CDeformView)
	enum { IDD = IDD_DEFORM_FORM };
	CComboBox	m_CtrlLast;
	CStatic	m_CtrlBitmap;
	CMyEdit	m_EditInput;
	CComboBox m_CtrlGroup;
	CComboBox m_CtrlStroke;
	//}}AFX_DATA

// Attributes
public:
	CDeformDoc* GetDocument();
	BOOL m_ShowEdit; // 입력창 공개 여부

	void InitCombo();
	void MakeStrokeCombo( int stroke );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeformView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDeformView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDeformView)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSelchangeComboGroup();
	afx_msg void OnSelchangeComboStroke();
	afx_msg void OnButtonOrder();
	afx_msg void OnButtonPoint();
	afx_msg void OnButtonDraw();
	afx_msg void OnButtonNew();
	afx_msg void OnSelchangeComboLast();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void DrawStrokeType( CDC *pDC);
	void DrawGuideLine(CDC *pDC);
	COLORREF m_Color[32];
	void SetColor();
	int m_Group;
	int m_Stroke;
};

#ifndef _DEBUG  // debug version in DeformView.cpp
inline CDeformDoc* CDeformView::GetDocument()
   { return (CDeformDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFORMVIEW_H__861B0A05_893C_427D_A96F_D15EB4E9A46D__INCLUDED_)
