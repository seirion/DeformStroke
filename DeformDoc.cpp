            // DeformDoc.cpp : implementation of the CDeformDoc class
//

#include "stdafx.h"
#include "Deform.h"
#include "DeformDoc.h"
#include "DeformView.h"
#include "MainFrm.h"
#include <math.h>
#include <limits.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeformDoc

IMPLEMENT_DYNCREATE(CDeformDoc, CDocument)

BEGIN_MESSAGE_MAP(CDeformDoc, CDocument)
	//{{AFX_MSG_MAP(CDeformDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeformDoc construction/destruction

CDeformDoc::CDeformDoc()
{
	m_FontSelection = 0;
	m_ResultStroke = NULL;
	m_ResultNum = NULL;
	m_LastNum = 0;
	Init();
}

CDeformDoc::~CDeformDoc()
{
	Init();
}

BOOL CDeformDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	Init();

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDeformDoc serialization

void CDeformDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDeformDoc diagnostics

#ifdef _DEBUG
void CDeformDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDeformDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDeformDoc commands

//
// ��Ʈ �ε�, ������ ����
BOOL CDeformDoc::LoadFont(int code)
{
	if( m_FontLoad ) Init();
	
	BOOL result = TRUE;
	char fontPath[256];
	FT_Library library;
	FT_Face face;

	if ( FT_Init_FreeType( &library ) ) result = FALSE;
	GetWindowsDirectory( fontPath, 256 );
	CString path;
	path.Format( "%s", fontPath );

	if( m_FontSelection % 2 ) path += "\\Fonts\\UNI_HSR.TTF";
	else path += "\\Fonts\\gulim.ttc";
	//
	//path += "\\Fonts\\arial.ttf";
	//path = "E:\\fonts\\HMFMOLD.TTF";

	if( FT_New_Face( library, path, 0, &face) ) return FALSE;
	
	if( FT_Set_Pixel_Sizes( face, BITMAP_WIDTH, BITMAP_HEIGHT) ) return FALSE;

	// transform
	FT_Vector     pen;                    /* untransformed origin  */
	FT_Matrix     matrix;                 /* transformation matrix */
	double angle  = 0.0;
	
	/* set up matrix */
	matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
	matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
	matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
	matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );

	/* the pen position in 26.6 cartesian space coordinates; */
	pen.x = 0 * 64;		//���� ����
	pen.y = (int)(BITMAP_HEIGHT/10*1.5) * 64;		//�Ʒ� ���� ����
	FT_Set_Transform( face, &matrix, &pen );

	// load glyph image into the slot (erase previous one)
	if( FT_Load_Char( face, code, FT_LOAD_RENDER ) ) return FALSE;
	
	// 1. outline
	int i;
	int size = face->glyph->outline.n_points;
	m_ControlPoint[0].m_Min.x = m_ControlPoint[0].m_Min.y = INT_MAX;
	m_ControlPoint[0].m_Max.x = m_ControlPoint[0].m_Max.y = INT_MIN;
	int t_x, t_y;
	for( i = 0; i < size; i++)
	{
		t_x = face->glyph->outline.points[i].x/64;
		t_y = BITMAP_HEIGHT - face->glyph->outline.points[i].y/64;
		m_ControlPoint[m_ContourNum].Add( t_x, t_y, face->glyph->outline.tags[i]%2 );

		if( t_x < m_ControlPoint[m_ContourNum].m_Min.x ) m_ControlPoint[m_ContourNum].m_Min.x = t_x;
		else if(t_x > m_ControlPoint[m_ContourNum].m_Max.x)m_ControlPoint[m_ContourNum].m_Max.x = t_x;
		if( t_y < m_ControlPoint[m_ContourNum].m_Min.y ) m_ControlPoint[m_ContourNum].m_Min.y = t_y;
		else if(t_y > m_ControlPoint[m_ContourNum].m_Max.y)m_ControlPoint[m_ContourNum].m_Max.y = t_y;

		if( face->glyph->outline.contours[m_ContourNum] == i )
		{
			m_ContourNum++;
			m_ControlPoint[m_ContourNum].m_Min.x = m_ControlPoint[m_ContourNum].m_Min.y = INT_MAX;
			m_ControlPoint[m_ContourNum].m_Max.x = m_ControlPoint[m_ContourNum].m_Max.y = INT_MIN;

			// cw or ccw �˻� !
			m_ControlPoint[m_ContourNum-1].SetDirection();
		}
		
	}

	// 2. bitmap
	MakeBitmap( &face->glyph->bitmap, face->glyph->bitmap_left, BITMAP_HEIGHT - face->glyph->bitmap_top );

	// 3. grouping & sorting
	MakeGroup();
	GroupSort();

	// 4. make line list
 	MakeLineList();
	
	// 5. find(make) stokes ! - main event & 
	MakeStroke();

	// 6. stroke merge
	for( i = 0; i < m_GroupList.GetSize(); i++ )
	{
		m_StrokeList[i].Sort();				// 6.1 sorting
		m_StrokeList[i].SetContinuity();	// 6.2 check continuity
		m_StrokeList[i].ConnectStroke();	// 6.3 connecting
		
		m_StrokeList[i].MakeTurning();
	}
	
	// ��ο��� ���̵� �ο��ϰ� ���̵𺰷� ���� �ϱ� ! 
	// ���⼭ �̹� ������ �����ϴ� ���� ������ ?
	// 7. �׷찣�� ����
	// 7.1. ���� -> ������ 
	// 7.2. �� -> �Ʒ�
	GroupNumbering(); // �׷��� �ٽ� �ѹ� ���� ()
	//close �ϱ� ���� �غ� 

	//////////////////////////////////////////////////////////////////////////
	// 8. closing stroke
	CloseStroke();
	
	//////////////////////////////////////////////////////////////////////////
	// 9. ordering
	for( i = 0; i < m_GroupList.GetSize(); i++ )
	{
		m_StrokeList[i].Ordering();
	}
	// (view update)

	//////////////////////////////////////////////////////////////////////////
	// 10 Relexation Labeling & parent ���� ���� 
	RelexationLabeling();

	// 11
	MakeLastSort(); // ������ ���� 

	// �׷����� (test)
	CDeformView *pView = (CDeformView *)( ((CMainFrame *)AfxGetMainWnd())->GetActiveView());
	pView->InitCombo();

	//////////////////////////////////////////////////////////////////////////
	// release memory
	FT_Done_FreeType( library ); 
	m_FontLoad = TRUE;
	m_DrawMode = DRAW_MODE_BITMAP;
	
	UpdateAllViews(NULL);
	return result;
}

//
// ���� �ʱ�ȭ, �޸� ����
void CDeformDoc::Init()
{
	m_DrawMode = DRAW_MODE_NONE;
	for( int i = 0; i < m_ContourNum; i++)
	{
		m_ControlPoint[i].Init();
		m_StrokeList[i].Init();
		m_LineList[i].Init();
	}
	m_VirtualLineList.Init();
	
	if( m_ResultStroke ) 
	{
		for( i = 0; i < m_GroupList.GetSize(); i++)
		{
			delete [] m_ResultStroke[i];
		}
		delete [] m_ResultNum;
		delete [] m_ResultStroke;
		m_ResultStroke = NULL;
		m_ResultNum = NULL;
	}

	m_ContourNum = 0;
	m_FontImage.ReleaseImage();
	m_GroupList.Init();
	m_LastNum = 0;
	m_FontLoad = FALSE;
}

//
// ��Ʈ�� Ÿ������ ��ȯ�Ͽ� ����
void CDeformDoc::MakeBitmap(FT_Bitmap *bitmap, int x, int y)
{
	int  i, j, p, q;

	int  x_max = x + bitmap->width;
	int  y_max = y + bitmap->rows;

	m_FontImage.MakeBitmap( BITMAP_WIDTH, BITMAP_HEIGHT );
	int color;

	if( x_max > BITMAP_WIDTH) x_max = BITMAP_WIDTH;
	if( y_max > BITMAP_HEIGHT) y_max = BITMAP_HEIGHT;
	
	for ( i = x, p = 0; i < x_max; i++, p++ )
	{
		for ( j = y, q = 0; j < y_max; j++, q++ )
		{

			if ( i >= BITMAP_WIDTH || j >= BITMAP_HEIGHT )
			{
				m_FontImage.SetAt(i, j, RGB(255, 255, 255) );
				continue;
			}
			if( i < 0 || j < 0 ) continue;
			color = bitmap->buffer[q * bitmap->width + p];
			m_FontImage.SetAt(i, j, RGB(color, color, color) );
		}
	}
}

//
// make group of closed bezier contour
void CDeformDoc::MakeGroup()
{
	int i, j, k;
	// 1. �ܼ� ��ǥ ��; ũ�� ���� ���� 
	int *tempArray;
	tempArray = new int[m_ContourNum];
	for( i = 0; i < m_ContourNum; i++) tempArray[i] = i;

	int value1, value2;
	// selection sorting ; ���� ũ�� (width)
	for( i = 1; i < m_ContourNum; i++)
	{
		for( j = i; j > 0; j--)
		{
			value1 = m_ControlPoint[tempArray[j-1]].m_Max.x - m_ControlPoint[tempArray[j-1]].m_Min.x;
			value2 = m_ControlPoint[tempArray[j]].m_Max.x - m_ControlPoint[tempArray[j]].m_Min.x;

			if( value1 < value2 )
			{ // swapping
				k = tempArray[j-1];
				tempArray[j-1] = tempArray[j];
				tempArray[j] = k;
			}
			else break;
		}
	}

	// selection sorting ; ���� ũ�� ( height)
	for( i = 1; i < m_ContourNum; i++)
	{
		for( j = i; j > 0; j--)
		{
			value1 = m_ControlPoint[tempArray[j-1]].m_Max.y - m_ControlPoint[tempArray[j-1]].m_Min.y;
			value2 = m_ControlPoint[tempArray[j]].m_Max.y - m_ControlPoint[tempArray[j]].m_Min.y;

			if( value1 < value2 )
			{ // swapping
				k = tempArray[j-1];
				tempArray[j-1] = tempArray[j];
				tempArray[j] = k;
			}
			else break;
		}
	}

	// 2. group tree�� ���� & �񱳿����� ���� ���� ����
	m_GroupList.Init(m_ContourNum);
	for( i = 0; i < m_ContourNum; i++)
	{
		k = tempArray[i];

		if( m_ControlPoint[k].m_Direction == DIRECTION_CCW ) 
			m_GroupList.AddRoot(k,m_ControlPoint[k].m_Min, m_ControlPoint[k].m_Max);
		else m_GroupList.AddSub(k,m_ControlPoint[k].m_Min, m_ControlPoint[k].m_Max);
	}

	delete [] tempArray;
}

//
// make simple lines
void CDeformDoc::MakeLineList()
{
	int i, j, k;
	CPoint p1, p2;
	int size = m_GroupList.GetSize();
	int group, length, num, curveNum;
	CPoint curvePoint[2];
	for( i = 0; i < size; i++) // for each group
	{
		group = m_GroupList.m_Buffer[i].parent;
		length = m_ControlPoint[group].m_Size;
		num = 0;
		curveNum = 0;
		for( j = 0; j < length; j++) // # control point
		{
			if( !m_ControlPoint[group].m_Buffer[j].tag )
			{
				if( curveNum < 2 )
				{
					curvePoint[curveNum].x = m_ControlPoint[group].m_Buffer[j].x;
					curvePoint[curveNum].y = m_ControlPoint[group].m_Buffer[j].y;
					curveNum++;
				}
				continue;
			}
			p1.x = m_ControlPoint[group].m_Buffer[j].x;
			p1.y = m_ControlPoint[group].m_Buffer[j].y;
			num++;
		
			if( num ==2 )
			{
				m_LineList[i].Add(p2, p1, group, curveNum, curvePoint[0], curvePoint[1]); 
				num = 1;
				curveNum = 0;
			}
			p2 = p1;
		}
		p1.x = m_ControlPoint[group].m_Buffer[0].x;
		p1.y = m_ControlPoint[group].m_Buffer[0].y;
		m_LineList[i].Add(p2, p1, group, curveNum, curvePoint[0], curvePoint[1]); // closed 

		/* ************************************************ */
		for( k = 0; k < m_GroupList.m_Buffer[i].childNum; k++)
		{
		/* ************************************************ */
		group = m_GroupList.m_Buffer[i].child[k];
		length = m_ControlPoint[group].m_Size;
		num = 0;
		curveNum = 0;
		for( j = 0; j < length; j++) // # control point
		{
			if( !m_ControlPoint[group].m_Buffer[j].tag )
			{
				if( curveNum < 2 )
				{
					curvePoint[curveNum].x = m_ControlPoint[group].m_Buffer[j].x;
					curvePoint[curveNum].y = m_ControlPoint[group].m_Buffer[j].y;
					curveNum++;
				}
				continue;
			}
			p1.x = m_ControlPoint[group].m_Buffer[j].x;
			p1.y = m_ControlPoint[group].m_Buffer[j].y;
			num++;
		
			if( num ==2 )
			{
				m_LineList[i].Add(p2, p1, group, curveNum, curvePoint[0], curvePoint[1]);
				num = 1;
				curveNum = 0;
			}
			p2 = p1;
		}
		p1.x = m_ControlPoint[group].m_Buffer[0].x;
		p1.y = m_ControlPoint[group].m_Buffer[0].y;
		m_LineList[i].Add(p2, p1, group, curveNum, curvePoint[0], curvePoint[1]); // closed 
		/* ************************************************ */
		}
		/* ************************************************ */
	}
}

//
// make simple strokes
void CDeformDoc::MakeStroke()
{
	int i; // for indexing & simple variables

	for( i = 0; i < m_GroupList.GetSize(); i++) // �׷� ���� ȹ ã�� 
	{
		m_StrokeList[i].SetLineList(&m_LineList[i]);
		m_StrokeList[i].MakeStroke();
	}
}


// 07. Closing Stroke
void CDeformDoc::CloseStroke()
{
	// 1 memory allocation
	CPoint p1, p2, p3, p4, op2;
	int i, j, k, size;
	int contour1, pointIndex1, contour2, pointIndex2; // ã�� �� ���
	int m , n;

	m_ResultStroke = new CControlPoint *[m_GroupList.GetSize()];
	m_ResultNum = new int [m_GroupList.GetSize()];
	for( i = 0; i < m_GroupList.GetSize(); i++ )
	{
		// ���� ����� �ٽ� �ؾ� ��
		m_ResultStroke[i] = new CControlPoint[m_StrokeList[i].m_Size];
		m_ResultNum[i] = 0;

		// ����Ʈ �߰��ϱ� 
		for( j = 0; j < m_StrokeList[i].m_Size; j++)
		{	
			if( m_StrokeList[i].m_Buffer[j].use == FALSE ) continue;	// �߰� �� ��

			//////////////////////////////////////////////////////////////////////////
			// 1. main
			size = m_StrokeList[i].m_Buffer[j].num_main;

			for( k = 0; k < size; k++)
			{
				if( k > 0 )
				{
					m = m_StrokeList[i].m_Buffer[j].MainIndex[0][k];
					p2 = m_LineList[i].m_Buffer[m].p1;
					GetPointIndex(p1, i, contour1, pointIndex1);
					if( op2 != p2 )
					{
						m_ResultStroke[i][m_ResultNum[i]].Add( op2.x, op2.y, 1);
					}
				}
				m = m_StrokeList[i].m_Buffer[j].MainIndex[0][k];
				p1 = m_LineList[i].m_Buffer[m].p1;
				p2 = m_LineList[i].m_Buffer[m].p2;

				// CControlPoint
				GetPointIndex(p1, i, contour1, pointIndex1);
				GetPointIndex(p2, i, contour2, pointIndex2);
				op2 = p2;
				if( contour1 != contour2) AfxMessageBox("xxddxx");// error
				if( pointIndex2 == 0 ) pointIndex2 = m_ControlPoint[contour1].m_Size;
				for( n = pointIndex1; n < pointIndex2; n++)
				{
					m_ResultStroke[i][m_ResultNum[i]].Add( m_ControlPoint[contour1].m_Buffer[n].x, // add control points
						m_ControlPoint[contour1].m_Buffer[n].y, m_ControlPoint[contour1].m_Buffer[n].tag);
				}
			}

			//////////////////////////////////////////////////////////////////////////
			// 2. main - opposite (2)
			n = m_StrokeList[i].m_Buffer[j].num_main-1;
			m = m_StrokeList[i].m_Buffer[j].MainIndex[0][n];
			p1 = m_LineList[i].m_Buffer[m].p2;
			m = m_StrokeList[i].m_Buffer[j].MainIndex[1][0];
			p2 = m_LineList[i].m_Buffer[m].p1;

			// CControlPoint
			GetPointIndex(p1, i, contour1, pointIndex1);
			GetPointIndex(p2, i, contour2, pointIndex2);
			if( pointIndex2 == 0 ) pointIndex2 = m_ControlPoint[contour1].m_Size;
			
			if( m_StrokeList[i].m_Buffer[j].open2 == 0 )			// �� �߰� 
			{
				if( contour1 != contour2) AfxMessageBox("xxddxx");// error
				
				for( n = pointIndex1; n < pointIndex2; n++)
				{
					m_ResultStroke[i][m_ResultNum[i]].Add( m_ControlPoint[contour1].m_Buffer[n].x, // add control points
						m_ControlPoint[contour1].m_Buffer[n].y, m_ControlPoint[contour1].m_Buffer[n].tag);
				}
			}
			else if( m_StrokeList[i].m_Buffer[j].open2 == 1 && contour1 == contour2 && // ���� 
				pointIndex1 < pointIndex2 && PointNotUse(i, j, p1, p2) )
			{
				if( pointIndex2 == 0 ) pointIndex2 = m_ControlPoint[contour1].m_Size;
				for( n = pointIndex1; n < pointIndex2; n++)
				{
					m_ResultStroke[i][m_ResultNum[i]].Add( m_ControlPoint[contour1].m_Buffer[n].x, // add control points
						m_ControlPoint[contour1].m_Buffer[n].y, m_ControlPoint[contour1].m_Buffer[n].tag);
				}
			}
			else
			{
				n = m_StrokeList[i].m_Buffer[j].num_main-1;
				m = m_StrokeList[i].m_Buffer[j].MainIndex[0][n];
				p1 = m_LineList[i].m_Buffer[m].p2;
				GetPointIndex(p1, i, contour1, pointIndex1);
				m_ResultStroke[i][m_ResultNum[i]].Add( m_ControlPoint[contour1].m_Buffer[pointIndex1].x, // add control points
					m_ControlPoint[contour1].m_Buffer[pointIndex1].y, m_ControlPoint[contour1].m_Buffer[pointIndex1].tag);
			}
			

			//////////////////////////////////////////////////////////////////////////
			// 3. opposite
			size = m_StrokeList[i].m_Buffer[j].num_oppsite;
			for( k = 0; k < size; k++)
			{
				if( k > 0 )
				{
					m = m_StrokeList[i].m_Buffer[j].MainIndex[1][k];
					p2 = m_LineList[i].m_Buffer[m].p1;
					GetPointIndex(p1, i, contour1, pointIndex1);
					if( op2 != p2 )
					{
						m_ResultStroke[i][m_ResultNum[i]].Add( op2.x, op2.y, 1);
					}
				}
				m = m_StrokeList[i].m_Buffer[j].MainIndex[1][k];
				p1 = m_LineList[i].m_Buffer[m].p1;
				p2 = m_LineList[i].m_Buffer[m].p2;

				// CControlPoint
				GetPointIndex(p1, i, contour1, pointIndex1);
				GetPointIndex(p2, i, contour2, pointIndex2);
				op2 = p2;
				if( contour1 != contour2) AfxMessageBox("xxddxx");// error
				if( pointIndex2 == 0 ) pointIndex2 = m_ControlPoint[contour1].m_Size;
				for( n = pointIndex1; n < pointIndex2; n++)
				{
					m_ResultStroke[i][m_ResultNum[i]].Add( m_ControlPoint[contour1].m_Buffer[n].x, // add control points
						m_ControlPoint[contour1].m_Buffer[n].y, m_ControlPoint[contour1].m_Buffer[n].tag);
				}
			}


			//////////////////////////////////////////////////////////////////////////
			// 4. opposite - main (1)
			n = m_StrokeList[i].m_Buffer[j].num_oppsite-1;
			m = m_StrokeList[i].m_Buffer[j].MainIndex[1][n];
			p1 = m_LineList[i].m_Buffer[m].p2;
			m = m_StrokeList[i].m_Buffer[j].MainIndex[0][0];
			p2 = m_LineList[i].m_Buffer[m].p1;

			// CControlPoint
			GetPointIndex(p1, i, contour1, pointIndex1);
			GetPointIndex(p2, i, contour2, pointIndex2);
			if( pointIndex2 == 0 ) pointIndex2 = m_ControlPoint[contour1].m_Size;

			if( m_StrokeList[i].m_Buffer[j].open1 == 0 )			// �� �߰� 
			{
				if( contour1 != contour2) AfxMessageBox("xxddxx");// error
				
				for( n = pointIndex1; n < pointIndex2; n++)
				{
					m_ResultStroke[i][m_ResultNum[i]].Add( m_ControlPoint[contour1].m_Buffer[n].x, // add control points
						m_ControlPoint[contour1].m_Buffer[n].y, m_ControlPoint[contour1].m_Buffer[n].tag);
				}
			}
			else if( m_StrokeList[i].m_Buffer[j].open1 == 1 && contour1 == contour2 && // ���� 
				pointIndex1 < pointIndex2 && PointNotUse(i, j, p1, p2) )
			{
				if( pointIndex2 == 0 ) pointIndex2 = m_ControlPoint[contour1].m_Size;
				for( n = pointIndex1; n < pointIndex2; n++)
				{
					m_ResultStroke[i][m_ResultNum[i]].Add( m_ControlPoint[contour1].m_Buffer[n].x, // add control points
						m_ControlPoint[contour1].m_Buffer[n].y, m_ControlPoint[contour1].m_Buffer[n].tag);
				}
			}
			else 
			{
				n = m_StrokeList[i].m_Buffer[j].num_oppsite-1;
				m = m_StrokeList[i].m_Buffer[j].MainIndex[1][n];
				p1 = m_LineList[i].m_Buffer[m].p2;
				GetPointIndex(p1, i, contour1, pointIndex1);
				m_ResultStroke[i][m_ResultNum[i]].Add( m_ControlPoint[contour1].m_Buffer[pointIndex1].x, // add control points
					m_ControlPoint[contour1].m_Buffer[pointIndex1].y, m_ControlPoint[contour1].m_Buffer[pointIndex1].tag);
			}
			m_ResultNum[i]++;
		}// for(j)
	}// for(i)
}

//
// ����Ʈ ��ġ, �׷� ��ȣ, ���� , ����
void CDeformDoc::GetPointIndex(CPoint& p, int group, int& contour, int& index)
{
	int i, j, size;
	contour = m_GroupList.m_Buffer[group].parent;
	for( i = 0; i < m_ControlPoint[contour].m_Size; i++ )
	{
		if( m_ControlPoint[contour].m_Buffer[i].x == p.x && m_ControlPoint[contour].m_Buffer[i].y == p.y )
		{
			index = i;
			return;
		}
	}

	size = m_GroupList.m_Buffer[group].childNum;
	for( j = 0; j < size; j++ )
	{
		contour = m_GroupList.m_Buffer[group].child[j];
		for( i = 0; i < m_ControlPoint[contour].m_Size; i++ )
		{
			if( m_ControlPoint[contour].m_Buffer[i].x == p.x && m_ControlPoint[contour].m_Buffer[i].y == p.y )
			{
				index = i;
				return;
			}
		} // for(i)
	} // for(j)

	contour = -1;
	index = -1;
}

//
// �ٸ� ���� ��� ���ϸ� TRUE 
BOOL CDeformDoc::PointNotUse( int group, int index, CPoint& op1, CPoint &op2)
{
	int i, in, size, last;
	
	CPoint p1, p2;

	size = m_StrokeList[group].m_Size;
	for( i = 0; i < size; i++ )
	{
		if( i == index ) continue;
		if( m_StrokeList[group].m_Buffer[i].use == FALSE ) continue;
		// 0
		in = m_StrokeList[group].m_Buffer[i].MainIndex[0][0];
		p1 = m_LineList[group].m_Buffer[in].p1;
		last = m_StrokeList[group].m_Buffer[i].num_oppsite-1;
		in = m_StrokeList[group].m_Buffer[i].MainIndex[1][last];
		p2 = m_LineList[group].m_Buffer[in].p2;

		if( p1 == op1 || p2 == op2 || p1 == op2 || p2 == op1) return FALSE;
		// 1
		last = m_StrokeList[group].m_Buffer[i].num_main-1;
		in = m_StrokeList[group].m_Buffer[i].MainIndex[0][last];
		p1 = m_LineList[group].m_Buffer[in].p2;
		
		in = m_StrokeList[group].m_Buffer[i].MainIndex[1][0];
		p2 = m_LineList[group].m_Buffer[in].p1;
		if( p1 == op1 || p2 == op2|| p1 == op2 || p2 == op1) return FALSE;

	}
	
	return TRUE;
}

//
// �׷쿡 ���ؼ� ������ ( �¿� �켱 -> ���� )
void CDeformDoc::GroupSort()
{
	int i, j;
	int left, right, width;
	MyGroup t_Group;
	const int error=50;		// ���� ����
	// center �� �ִ� �� ���� 
	for( i = 0; i < m_GroupList.GetSize(); i++)
	{
		width = m_GroupList.m_Buffer[i].max.x - m_GroupList.m_Buffer[i].min.x;
		left = BITMAP_WIDTH/2 - m_GroupList.m_Buffer[i].min.x;
		right = m_GroupList.m_Buffer[i].min.y - BITMAP_WIDTH/2;

		if( abs(left-right) < width/10) m_GroupList.m_Buffer[i].center = TRUE;
		else m_GroupList.m_Buffer[i].center = FALSE;
	}
	// ����
	for( i = 1; i < m_GroupList.GetSize(); i++)
	{
		// overlap
		for( j = i; j > 0; j--)
		{
			// 1. �¿� 
			if( m_GroupList.m_Buffer[j].center == FALSE && m_GroupList.m_Buffer[j-1].center == FALSE )
			{ //-_- error
				if( m_GroupList.m_Buffer[j-1].max.x-error < m_GroupList.m_Buffer[j].min.x) break; // break
				else if(m_GroupList.m_Buffer[j].max.x-error < m_GroupList.m_Buffer[j-1].min.x)
				{ // swap 
					t_Group = m_GroupList.m_Buffer[j-1];
					m_GroupList.m_Buffer[j-1] = m_GroupList.m_Buffer[j];
					m_GroupList.m_Buffer[j] = t_Group;
					continue;
				}
			}
			
			// 2. ���� 
			if( m_GroupList.m_Buffer[j-1].max.y < m_GroupList.m_Buffer[j].min.y) break;
			else if(m_GroupList.m_Buffer[j].max.y < m_GroupList.m_Buffer[j-1].min.y)
			{//swap
				t_Group = m_GroupList.m_Buffer[j-1];
				m_GroupList.m_Buffer[j-1] = m_GroupList.m_Buffer[j];
				m_GroupList.m_Buffer[j] = t_Group;
				continue;
			}
			else
			{
				//AfxMessageBox("1-_-��Ÿ");
				// 1 ���ϴ� �� ? (j-1)
				if( m_GroupList.m_Buffer[j-1].max.y > m_GroupList.m_Buffer[j].max.y &&
					m_GroupList.m_Buffer[j-1].min.y < m_GroupList.m_Buffer[j].min.y &&
					m_GroupList.m_Buffer[j-1].max.x > m_GroupList.m_Buffer[j].max.x &&
					m_GroupList.m_Buffer[j-1].min.x < m_GroupList.m_Buffer[j].min.x  )
				{break;
				}
				else if( m_GroupList.m_Buffer[j-1].max.y < m_GroupList.m_Buffer[j].max.y && // (j)
					m_GroupList.m_Buffer[j-1].min.y > m_GroupList.m_Buffer[j].min.y &&
					m_GroupList.m_Buffer[j-1].max.x < m_GroupList.m_Buffer[j].max.x &&
					m_GroupList.m_Buffer[j-1].min.x > m_GroupList.m_Buffer[j].min.x  )
				{// swap 
					t_Group = m_GroupList.m_Buffer[j-1];
					m_GroupList.m_Buffer[j-1] = m_GroupList.m_Buffer[j];
					m_GroupList.m_Buffer[j] = t_Group;
					continue;
				}
				else // ������
				{
					// �� -> �� 
					if( m_GroupList.m_Buffer[j-1].min.x < m_GroupList.m_Buffer[j].min.x &&
						m_GroupList.m_Buffer[j-1].max.x < m_GroupList.m_Buffer[j].max.x )
					{
						break; // break
					}
					else if(m_GroupList.m_Buffer[j-1].min.x > m_GroupList.m_Buffer[j].min.x &&
							m_GroupList.m_Buffer[j-1].max.x > m_GroupList.m_Buffer[j].max.x )
					{// swap
						t_Group = m_GroupList.m_Buffer[j-1];
						m_GroupList.m_Buffer[j-1] = m_GroupList.m_Buffer[j];
						m_GroupList.m_Buffer[j] = t_Group;
						continue;
					}
					if( m_GroupList.m_Buffer[j-1].min.y < m_GroupList.m_Buffer[j].min.y && // y �� 
						m_GroupList.m_Buffer[j-1].max.y < m_GroupList.m_Buffer[j].max.y )
					{  // 
						break;
					}
					else
					{
						t_Group = m_GroupList.m_Buffer[j-1];
						m_GroupList.m_Buffer[j-1] = m_GroupList.m_Buffer[j];
						m_GroupList.m_Buffer[j] = t_Group;
					}
				}
			}// 
		}// for(j)
	}// for(i)
}




void CDeformDoc::GroupNumbering()
{
	int i, j;
	int width1, height1, width2, height2;	// �ٿ�� �ڽ� ũ�� 
	int candidate;		// ��Ī �Ǵ� ���� �ĺ�
	int center;			// ��Ī �Ǵ� ���� �߰��� �� �׷� �ĺ� 

	for( i = 0; i < m_GroupList.GetSize(); i++ ) m_StrokeList[i].m_GroupNum = i*100; // �ʱ�ȭ 

	//////////////////////////////////////////////////////////////////////////
	// ���� ���� numbering
	for( i = 0; i < m_GroupList.GetSize(); i++ )
	{
		if( m_StrokeList[i].m_GroupNum != i*100 ) continue;
		
		// 01. ���ϴ� �� 
		for( j = 0; j < i; j++)
		{
			if( m_GroupList.m_Buffer[j].childNum > 0 &&
				m_GroupList.m_Buffer[j].min.x < m_GroupList.m_Buffer[i].min.x &&
				m_GroupList.m_Buffer[j].min.y < m_GroupList.m_Buffer[i].min.y &&
				m_GroupList.m_Buffer[j].max.x > m_GroupList.m_Buffer[i].max.x &&
				m_GroupList.m_Buffer[j].max.y > m_GroupList.m_Buffer[i].max.y )
			{
				m_StrokeList[i].m_GroupNum = m_StrokeList[j].m_GroupNum;
				break;
			}
		}

		if( i != j ) continue;

		// 02. 1��¥�� stroke �� ��� ¦ ã��
		if( m_StrokeList[i].m_Size > 1 ) continue;
		width1 = m_GroupList.m_Buffer[i].max.x - m_GroupList.m_Buffer[i].min.x;
		height1 = m_GroupList.m_Buffer[i].max.y - m_GroupList.m_Buffer[i].min.y;

		if( width1 > height1*2) continue;

		// candidate ã�� 
		candidate = -1;
		for( j = 0; j < m_GroupList.GetSize(); j++ )
		{
			if( i == j ) continue; // ���� ��ȣ ���� 
			if( m_StrokeList[j].m_Size > 1 ) continue; // �ϳ�¥�� ȹ�� ���� 
			width2 = m_GroupList.m_Buffer[j].max.x - m_GroupList.m_Buffer[j].min.x;
			height2 = m_GroupList.m_Buffer[j].max.y - m_GroupList.m_Buffer[j].min.y;

			if( width2 > height2*2) continue;
			if( m_GroupList.m_Buffer[i].max.y < m_GroupList.m_Buffer[j].min.y ||
				m_GroupList.m_Buffer[j].max.y < m_GroupList.m_Buffer[i].min.y) continue;

			if( width1+width2 > abs(width1-width2)*5 && height1+height2 > abs(height1-height2)*5 )
			{
				candidate = j;
				break;
			}
		}
		
		if( candidate == -1 ) continue;
		
		// center ã�� ( y �� ���̷� ���� �ϸ鼭 ��ġ������ �߰��� �;��� )
		center = -1;
		for( j = 0; j < m_GroupList.GetSize(); j++ )
		{
			if( i == j || i == candidate ) continue; // ���� ��ȣ ���� 
			
			if( m_GroupList.m_Buffer[j].min.y < m_GroupList.m_Buffer[i].min.y && // i
				m_GroupList.m_Buffer[j].max.y > m_GroupList.m_Buffer[i].max.y && // i
				m_GroupList.m_Buffer[j].min.y < m_GroupList.m_Buffer[candidate].min.y && // candidate
				m_GroupList.m_Buffer[j].max.y > m_GroupList.m_Buffer[candidate].max.y && // candidate
				m_GroupList.m_Buffer[j].max.x > m_GroupList.m_Buffer[i].min.x && // center -- i 
				m_GroupList.m_Buffer[j].min.x < m_GroupList.m_Buffer[candidate].max.x && // center -- candidate 

				m_GroupList.m_Buffer[j].max.x > m_GroupList.m_Buffer[i].max.x &&
				m_GroupList.m_Buffer[j].min.x < m_GroupList.m_Buffer[candidate].min.x )
			{
				center = j;
				break;
			}
		}

		if( center != -1 ) // ã����
		{
			// ���� ���ϰ� �Ǹ� ���� �׷� ��ȣ�� �ְ� �� 
			if(m_GroupList.m_Buffer[center].min.x < m_GroupList.m_Buffer[i].min.x &&
				m_GroupList.m_Buffer[center].min.y < m_GroupList.m_Buffer[i].min.y &&
				m_GroupList.m_Buffer[center].max.x > m_GroupList.m_Buffer[i].max.x &&
				m_GroupList.m_Buffer[center].max.y > m_GroupList.m_Buffer[i].max.y )
			{
				m_StrokeList[i].m_GroupNum = m_StrokeList[center].m_GroupNum;
				m_StrokeList[candidate].m_GroupNum = m_StrokeList[center].m_GroupNum;
			}
			else // ������ �ʴ� ��� 
			{
				m_StrokeList[i].m_GroupNum = m_StrokeList[center].m_GroupNum +1;
				m_StrokeList[candidate].m_GroupNum = m_StrokeList[center].m_GroupNum +2;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// ������ 
	CString str, sub;
	str.Empty();
	for( i = 0; i < m_GroupList.GetSize(); i++ )
	{
		sub.Format("%d  ", m_StrokeList[i].m_GroupNum);
		str += sub;
	}
	AfxMessageBox(str);
}

//////////////////////////////////////////////////////////////////////////
// ������ ���� 
void CDeformDoc::MakeLastSort()
{
	int i, j;

	MyStroke* temp_stroke; // swap ��
	int ps1, ps2, compare; // parent stroke
	//////////////////////////////////////////////////////////////////////////
	// ���� 
	for( i = 1; i < m_LastNum; i++ )
	{
		for( j = i; j > 0; j-- )
		{ // ������ ũ�� 1, ������ 0, �������� ũ�� -1 (ũ�� == �켱��������)

			// parent �� üũ ���� 
			ps1 = GetParent(j-1);
			ps2 = GetParent(j);

			if( ps1 == -1 ) ps1 = j-1;
			if( ps2 == -1 ) ps2 = j;

			if( ps1 != ps2 ) compare = CompareStroke(ps1, ps2);
			else compare = CompareStroke(j-1,j);
			if( compare == 1)
			{// swap
				temp_stroke = m_LastResult[j];
				m_LastResult[j] = m_LastResult[j-1];
				m_LastResult[j-1] = temp_stroke;
			}else break;
		}
	}
}




//////////////////////////////////////////////////////////////////////////
// ȹ ���� ����



// stroke1 �� �����̸� return -1
int CDeformDoc::CompareStroke(int stroke1, int stroke2 )
{
	// �ٸ� �׷��� ���� �� ���� ����
	if( m_LastResult[stroke1]->r_key > m_LastResult[stroke2]->r_key ) return 1;
	else if( m_LastResult[stroke1]->r_key < m_LastResult[stroke2]->r_key ) return -1;

	//////////////////////////////////////////////////////////////////////////
	// ���� �׷� �������� ��


	int i, j, size1, size2, group1, group2, index1, index2;
	int over = 0; // ��ġ�� ����Ʈ ���� 

	group1 = m_LastResult[stroke1]->r_group;
	index1 = m_LastResult[stroke1]->r_index;
	size1 = m_ResultStroke[group1][index1].m_Size;

	group2 = m_LastResult[stroke2]->r_group;
	index2 = m_LastResult[stroke2]->r_index;
	size2 = m_ResultStroke[group2][index2].m_Size;
	
	for( i = 0; i < size1; i++)
	{
		if( m_ResultStroke[group1][index1].m_Buffer[i].tag == 0 ) continue; // fix point �� �ش�
		for( j = 0; j < size2; j++) // ��� ��츦 ����
		{
			if( m_ResultStroke[group2][index2].m_Buffer[j].tag == 0 ) continue;
			if( m_ResultStroke[group1][index1].m_Buffer[i].x == m_ResultStroke[group2][index2].m_Buffer[j].x &&
				m_ResultStroke[group1][index1].m_Buffer[i].y == m_ResultStroke[group2][index2].m_Buffer[j].y)
			{
				over++;
				break;
			}
		}
	}

	if( over == 0 ) return CompareAway(stroke1, stroke2); // 1 �� ��ġ�� ��� - �Ϲ��� ��
	else return CompareOverlap(stroke1, stroke2, over); // 2 ��ġ�� ��� 
}

//
// ���� ������ �ִ� ȹ ������ �� (���� ����)
// ������ ū ���(�������� ���� ȹ�� ��� ) return 1,    otherwise return 0
int CDeformDoc::CompareAway(int stroke1, int stroke2)
{
	int group1, group2, index1, index2;
	CRect gRect, sRect; // �׷� �ڽ��� �����ũ �ڽ�

	group1 = m_LastResult[stroke1]->r_group;
	index1 = m_LastResult[stroke1]->r_orgIndex;

	group2 = m_LastResult[stroke2]->r_group;
	index2 = m_LastResult[stroke2]->r_orgIndex;

	// stroke ������
	MyStroke *pStroke1, *pStroke2;
	pStroke1 = &m_StrokeList[group1].m_Buffer[index1];
	pStroke2 = &m_StrokeList[group2].m_Buffer[index2];
	
	
	gRect.left = m_GroupList.m_Buffer[pStroke1->r_key/100].min.x; // bound box (group)
	gRect.right = m_GroupList.m_Buffer[pStroke1->r_key/100].max.x;
	gRect.top = m_GroupList.m_Buffer[pStroke1->r_key/100].min.y;
	gRect.bottom = m_GroupList.m_Buffer[pStroke1->r_key/100].max.y;

	//////////////////////////////////////////////////////////////////////////
	// 0 DOT ó�� 
	if( pStroke1->type == STROKE_TYPE_DOT && pStroke2->type != STROKE_TYPE_DOT )
	{
		if( pStroke2->type != STROKE_TYPE_HORIZONTAL ) return 1;
		
		// ��ġ ��
		sRect = pStroke2->bound;

		if( (gRect.bottom-gRect.top) > 4*(sRect.top-gRect.bottom) ) return -1;
		else return 1;
	}
	else if( pStroke1->type != STROKE_TYPE_DOT && pStroke2->type == STROKE_TYPE_DOT )
	{
		if( pStroke1->type != STROKE_TYPE_HORIZONTAL ) return -1;

		// ��ġ ��
		sRect = pStroke1->bound;

		if( (gRect.bottom-gRect.top) > 4*(sRect.top-gRect.bottom) ) return 1;
		else return -1;
	}

	//////////////////////////////////////////////////////////////////////////
	// 1. center �߽����� �¿� ��Ī

	if( (pStroke1->type == STROKE_TYPE_LEFTFALLING || pStroke1->type == STROKE_TYPE_RIGHTFALLING) &&
		(pStroke2->type == STROKE_TYPE_VERTICAL || pStroke2->type == STROKE_TYPE_HOOK )	)
	{
		if( FindCandidate(stroke1, stroke2) == TRUE ) return 1; // stroke2 �� ���� 
	}
	else 
	if( (pStroke2->type == STROKE_TYPE_LEFTFALLING || pStroke2->type == STROKE_TYPE_RIGHTFALLING) &&
		(pStroke1->type == STROKE_TYPE_VERTICAL || pStroke1->type == STROKE_TYPE_HOOK )	)
	{
		if( FindCandidate(stroke2, stroke1) == TRUE ) return -1; // stroke1 �� ���� 
	}



	//////////////////////////////////////////////////////////////////////////
	// 2. ������ (score base)  
	int height, width;


	// �� - �Ʒ� 
	if( pStroke1->bound.bottom <= pStroke2->bound.top ) return -1;
	if( pStroke2->bound.bottom <= pStroke1->bound.top ) return 1; 

	// �� - ��      (���� ���)
	if( pStroke1->bound.right <= pStroke2->bound.left ) return -1;
	if( pStroke2->bound.right <= pStroke1->bound.left ) return 1;



	// 1���� ��ġ
	//       ----- 2
	//
	//  ----- 1
	height = (pStroke1->bound.top - pStroke2->bound.top)+(pStroke1->bound.bottom - pStroke2->bound.bottom); 
	width = (pStroke2->bound.left - pStroke1->bound.left)+(pStroke2->bound.right - pStroke1->bound.right);
	if( pStroke1->bound.top > pStroke2->bound.top && pStroke1->bound.bottom > pStroke2->bound.bottom &&
		pStroke2->bound.left > pStroke1->bound.left && pStroke2->bound.right > pStroke1->bound.right &&
		height > 0 && width > 0 )
	{
		if( width > height ) return -1;
		else return 1;
	}



	// 1���� ��ġ
	//       ----- 1
	//
	//  ----- 2
	height = (pStroke2->bound.top - pStroke1->bound.top)+(pStroke2->bound.bottom - pStroke1->bound.bottom); 
	width = (pStroke1->bound.left - pStroke2->bound.left)+(pStroke1->bound.right - pStroke2->bound.right);
	if( height > 0 && width > 0 )
	{
		if( width > height ) return 1;
		else return -1;
	}





	// �� - �Ʒ� 
	if( pStroke1->bound.top < pStroke2->bound.top && pStroke1->bound.bottom < pStroke2->bound.bottom ) return -1;
	if( pStroke2->bound.top < pStroke1->bound.top && pStroke2->bound.bottom < pStroke1->bound.bottom) return 1; 

	// �� - ��     (������)
	if( pStroke1->bound.left < pStroke2->bound.left && pStroke1->bound.right < pStroke2->bound.right ) return -1;
	if( pStroke2->bound.left < pStroke1->bound.left && pStroke2->bound.right < pStroke1->bound.right) return 1; 


	/* �߸� �� ���*/
	return 0;
}


//
// ��ġ�� ȹ ������ �� 
// ������ ū ��� return 1, otherwise return 0
int CDeformDoc::CompareOverlap(int stroke1, int stroke2, int over)
{
	int size1, size2, group1, group2, index1, index2;
	CRect gRect, sRect; // �׷� �ڽ��� �����ũ �ڽ�

	group1 = m_LastResult[stroke1]->r_group;
	index1 = m_LastResult[stroke1]->r_orgIndex;
	size1 = m_ResultStroke[group1][index1].m_Size;

	group2 = m_LastResult[stroke2]->r_group;
	index2 = m_LastResult[stroke2]->r_orgIndex;
	size2 = m_ResultStroke[group2][index2].m_Size;

	// stroke ������
	MyStroke *pStroke1, *pStroke2;
	pStroke1 = &m_StrokeList[group1].m_Buffer[index1];
	pStroke2 = &m_StrokeList[group2].m_Buffer[index2];
	
	
	gRect.left = m_GroupList.m_Buffer[pStroke1->r_key/100].min.x; // bound box (group)
	gRect.right = m_GroupList.m_Buffer[pStroke1->r_key/100].max.x;
	gRect.top = m_GroupList.m_Buffer[pStroke1->r_key/100].min.y;
	gRect.bottom = m_GroupList.m_Buffer[pStroke1->r_key/100].max.y;

	//////////////////////////////////////////////////////////////////////////
	/*
	STROKE_TYPE_HORIZONTAL			1

	STROKE_TYPE_VERTICAL			2
	STROKE_TYPE_HOOK				3

	STROKE_TYPE_LEFTFALLING			4
	STROKE_TYPE_RIGHTFALLING		5

	STROKE_TYPE_TURNING				6
	*/
	int type1, type2;
	type1 = pStroke1->type /0x10*0x10;
	type2 = pStroke2->type /0x10*0x10;

	if( type1 == type2 ) return CompareAway(stroke1, stroke2);

	// 1 - (2 ~ 6)
	if( type1 == STROKE_TYPE_HORIZONTAL && type2 >= STROKE_TYPE_VERTICAL )
		return Compare_Horizontal_Vertical( stroke1, stroke2, over );
	
	if( type2 == STROKE_TYPE_HORIZONTAL && type1 >= STROKE_TYPE_VERTICAL )
		return -1 * Compare_Horizontal_Vertical( stroke2, stroke1, over );

	// 2 - 4,       3 - 4 
	if( (type1 == STROKE_TYPE_VERTICAL || type1 == STROKE_TYPE_HOOK ) && 
		type2 == STROKE_TYPE_LEFTFALLING )
		return Compare_Vertical_Leftfalling(stroke1, stroke2, over );

	if( (type2 == STROKE_TYPE_VERTICAL || type2 == STROKE_TYPE_HOOK ) && 
		type1 == STROKE_TYPE_LEFTFALLING )
		return -1 * Compare_Vertical_Leftfalling(stroke2, stroke1, over );

	
	// 2 - 5
	if( (type1 == STROKE_TYPE_VERTICAL || type1 == STROKE_TYPE_HOOK ) && 
		type2 == STROKE_TYPE_RIGHTFALLING )
		return Compare_Vertical_Rightfalling(stroke1, stroke2, over );

	if( (type2 == STROKE_TYPE_VERTICAL || type2 == STROKE_TYPE_HOOK ) && 
		type1 == STROKE_TYPE_RIGHTFALLING )
		return -1 * Compare_Vertical_Rightfalling(stroke2, stroke1, over );


	// 2 - 6,     3 - 6 ,       4 - 6
	if( (type1 == STROKE_TYPE_VERTICAL || type1 == STROKE_TYPE_HOOK || type1 == STROKE_TYPE_LEFTFALLING) && 
		type2 == STROKE_TYPE_TURNING )
		return Compare_Vertical_Turning(stroke1, stroke2, over );

	if( (type2 == STROKE_TYPE_VERTICAL || type2 == STROKE_TYPE_HOOK || type2 == STROKE_TYPE_LEFTFALLING) && 
		type1 == STROKE_TYPE_TURNING )
		return -1 * Compare_Vertical_Turning(stroke2, stroke1, over );

	// 3 - 6

	// 4 - 5 
	if( type1 == STROKE_TYPE_LEFTFALLING && type2 >= STROKE_TYPE_RIGHTFALLING )
		return -1; // Compare_Leftfalling_Rightfalling( stroke1, stroke2, over );

	if( type2 == STROKE_TYPE_LEFTFALLING && type1 >= STROKE_TYPE_RIGHTFALLING )
		return 1;

	// 5 - 6
	// 
	/* �߸� �� ���*/
	return 0;
}

//
// ��Ī �� �߽����� ���ʿ� ȹ�� �ִ� ���
// ( stroke1 �� ���� , stroke2 �� �߽ɼ� )
// ���� ȹ�� ���� ���� return TRUE
BOOL CDeformDoc::FindCandidate( int stroke1, int stroke2)
{
	int i, size1, group1, group2, index1, index2;
	int width1, width2, height1, height2;
	CRect gRect, sRect; // �׷� �ڽ��� �����ũ �ڽ�
	
	group1 = m_LastResult[stroke1]->r_group;
	index1 = m_LastResult[stroke1]->r_orgIndex;

	group2 = m_LastResult[stroke2]->r_group;
	index2 = m_LastResult[stroke2]->r_orgIndex;

	// stroke ������
	MyStroke *pStroke1, *pStroke2, *pStroke_temp;
	pStroke1 = &m_StrokeList[group1].m_Buffer[index1];
	pStroke2 = &m_StrokeList[group2].m_Buffer[index2];
	
	
	gRect.left = m_GroupList.m_Buffer[pStroke1->r_key/100].min.x; // bound box (group)
	gRect.right = m_GroupList.m_Buffer[pStroke1->r_key/100].max.x;
	gRect.top = m_GroupList.m_Buffer[pStroke1->r_key/100].min.y;
	gRect.bottom = m_GroupList.m_Buffer[pStroke1->r_key/100].max.y;
	
	//////////////////////////////////////////////////////////////////////////
	int candidate;

	if( pStroke1->bound.left > pStroke2->bound.left ) return FALSE; // ��ġ ���� 

	width1 = pStroke1->bound.right - pStroke1->bound.left;
	height1 = pStroke1->bound.bottom - pStroke1->bound.top;


	// candidate ã�� 
	candidate = -1;

	size1 = m_LastNum;
	for( i = 0; i < size1; i++ )
	{
		if( i == stroke1 || i == stroke2 ) continue; // ���� ��ȣ ���� 

		// i ��° ȹ�� ������
		group1 = m_LastResult[i]->r_group;
		index1 = m_LastResult[i]->r_orgIndex;
		pStroke_temp = &m_StrokeList[group1].m_Buffer[index1];

		width2 = pStroke_temp->bound.right - pStroke_temp->bound.left;
		height2 = pStroke_temp->bound.bottom - pStroke_temp->bound.top;

		if( pStroke1->bound.bottom < pStroke_temp->bound.top ||
			pStroke_temp->bound.bottom < pStroke1->bound.top ) continue;

		if( width1+width2 > abs(width1-width2)*5 && height1+height2 > abs(height1-height2)*5 )
		{
			if( pStroke2->bound.right < pStroke_temp->bound.right ) return TRUE;
		}
	}
	
	return FALSE;
}





//////////////////////////////////////////////////////////////////////////
// �������� ȹ ��

//
// 1 - (2 ~ 6 )
// stroke1 == STROKE_TYPE_HORIZONTAL
// stroke2 == VERTICAL || HOOK || LEFTFALLING || RIGHTFALLING || TURNING
int CDeformDoc::Compare_Horizontal_Vertical(int stroke1, int stroke2, int over)
{
//	if( over > 2)  return -1;

	int group1, group2, index1, index2;
	
	group1 = m_LastResult[stroke1]->r_group;
	index1 = m_LastResult[stroke1]->r_orgIndex;

	group2 = m_LastResult[stroke2]->r_group;
	index2 = m_LastResult[stroke2]->r_orgIndex;

	// stroke ������
	MyStroke *pStroke1, *pStroke2;
	pStroke1 = &m_StrokeList[group1].m_Buffer[index1];
	pStroke2 = &m_StrokeList[group2].m_Buffer[index2];
	
	//////////////////////////////////////////////////////////////////////////
	
	// + 
	if( pStroke2->bound.top < pStroke1->bound.top && pStroke2->bound.bottom > pStroke1->bound.bottom ) 
	{
		if( pStroke2->bound.left > pStroke1->bound.left && pStroke2->bound.right < pStroke1->bound.right )
		{
			if( pStroke2->type == STROKE_TYPE_VERTICAL_T2 || 
				pStroke2->type == STROKE_TYPE_VERTICAL_CROSS2 || 
				pStroke2->type == STROKE_TYPE_HOOK_CROSS2 ) return 1;
			else return -1;
		}
	}


	// T
	if( pStroke1->bound.top <= pStroke2->bound.top && pStroke1->bound.bottom < pStroke2->bound.bottom )
		return -1;

	if( pStroke2->type == STROKE_TYPE_RIGHTFALLING )
	{
		if( pStroke1->bound.left < pStroke2->bound.left && pStroke1->bound.right < pStroke2->bound.right )
			return -1;
	}
	// ��

	// ��

	// ��
	return 1;

}

//
// 2 - 4 ,        3 - 4
// stroke1 == STROKE_TYPE_VERTICAL || STROKE_TYPE_HOOK
// stroke2 == STROKE_TYPE_LEFTFALLING
int CDeformDoc::Compare_Vertical_Leftfalling(int stroke1, int stroke2, int over)
{
	int group1, group2, index1, index2;
	
	group1 = m_LastResult[stroke1]->r_group;
	index1 = m_LastResult[stroke1]->r_orgIndex;

	group2 = m_LastResult[stroke2]->r_group;
	index2 = m_LastResult[stroke2]->r_orgIndex;

	// stroke ������
	MyStroke *pStroke1, *pStroke2;
	pStroke1 = &m_StrokeList[group1].m_Buffer[index1];
	pStroke2 = &m_StrokeList[group2].m_Buffer[index2];
	
	//////////////////////////////////////////////////////////////////////////
	if( over > 2 ) return -1;

	// ��
	if( pStroke1->bound.left <= pStroke2->bound.left ) return 1;

	// �� 
	if( pStroke1->bound.top > pStroke2->bound.top && pStroke1->bound.bottom > pStroke2->bound.bottom )
		return 1;



	return -1;

}

//
// 2 - 5 ,        3 - 5
// stroke1 == STROKE_TYPE_VERTICAL || STROKE_TYPE_HOOK
// stroke2 == STROKE_TYPE_RIGHTFALLING
int CDeformDoc::Compare_Vertical_Rightfalling(int stroke1, int stroke2, int over)
{
	if( over > 2) return -1;
	int group1, group2, index1, index2;
	
	group1 = m_LastResult[stroke1]->r_group;
	index1 = m_LastResult[stroke1]->r_orgIndex;

	group2 = m_LastResult[stroke2]->r_group;
	index2 = m_LastResult[stroke2]->r_orgIndex;

	// stroke ������
	MyStroke *pStroke1, *pStroke2;
	pStroke1 = &m_StrokeList[group1].m_Buffer[index1];
	pStroke2 = &m_StrokeList[group2].m_Buffer[index2];
	
	//////////////////////////////////////////////////////////////////////////

	// (
	// �� 
	if( pStroke1->bound.top < pStroke2->bound.top ) return 1;


	return -1;
}


//
// 2 - 6 ,        3 - 6
// stroke1 == STROKE_TYPE_VERTICAL || STROKE_TYPE_HOOK || STROKE_TYPE_LEFTFALLING
// stroke2 == STROKE_TYPE_TURNING
int CDeformDoc::Compare_Vertical_Turning(int stroke1, int stroke2, int over)
{
	int group1, group2, index1, index2;
	
	group1 = m_LastResult[stroke1]->r_group;
	index1 = m_LastResult[stroke1]->r_orgIndex;

	group2 = m_LastResult[stroke2]->r_group;
	index2 = m_LastResult[stroke2]->r_orgIndex;

	// stroke ������
	MyStroke *pStroke1, *pStroke2;
	pStroke1 = &m_StrokeList[group1].m_Buffer[index1];
	pStroke2 = &m_StrokeList[group2].m_Buffer[index2];
	
	//////////////////////////////////////////////////////////////////////////

	if( over == 2 && pStroke2->type == STROKE_TYPE_TURNING1 )
	{// ��
		if( pStroke1->bound.left <= pStroke2->bound.left && 
			pStroke1->bound.right <= pStroke2->bound.right ) return -1;
	}
	if( over == 2 && pStroke2->type == STROKE_TYPE_TURNING_F )
	{// ��
		if( pStroke1->bound.left <= pStroke2->bound.left && 
			pStroke1->bound.right <= pStroke2->bound.right ) return 1;
	}
	
	// Ϣ
	if( over <= 2 )
	{
		if( pStroke1->bound.top < pStroke2->bound.top && pStroke1->bound.bottom <= pStroke2->bound.top ) return -1;
		if( pStroke1->bound.left <= pStroke2->bound.left && pStroke1->bound.right <= pStroke2->bound.right ) return -1;
		else return 1;
	}

	// ��
/*	if( pStroke1->bound.bottom > pStroke2->bound.bottom + THRESHOLD_DISTANCE_MAX )
	{
		return 1;	
	}*/

	if( pStroke1->bound.bottom < pStroke2->bound.bottom - THRESHOLD_DISTANCE_MAX*2 ) return 1;

	// /      1
	// if( pStroke1->vector1.x < 0 ) return -1;

	// \      -1

	return 1;
}


//
// 4 - 5 
// stroke1 == STROKE_TYPE_LEFTFALLING
// stroke2 == STROKE_TYPE_RIGHRFALLING
int CDeformDoc::Compare_Leftfalling_Rightfalling(int stroke1, int stroke2, int over)
{
	return -1;
}


void CDeformDoc::RelexationLabeling()
{
	int i, j;
	// ���� �غ� 
	int in; // ���� ��Ʈ��ũ ���� �ε��� (���� �׷�ƴ�)
	for( i = 0; i < m_GroupList.GetSize(); i++ ) // �׷캰 
	{
		in = 0;
		for( j = 0; j < m_StrokeList[i].m_Size; j++ ) // strokeList �� 
		{
			if( m_StrokeList[i].m_Buffer[j].use == FALSE ) continue;
			m_StrokeList[i].m_Buffer[j].r_key = m_StrokeList[i].m_GroupNum;
			m_StrokeList[i].m_Buffer[j].r_group = i;
			m_StrokeList[i].m_Buffer[j].r_orgIndex = j;
			m_StrokeList[i].m_Buffer[j].r_index = in;
			
			// �̰� �������� ������ ���� ���� �ʱ�ȭ
			m_StrokeList[i].m_Buffer[j].r_parentGroup = -1;
			m_StrokeList[i].m_Buffer[j].r_parentIndex = -1;
			//

			m_LastResult[m_LastNum] = &(m_StrokeList[i].m_Buffer[j]);
			m_LastNum++;
			in++;

		}// for( j)
	}// for(i)

	// ���� ?
	MyStroke* temp_stroke; // swap ��
	//////////////////////////////////////////////////////////////////////////
	// ���� 
	for( i = 0; i < m_LastNum; i++ )
	{
		for( j = i; j > 0; j-- )
		{ // ������ ũ�� 1, ������ 0, �������� ũ�� -1 (ũ�� == �켱��������)

			if( m_LastResult[j-1]->r_key > m_LastResult[j]->r_key)
			{// swap
				temp_stroke = m_LastResult[j];
				m_LastResult[j] = m_LastResult[j-1];
				m_LastResult[j-1] = temp_stroke;
			}else break;
		}
	}
	
	BOOL repeat, repeat_sub;
	int rNum; // ������ ��ġ�� �ٸ� ȹ�� (����)
//	int rList[16];	// ������ ��ġ�� �ٸ� ȹ�� 
	//////////////////////////////////////////////////////////////////////////
	// Relaxation Labeling
	int start, end, gnum, over;
	int t,m,b;
	repeat = TRUE;
	while(repeat)
	{//////////////////////////////////////////////////////////////////////////
	repeat = FALSE;
	for( i = 0; i < m_LastNum; i++ ) // ��� stroke �� ���ؼ�
	{
		if( i == 0 || gnum != m_LastResult[i]->r_key)
		{
			gnum = m_LastResult[i]->r_key;
			start = i;
			end = i;
			while(TRUE)
			{
				if( end < m_LastNum-1 && gnum == m_LastResult[end+1]->r_key )
				{
					end++;
				}else break;
			}
		}

		repeat = FALSE;
		repeat_sub = FALSE;
		rNum = 0;
		t = m = b = 0;
		for( j = start; j <= end; j++ )
		{
			if( i == j /*|| m_LastResult[i]->type % 0x10 != 0*/) continue;
			over = GetOver(i,j);

			// 1. horizontal
			if( m_LastResult[i]->type /0x10*0x10 == STROKE_TYPE_HORIZONTAL )
			{
				
				if( over == 0 || over > 2 )	continue;	// ���� ����
								
				// �̳��� �� ���� �� ��� 
				if( m_LastResult[i]->bound.left < m_LastResult[j]->bound.left &&
					m_LastResult[i]->bound.right > m_LastResult[j]->bound.right ) continue;	// ���� ����

				// vertical
				if( m_LastResult[j]->type >= STROKE_TYPE_VERTICAL && m_LastResult[j]->type < STROKE_TYPE_HOOK )
				{
					// close or L 
					if( m_LastResult[i]->bound.bottom <= m_LastResult[j]->bound.bottom - THRESHOLD_DISTANCE_MAX 
						|| ( m_LastResult[j]->bound.bottom-m_LastResult[j]->bound.top > 300 && 
						m_LastResult[i]->bound.bottom <= m_LastResult[j]->bound.bottom -50 ))
					{
						if( m_LastResult[i]->type % 0x10 == 0 ) 
						{
							m_LastResult[i]->type = STROKE_TYPE_HORIZONTAL_RIGHT_BOT2;
							m_LastResult[i]->r_parentGroup = m_LastResult[j]->r_group;
							m_LastResult[i]->r_parentIndex = m_LastResult[j]->r_orgIndex;
						}
					}
				}
				else if( m_LastResult[j]->type >= STROKE_TYPE_HOOK )
				{
					if( m_LastResult[i]->bound.top >= m_LastResult[j]->bound.top && 
						m_LastResult[i]->bound.bottom <= m_LastResult[j]->bound.bottom)
					{
						m_LastResult[i]->type = STROKE_TYPE_HORIZONTAL1;
						m_LastResult[i]->r_parentGroup = m_LastResult[j]->r_group;
						m_LastResult[i]->r_parentIndex = m_LastResult[j]->r_orgIndex;
						repeat_sub = FALSE;
						break;
					}
					
					
				}

			}

			// 2. vertical
			else if( m_LastResult[i]->type /0x10*0x10 == STROKE_TYPE_VERTICAL )
			{
				if( over == 4 && m_LastResult[j]->type/0x10*0x10 == STROKE_TYPE_HORIZONTAL) m++;
				
				if( over == 0 || over > 2 )	continue;	// ���� ����

				// �
				if( m_LastResult[j]->type >= STROKE_TYPE_TURNING )
				{
					if( m_LastResult[j]->bound.bottom <= m_LastResult[i]->bound.top + THRESHOLD_DISTANCE_MAX)
					{
						m_LastResult[i]->r_parentGroup = m_LastResult[j]->r_group;
						m_LastResult[i]->r_parentIndex = m_LastResult[j]->r_orgIndex;
						m_LastResult[i]->type = STROKE_TYPE_VERTICAL_CROSS2;
						break;
					}
					else if( m_LastResult[j]->bound.top < m_LastResult[i]->bound.top )
					{
						m_LastResult[i]->r_parentGroup = m_LastResult[j]->r_group;
						m_LastResult[i]->r_parentIndex = m_LastResult[j]->r_orgIndex;
						m_LastResult[i]->type = STROKE_TYPE_VERTICAL1;
						break;
					}

				}
				
				// ��
				if( m_LastResult[j]->type/0x10*0x10 == STROKE_TYPE_HORIZONTAL || 
					m_LastResult[j]->type/0x10*0x10 == STROKE_TYPE_LEFTFALLING )
				{
					if( m_LastResult[i]->bound.top > m_LastResult[j]->bound.top) t++;
					else if( m_LastResult[i]->bound.bottom < m_LastResult[j]->bound.bottom) b++;
				}
				
			}

			// 3. hook
			else if( m_LastResult[i]->type /0x10*0x10 == STROKE_TYPE_HOOK )
			{
				if( over == 0 || over > 2 )	continue;	// ���� ����

				// �
				if( m_LastResult[j]->type >= STROKE_TYPE_TURNING )
				{
					if( m_LastResult[j]->bound.bottom <= m_LastResult[i]->bound.top + THRESHOLD_DISTANCE_MAX)
					{
						m_LastResult[i]->type = STROKE_TYPE_VERTICAL_CROSS2;
						break;
					}
				}
			}

			// 4. lefe-falling , 5. right-falling
			else if( m_LastResult[i]->type /0x10*0x10 == STROKE_TYPE_LEFTFALLING  || 
				m_LastResult[i]->type /0x10*0x10 == STROKE_TYPE_RIGHTFALLING )
			{
				if( over > 0 )
				{
					if( m_LastResult[j]->type /0x10*0x10 == STROKE_TYPE_HORIZONTAL || 
						m_LastResult[j]->type /0x10*0x10 == STROKE_TYPE_TURNING )
					{
						if( m_LastResult[i]->bound.top > m_LastResult[j]->bound.top )
						{
							m_LastResult[i]->r_parentGroup = m_LastResult[j]->r_group;
							m_LastResult[i]->r_parentIndex = m_LastResult[j]->r_orgIndex;
							m_LastResult[i]->type++;
							break;
						}
					}
				}
				// ڷ
				if( over == 0 && m_LastResult[i]->type %0x10 == 0 &&
					m_LastResult[j]->type /0x10*0x10 == STROKE_TYPE_HORIZONTAL  )
				{
					if( m_LastResult[i]->bound.bottom < m_LastResult[j]->bound.top && 
						m_LastResult[j]->bound.top- m_LastResult[i]->bound.bottom < THRESHOLD_DISTANCE_MAX*2 &&
						m_LastResult[i]->bound.left > m_LastResult[j]->bound.left &&
						m_LastResult[i]->bound.right < m_LastResult[j]->bound.right )
					{
						m_LastResult[i]->type = STROKE_TYPE_LEFTFALLING_DOUBLE2;
						m_LastResult[i]->r_parentGroup = m_LastResult[j]->r_group;
						m_LastResult[i]->r_parentIndex = m_LastResult[j]->r_orgIndex;
					}
				}

				// �ٱ� 
				if( over > 0 && over < 3)
				{
				}
			}

			// 6. turning
			else if( m_LastResult[i]->type /0x10*0x10 == STROKE_TYPE_TURNING )
			{
				if( over == 2 && m_LastResult[i]->bound.top > m_LastResult[j]->bound.top )
					t++;
				
				if( over == 0 || over > 2) continue; 
				if( m_LastResult[j]->type /0x10*0x10 == STROKE_TYPE_VERTICAL  ||  // �� 
					m_LastResult[j]->type /0x10*0x10 == STROKE_TYPE_HOOK  )
				{
					if( m_LastResult[i]->bound.left >= m_LastResult[j]->bound.left )
					{
						m_LastResult[i]->type = STROKE_TYPE_TURNING1;
						m_LastResult[i]->r_parentGroup = m_LastResult[j]->r_group;
						m_LastResult[i]->r_parentIndex = m_LastResult[j]->r_orgIndex;
					}
				}
			}
		}// for(j)
			
		if( m_LastResult[i]->type == STROKE_TYPE_VERTICAL && t == 1 && m == 1 && b == 1 )
		{
			m_LastResult[i]->type = STROKE_TYPE_VERTICAL_T2;
		}
		else if( m_LastResult[i]->type == STROKE_TYPE_TURNING )
		{
			m_LastResult[i]->type = STROKE_TYPE_TURNING2;
		}
		else if( m_LastResult[i]->type == STROKE_TYPE_TURNING1 && t == 2 )
		{
			m_LastResult[i]->type = STROKE_TYPE_TURNING_F;
			m_LastResult[i]->r_parentGroup = -1;
			m_LastResult[i]->r_parentIndex = -1; 
		}

		if( repeat_sub == FALSE && m_LastResult[i]->type%0x10 == 0 ) m_LastResult[i]->type++;
		if( repeat_sub == TRUE ) repeat = TRUE;

	} // for(i)
	//////////////////////////////////////////////////////////////////////////
	}// while(repeat)
}

int CDeformDoc::GetOver( int stroke1, int stroke2)
{
	int i, j, size1, size2, group1, group2, index1, index2;
	int over = 0; // ��ġ�� ����Ʈ ���� 

	group1 = m_LastResult[stroke1]->r_group;
	index1 = m_LastResult[stroke1]->r_index;
	size1 = m_ResultStroke[group1][index1].m_Size;

	group2 = m_LastResult[stroke2]->r_group;
	index2 = m_LastResult[stroke2]->r_index;
	size2 = m_ResultStroke[group2][index2].m_Size;
	
	for( i = 0; i < size1; i++)
	{
		if( m_ResultStroke[group1][index1].m_Buffer[i].tag == 0 ) continue; // fix point �� �ش�
		for( j = 0; j < size2; j++) // ��� ��츦 ����
		{
			if( m_ResultStroke[group2][index2].m_Buffer[j].tag == 0 ) continue;
			if( m_ResultStroke[group1][index1].m_Buffer[i].x == m_ResultStroke[group2][index2].m_Buffer[j].x &&
				m_ResultStroke[group1][index1].m_Buffer[i].y == m_ResultStroke[group2][index2].m_Buffer[j].y)
			{
				over++;
				break;
			}
		}
	}

	return over;
}

//
// parent ��Ʈ��ũ�� ������ 
// ������ -1 
int CDeformDoc::GetParent( int stroke )
{
	int i, group1, index1, r_group, r_index;

	group1 = m_LastResult[stroke]->r_group;
	index1 = m_LastResult[stroke]->r_orgIndex;

	r_group = m_StrokeList[group1].m_Buffer[index1].r_parentGroup;
	r_index = m_StrokeList[group1].m_Buffer[index1].r_parentIndex;
	if( r_group == -1 || r_index == -1 ) return -1;

	for( i = 0; i < m_LastNum; i++)
	{
		if( m_LastResult[i]->r_group == r_group && m_LastResult[i]->r_orgIndex == r_index ) return i;
	}
	return -1;
}