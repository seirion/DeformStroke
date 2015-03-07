// DeformDoc.h : interface of the CDeformDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEFORMDOC_H__1F1157DD_8E68_42F0_B1C7_3378246E1AD1__INCLUDED_)
#define AFX_DEFORMDOC_H__1F1157DD_8E68_42F0_B1C7_3378246E1AD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////////
// inlcude files
#include <ft2build.h>
#include FT_FREETYPE_H
//////////////////////////////////////////////////////////////////////////

#include "ControlPoint.h"
#include "BaboImage.h"
#include "LineList.h"
#include "GroupList.h"
#include "StrokeList.h"

//////////////////////////////////////////////////////////////////////////
// define constants
#define BITMAP_WIDTH	500
#define BITMAP_HEIGHT	500

#define DRAW_MODE_NONE			0x0000
#define DRAW_MODE_BITMAP		0x0001
#define DRAW_MODE_LINE			0x0002
#define DRAW_MODE_STROKE		0x0003 //
#define DRAW_MODE_STROKE_ORDER	0x0004 //
#define DRAW_MODE_STROKE_EACH	0x0005
#define DRAW_MODE_STROKE_RESULT	0x0006

#define BRUSH_SIZE_MIN		20
#define BRUSH_SIZE_MAX		30

class CDeformDoc : public CDocument
{
protected: // create from serialization only
	CDeformDoc();
	DECLARE_DYNCREATE(CDeformDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeformDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	
	
	/* simple functions */
	void MakeBitmap(FT_Bitmap* bitmap, int x, int y);	// 2
	void MakeGroup();			// 3
	void CloseStroke();			// 3.1
	void MakeLineList();		// 4
	void MakeStroke();			// 5
	// 6. stroke merge
	void GroupSort();			// 7 (�׷쿡 ���� ����)
	void GroupNumbering();		//    (2��° �׷� ���� )
	void RelexationLabeling();	// 10
	void MakeLastSort();		// 11 ������ ����
	

	int GetOver( int stroke1, int stroke2);
	/* ������ ���� ���� */
	int CompareStroke(int stroke1, int stroke2 );	// ���� 1
	int CompareAway(int stroke1, int stroke2);		// ���� 1.1 (�ܼ� ��ġ ��)
	int CompareOverlap(int stroke1, int stroke2, int over); // ���� 1.2 (��ģ ȹ ��)
	BOOL FindCandidate( int stroke1, int stroke2);
	/* ������ ���� ���� ------------------------------------------------------------------- */


	/* �������� ȹ �� */
	int Compare_Horizontal_Vertical(int stroke1, int stroke2, int over);
	int Compare_Vertical_Leftfalling(int stroke1, int stroke2, int over);
	int Compare_Vertical_Rightfalling(int stroke1, int stroke2, int over);
	int Compare_Vertical_Turning(int stroke1, int stroke2, int over);
	int Compare_Leftfalling_Rightfalling(int stroke1, int stroke2, int over);
	/* �������� ȹ �� ------------------------------------------------------------------- */

	int GetParent( int stroke );
	void GetPointIndex(CPoint& p, int group, int& contour, int& index);
	BOOL PointNotUse( int group, int index, CPoint& p1, CPoint &p2);
	

	/* variables */
	int m_DrawMode;
	BOOL m_FontLoad; // ��Ʈ �ε� ����
	int m_FontSelection; // ��Ʈ ���� 

	/* class instances */
	int m_ContourNum;
	CControlPoint m_ControlPoint[32]; // outline ���� 
	CBaboImage m_FontImage; // bitmap �̹��� ����
	CLineList m_LineList[32];
	CLineList m_VirtualLineList;
	CGroupList m_GroupList;
	CStrokeList m_StrokeList[32];

	/* the result */
	CControlPoint **m_ResultStroke; // ��� ����Ʈ
	int *m_ResultNum;				// ��� ���� 

	MyStroke *m_LastResult[64]; // 1���� ������
	int m_LastNum;

	void Init();
	BOOL LoadFont(int code);
	virtual ~CDeformDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDeformDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFORMDOC_H__1F1157DD_8E68_42F0_B1C7_3378246E1AD1__INCLUDED_)
