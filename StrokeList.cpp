// StrokeList.cpp: implementation of the CStrokeList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Deform.h"
#include "StrokeList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStrokeList::CStrokeList()
{
	m_pen = NULL;
	m_brush = NULL;
	Init();
}

CStrokeList::~CStrokeList()
{
	Init();
}

void CStrokeList::Init()
{
	m_LineList = NULL;
	m_TextMode = 0;
	//////////////////////////////////////////////////////////////////////////
	int i, j;
	for( j = 0; j < 128; j++)
	{
	
		for( i = 0; i < 128; i++ )
		{
			m_Buffer[j].MainIndex[0][i] = -2;
			m_Buffer[j].MainIndex[1][i] = -2;
		}
//		for( i = 0; i < 32; i++ )
//		{
//			m_Buffer[j].SideIndex[0][i] = -2;
//			m_Buffer[j].SideIndex[1][i] = -2;
//		}

		m_Buffer[j].type = 0;
		m_Buffer[j].num_main = m_Buffer[j].num_oppsite = m_Buffer[j].num_side1 = m_Buffer[j].num_side2 = 0;
	}
	m_Size = 0;
}

void CStrokeList::MakeStroke()
{
	// line 하나씩 추가하면서 ... OTL
	int size = m_LineList->m_Size;
	int i, j, result;
	int repeat;
	for( i = 0; i < size; i++ )
	{
		if( m_LineList->m_Buffer[i].used ) continue;
		if( m_Buffer[m_Size].num_oppsite > 0 ) m_Size++;
		
		m_Buffer[m_Size].MainIndex[0][0] = i;
		m_Buffer[m_Size].num_main = 1;
		m_Buffer[m_Size].num_oppsite = 0;
		
		

		do {
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/	
			repeat = 0;
			for( j = 0; j < size; j++)
			{
				if( i == j ) continue;
				if( m_LineList->m_Buffer[j].used ) continue;
				
				result = BelongTo(j);

				// while for
				if( result == LINE_AWAY ) continue;
				if( result == LINE_CONNECTED || result == LINE_SAME )
				{
					m_Buffer[m_Size].MainIndex[0][m_Buffer[m_Size].num_main] = j;
					m_Buffer[m_Size].num_main++;
					m_LineList->m_Buffer[i].used = TRUE;
					m_LineList->m_Buffer[j].used = TRUE;
					repeat++;
				}
				else if( result == LINE_OPPOSITE )
				{	
					m_Buffer[m_Size].MainIndex[1][m_Buffer[m_Size].num_oppsite] = j;
					m_Buffer[m_Size].num_oppsite++;
					m_LineList->m_Buffer[i].used = TRUE;
					m_LineList->m_Buffer[j].used = TRUE;
					repeat++;
				}
			}
		/************************************************************************/
		/*                                                                      */
		/************************************************************************/
		} while(repeat);
	}
	if( m_Buffer[m_Size].num_oppsite > 0 ) m_Size++;
}


int CStrokeList::BelongTo(int index)
{
	/*
	#define LINE_AWAY		0
	#define LINE_CONNECTED	1
	#define LINE_SAME		2
	#define LINE_OPPOSITE	4
	*/
	int i, mainLine, result;
	// 1. main
	int size = m_Buffer[m_Size].num_main;
	for( i = 0; i < size; i++ )
	{
		
		mainLine = m_Buffer[m_Size].MainIndex[0][i];
		result = m_LineList->GetRelation(mainLine, index);
		if( result == LINE_CONNECTED ) return LINE_SAME;
//		else if( result == LINE_SAME ) return LINE_SAME;
		else if( result == LINE_OPPOSITE ) return LINE_OPPOSITE;
	}
	// 2. opposite
	size = m_Buffer[m_Size].num_oppsite;
	for( i = 0; i < size; i++ )
	{
		mainLine = m_Buffer[m_Size].MainIndex[1][i];
		result = m_LineList->GetRelation(mainLine, index);
		if( result == LINE_CONNECTED ) return LINE_OPPOSITE;
//		else if( result == LINE_SAME ) return LINE_OPPOSITE;
		else if( result == LINE_OPPOSITE ) return LINE_SAME;
	}
	return LINE_AWAY;
}


/////////////////////////////////////////////////////////////////////////////////
// 그리기 관련 함수들

void CStrokeList::ReleasePen(CDC *pDC)
{
	if( m_pen == NULL)return;
	pDC->SelectObject(m_oldPen);
	delete m_pen;
	m_pen = NULL;
}

void CStrokeList::ReleaseBrush(CDC *pDC)
{
	if( m_brush == NULL)return;
	pDC->SelectObject(m_oldBrush);
	delete m_brush;
	m_brush = NULL;
}

void CStrokeList::SetBrush(CDC *pDC, COLORREF color)
{
	if( m_brush != NULL) delete m_brush;
	m_brush = new CBrush;
	m_brush->CreateSolidBrush(color);
	m_oldBrush = pDC->SelectObject(m_brush);
}

void CStrokeList::SetPen(CDC *pDC, int width, COLORREF color)
{
	if( m_pen != NULL) delete m_pen;
	m_pen = new CPen;
	m_pen->CreatePen(PS_SOLID, width, color);
	m_oldPen = pDC->SelectObject(m_pen);
}

void CStrokeList::Draw(CDC *pDC)
{
	int i;
	BYTE r, g, b;
	for( i = 0; i < m_Size; i++)
	{
		if( i % 3 == 0 )
		{
			r = i * 5;
			g = b = 255;
		}
		else if( i % 3 == 1 )
		{
			g = i * 5;
			r = b = 255;
		}
		else 
		{
			b = i * 5;
			r = g = 255;
		}
		DrawStroke( pDC, i,  RGB(255-r,255-g,255-b) );
	}
}

void CStrokeList::DrawStroke(CDC *pDC, int index, COLORREF color )
{

	if( !m_Buffer[index].use ) return;
	if( m_Size <= 0	|| index < 0 || index >= m_Size ) return;
	int i, line;


	SetPen(pDC, index%3+1, color);
	CString str;
	CRect rect;
	// 1. main side
	int size = m_Buffer[index].num_main;
	for( i = 0; i < size; i++ )
	{
		line = m_Buffer[index].MainIndex[0][i];

		pDC->MoveTo(m_LineList->m_Buffer[line].p1);
		pDC->LineTo(m_LineList->m_Buffer[line].p2);

		if( m_TextMode == 1 )
		{
			str.Format("%d", i);
			rect.top = m_LineList->m_Buffer[line].p1.y - 15;
			rect.bottom = m_LineList->m_Buffer[line].p1.y;
			rect.left = m_LineList->m_Buffer[line].p1.x - 15;
			rect.right = m_LineList->m_Buffer[line].p1.x;
			pDC->DrawText(str, rect, 1);
		}
	}

	// 2. opposite side
	SetPen(pDC, index%3+2, color);
	size = m_Buffer[index].num_oppsite;
	for( i = 0; i < size; i++ )
	{
		line = m_Buffer[index].MainIndex[1][i];
		pDC->MoveTo(m_LineList->m_Buffer[line].p1);
		pDC->LineTo(m_LineList->m_Buffer[line].p2);

		if( m_TextMode == 2 )
		{
			str.Format("%d", i);
			rect.top = m_LineList->m_Buffer[line].p1.y - 15;
			rect.bottom = m_LineList->m_Buffer[line].p1.y;
			rect.left = m_LineList->m_Buffer[line].p1.x - 15;
			rect.right = m_LineList->m_Buffer[line].p1.x;
			pDC->DrawText(str, rect, 1);
		}
	}

	ReleasePen(pDC);
}
void CStrokeList::DrawStrokeOrder(CDC *pDC, int index, COLORREF color )
{
	//color = ~color;
	int i, line, before;

	if( m_Size <= 0 || index < 0 || index >= m_Size ) return;

	SetPen(pDC, index%3+1, color);
	// 1. main side
	int size = m_Buffer[index].num_main;
	for( i = 1; i < size; i++ )
	{
		before = m_Buffer[index].MainIndex[0][i-1];
		line = m_Buffer[index].MainIndex[0][i];
		
		if( m_LineList->m_Buffer[before].p2 != m_LineList->m_Buffer[line].p1 )
		{
			pDC->MoveTo(m_LineList->m_Buffer[before].p2);
			pDC->LineTo(m_LineList->m_Buffer[line].p1);
		}
	
	}

	// 2. opposite side
	SetPen(pDC, index%3+2, color);
	size = m_Buffer[index].num_oppsite;
	for( i = 1; i < size; i++ )
	{
		before = m_Buffer[index].MainIndex[1][i-1];
		line = m_Buffer[index].MainIndex[1][i];
		
		if ( m_LineList->m_Buffer[before].p2 != m_LineList->m_Buffer[line].p1)
		{	
			pDC->MoveTo(m_LineList->m_Buffer[before].p2);
			pDC->LineTo(m_LineList->m_Buffer[line].p1);
		}
	}

	ReleasePen(pDC);
}

//
// 연속성(다른 획과 합체 가능성) 체크
void CStrokeList::SetContinuity( int index)
{
	CPoint current1, current2, next;	// points
	int angle = m_Buffer[index].angle;
	m_Buffer[index].open1 = 0;	// initializing
	m_Buffer[index].open2 = 0;

	int line1, line2;	// 비교 대상  line 번호
	int k;
	// open 여부를 판단해야 함

	// 1.left
	line1 = m_Buffer[index].MainIndex[0][0];
	line2 =  m_LineList->FindLineIndex(line1, 2); // searching
	if(!m_LineList->CrossProduct(line2, line1)) m_Buffer[index].open1++;
		
	k = m_Buffer[index].num_oppsite-1;
	line1 = m_Buffer[index].MainIndex[1][k];
	line2 = m_LineList->FindLineIndex(line1, 1); // searching
	if(!m_LineList->CrossProduct(line1, line2)) m_Buffer[index].open1++;

	// 2. right
	k = m_Buffer[index].num_main-1;
	line1 = m_Buffer[index].MainIndex[0][k];
	line2 =  m_LineList->FindLineIndex(line1, 1); // searching
	if(!m_LineList->CrossProduct(line1, line2)) m_Buffer[index].open2++;

	line1 = m_Buffer[index].MainIndex[1][0];
	line2 = m_LineList->FindLineIndex(line1, 2); // searching
	if(!m_LineList->CrossProduct(line2, line1)) m_Buffer[index].open2++;

}

//////////////////////////////////////////////////////////////////////////
// 소팅 관련

// 
// 방향에 따라서 라인 소팅 
void CStrokeList::Sort( int index)
{

	CPoint p1, p2;
	int index1, index2;
	index1 = m_Buffer[index].MainIndex[0][0];
	p1 = m_LineList->m_Buffer[index1].p1;
	index2 = m_Buffer[index].num_main-1;
	index1 = m_Buffer[index].MainIndex[0][index2];
	p2 = m_LineList->m_Buffer[index1].p2;

	int angle;
	if( p1.x == p2.x ) angle = 90;
	else 
	{
		angle = (int)( atan( (p2.y-p1.y)/(double)(p2.x-p1.x)) *180/3.141592 );
	}

//	angle = angle/size;
//	m_Buffer[index].angle = angle;
	if( angle < 0 ) angle = -angle;

	if( IsHookStroke(index) == TRUE )
	{
		m_Buffer[index].direct = 1;
		SortY(index);
		return;
	}

	if( abs(angle) > SORT_ANGLE ) 
	{
		m_Buffer[index].direct = 1;
		SortY(index);
	}
	else
	{
		SortX(index);
		m_Buffer[index].direct = 0;
	}
}


void CStrokeList::SortX(int index)
{
	int i, j;
	int size = m_Buffer[index].num_main;
	int num1, num2;
	int x1, x2;
	
	for( i = 1; i < size; i++ )
	{
		for( j = i; j > 0; j--)
		{
			num1 = m_Buffer[index].MainIndex[0][j-1];
			num2 = m_Buffer[index].MainIndex[0][j];	
			x1 = m_LineList->m_Buffer[num1].p1.x;
			x2 = m_LineList->m_Buffer[num2].p1.x;
			if( x1 > x2 )
			{
				m_Buffer[index].MainIndex[0][j] = num1;
				m_Buffer[index].MainIndex[0][j-1] = num2;
			}
			else break;
		}
	}

	size = m_Buffer[index].num_oppsite;
	for( i = 1; i < size; i++ )
	{
		for( j = i; j > 0; j--)
		{
			num1 = m_Buffer[index].MainIndex[1][j-1];
			num2 = m_Buffer[index].MainIndex[1][j];
			x1 = m_LineList->m_Buffer[num1].p2.x;
			x2 = m_LineList->m_Buffer[num2].p2.x;
			if( x1 > x2 )
			{
				m_Buffer[index].MainIndex[1][j] = num1;
				m_Buffer[index].MainIndex[1][j-1] = num2;
			}
			else break;
		}
	}

	num1 = m_Buffer[index].MainIndex[0][0];
	x1 = m_LineList->m_Buffer[num1].p1.x;
	x2 = m_LineList->m_Buffer[num1].p2.x;

	if( x1 < x2 )
	{
		Reverse(index, 1);
	}
	else
	{
		Reverse(index, 0);
		MakeRightWay(index);
	}

	//////////////////////////////////////////////////////////////////////////
	// make order
	MakeOrderX(index);
}

//
// y 좌표에 대한 소팅
void CStrokeList::SortY(int index)
{
	int i, j;
	int size = m_Buffer[index].num_main;
	int num1, num2;
	int y1, y2;
	
	for( i = 1; i < size; i++ )
	{
		for( j = i; j > 0; j--)
		{
			num1 = m_Buffer[index].MainIndex[0][j-1];	
			num2 = m_Buffer[index].MainIndex[0][j];
			y1 = m_LineList->m_Buffer[num1].p1.y;
			y2 = m_LineList->m_Buffer[num2].p1.y;
			if( y1 > y2 )
			{
				m_Buffer[index].MainIndex[0][j] = num1;
				m_Buffer[index].MainIndex[0][j-1] = num2;
			}
			else break;
		}
	}

	size = m_Buffer[index].num_oppsite;
	for( i = 1; i < size; i++ )
	{
		for( j = i; j > 0; j--)
		{
			num1 = m_Buffer[index].MainIndex[1][j-1];
			num2 = m_Buffer[index].MainIndex[1][j];
			y1 = m_LineList->m_Buffer[num1].p2.y;
			y2 = m_LineList->m_Buffer[num2].p2.y;
			if( y1 > y2 )
			{
				m_Buffer[index].MainIndex[1][j] = num1;
				m_Buffer[index].MainIndex[1][j-1] = num2;
			}
			else break;
		}
	}

	num1 = m_Buffer[index].MainIndex[0][0];
	y1 = m_LineList->m_Buffer[num1].p1.y;
	y2 = m_LineList->m_Buffer[num1].p2.y;

	if( y1 < y2 )
	{

		Reverse(index, 1);
	}
	else
	{
		Reverse(index, 0);
		MakeRightWay(index);
	}
	MakeOrderY( index );
}

//
// x+y 에 대한 소팅 ㅡ
void CStrokeList::SortXY(int index)
{
	int i, j;
	int size = m_Buffer[index].num_main;
	int num1, num2;
	int xy1, xy2;
	
	for( i = 1; i < size; i++ )
	{
		for( j = i; j > 0; j--)
		{
			num1 = m_Buffer[index].MainIndex[0][j-1];
			num2 = m_Buffer[index].MainIndex[0][j];
			xy1 = m_LineList->m_Buffer[num1].p1.x+m_LineList->m_Buffer[num1].p1.y;
			xy2 = m_LineList->m_Buffer[num2].p1.x+m_LineList->m_Buffer[num2].p1.y;
			if( xy1 > xy2 )
			{
				m_Buffer[index].MainIndex[0][j] = num1;
				m_Buffer[index].MainIndex[0][j-1] = num2;
			}
			else break;
		}
	}

	size = m_Buffer[index].num_oppsite;
	for( i = 1; i < size; i++ )
	{
		for( j = i; j > 0; j--)
		{
			num1 = m_Buffer[index].MainIndex[1][j-1];
			num2 = m_Buffer[index].MainIndex[1][j];
			xy1 = m_LineList->m_Buffer[num1].p1.x+m_LineList->m_Buffer[num1].p1.y;
			xy2 = m_LineList->m_Buffer[num2].p1.x+m_LineList->m_Buffer[num2].p1.y;
			if( xy1 > xy2 )
			{
				m_Buffer[index].MainIndex[1][j] = num1;
				m_Buffer[index].MainIndex[1][j-1] = num2;
			}
			else break;
		}
	}

	num1 = m_Buffer[index].MainIndex[0][0];
	xy1 = m_LineList->m_Buffer[num1].p1.x+m_LineList->m_Buffer[num1].p1.y;
	xy2 = m_LineList->m_Buffer[num1].p2.x+m_LineList->m_Buffer[num1].p2.y;
	if( xy1 < xy2 ) Reverse(index, 1);
	else Reverse(index, 0);
}

//
// x-y 에 대한 소팅 ㅡ
void CStrokeList::SortX_Y(int index)
{
	int i, j;
	int size = m_Buffer[index].num_main;
	int num1, num2;
	int xy1, xy2;
	
	for( i = 1; i < size; i++ )
	{
		for( j = i; j > 0; j--)
		{
			num1 = m_Buffer[index].MainIndex[0][j-1];
			num2 = m_Buffer[index].MainIndex[0][j];
			xy1 = m_LineList->m_Buffer[num1].p1.x-m_LineList->m_Buffer[num1].p1.y;
			xy2 = m_LineList->m_Buffer[num2].p1.x-m_LineList->m_Buffer[num2].p1.y;
			if( xy1 > xy2 )
			{
				m_Buffer[index].MainIndex[0][j] = num1;
				m_Buffer[index].MainIndex[0][j-1] = num2;
			}
			else break;
		}
	}

	size = m_Buffer[index].num_oppsite;
	for( i = 1; i < size; i++ )
	{
		for( j = i; j > 0; j--)
		{
			num1 = m_Buffer[index].MainIndex[1][j-1];
			num2 = m_Buffer[index].MainIndex[1][j];
			xy1 = m_LineList->m_Buffer[num1].p1.x-m_LineList->m_Buffer[num1].p1.y;
			xy2 = m_LineList->m_Buffer[num2].p1.x-m_LineList->m_Buffer[num2].p1.y;
			if( xy1 > xy2 )
			{
				m_Buffer[index].MainIndex[1][j] = num1;
				m_Buffer[index].MainIndex[1][j-1] = num2;
			}
			else break;
		}
	}

	num1 = m_Buffer[index].MainIndex[0][0];
	xy1 = m_LineList->m_Buffer[num1].p1.x-m_LineList->m_Buffer[num1].p1.y;
	xy2 = m_LineList->m_Buffer[num1].p2.x-m_LineList->m_Buffer[num1].p2.y;
	if( xy1 < xy2 ) Reverse(index, 1);
	else Reverse(index, 0);
}
//
// 데이터 뒤집기
void CStrokeList::Reverse(int index, int flag)
{
	int end;
	int start = 0;
	if(flag == 0 ) end = m_Buffer[index].num_main-1;
	else end = m_Buffer[index].num_oppsite-1; 

	int temp;
	while(start < end)
	{
		temp = m_Buffer[index].MainIndex[flag][start];
		m_Buffer[index].MainIndex[flag][start] = m_Buffer[index].MainIndex[flag][end];
		m_Buffer[index].MainIndex[flag][end] = temp;
		start++;
		end--;
	}
}

//
// 연결 후보들을 찾아서 이어주기 
void CStrokeList::ConnectStroke()
{
	int i, j, index, angle, line, line1, line2;
	int distance1, distance2, distance3, mg_angle;;
	BOOL repeat;
	CPoint p1, p2, op1, op2;
	int mergeIndex; // merge 가능한 것들 중 가장 가까운(적합한) 획 인덱스
	int mergeDistance; // 최소 거리 값 저장
	for( i = 0; i < m_Size; i++ )
	{
		m_Buffer[i].check = FALSE;
		m_Buffer[i].use = TRUE;
		m_Buffer[i].vector1 = GetVector(i);
	}
	
	for( i = 0; i < m_Size; i++ )
	{
		if( m_Buffer[i].open2 < 2 ) continue;
		if( m_Buffer[i].check ) continue; 

		repeat = TRUE;
		//////////////////////////////////////////////////////////////////////////
		// while
		while( repeat ){
		//////////////////////////////////////////////////////////////////////////
		if( m_Buffer[i].open2 < 2 ) break;
		
		mergeIndex = -1; // 초기화 
		mergeDistance = INT_MAX;

		for( j = 0; j < m_Size; j++)
		{
			repeat = FALSE;
		
			if( i == j) continue;
			if( m_Buffer[j].check ) continue;
			if( m_Buffer[j].open1 < 2 ) continue;

			angle = DotProductAngle(i, j);
			if( angle > 25 ) continue;
			
			// right or bottom (only one way) rightway is 0 or 1
			index = m_Buffer[i].num_main-1;
			line1 = m_Buffer[i].MainIndex[0][index];
			p1 = m_LineList->m_Buffer[line1].p2;
			line = m_Buffer[i].MainIndex[1][0];
			p2 = m_LineList->m_Buffer[line].p1;
			
			// left or top 
			line2 = m_Buffer[j].MainIndex[0][0];
			op1 = m_LineList->m_Buffer[line2].p1;
			index = m_Buffer[j].num_oppsite-1;
			line = m_Buffer[j].MainIndex[1][index];
			op2 = m_LineList->m_Buffer[line].p2;

			// simple distance 
			distance3 = m_LineList->GetDistance2(line1, line2);

			
			// 거리 비교 
			distance1 = (p1.x-op1.x)*(p1.x-op1.x)+(p1.y-op1.y)*(p1.y-op1.y);
			distance2 = (p2.x-op2.x)*(p2.x-op2.x)+(p2.y-op2.y)*(p2.y-op2.y);
			
			mg_angle = MergeAngle(line1,line2);
			if( mg_angle == -1 ) mg_angle = angle;
			
			if( distance1 < 60*60 && distance2 < 60*60 && distance3 < 900 && MergeAngle(line1,line2) < 11) 
			{ //거리 계산을 한 번 더함			
				if( mergeDistance > min(distance1,distance2))
				{
					mergeDistance = min(distance1,distance2);
					mergeIndex = j;
				}
			}
			else if( distance1 < 120*120 && distance2 < 120*120 && distance3 < 25 && MergeAngle(line1,line2) == 0 )
			{
				if( mergeDistance > min(distance1,distance2))
				{
					mergeDistance = min(distance1,distance2);
					mergeIndex = j;
				}
			}
		} // for (j)

		// 여기서 비로소 머지함 
		if( mergeIndex != -1 )
		{
			repeat = TRUE;
			MergeStroke(i, mergeIndex);
		}
		//////////////////////////////////////////////////////////////////////////
		} // while

		repeat = TRUE;
		//////////////////////////////////////////////////////////////////////////
		// 2 - 1 open (it's diffrent direction)
		while( repeat ){
		//////////////////////////////////////////////////////////////////////////
		if( m_Buffer[i].open2 < 2 ) break;
		
		for( j = 0; j < m_Size; j++)
		{
			repeat = FALSE;
		
			if( i == j) continue;
			if( m_Buffer[j].check ) continue;
			if( m_Buffer[i].direct == m_Buffer[j].direct ) continue;
			if( m_Buffer[j].open2 < 2 ) continue;

			angle = DotProductAngle2(i, j);
			if( angle > 25 ) continue;
			
			// right or bottom (only one way)
			index = m_Buffer[i].num_main-1;
			line1 = m_Buffer[i].MainIndex[0][index];
			p1 = m_LineList->m_Buffer[line1].p2;
			line = m_Buffer[i].MainIndex[1][0];
			p2 = m_LineList->m_Buffer[line].p1;
			
			// left or top 
			index = m_Buffer[j].num_main-1;
			line2 = m_Buffer[j].MainIndex[0][index];
			op1 = m_LineList->m_Buffer[line2].p2;
			line2 = m_Buffer[j].MainIndex[1][0];
			op2 = m_LineList->m_Buffer[line2].p1;

			// simple distance 
			distance3 = m_LineList->GetDistance2(line1, line2);

			
			// 거리 비교 
			distance1 = (p1.x-op1.x)*(p1.x-op1.x)+(p1.y-op1.y)*(p1.y-op1.y);
			distance2 = (p2.x-op2.x)*(p2.x-op2.x)+(p2.y-op2.y)*(p2.y-op2.y);

			if( distance1 < 70*70 && distance2 < 70*70 && distance3 < 900)
			{
				repeat = TRUE;
				MergeStroke2(i, j);
				break;
			}
		} // for (j)
		//////////////////////////////////////////////////////////////////////////
		} // while
		//////////////////////////////////////////////////////////////////////////
	} // for (i)
}

void CStrokeList::MergeStroke(int stroke1, int stroke2 )
{
	int i; 
	int buffer1[128], buffer2[128];
	int size1, size2;

	// 1-1
	size1 = m_Buffer[stroke1].num_main;
	for( i = 0; i < size1; i++)
		buffer1[i] = m_Buffer[stroke1].MainIndex[0][i];

	
	// 1-2
	for( i = 0; i < m_Buffer[stroke2].num_main; i++)
		buffer1[size1+i] = m_Buffer[stroke2].MainIndex[0][i];
	size1 += m_Buffer[stroke2].num_main;

	size2 = m_Buffer[stroke2].num_oppsite;
	for( i = 0; i < size2; i++)
		buffer2[i] = m_Buffer[stroke2].MainIndex[1][i];


	// 2-1, 2-2
	for( i = 0; i < m_Buffer[stroke1].num_oppsite; i++)
		buffer2[size2+i] = m_Buffer[stroke1].MainIndex[1][i];
	size2 += m_Buffer[stroke1].num_oppsite;


	// 3
	for( i = 0; i < size1; i++ )
		m_Buffer[stroke1].MainIndex[0][i] = buffer1[i];
	m_Buffer[stroke1].num_main = size1;

	for( i = 0; i < size2; i++ )
		m_Buffer[stroke1].MainIndex[1][i] = buffer2[i];
	m_Buffer[stroke1].num_oppsite = size2;

	m_Buffer[stroke2].check = TRUE;
	m_Buffer[stroke2].use = FALSE;
	m_Buffer[stroke1].open2 = m_Buffer[stroke2].open2;

	// 4
	m_Buffer[stroke1].vector1 = GetVector(stroke1);
//	SetSortAngle(stroke1);
}

//
// stoke1 의 main 과 stroke2의 opposite
void CStrokeList::MergeStroke2(int stroke1, int stroke2 )
{
	int i; 
	int buffer1[128], buffer2[128];
	int size1, size2;

	// 1-1
	size1 = m_Buffer[stroke1].num_main;
	for( i = 0; i < size1; i++)
		buffer1[i] = m_Buffer[stroke1].MainIndex[0][i];

	
	// 1-2
	for( i = 0; i < m_Buffer[stroke2].num_oppsite; i++)
		buffer1[size1+i] = m_Buffer[stroke2].MainIndex[1][i];
	size1 += m_Buffer[stroke2].num_oppsite;

	size2 = m_Buffer[stroke2].num_main;
	for( i = 0; i < size2; i++)
		buffer2[i] = m_Buffer[stroke2].MainIndex[0][i];


	// 2-1, 2-2
	for( i = 0; i < m_Buffer[stroke1].num_oppsite; i++)
		buffer2[size2+i] = m_Buffer[stroke1].MainIndex[1][i];
	size2 += m_Buffer[stroke1].num_oppsite;


	// 3
	for( i = 0; i < size1; i++ )
		m_Buffer[stroke1].MainIndex[0][i] = buffer1[i];
	m_Buffer[stroke1].num_main = size1;

	for( i = 0; i < size2; i++ )
		m_Buffer[stroke1].MainIndex[1][i] = buffer2[i];
	m_Buffer[stroke1].num_oppsite = size2;

	m_Buffer[stroke2].check = TRUE;
	m_Buffer[stroke2].use = FALSE;
	m_Buffer[stroke1].open2 = m_Buffer[stroke2].open1;

	// 4
	m_Buffer[stroke1].vector1 = GetVector(stroke1);

	// 각도 구하기 //-_-
//	SetSortAngle(stroke1);
}

// sort 후에 순서를 다시 바로 잡기
void CStrokeList::MakeOrderX( int index )
{
	int i, j, k, m, minValue, minIndex;
	CPoint p1, p2;

	k = m_Buffer[index].MainIndex[0][0];
	p2 = m_LineList->m_Buffer[k].p2;
	for( i = 1; i < m_Buffer[index].num_main; i++)
	{
		k = m_Buffer[index].MainIndex[0][i];
		p1 = m_LineList->m_Buffer[k].p1;

		if( p1 == p2 )
		{
			p2 = m_LineList->m_Buffer[k].p2;
			continue;
		}
		else
		{
			// 맞는 거 찾기 
			for( j = i+1; j < m_Buffer[index].num_main; j++)
			{
				m = m_Buffer[index].MainIndex[0][j];
				p1 = m_LineList->m_Buffer[m].p1;
				if( p1 == p2 )
				{
					// swap i & j
					m = m_Buffer[index].MainIndex[0][j];
					m_Buffer[index].MainIndex[0][j] = m_Buffer[index].MainIndex[0][i];
					m_Buffer[index].MainIndex[0][i] = m;
					p2 = m_LineList->m_Buffer[m].p2;
					break;
				}
			} // for(j)
			if( j < m_Buffer[index].num_main ) continue;
				
			// 해당사항 없음 (소팅 문제) x 
			minIndex = i;
			m = m_Buffer[index].MainIndex[0][i];
			minValue = m_LineList->m_Buffer[m].p1.x;

			for( j = i+1; j < m_Buffer[index].num_main; j++)
			{
				m = m_Buffer[index].MainIndex[0][j];
				if( minValue > m_LineList->m_Buffer[m].p1.x )
				{
					minValue = m_LineList->m_Buffer[m].p1.x;
					minIndex = j;
				}
			}

			if( minIndex != i ) // swap (i & j)
			{
				m = m_Buffer[index].MainIndex[0][j];
				m_Buffer[index].MainIndex[0][j] = m_Buffer[index].MainIndex[0][i];
				m_Buffer[index].MainIndex[0][i] = m;	
				p2 = m_LineList->m_Buffer[m].p2;
			}
			else p2 = m_LineList->m_Buffer[i].p2;
		} // (else) if( p1 == p2 )
	} // for(i)

	//////////////////////////////////////////////////////////////////////////
	// 반대편 획 (이건 끝에서부터 소팅하자!)
	m = m_Buffer[index].num_oppsite-1;
	k = m_Buffer[index].MainIndex[1][m];
	p1 = m_LineList->m_Buffer[k].p1; 

	for( i = m_Buffer[index].num_oppsite-2; i >= 0; i--) // 뒤부터 소팅
	{
		k = m_Buffer[index].MainIndex[1][i];
		p2 = m_LineList->m_Buffer[k].p2;

		if( p1 == p2 )
		{
			p1 = m_LineList->m_Buffer[k].p1;
			continue;
		}
		else
		{
			// 맞는 거 찾기 
			for( j = i-1; j >= 0; j--)
			{
				m = m_Buffer[index].MainIndex[1][j];
				p2 = m_LineList->m_Buffer[m].p2;
				if( p1 == p2 )
				{
					// swap i & j
					m = m_Buffer[index].MainIndex[1][j];
					m_Buffer[index].MainIndex[1][j] = m_Buffer[index].MainIndex[1][i];
					m_Buffer[index].MainIndex[1][i] = m;
					p1 = m_LineList->m_Buffer[m].p1;
					break;
				}
			} // for(j)
			if( j >= 0 ) continue;
				
			// 해당사항 없음 (소팅 문제) x 
			minIndex = i;
			m = m_Buffer[index].MainIndex[1][i];
			minValue = m_LineList->m_Buffer[m].p1.x;

			for( j = i-1; j >= 0; j--)
			{
				m = m_Buffer[index].MainIndex[1][j];
				if( minValue > m_LineList->m_Buffer[m].p1.x )
				{
					minValue = m_LineList->m_Buffer[m].p1.x;
					minIndex = j;
				}
			}

			if( minIndex != i ) // swap (i & j)
			{
				m = m_Buffer[index].MainIndex[1][j];
				m_Buffer[index].MainIndex[1][j] = m_Buffer[index].MainIndex[1][i];
				m_Buffer[index].MainIndex[1][i] = m;	
				p1 = m_LineList->m_Buffer[m].p1;
			}
			else p1 = m_LineList->m_Buffer[i].p1;
		} // (else) if( p1 == p2 )
	} // for(i)

}

void CStrokeList::MakeOrderY( int index )
{
	int i, j, k, m, minValue, minIndex;
	CPoint p1, p2;

	k = m_Buffer[index].MainIndex[0][0];
	p2 = m_LineList->m_Buffer[k].p2;
	for( i = 1; i < m_Buffer[index].num_main; i++)
	{
		k = m_Buffer[index].MainIndex[0][i];
		p1 = m_LineList->m_Buffer[k].p1;

		if( p1 == p2 )
		{
			p2 = m_LineList->m_Buffer[k].p2;
			continue;
		}
		else
		{
			// 맞는 거 찾기 
			for( j = i+1; j < m_Buffer[index].num_main; j++)
			{
				m = m_Buffer[index].MainIndex[0][j];
				p1 = m_LineList->m_Buffer[m].p1;
				if( p1 == p2 )
				{
					// swap i & j
					m = m_Buffer[index].MainIndex[0][j];
					m_Buffer[index].MainIndex[0][j] = m_Buffer[index].MainIndex[0][i];
					m_Buffer[index].MainIndex[0][i] = m;
					p2 = m_LineList->m_Buffer[m].p2;
					break;
				}
			} // for(j)
			if( j < m_Buffer[index].num_main ) continue;
				
			// 해당사항 없음 (소팅 문제) y 
			minIndex = i;
			m = m_Buffer[index].MainIndex[0][i];
			minValue = m_LineList->m_Buffer[m].p1.y;

			for( j = i+1; j < m_Buffer[index].num_main; j++)
			{
				m = m_Buffer[index].MainIndex[0][j];
				if( minValue > m_LineList->m_Buffer[m].p1.y )
				{
					minValue = m_LineList->m_Buffer[m].p1.y;
					minIndex = j;
				}
			}

			if( minIndex != i ) // swap (i & j)
			{
				m = m_Buffer[index].MainIndex[0][j];
				m_Buffer[index].MainIndex[0][j] = m_Buffer[index].MainIndex[0][i];
				m_Buffer[index].MainIndex[0][i] = m;	
				p2 = m_LineList->m_Buffer[m].p2;
			}
			else p2 = m_LineList->m_Buffer[i].p2;
		} // (else) if( p1 == p2 )
	} // for(i)
	
	//////////////////////////////////////////////////////////////////////////
	// 반대편 획
	m = m_Buffer[index].num_oppsite-1;
	k = m_Buffer[index].MainIndex[1][m];
	p1 = m_LineList->m_Buffer[k].p1; 

	for( i = m_Buffer[index].num_oppsite-2; i >= 0; i--) // 뒤부터 소팅
	{
		k = m_Buffer[index].MainIndex[1][i];
		p2 = m_LineList->m_Buffer[k].p2;

		if( p1 == p2 )
		{
			p1 = m_LineList->m_Buffer[k].p1;
			continue;
		}
		else
		{
			// 맞는 거 찾기 
			for( j = i-1; j >= 0; j--)
			{
				m = m_Buffer[index].MainIndex[1][j];
				p2 = m_LineList->m_Buffer[m].p2;
				if( p1 == p2 )
				{
					// swap i & j
					m = m_Buffer[index].MainIndex[1][j];
					m_Buffer[index].MainIndex[1][j] = m_Buffer[index].MainIndex[1][i];
					m_Buffer[index].MainIndex[1][i] = m;
					p1 = m_LineList->m_Buffer[m].p1;
					break;
				}
			} // for(j)
			if( j >= 0 ) continue;
				
			// 해당사항 없음 (소팅 문제) x 
			minIndex = i;
			m = m_Buffer[index].MainIndex[1][i];
			minValue = m_LineList->m_Buffer[m].p2.y;

			for( j = i-1; j >= 0; j--)
			{
				m = m_Buffer[index].MainIndex[1][j];
				if( minValue > m_LineList->m_Buffer[m].p2.y )
				{
					minValue = m_LineList->m_Buffer[m].p2.y;
					minIndex = j;
				}
			}

			if( minIndex != i ) // swap (i & j)
			{
				m = m_Buffer[index].MainIndex[1][j];
				m_Buffer[index].MainIndex[1][j] = m_Buffer[index].MainIndex[1][i];
				m_Buffer[index].MainIndex[1][i] = m;	
				p1 = m_LineList->m_Buffer[m].p1;
			}
			else
			{
				m = m_Buffer[index].MainIndex[1][i];
				p1 = m_LineList->m_Buffer[m].p1;
			}
		} // (else) if( p1 == p2 )
	} // for(i)
	
}

CPoint CStrokeList::GetVector(int index)
{
	CPoint point,p1,p2;
	int c1, c2;
	int last;

	c1 = m_Buffer[index].MainIndex[0][0];
	p1 = m_LineList->m_Buffer[c1].p1;

	last = m_Buffer[index].num_main-1;
	c2 = m_Buffer[index].MainIndex[0][last];
	p2 = m_LineList->m_Buffer[c2].p2;
	
	point.x = p2.x - p1.x;
	point.y = p2.y - p1.y;

	return point;
}

//
// 획과 획의 각도 
int CStrokeList::DotProductAngle(int index1, int index2)
{
	int angle;
	CPoint p1, p2;

	int in, index;
	index = m_Buffer[index1].num_main-1;
	in = m_Buffer[index1].MainIndex[0][index];
	if( m_LineList->m_Buffer[in].curveNum == 0 )
	{
		p1.x = m_LineList->m_Buffer[in].p2.x - m_LineList->m_Buffer[in].p1.x;
		p1.y = m_LineList->m_Buffer[in].p2.y - m_LineList->m_Buffer[in].p1.y;
	}
	else //1 or 2
	{
		p1.x = m_LineList->m_Buffer[in].p2.x - m_LineList->m_Buffer[in].cp2.x;
		p1.y = (m_LineList->m_Buffer[in].p2.y - m_LineList->m_Buffer[in].cp2.y);
	}

	in = m_Buffer[index2].MainIndex[0][0];
	if( m_LineList->m_Buffer[in].curveNum == 0 )
	{
		p2.x = m_LineList->m_Buffer[in].p2.x - m_LineList->m_Buffer[in].p1.x;
		p2.y = m_LineList->m_Buffer[in].p2.y - m_LineList->m_Buffer[in].p1.y;
	}
	else
	{
		p2.x = m_LineList->m_Buffer[in].cp1.x - m_LineList->m_Buffer[in].p1.x;
		p2.y = (m_LineList->m_Buffer[in].cp1.y - m_LineList->m_Buffer[in].p1.y);
	}
	
	float norm1, norm2;
	norm1 = (float)sqrt( (float)(p1.x*p1.x + p1.y*p1.y) );
	norm2 = (float)sqrt( (float)(p2.x*p2.x + p2.y*p2.y) );
	int dotproduct = p1.x*p2.x + p1.y*p2.y;

	angle = (int)(acos( dotproduct/(norm1*norm2)) * 180/3.141592);
	if( angle > 90 ) return 180-angle;
	return angle;
}

int CStrokeList::DotProductAngle2(int index1, int index2)
{
	int angle;
	CPoint p1, p2;

	int in, index;
	index = m_Buffer[index1].num_main-1;
	in = m_Buffer[index1].MainIndex[0][index];
	
	if( m_LineList->m_Buffer[in].curveNum == 0 )
	{
		p1.x = m_LineList->m_Buffer[in].p2.x - m_LineList->m_Buffer[in].p1.x;
		p1.y = m_LineList->m_Buffer[in].p2.y - m_LineList->m_Buffer[in].p1.y;
	}
	else //1 or 2
	{
		p1.x = m_LineList->m_Buffer[in].p2.x - m_LineList->m_Buffer[in].cp2.x;
		p1.y = (m_LineList->m_Buffer[in].p2.y - m_LineList->m_Buffer[in].cp2.y);
	}

	in = m_Buffer[index2].MainIndex[1][0];
	if( m_LineList->m_Buffer[in].curveNum == 0 )
	{
		p2.x = m_LineList->m_Buffer[in].p2.x - m_LineList->m_Buffer[in].p1.x;
		p2.y = m_LineList->m_Buffer[in].p2.y - m_LineList->m_Buffer[in].p1.y;
	}
	else
	{
		p2.x = m_LineList->m_Buffer[in].cp1.x - m_LineList->m_Buffer[in].p1.x;
		p2.y = (m_LineList->m_Buffer[in].cp1.y - m_LineList->m_Buffer[in].p1.y);
	}
	
	float norm1, norm2;
	norm1 = (float)sqrt( (float)(p1.x*p1.x + p1.y*p1.y) );
	norm2 = (float)sqrt( (float)(p2.x*p2.x + p2.y*p2.y) );
	int dotproduct = p1.x*p2.x + p1.y*p2.y;

	angle = (int)(acos( dotproduct/(norm1*norm2)) * 180/3.141592);
	if( angle > 90 ) return 180-angle;
	return angle;
}

//
// main 과 oppsite 바꾸기
void CStrokeList::MakeRightWay( int index )
{
	int main[128], opp[128], main_num, opp_num, i;

	opp_num = m_Buffer[index].num_main;
	for( i =0; i < opp_num; i++)
		opp[i] = m_Buffer[index].MainIndex[0][i];

	main_num = m_Buffer[index].num_oppsite;
	for( i = 0; i < main_num; i++)
		main[i] = m_Buffer[index].MainIndex[1][i];

	// main
	m_Buffer[index].num_main = main_num;
	for( i = 0; i < main_num; i++)
		m_Buffer[index].MainIndex[0][i] = main[i];

	// oppsite
	m_Buffer[index].num_oppsite = opp_num;
	for( i = 0; i < opp_num; i++)
		m_Buffer[index].MainIndex[1][i] = opp[i];

}

void CStrokeList::SetSortAngle( int index)
{
	CPoint p1, p2;
	int index1, index2;
	index1 = m_Buffer[index].MainIndex[0][0];
	p1 = m_LineList->m_Buffer[index1].p1;
	index2 = m_Buffer[index].num_main-1;
	index1 = m_Buffer[index].MainIndex[0][index2];
	p2 = m_LineList->m_Buffer[index1].p2;

	int angle;
	if( p1.x == p2.x ) angle = 90;
	else 
	{
		angle = (int)( atan( (p2.y-p1.y)/(double)(p2.x-p1.x)) *180/3.141592 );
	}

	if( angle < 0 ) angle = -angle;

	if( abs(angle) > SORT_ANGLE ) m_Buffer[index].direct = 1;

	else m_Buffer[index].direct = 0;

}


// 꺾어진 획 만들기
void CStrokeList::MakeTurning()  
{
	int i, j, index, last;
	CPoint p1, p2, op1, op2;
	
	// 준비
	for( i = 0; i < m_Size; i++ ) m_Buffer[i].check = FALSE;

	for( i = 0; i < m_Size; i++)
	{
		if( m_Buffer[i].use == FALSE ) continue;
		if( m_Buffer[i].check == TRUE ) continue;
		if( m_Buffer[i].direct != 0) continue;
		
		// --- 이런 획이 아닌 경우
		if( abs(m_Buffer[i].vector1.x) < 3*abs(m_Buffer[i].vector1.y) )
		{
			int length = 0;
			for( int x = 0; x < m_Buffer[0].num_main; x++)
			{
				length += GetLength(m_Buffer[i].MainIndex[0][x]); // 전체 획의 길이 구하기
			}
			if( length > 150 )continue;
		}
			
		last = m_Buffer[i].num_main -1;
		index = m_Buffer[i].MainIndex[0][last];
		p1 = m_LineList->m_Buffer[index].p2;
		index = m_Buffer[i].MainIndex[1][0];
		p2 = m_LineList->m_Buffer[index].p1;

		for( j = 0; j < m_Size; j++ )
		{
			if( i == j ) continue;
			if( m_Buffer[j].use == FALSE ) continue;
			if( m_Buffer[j].check == TRUE ) continue;
			//if( m_Buffer[j].direct != 1 ) continue;

			index = m_Buffer[j].MainIndex[0][0];
			op1 = m_LineList->m_Buffer[index].p1;
			last = m_Buffer[j].num_oppsite-1;
			index = m_Buffer[j].MainIndex[1][last];
			op2 = m_LineList->m_Buffer[index].p2;

			if( p1 == op1 || p2 == op2 )
			{ // merging
				MergeStroke(i,j);
				m_Buffer[i].direct = 2;
				m_Buffer[i].check = TRUE;
				m_Buffer[j].check = TRUE;
				continue;
			}

			//////////////////////////////////////////////////////////////////////////
			// (두 획이 모두 가로 (0) 인 경우 )


			if( m_Buffer[j].direct == 1 ) continue;
			last = m_Buffer[j].num_main -1;
			index = m_Buffer[j].MainIndex[0][last];
			op2 = m_LineList->m_Buffer[index].p2;
			index = m_Buffer[j].MainIndex[1][0];
			op1 = m_LineList->m_Buffer[index].p1;
			if( p1 == op1 || p2 == op2 )
			{ // merging
				MergeStroke2(i,j);
				m_Buffer[i].direct = 2;
				m_Buffer[i].check = TRUE;
				m_Buffer[j].check = TRUE;
			} 
		}
	}
}

int CStrokeList::MergeAngle(int line1, int line2)
{
	CPoint v1, v2;

	if( m_LineList->m_Buffer[line1].curveNum == 0 )
	{
		v1.x = m_LineList->m_Buffer[line1].p2.x - m_LineList->m_Buffer[line1].p1.x;
		v1.y = m_LineList->m_Buffer[line1].p2.y - m_LineList->m_Buffer[line1].p1.y;
	}
	else //1 or 2
	{
		v1.x = m_LineList->m_Buffer[line1].p2.x - m_LineList->m_Buffer[line1].cp2.x;
		v1.y = m_LineList->m_Buffer[line1].p2.y - m_LineList->m_Buffer[line1].cp2.y;
	}
	
	v2.x = m_LineList->m_Buffer[line2].p1.x - m_LineList->m_Buffer[line1].p2.x;
	v2.y = m_LineList->m_Buffer[line2].p1.y - m_LineList->m_Buffer[line1].p2.y;

	if( v2.x ==0 && v2.y == 0 ) return -1; /* 이상 종료 */

	float norm1, norm2;
	norm1 = (float)sqrt( (float)(v1.x*v1.x + v1.y*v1.y) );
	norm2 = (float)sqrt( (float)(v2.x*v2.x + v2.y*v2.y) );
	int dotproduct = v1.x*v2.x + v1.y*v2.y;

	int angle = (int)(acos( dotproduct/(norm1*norm2)) * 180/3.141592);
	if( angle > 90 ) return 180-angle;
	return angle;
	
}

//
// 획 정의하기 
void CStrokeList::Ordering()
{
	int i, j, size, index;
	CRect rect(500,500,0,0);
	CPoint p1;
	
	// bound box 구하기 
	for( i = 0; i < m_Size; i++)
	{
		if( m_Buffer[i].use == FALSE ) continue;
		m_Buffer[i].bound = rect;
		size = m_Buffer[i].num_main;
		for( j = 0; j < size; j++)
		{
			index = m_Buffer[i].MainIndex[0][j];
			p1 = m_LineList->m_Buffer[index].p1;
			
			if( m_Buffer[i].bound.left > p1.x ) m_Buffer[i].bound.left = p1.x;
			if( m_Buffer[i].bound.right < p1.x ) m_Buffer[i].bound.right = p1.x;
			if( m_Buffer[i].bound.top > p1.y ) m_Buffer[i].bound.top = p1.y;
			if( m_Buffer[i].bound.bottom < p1.y ) m_Buffer[i].bound.bottom = p1.y;

			p1 = m_LineList->m_Buffer[index].p2;
			if( m_Buffer[i].bound.left > p1.x ) m_Buffer[i].bound.left = p1.x;
			if( m_Buffer[i].bound.right < p1.x ) m_Buffer[i].bound.right = p1.x;
			if( m_Buffer[i].bound.top > p1.y ) m_Buffer[i].bound.top = p1.y;
			if( m_Buffer[i].bound.bottom < p1.y ) m_Buffer[i].bound.bottom = p1.y;
		}

		size = m_Buffer[i].num_oppsite;
		for( j = 0; j < size; j++)
		{
			index = m_Buffer[i].MainIndex[1][j];
			p1 = m_LineList->m_Buffer[index].p1;
			
			if( m_Buffer[i].bound.left > p1.x ) m_Buffer[i].bound.left = p1.x;
			if( m_Buffer[i].bound.right < p1.x ) m_Buffer[i].bound.right = p1.x;
			if( m_Buffer[i].bound.top > p1.y ) m_Buffer[i].bound.top = p1.y;
			if( m_Buffer[i].bound.bottom < p1.y ) m_Buffer[i].bound.bottom = p1.y;

			p1 = m_LineList->m_Buffer[index].p2;
			if( m_Buffer[i].bound.left > p1.x ) m_Buffer[i].bound.left = p1.x;
			if( m_Buffer[i].bound.right < p1.x ) m_Buffer[i].bound.right = p1.x;
			if( m_Buffer[i].bound.top > p1.y ) m_Buffer[i].bound.top = p1.y;
			if( m_Buffer[i].bound.bottom < p1.y ) m_Buffer[i].bound.bottom = p1.y;
		}

	}
	
	// 0. DOT 찾기;  STROKE_TYPE_DOT	0x0001
	if( m_Size == 1 )
	{
		if( m_GroupNum % 100 == 1 )m_Buffer[0].type = STROKE_TYPE_LEFTFALLING;
		else if( m_GroupNum % 100 == 1 ) m_Buffer[0].type = STROKE_TYPE_RIGHTFALLING;
		
		if ( m_GroupNum % 100 != 0 || FindDotStroke() == TRUE ) return;
	}

	// type ? (크기와 방향) -> m_Buffer[i].vector1
	CPoint t_vector;

	for( i = 0; i < m_Size; i++)
	{
		if( m_Buffer[i].use == FALSE ) continue;

		if( m_Buffer[i].direct == 2 )
		{ // STROKE_TYPE_TURNING			0x0200
			m_Buffer[i].type = STROKE_TYPE_TURNING;
			continue;
		}
			
		//	STROKE_TYPE_HORIZONTAL		0x0010
		if( m_Buffer[i].direct == 1 ) // VERTICAL 후보
		{
			if( IsHookStroke(i) == TRUE ) // STROKE_TYPE_HOOK	0x0021
			{
				m_Buffer[i].type = STROKE_TYPE_HOOK;
			}
			else
			{
				if( IsVerticalStroke(i) == TRUE ) // STROKE_TYPE_VERTICAL		0x0020
				{
					m_Buffer[i].type = STROKE_TYPE_VERTICAL;
				}
				else // 방향에 따라서 
				{
					if( m_Buffer[i].vector1.x > 0 ) m_Buffer[i].type = STROKE_TYPE_RIGHTFALLING;
					else m_Buffer[i].type = STROKE_TYPE_LEFTFALLING;
				}
			}
		}
		else // HORIZONTAL 후보
		{
			if( IsHorizontalStroke(i) == TRUE )
			{
				m_Buffer[i].type = STROKE_TYPE_HORIZONTAL;
			}
			else // 방향에 따라서 
			{
				if( m_Buffer[i].vector1.y > 0 ) m_Buffer[i].type = STROKE_TYPE_RIGHTFALLING;
				else m_Buffer[i].type = STROKE_TYPE_LEFTFALLING;
			}
		}	
	}
}


//
// 획 중에 DOT 인 것만 찾는다
BOOL CStrokeList::FindDotStroke()
{
	int i;

	// 1. 길이
	int length = 0;
	
	for( i = 0; i < m_Buffer[0].num_main; i++)
	{
		length += GetLength(m_Buffer[0].MainIndex[0][i]); // 전체 획의 길이 구하기
	}
	if( length > 90 ) return FALSE;
	
	// 2. 각도
	if( abs(m_Buffer[0].vector1.x) > length/5 || abs(m_Buffer[0].vector1.y) > length/5 )
	{
		m_Buffer[0].type = STROKE_TYPE_DOT;
		return TRUE;
	}
	return FALSE;
}

//
// 라인 하나의 길이 
int CStrokeList::GetLength( int line)
{
	CPoint p1, p2;
	p1 = m_LineList->m_Buffer[line].p1;
	p2 = m_LineList->m_Buffer[line].p2;

	// 길이 구하기
	return (int)sqrt((p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y));
}

//
// HOOK 획인 지 여부 
BOOL CStrokeList::IsHookStroke(int stroke)
{
	int i, j, line1, line2;
	int size = m_Buffer[stroke].num_main;

	if( size == 1 ) return FALSE;
		
	for( i = 0; i < size-1; i++)
	{
		line1 = m_Buffer[stroke].MainIndex[0][i];
		for( j = i+1; j < size; j++)
		{
			line2 = m_Buffer[stroke].MainIndex[0][j];
			if( m_LineList->AngleDotProduct( line1, line2) > 75 ) return TRUE;
		}
	}
	return FALSE;
}

//
// VERTICAL 인지 여부 
BOOL CStrokeList::IsVerticalStroke(int stroke)
{
	if( m_Buffer[stroke].vector1.x == 0 ) return TRUE;

	// 72 degree == 3.0777
	if( abs(m_Buffer[stroke].vector1.y) > 3*abs(m_Buffer[stroke].vector1.x)) return TRUE;
	return FALSE;
}

//
// HORIZONTAL 인지 여부
BOOL CStrokeList::IsHorizontalStroke(int stroke)
{
	// 15 degree == .2679 (.25)
	if( 4*abs(m_Buffer[stroke].vector1.y) < abs(m_Buffer[stroke].vector1.x)) return TRUE;
	return FALSE;
}
