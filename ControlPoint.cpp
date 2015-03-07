  // ControlPoint.cpp: implementation of the CControlPoint class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Deform.h"
#include "ControlPoint.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CControlPoint::CControlPoint()
{
	m_Size = 0;
	m_ArraySize = 64;
	m_Buffer = new MyPoint[m_ArraySize];

	m_pen = NULL;
	m_brush = NULL;
	m_DrawMode = DRAW_BOTH;

}

CControlPoint::CControlPoint( int size )
{
	m_Size = 0;
	m_ArraySize = size;
	m_Buffer = new MyPoint[m_ArraySize];

	m_pen = NULL;
	m_brush = NULL;
	m_DrawMode = DRAW_BOTH;
}

// destructor
CControlPoint::~CControlPoint()
{
	delete [] m_Buffer;
}

void CControlPoint::Init()
{
	delete[] m_Buffer;	

	m_Size = 0;
	m_ArraySize = 64;
	m_Buffer = new MyPoint[m_ArraySize];

	m_DrawMode = DRAW_BOTH;
}

void CControlPoint::ArrayDouble()
{
	MyPoint *newArray;
	newArray = new MyPoint[m_ArraySize*2];

	m_ArraySize = m_ArraySize * 2;
	
	for( int i = 0; i < m_Size; i++)
		newArray[i] = m_Buffer[i];

	// 메모리 해제		
	delete [] m_Buffer;

	// 새로운 array 받음
	m_Buffer = newArray;	
}


// return last index
int CControlPoint::Add( int x, int y, int tag)
{
	if( m_ArraySize == m_Size ) ArrayDouble();

	m_Buffer[m_Size].x = x;
	m_Buffer[m_Size].y = y;
	m_Buffer[m_Size].tag = tag%2;

	m_Size++;

	return m_Size;
}


/////////////////////////////////////////////////////////////////////////////////
// 그리기 관련 함수들


void CControlPoint::ReleasePen(CDC *pDC)
{
	if( m_pen == NULL)return;
	pDC->SelectObject(m_oldPen);
	delete m_pen;
	m_pen = NULL;
}

void CControlPoint::ReleaseBrush(CDC *pDC)
{
	if( m_brush == NULL)return;
	pDC->SelectObject(m_oldBrush);
	delete m_brush;
	m_brush = NULL;
}

void CControlPoint::SetBrush(CDC *pDC, COLORREF color)
{
	if( m_brush != NULL) delete m_brush;
	m_brush = new CBrush;
	m_brush->CreateSolidBrush(color);
	m_oldBrush = pDC->SelectObject(m_brush);
}

void CControlPoint::SetPen(CDC *pDC, int width, COLORREF color)
{
	if( m_pen != NULL) delete m_pen;
	m_pen = new CPen;
	m_pen->CreatePen(PS_SOLID, width, color);
	m_oldPen = pDC->SelectObject(m_pen);
}

////////////////////////////////////////////////////////////////////////////

MyPoint* CControlPoint::GetPointPtr(int x, int y)
{
	for (int i=0; i<m_Size; i++)
		if (m_Buffer[i].x == x && m_Buffer[i].y == y)
			return &m_Buffer[i];

	return &m_Buffer[i];
}

void CControlPoint::SetPointAt(int index, int x, int y, int tag)
{
	m_Buffer[index].x = x;
	m_Buffer[index].y = y;
	m_Buffer[index].tag = tag;
}


// 컨트롤 포인트 출력하기
void CControlPoint::DrawControlPoints(CDC *pDC)
{
	if( m_DrawMode == DRAW_NONE ) return;
	int i;

	CBrush nullBrush, *oldBrush;
	nullBrush.CreateStockObject(NULL_BRUSH);
	oldBrush = (CBrush *)pDC->SelectObject(&nullBrush);
	
	if( m_Buffer[0].tag )	// 1 : fixed - points on boundary
	{
		SetPen(pDC, 2, RGB(0,0,0));
		pDC->Rectangle( m_Buffer[0].x - POINT_SIZE-1, m_Buffer[0].y - POINT_SIZE-1,
			m_Buffer[0].x + POINT_SIZE+1, m_Buffer[0].y + POINT_SIZE+1 );		
	}

	if( m_Buffer[1].tag )	// 1 : fixed - points on boundary
	{
		SetPen(pDC, 2, RGB(127,127,127));
		pDC->Rectangle( m_Buffer[1].x - POINT_SIZE-1, m_Buffer[1].y - POINT_SIZE-1,
			m_Buffer[1].x + POINT_SIZE+1, m_Buffer[1].y + POINT_SIZE+1 );		
	}
	else
	{
		SetPen(pDC, 2, RGB(127,127,127));
		pDC->Ellipse(m_Buffer[1].x - POINT_SIZE-1, m_Buffer[1].y - POINT_SIZE-1,
			m_Buffer[1].x + POINT_SIZE+1, m_Buffer[1].y + POINT_SIZE+1 );
	}

	for( i = 2; i < m_Size; i++)
	{

		if( m_Buffer[i].tag )	// 1 : fixed - points on boundary
		{
			SetPen(pDC, 2, RGB(0,0,255));
			pDC->Rectangle( m_Buffer[i].x - POINT_SIZE-1, m_Buffer[i].y - POINT_SIZE-1,
				m_Buffer[i].x + POINT_SIZE+1, m_Buffer[i].y + POINT_SIZE+1 );		
		}
		else if( m_DrawMode == DRAW_LINE_ONLY ) continue;
		else
		{
			SetPen(pDC, 2, RGB(255,0,0));
			pDC->Ellipse(m_Buffer[i].x - POINT_SIZE-1, m_Buffer[i].y - POINT_SIZE-1,
				m_Buffer[i].x + POINT_SIZE+1, m_Buffer[i].y + POINT_SIZE+1 );
		}
	}

	pDC->SelectObject(oldBrush);
	nullBrush.DeleteObject();
	ReleasePen( pDC);
}

//
// set contour direction; clockwise or counter clockwise
void CControlPoint::SetDirection()
{
	int i, angle, num;
	int angleSum = 0;
	double norm1, norm2, norm3;
	CPoint p1, p2, v1, v2, v;		// points and vectors

	p2.x = m_Buffer[0].x;
	p2.y = m_Buffer[0].y;
	num = 0;
	for( i = 1; i < m_Size; i++)
	{
		if( !m_Buffer[i].tag ) continue;
		p1.x = m_Buffer[i].x;
 		p1.y = m_Buffer[i].y; 

		num++;
		if( num == 1 )
		{
			v1.x = p1.x - p2.x;
			v1.y = p1.y - p2.y;
			p2 = p1;
			v = v1;
			continue;
		}
	
		//////////////////////////////////////////////////////////////////////////
		// 여기서부터 계산
		v2.x = p1.x - p2.x;
		v2.y = p1.y - p2.y;

		norm1 = sqrt((double)v1.x*v1.x + v1.y*v1.y);
		norm2 = sqrt((double)v2.x*v2.x + v2.y*v2.y);
		norm3 = (double)v1.x*v2.y - v1.y*v2.x;
		
		angle = (int)(asin(fabs(norm3)/norm1/norm2)*180/3.141592);
		if( norm3 > 0 ) angleSum -= angle;
		else angleSum += angle;
		
		//////////////////////////////////////////////////////////////////////////
		// 후처리
		p2 = p1;
		v1 = v2;
	}
	p1.x = m_Buffer[0].x;
	p1.y = m_Buffer[0].y;

	v2.x = p1.x - p2.x;
	v2.y = p1.y - p2.y;

	norm1 = sqrt((double)v1.x*v1.x + v1.y*v1.y);
	norm2 = sqrt((double)v2.x*v2.x + v2.y*v2.y);
	norm3 = (double)v1.x*v2.y - v1.y*v2.x;
	angle = (int)(asin(fabs(norm3)/norm1/norm2)*180/3.141592);
	if( norm3 > 0 ) angleSum -= angle;
	else angleSum += angle;

	//////////////////////////////////////////////////////////////////////////
	
	norm1 = sqrt((double)v2.x*v2.x + v2.y*v2.y);
	norm2 = sqrt((double)v.x*v.x + v.y*v.y);
	norm3 = (double)v2.x*v.y - v2.y*v.x;
	angle = (int)(asin(fabs(norm3)/norm1/norm2)*180/3.141592);
	if( norm3 > 0 ) angleSum -= angle;
	else angleSum += angle;
	
	if( angleSum > 0 ) m_Direction = DIRECTION_CCW;
	else m_Direction = DIRECTION_CW;
}


//////////////////////////////////////////////////////////////////////////
// 그리기 관련 
void CControlPoint::DrawGlyph(CDC *pDC)
{
	int contour = 0;
	int start, end;
	//////////////////////////////////////////////////////////////////////////////
	// 외곽선 그리기
	double INCREMENT = 0.01;

	int bezierStart, bezierEnd;
	SetPen(pDC, 1, RGB(0,0,0));
	int pointNum;				// bezier curve : controll point #

	pointNum = 1;

	// 새로운 contour의 시작
	start = 0;
	end = m_Size-1;

	pDC->MoveTo( m_Buffer[start].x ,m_Buffer[start].y);

	double u;	// for bezier curve's indexing
	//////////////////////////////////////////////////////////////////

	bezierStart = start;
	while(true)
	{
		if( bezierStart != start && bezierStart == end ) bezierEnd = start;
		else bezierEnd = bezierStart+1;
		
		while( true )
		{
			if( m_Buffer[bezierEnd].tag == 1 ) break;	// 바로 직선을 그으면 됨
			if( bezierEnd == start ) break;		// 마지막 point까지 도달했음
			if( bezierEnd == end )  // contour 마지막 point 까지 도달했음
			{
				bezierEnd = start;
				pointNum++;
				break;
			}
			else bezierEnd++;
			pointNum++;
		}

		if( bezierEnd == start )
		{
			if( pointNum == 1 ) pDC->LineTo(m_Buffer[bezierEnd].x, m_Buffer[bezierEnd].y);
			else
			{
				for( u = 0.0; u <= 1.1; u += INCREMENT )
					MakeBezierCurve2(pDC, bezierStart, end, start, u );
				pointNum = 1;
			}
			break;
		}
		else if( pointNum == 1 )
		{
			pDC->LineTo(m_Buffer[bezierEnd].x,m_Buffer[bezierEnd].y);
			bezierStart = bezierEnd;
		}
		else
		{
			for( u = 0.0; u <= 1.1; u += INCREMENT )
				MakeBezierCurve(pDC, bezierStart, bezierEnd, u );
			bezierStart = bezierEnd;
			pointNum = 1;
		}

	}// end of while ; for some bezier' controll points


	ReleasePen(pDC);
	ReleaseBrush(pDC);
	
}


void CControlPoint::MakeBezierCurve(CDC *pDC, int start, int end, double u)
{
	if( u > 1.0) u = 1.0;
	if( u == 0.0 )
	{
		//pDC->MoveTo(pDoc->m_point[start].x, pDoc->m_point[start].y );
		return;
	}
	if( u == 1.0 )
	{
		pDC->LineTo(m_Buffer[end].x,m_Buffer[end].y);
		return;
	}
	int pointNum = end - start;
	
	int nFact = Factorial( pointNum );

	double x,y;			// estimated point coordinates
	x = 0.0;
	y = 0.0;

	for( int i = 0; i <= pointNum; i++)
	{
		x += m_Buffer[start+i].x * nFact / Factorial(i) / Factorial(pointNum-i)
			* Power( u, i ) * Power( 1.0-u, pointNum-i);
		y += m_Buffer[start+i].y * nFact / Factorial(i) / Factorial(pointNum-i)
			* Power( u, i ) * Power( 1.0-u, pointNum-i);
	}

	pDC->LineTo((int)(x+.5),(int)(y+.5));
}

void CControlPoint::MakeBezierCurve2(CDC *pDC, int start, int end,int last, double u)
{
	if( u > 1.0) u = 1.0;
	if( u == 0.0 )
	{
		//pDC->MoveTo(pDoc->m_point[last].x, pDoc->m_point[last].y );
		return;
	}
	if( u == 1.0 )
	{
		pDC->LineTo(m_Buffer[last].x,m_Buffer[last].y);
		return;
	}
	int pointNum = end - start+1;
	
	int nFact = Factorial( pointNum );

	double x,y;			// estimated point coordinates
	x = 0.0;
	y = 0.0;

	for( int i = 0; i < pointNum; i++)
	{
		x += m_Buffer[start+i].x * nFact / Factorial(i) / Factorial(pointNum-i)
			* Power( u, i ) * Power( 1.0-u, pointNum-i);
		y += m_Buffer[start+i].y * nFact / Factorial(i) / Factorial(pointNum-i)
			* Power( u, i ) * Power( 1.0-u, pointNum-i);
	}

	x += m_Buffer[last].x * Power( u, pointNum );
	y += m_Buffer[last].y * Power( u, pointNum );

	pDC->LineTo((int)(x+.5),(int)(y+.5));
}

//
// return factorial number
int CControlPoint::Factorial(int n)
{
	if ( n == 0 || n == 1 ) return 1;
	
	int result = 1;
	for( int i = 2; i <= n; i++)
		result = result * i;
	return result;
}

double CControlPoint::Power(double n, int k)
{
	if( n == 0 ) return 1;
	if( k == 0 ) return 1;
	if( k == 1 ) return n;

	double result = n;

	for( int i = 1; i < k; i++)
		result = result * n;
	return result;
}