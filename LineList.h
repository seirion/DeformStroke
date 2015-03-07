// LineList.h: interface for the CLineList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINELIST_H__B8F4F738_CCB0_4C20_BDA9_3DDFFDB937D2__INCLUDED_)
#define AFX_LINELIST_H__B8F4F738_CCB0_4C20_BDA9_3DDFFDB937D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaboImage.h"

#define PI 3.141592
#define THRESHOLD_ANGLE 20
#define THRESHOLD_DISTANCE_MIN 25
#define THRESHOLD_DISTANCE_MAX 36
#define PRE_CONNECT_ANGLE 65
#define CONNECT_ANGLE 10
     
#define LINE_AWAY		0
#define LINE_CONNECTED	1
#define LINE_SAME		2
#define LINE_OPPOSITE	4

struct MyLine {
	CPoint p1;
	CPoint p2;
	int contour;	/* source number */
	int angle;		// slope angle
	BOOL used;		// if the line is used to find stroke

	/* variables to store angle of curves */
	int curveAngle1;
	int curveAngle2;
	int curveNum; // the number of curve point
	CPoint cp1;		// curve points
	CPoint cp2;
};

class CLineList  
{
public:
	int FindLineIndex(int line, int index);
	void Sort();
	void Draw(CDC *pDC);
	int Add(CPoint& p1, CPoint& p2, int contour, int curveNum, CPoint& cPoint1, CPoint& cPoint2);
	void ArrayDouble();
	void Init();
	CLineList();
	virtual ~CLineList();

	void SetPen( CDC *pDC, int width, COLORREF color);
	void SetBrush(CDC *pDC, COLORREF color);

	int m_Size;
	int m_BufferSize;
	MyLine *m_Buffer;

	int Point_Point(CPoint &p1, CPoint &p2)
	{return (p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y);}; // distance between two points
	
	int MinDistance(CPoint &sp1, CPoint &sp2, CPoint &ep1, CPoint &ep2 );
	int GetDistance( int line1, int line2);
	int GetDistance2( int line1, int line2);
	BOOL IsInner(int line1, int line2);
	void SetXY(int& x, int&y, int i, int j, int position );
	int GetRelation(int line1, int line2);
	int GetLine(CPoint& p, int index);
	BOOL CrossProduct(int line1, int line2);
	int AngleDotProduct(int line1, int line2);
private:
	int GetAngle(CPoint& p1, CPoint& p2);
	BOOL CompareDirection(int line1, int line2);
	int GetCurveRelation(int line1, int line2);
	////////////////////////////////////////////////////////////////////////
	// 그리기 관련
	CPen *m_pen, *m_oldPen;
	CBrush *m_brush, *m_oldBrush;

	void ReleaseBrush(CDC *pDC);
	void ReleasePen(CDC *pDC);
};

#endif // !defined(AFX_LINELIST_H__B8F4F738_CCB0_4C20_BDA9_3DDFFDB937D2__INCLUDED_)
