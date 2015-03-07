// ControlPoint.h: interface for the CControlPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTROLPOINT_H__1DABD542_0B2A_4992_8698_DEFFF4E2B706__INCLUDED_)
#define AFX_CONTROLPOINT_H__1DABD542_0B2A_4992_8698_DEFFF4E2B706__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

////////////////////////////////////////////////////////////////////////
// point type
#define TAG_LINE	1
#define TAG_CURVE	0
#define POINT_SIZE	2

#define DRAW_NONE		0
#define DRAW_LINE_ONLY	1
#define DRAW_BOTH		2

//////////////////////////////////////////////////////////////////////////
// contour direction (clock-wise/counter clock-wise)
#define DIRECTION_CW	0
#define DIRECTION_CCW	1	// default value

struct MyPoint
{
	int x;	
	int y;
	short tag;		// contour 상에 있으면 1
//	int num;		// 순서 
};

class CControlPoint
{
public:	
	void SetDirection();
	void SetDrawMode( int mode ){ m_DrawMode = mode; };
	void SetDrawMode(){ m_DrawMode++; m_DrawMode %= 3;};

	int FindRegion( CPoint start, CPoint end, int selected[] );
	void SetPointAt(int index, int x, int y, int tag);
	MyPoint* GetPointPtr(int x, int y);
	int Add(int x, int y, int tag);
	
	void Init();
	CControlPoint();
	CControlPoint( int size );
	virtual ~CControlPoint();

	int Find(CPoint point);
	void DrawControlPoints(CDC *pDC);

	void SetPen( CDC *pDC, int width, COLORREF color);
	void SetBrush(CDC *pDC, COLORREF color);

	//////////////////////////////////////////////////////////////////////////
	// Add - in
	CPoint m_Min;
	CPoint m_Max;

	///////////////////////////////////////////////////////////////////////
	// data
	MyPoint *m_Buffer;		// 데이터의 list
	int m_Size;				// 실제 data의 수
	int m_ArraySize;		// 할당된 array의 크기
	BOOL m_DrawMode;		// 그리기 모드
	int m_Direction;		// cw or ccw

	//////////////////////////////////////////////////////////////////////////
	// 그리기 관련 함수
	void DrawGlyph(CDC *pDC);
	void MakeBezierCurve(CDC *pDC, int start, int end, double u);
	void MakeBezierCurve2(CDC *pDC, int start, int end,int last, double u);
	int Factorial(int n);
	double Power(double n, int k);
private:
	BOOL NearPoint( CPoint &point, int x, int y);
	void ArrayDouble();

	////////////////////////////////////////////////////////////////////////
	// 그리기 관련
	CPen *m_pen, *m_oldPen;
	CBrush *m_brush, *m_oldBrush;

	void ReleaseBrush(CDC *pDC);
	void ReleasePen(CDC *pDC);

};

#endif // !defined(AFX_CONTROLPOINT_H__1DABD542_0B2A_4992_8698_DEFFF4E2B706__INCLUDED_)
