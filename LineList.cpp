   // LineList.cpp: implementation of the CLineList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Deform.h"
#include "LineList.h"

#include <limits.h>
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLineList::CLineList()
{
	m_Size = 0;
	m_Buffer = new MyLine[128];
	m_BufferSize = 128;

	m_pen = NULL;
	m_brush = NULL;
}

CLineList::~CLineList()
{
	delete [] m_Buffer;
}

// 
// 초기화
void CLineList::Init()
{
	m_Size = 0;
	delete [] m_Buffer;
	m_Buffer = new MyLine[128];
	m_BufferSize = 128;
}

//
// 버퍼 확장
void CLineList::ArrayDouble()
{
	MyLine *temp = new MyLine[m_BufferSize*2];
	for( int i = 0; i < m_Size; i++)
		temp[i] = m_Buffer[i];

	delete []m_Buffer;
	m_Buffer = temp;
	m_BufferSize *= 2;
}

//
// 데이터 추가
int CLineList::Add(CPoint& p1, CPoint& p2, int contour, int curveNum, CPoint& cPoint1, CPoint& cPoint2)
{
	if( p1.x == p2.x && p1.y == p2.y ) return m_Size;
	if( m_Size == m_BufferSize ) ArrayDouble();

	/* 자리 바꾸기 */
	//if( p1.x == p2.x)
	/*             */

	m_Buffer[m_Size].p1 = p1;
	m_Buffer[m_Size].p2 = p2;
	
	// 기울기 구하기
	double radian;
	if( p1.x == p2.x ) m_Buffer[m_Size].angle = 90;
	else if( p1.y == p2.y ) m_Buffer[m_Size].angle = 0;
	else
	{
		 radian = (double)(atan(((double)p1.y-p2.y)/(p1.x - p2.x)));
		 m_Buffer[m_Size].angle = (int)(radian*180/PI);
	}
	m_Buffer[m_Size].used = FALSE;

	/* for angle of curve */
	if( curveNum == 0 )
	{
		m_Buffer[m_Size].curveAngle1 = m_Buffer[m_Size].angle;
		m_Buffer[m_Size].curveAngle2 = m_Buffer[m_Size].angle;
		m_Buffer[m_Size].curveNum = 0;
		m_Buffer[m_Size].cp1 = p2;
		m_Buffer[m_Size].cp2 = p1;
	}
	else if( curveNum == 1 )
	{
		m_Buffer[m_Size].curveAngle1 = GetAngle(p1, cPoint1); // angle 1
		m_Buffer[m_Size].curveAngle2 = GetAngle(cPoint1, p2 ); // angle 2
		m_Buffer[m_Size].curveNum = 1;
		m_Buffer[m_Size].cp1 = cPoint1;
		m_Buffer[m_Size].cp2 = cPoint1;
	}
	else // if( curveNum == 2 )
	{
		m_Buffer[m_Size].curveAngle1 = GetAngle(p1, cPoint1); // angle 1
		m_Buffer[m_Size].curveAngle2 = GetAngle(cPoint2, p2); // angle 2
		m_Buffer[m_Size].curveNum = 2;
		m_Buffer[m_Size].cp1 = cPoint1;
		m_Buffer[m_Size].cp2 = cPoint2;
	}

	return ++m_Size;
}

int CLineList::GetAngle( CPoint& p1, CPoint& p2)
{
	int angle;
	double radian;
	if( p1.x == p2.x ) angle = 90;
	else if( p1.y == p2.y ) angle = 0;
	else
	{
		 radian = (double)(atan(((double)p1.y-p2.y)*3/(p1.x - p2.x)));
		 angle = (int)(radian*180/PI);
	}

	return angle;
}
/////////////////////////////////////////////////////////////////////////////////
// 그리기 관련 함수들

void CLineList::ReleasePen(CDC *pDC)
{
	if( m_pen == NULL) return;
	pDC->SelectObject(m_oldPen);
	delete m_pen;
	m_pen = NULL;
}

void CLineList::ReleaseBrush(CDC *pDC)
{
	if( m_brush == NULL)return;
	pDC->SelectObject(m_oldBrush);
	delete m_brush;
	m_brush = NULL;
}

void CLineList::SetBrush(CDC *pDC, COLORREF color)
{
	if( m_brush != NULL) delete m_brush;
	m_brush = new CBrush;
	m_brush->CreateSolidBrush(color);
	m_oldBrush = pDC->SelectObject(m_brush);
}

void CLineList::SetPen(CDC *pDC, int width, COLORREF color)
{
	if( m_pen != NULL) delete m_pen;
	m_pen = new CPen;
	m_pen->CreatePen(PS_SOLID, width, color);
	m_oldPen = pDC->SelectObject(m_pen);
}

void CLineList::Draw(CDC *pDC)
{
	SetPen(pDC, 1, RGB(0,255,0));
	for( int i = 0; i < m_Size; i++)
	{
		pDC->MoveTo(m_Buffer[i].p1);
		pDC->LineTo(m_Buffer[i].p2);
	}
	ReleasePen(pDC);
}

//
// slop(key) 순으로 소팅 (selection sorting)
void CLineList::Sort()
{
	int i, j;
	MyLine line;
	for( i = 1; i < m_Size; i++)
	{
		for( j = i; j > 0; j-- )
		{
			if( m_Buffer[j].angle >= m_Buffer[j-1].angle ) break;
			// swap
			line = m_Buffer[j];
			m_Buffer[j] = m_Buffer[j-1];
			m_Buffer[j-1] = line;
		}
	}
}

//
// 두개의 line segment 가 일정 거리 내에 있는가 ?
// return TRUE, if two line-segments lie on short distance
// return FALSE, otherwise
int CLineList::GetRelation(int line1, int line2)
{
	// 0. U 에 대한 검사 
	//if( !IsInner(line1, line2) ) return LINE_AWAY;

	// 1. 거리 비교 
	int temp;
	int distance = GetDistance(line1, line2);
	temp = GetDistance(line2, line1);
	if( distance > temp ) distance = temp;
	if( distance > THRESHOLD_DISTANCE_MAX*THRESHOLD_DISTANCE_MAX ) return LINE_AWAY;
	
	// 2. 각도 비교
	int angle;

	angle = AngleDotProduct(line1, line2);

	if( angle >= THRESHOLD_ANGLE)
	{
		if( angle < PRE_CONNECT_ANGLE && distance == 0 ) return GetCurveRelation(line1, line2);

		return LINE_AWAY; // 떨어져 있음
	}
	
	// 방향이 같은 지 
	BOOL direction = CompareDirection(line1,line2);

	if( distance == 0 /*&& !direction */) return LINE_CONNECTED;
	if( distance == 0 ) return LINE_AWAY;

	if( !direction && !CompareDirection(line2,line1) ) return LINE_OPPOSITE;
	return LINE_AWAY;
}

int CLineList::GetCurveRelation(int line1, int line2)
{
	CPoint v1, v2;
	if( m_Buffer[line1].p1 == m_Buffer[line2].p2)
	{
		v1.x = m_Buffer[line1].cp1.x - m_Buffer[line1].p1.x;
		v1.y = m_Buffer[line1].cp1.y - m_Buffer[line1].p1.y;
		

		v2.x = m_Buffer[line2].p2.x - m_Buffer[line2].cp2.x;
		v2.y = m_Buffer[line2].p2.y - m_Buffer[line2].cp2.y;
	}
	else if(m_Buffer[line1].p2 == m_Buffer[line2].p1) 
	{
		//angle = abs(m_Buffer[line1].curveAngle2 - m_Buffer[line2].curveAngle1);
		v1.x = m_Buffer[line1].p2.x - m_Buffer[line1].cp2.x;
		v1.y = m_Buffer[line1].p2.y - m_Buffer[line1].cp2.y;

		v2.x = m_Buffer[line2].cp1.x - m_Buffer[line2].p1.x;
		v2.y = m_Buffer[line2].cp1.y - m_Buffer[line2].p1.y;
	}
	else return LINE_AWAY;

	// cross product
	int norm1, norm2;
	norm1 = v1.x*v1.x + v1.y*v1.y;
	norm2 = v2.x*v2.x + v2.y*v2.y;

	double radian;
	int product = abs(v1.x*v2.x + v1.y*v2.y);
	
	radian = acos( product/sqrt(norm1*norm2));
	int angle = (int)(radian*180/PI);

	if( angle < CONNECT_ANGLE ) return LINE_CONNECTED;

	return LINE_AWAY;
}

void CLineList::SetXY(int &x, int &y, int i, int j, int position)
{
	if( position == 1 )
	{
		x = (m_Buffer[i].p1.x + m_Buffer[j].p1.x)/2;
		y = (m_Buffer[i].p1.y + m_Buffer[j].p1.y)/2;
	}
	else if( position == 2)
	{
		x = (m_Buffer[i].p1.x + m_Buffer[j].p2.x)/2;
		y = (m_Buffer[i].p1.y + m_Buffer[j].p2.y)/2;
	}
	else if( position == 3)
	{
		x = (m_Buffer[i].p2.x + m_Buffer[j].p1.x)/2;
		y = (m_Buffer[i].p2.y + m_Buffer[j].p1.y)/2;
	}
	else
	{
		x = (m_Buffer[i].p2.x + m_Buffer[j].p2.x)/2;
		y = (m_Buffer[i].p2.y + m_Buffer[j].p2.y)/2;
	}
}

//
// 직선 사이의 거리 
// return ; distance ^2
int CLineList::GetDistance(int line1, int line2)
{
	int distance = 0, temp;
	float U;
	CPoint intersection;
	BOOL in = FALSE;
	
	int lineLengthSquare = // line length ^2
		(m_Buffer[line1].p1.x-m_Buffer[line1].p2.x)*(m_Buffer[line1].p1.x-m_Buffer[line1].p2.x)+
		(m_Buffer[line1].p1.y-m_Buffer[line1].p2.y)*(m_Buffer[line1].p1.y-m_Buffer[line1].p2.y);
	
	CPoint p1, p2, p3;
	p1 = m_Buffer[line1].p1;
	p2 = m_Buffer[line1].p2;

	// line 1 ----- p1
	p3 = m_Buffer[line2].p1;
	U = ((float)( p3.x - p1.x ) * ( p2.x - p1.x ) +
        ( p3.y - p1.y ) * ( p2.y - p1.y )) / lineLengthSquare;

	intersection.x = (long)(p1.x + U * ( p2.x - p1.x ));
    intersection.y = (long)(p1.y + U * ( p2.y - p1.y ));

	distance = (p3.x-intersection.x)*(p3.x-intersection.x)+
				(p3.y-intersection.y)*(p3.y-intersection.y);

	if( U >= 0.0f && U <= 1.0f ) in = TRUE;
	// line 1 ----- p2
	p3 = m_Buffer[line2].p2;
	U = ((float)( p3.x - p1.x ) * ( p2.x - p1.x ) +
        ( p3.y - p1.y ) * ( p2.y - p1.y )) / lineLengthSquare;

	intersection.x = (long)(p1.x + U * ( p2.x - p1.x ));
    intersection.y = (long)(p1.y + U * ( p2.y - p1.y ));

	temp = (p3.x-intersection.x)*(p3.x-intersection.x)+
				(p3.y-intersection.y)*(p3.y-intersection.y);
	if( U >= 0.0f && U <= 1.0f )
	{
		if( in == TRUE && distance > temp ) distance = temp;
		if( in == FALSE ) return temp;
		return distance;
	}
	if( in == TRUE  ) return distance; // closest point does not fall within the line segment
	return 10000000;
}

int CLineList::GetDistance2(int line1, int line2)
{
	int distance, temp;
	int length;
	CPoint p1, p2, p3;
	p1 = m_Buffer[line1].p1;
	p2 = m_Buffer[line1].p2;

	length = (p2.x-p1.x)*(p2.x-p1.x)+(p2.y-p1.y)*(p2.y-p1.y);
	// line 1 ----- p1
	p3 = m_Buffer[line2].p1;



	distance = (p2.x-p1.x)*(p1.y-p3.y)-(p1.x-p3.x)*(p2.y-p1.y);
	distance = distance*distance/length;
	// line 1 ----- p2
	p3 = m_Buffer[line2].p2;

	temp = (p2.x-p1.x)*(p1.y-p3.y)-(p1.x-p3.x)*(p2.y-p1.y);
	temp = temp*temp/length;
	if( distance > temp ) distance = temp;
	
	return distance;
}

int CLineList::MinDistance(CPoint &sp1, CPoint &sp2, CPoint &ep1, CPoint &ep2 )
{
	int distance, temp;
	distance = (sp1.x - ep1.x)*(sp1.x - ep1.x)+(sp1.y - ep1.y)*(sp1.y - ep1.y);
	temp = (sp1.x - ep2.x)*(sp1.x - ep2.x)+(sp1.y - ep2.y)*(sp1.y - ep2.y);
	if( distance > temp ) distance = temp;

	temp = (sp2.x - ep1.x)*(sp2.x - ep1.x)+(sp2.y - ep1.y)*(sp2.y - ep1.y);
	if( distance > temp ) distance = temp;

	temp = (sp2.x - ep2.x)*(sp2.x - ep2.x)+(sp2.y - ep2.y)*(sp2.y - ep2.y);
	if( distance > temp ) distance = temp;

	return distance;
}

BOOL CLineList::IsInner(int line1, int line2)
{
	float U;
	CPoint intersection;
	
	int lineLengthSquare = // line length ^2
		(m_Buffer[line1].p1.x-m_Buffer[line1].p2.x)*(m_Buffer[line1].p1.x-m_Buffer[line1].p2.x)+
		(m_Buffer[line1].p1.y-m_Buffer[line1].p2.y)*(m_Buffer[line1].p1.y-m_Buffer[line1].p2.y);
	
	CPoint p1, p2, p3;
	p1 = m_Buffer[line1].p1;
	p2 = m_Buffer[line1].p2;

	// line 1 ----- p1
	p3 = m_Buffer[line2].p1;
	U = (float)( p3.x - p1.x ) * ( p2.x - p1.x ) +
        ( p3.y - p1.y ) * ( p2.y - p1.y ) / lineLengthSquare;

	if( U >= 0.0f && U <= 1.0f ) return TRUE;
	
	// line 1 ----- p2
	p3 = m_Buffer[line2].p2;
	U = (float)( p3.x - p1.x ) * ( p2.x - p1.x ) +
        ( p3.y - p1.y ) * ( p2.y - p1.y ) / lineLengthSquare;

	if( U >= 0.0f && U <= 1.0f ) return TRUE;
	return FALSE;
}

//
// 방향 비교 (화면상엔 오른손 법칙, 좌표상 왼손 법칙)
BOOL CLineList::CompareDirection(int line1, int line2)
{
	CPoint v1, v2;

	v1.x = m_Buffer[line1].p1.x - m_Buffer[line1].p2.x;
	v1.y = m_Buffer[line1].p1.y - m_Buffer[line1].p2.y;
	
	v2.x = m_Buffer[line2].p1.x - m_Buffer[line1].p2.x;
	v2.y = m_Buffer[line2].p1.y - m_Buffer[line1].p2.y;

	long result = v1.x*v2.y - v1.y*v2.x;
	if( result == 0 )
	{
		int lineLengthSquare = // line length ^2
			(m_Buffer[line1].p1.x-m_Buffer[line1].p2.x)*(m_Buffer[line1].p1.x-m_Buffer[line1].p2.x)+
			(m_Buffer[line1].p1.y-m_Buffer[line1].p2.y)*(m_Buffer[line1].p1.y-m_Buffer[line1].p2.y);
		CPoint p1, p2, p3;
		p1 = m_Buffer[line1].p1;
		p2 = m_Buffer[line1].p2;

		// line 1 ----- p1
		p3 = m_Buffer[line2].p2;
		float U = ((float)( p3.x - p1.x ) * ( p2.x - p1.x ) +
			( p3.y - p1.y ) * ( p2.y - p1.y )) / lineLengthSquare;

		if( U >= 0.0f && U <= 1.0f ) return FALSE;
		else return TRUE;
	}
	else if( result > 0 ) return FALSE;
	return TRUE; // 화면상의 시계방향
}

//
// if index is 1, search p1
// if index is 2, search p2
int CLineList::GetLine(CPoint& p, int index)
{
	int i;
	for( i = 0; i < m_Size; i++)
	{
		if( index == 1)
		{
			if( m_Buffer[i].p1 == p ) return i;
		}
		else // if( index == 2)
		{
			if( m_Buffer[i].p2 == p ) return i;
		}
	}
	return -1;
}

//
int CLineList::FindLineIndex(int line, int index)
{
	int i;
	if( index == 2 ) // find p2
	{
		for( i = 0; i < m_Size; i++)
			if (m_Buffer[line].p1 == m_Buffer[i].p2) return i;

	}
	else // if( index == 1) // find p1
	{
		for( i = 0; i < m_Size; i++)
			if (m_Buffer[line].p2 == m_Buffer[i].p1) return i;
	}
	
	return -1;
}

BOOL CLineList::CrossProduct(int line1, int line2)
{
	CPoint v1, v2;

	v1.x = m_Buffer[line1].p2.x - m_Buffer[line1].p1.x;
	v1.y = m_Buffer[line1].p2.y - m_Buffer[line1].p1.y;
	
	v2.x = m_Buffer[line2].p2.x - m_Buffer[line2].p1.x;
	v2.y = m_Buffer[line2].p2.y - m_Buffer[line2].p1.y;

	if( (v1.x*v2.y - v1.y*v2.x) > 0 ) return FALSE;
	return TRUE; // 화면상의 시계방향
}

int CLineList::AngleDotProduct(int line1, int line2)
{
	CPoint v1, v2;

	v1.x = m_Buffer[line1].p2.x - m_Buffer[line1].p1.x;
	v1.y = m_Buffer[line1].p2.y - m_Buffer[line1].p1.y;

	v2.x = m_Buffer[line2].p2.x - m_Buffer[line2].p1.x;
	v2.y = m_Buffer[line2].p2.y - m_Buffer[line2].p1.y;

	// cross product
	double norm1, norm2;
	norm1 = sqrt(v1.x*v1.x + v1.y*v1.y);
	norm2 = sqrt(v2.x*v2.x + v2.y*v2.y);

	double radian;
	int product = abs(v1.x*v2.x + v1.y*v2.y);
	double value = product/(norm1*norm2);
	radian = acos( value );
	int angle = (int)(radian*180/PI);
	if( value < 0.00001 ) return 90;

	return angle;

}