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
	void GroupSort();			// 7 (그룹에 대한 소팅)
	void GroupNumbering();		//    (2번째 그룹 소팅 )
	void RelexationLabeling();	// 10
	void MakeLastSort();		// 11 마지막 소팅
	

	int GetOver( int stroke1, int stroke2);
	/* 마지막 소팅 관련 */
	int CompareStroke(int stroke1, int stroke2 );	// 소팅 1
	int CompareAway(int stroke1, int stroke2);		// 소팅 1.1 (단순 위치 비교)
	int CompareOverlap(int stroke1, int stroke2, int over); // 소팅 1.2 (겹친 획 비교)
	BOOL FindCandidate( int stroke1, int stroke2);
	/* 마지막 소팅 관련 ------------------------------------------------------------------- */


	/* 본격적인 획 비교 */
	int Compare_Horizontal_Vertical(int stroke1, int stroke2, int over);
	int Compare_Vertical_Leftfalling(int stroke1, int stroke2, int over);
	int Compare_Vertical_Rightfalling(int stroke1, int stroke2, int over);
	int Compare_Vertical_Turning(int stroke1, int stroke2, int over);
	int Compare_Leftfalling_Rightfalling(int stroke1, int stroke2, int over);
	/* 본격적인 획 비교 ------------------------------------------------------------------- */

	int GetParent( int stroke );
	void GetPointIndex(CPoint& p, int group, int& contour, int& index);
	BOOL PointNotUse( int group, int index, CPoint& p1, CPoint &p2);
	

	/* variables */
	int m_DrawMode;
	BOOL m_FontLoad; // 폰트 로드 여부
	int m_FontSelection; // 폰트 종류 

	/* class instances */
	int m_ContourNum;
	CControlPoint m_ControlPoint[32]; // outline 저장 
	CBaboImage m_FontImage; // bitmap 이미지 저장
	CLineList m_LineList[32];
	CLineList m_VirtualLineList;
	CGroupList m_GroupList;
	CStrokeList m_StrokeList[32];

	/* the result */
	CControlPoint **m_ResultStroke; // 결과 포인트
	int *m_ResultNum;				// 결과 개수 

	MyStroke *m_LastResult[64]; // 1차원 포인터
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
