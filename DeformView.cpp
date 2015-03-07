 // DeformView.cpp : implementation of the CDeformView class
//

#include "stdafx.h"
#include "Deform.h"

#include "DeformDoc.h"
#include "DeformView.h"

#include "MyEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeformView

IMPLEMENT_DYNCREATE(CDeformView, CFormView)

BEGIN_MESSAGE_MAP(CDeformView, CFormView)
	//{{AFX_MSG_MAP(CDeformView)
	ON_WM_KEYDOWN()
	ON_CBN_SELCHANGE(IDC_COMBO_GROUP, OnSelchangeComboGroup)
	ON_CBN_SELCHANGE(IDC_COMBO_STROKE, OnSelchangeComboStroke)
	ON_BN_CLICKED(IDC_BUTTON_ORDER, OnButtonOrder)
	ON_BN_CLICKED(IDC_BUTTON_POINT, OnButtonPoint)
	ON_BN_CLICKED(IDC_BUTTON_DRAW, OnButtonDraw)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_CBN_SELCHANGE(IDC_COMBO_LAST, OnSelchangeComboLast)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeformView construction/destruction

CDeformView::CDeformView()
	: CFormView(CDeformView::IDD)
{
	//{{AFX_DATA_INIT(CDeformView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	m_ShowEdit = TRUE;

	m_Group = -1;
	m_Stroke = -1;
}

CDeformView::~CDeformView()
{
}

void CDeformView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeformView)
	DDX_Control(pDX, IDC_COMBO_LAST, m_CtrlLast);
	DDX_Control(pDX, IDC_BITMAP, m_CtrlBitmap);
	DDX_Control(pDX, IDC_EDIT_INPUT, m_EditInput);
	DDX_Control(pDX, IDC_COMBO_GROUP, m_CtrlGroup);
	DDX_Control(pDX, IDC_COMBO_STROKE, m_CtrlStroke);
	//}}AFX_DATA_MAP
}

BOOL CDeformView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

void CDeformView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
	SetColor();
}

/////////////////////////////////////////////////////////////////////////////
// CDeformView printing

BOOL CDeformView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDeformView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDeformView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CDeformView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}

/////////////////////////////////////////////////////////////////////////////
// CDeformView diagnostics

#ifdef _DEBUG
void CDeformView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDeformView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDeformDoc* CDeformView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDeformDoc)));
	return (CDeformDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDeformView message handlers

void CDeformView::OnDraw(CDC* pDC) 
{
	CDeformDoc *pDoc = (CDeformDoc *)GetDocument();
	
	CDC *dc = m_CtrlBitmap.GetDC();
	CPen pen, *oldPen;
	pen.CreatePen(PS_SOLID, 1, RGB(100,100,100) );
	oldPen = (CPen *)dc->SelectObject(&pen);
	dc->Rectangle(0,0,BITMAP_WIDTH, BITMAP_HEIGHT);

	if( !pDoc->m_FontLoad ) return;
	DrawGuideLine(dc);

	int i, j, size, size1;
	switch( pDoc->m_DrawMode)
	{
	case DRAW_MODE_BITMAP :
		pDoc->m_FontImage.DrawBitmap(0,0, dc,0,0, SRCCOPY);
		for( i = 0; i < pDoc->m_ContourNum; i++ ) pDoc->m_ControlPoint[i].DrawControlPoints(dc);
		break;
	case DRAW_MODE_LINE :
		for( i = 0; i < pDoc->m_ContourNum; i++ ) pDoc->m_LineList[i].Draw(dc);
		for( i = 0; i < pDoc->m_ContourNum; i++ ) pDoc->m_ControlPoint[i].DrawControlPoints(dc);
		break;
	case DRAW_MODE_STROKE :
		size = pDoc->m_GroupList.GetSize();
		for( i = 0; i < size; i++)
		{
			size1 = pDoc->m_StrokeList[i].m_Size;
			for( j = 0; j < size1; j++ )
				pDoc->m_StrokeList[i].DrawStroke(dc, j, m_Color[j]);
		}
		break;
	case DRAW_MODE_STROKE_ORDER :
		size = pDoc->m_GroupList.GetSize();
		for( i = 0; i < size; i++)
		{
			size1 = pDoc->m_StrokeList[i].m_Size;
			for( j = 0; j < size1; j++ )
				pDoc->m_StrokeList[i].DrawStroke(dc, j, m_Color[j]);
		}
		for( i = 0; i < size; i++)
		{
			size1 = pDoc->m_StrokeList[i].m_Size;
			for( j = 0; j < size1; j++ )
				pDoc->m_StrokeList[i].DrawStrokeOrder(dc, j, m_Color[j]);			
		}
		break;
	case DRAW_MODE_STROKE_EACH : // 5 획별로 보여줌 (컨트롤 포인트)
		size = pDoc->m_GroupList.GetSize();
		for( i = 0; i < size; i++)
		{
			size1 = pDoc->m_ResultNum[i];
			for( j = 0; j < size1; j++ )
			{
				pDoc->m_ResultStroke[i][j].DrawGlyph(dc);
			}
		}
		pDoc->m_ResultStroke[m_Group][m_Stroke].DrawControlPoints(dc);
		DrawStrokeType( dc);
		break;
	case DRAW_MODE_STROKE_RESULT : // 마지막 결과 
		size = pDoc->m_GroupList.GetSize();
		for( i = 0; i < size; i++)
		{
			size1 = pDoc->m_ResultNum[i];
			for( j = 0; j < size1; j++ )
			{
				pDoc->m_ResultStroke[i][j].DrawGlyph(dc);
			}
		}
		// pDoc->m_ResultStroke[m_Group][m_Stroke].DrawControlPoints(dc);
		size = m_CtrlLast.GetCurSel();
		i = pDoc->m_LastResult[size]->r_group;
		j = pDoc->m_LastResult[size]->r_index;
		pDoc->m_ResultStroke[i][j].DrawControlPoints(dc);
		break;
	default : break;
	}

	dc->SelectObject(oldPen);
	pen.DeleteObject();

}

void CDeformView::DrawGuideLine(CDC *pDC)
{
	int i;
	CPen pen, *pOldPen;
	pen.CreatePen(PS_SOLID,1,RGB(230,230,230));
	pOldPen = (CPen*)pDC->SelectObject(&pen);
	// 1. 가로 
	i = 0;
	while( i <= 500 )
	{
		pDC->MoveTo(i,0);
		pDC->LineTo(i,500);
		i += 50;
	}
	// 2. 세로 
	i = 0;
	while( i <= 500 )
	{
		pDC->MoveTo(0,i);
		pDC->LineTo(500,i);
		i += 50;
	}

	pDC->SelectObject(pOldPen);
	pen.DeleteObject();

}


void CDeformView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CDeformDoc *pDoc = (CDeformDoc *)GetDocument();
	int i;
	switch(nChar)
	{
	case VK_F2 : 
		pDoc->m_FontSelection++;
		break;
	case VK_SPACE :
		if( m_ShowEdit ) break;
		m_EditInput.EnableWindow(TRUE);
		m_EditInput.SetWindowText("");
		m_EditInput.ShowWindow(SW_SHOW);
		m_EditInput.SetFocus();
		m_ShowEdit = TRUE;
		Invalidate(NULL);
		break;
	case '0' :
		for( i = 0; i < pDoc->m_ContourNum; i++ )
		{
			pDoc->m_StrokeList[i].m_TextMode++;
			pDoc->m_StrokeList[i].m_TextMode = pDoc->m_StrokeList[i].m_TextMode%3;
		}
		Invalidate(NULL);
		break;
	case '1' :
		pDoc->m_DrawMode++;
		pDoc->m_DrawMode %= 7;
		Invalidate(NULL);
		break;
	case '2' :
		for( i = 0; i < pDoc->m_ContourNum; i++ ) pDoc->m_ControlPoint[i].SetDrawMode();
		Invalidate(NULL);
		break;
	default: break;
	}
	
	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CDeformView::SetColor()
{
	int i;
	for(i = 0; i < 10; i+=3 )
	{
		m_Color[i] = RGB(255-i*20,0,0);
		m_Color[i+1] = RGB(0,255-i*20,0);
		m_Color[i+2] = RGB(0,0,255-i*20);
	}

	for(i = 0; i < 10; i+=3 )
	{
		m_Color[i+12] = RGB(255-i*20,64,32);
		m_Color[i+13] = RGB(32,255-i*20,64);
		m_Color[i+14] = RGB(64,32,255-2*10);
	}

/*	for(i = 0; i < 5; i+=3 )
	{
		m_Color[i+27] = RGB(255-i*10,128,32);
		m_Color[i+28] = RGB(32,255-i*10,128);
		m_Color[i+29] = RGB(128,32,255-i*10);
	}*/
}

//////////////////////////////////////////////////////////////////////////
// 콤보 관련

// 콤보 초기화 
void CDeformView::InitCombo()
{
	CDeformDoc *pDoc = (CDeformDoc *)GetDocument();
	int i;
	CString str;

	// last
	while ( m_CtrlLast.GetCount() )
	{
		m_CtrlLast.DeleteString(0);
	}
	int size = pDoc->m_LastNum;
	for( i = 0; i < size; i++)
	{
		str.Format("%d", i);
		m_CtrlLast.AddString(str);
	}
	m_CtrlLast.SetCurSel(0);
	
	// 나머지 
	while ( m_CtrlGroup.GetCount() )
	{
		m_CtrlGroup.DeleteString(0);
	}

	size = pDoc->m_GroupList.GetSize();

	for( i = 0; i < size; i++)
	{
		str.Format("%d", i);
		m_CtrlGroup.AddString(str);
	}
	m_CtrlGroup.SetCurSel(0);
	m_Group = 0;
	MakeStrokeCombo( 0 );
	Invalidate();
}

void CDeformView::OnSelchangeComboGroup() 
{
	int group = m_CtrlGroup.GetCurSel();
//	if( m_Stroke == stroke ) return;

	m_Group = group;
	MakeStrokeCombo( m_Group );
	Invalidate();
}

void CDeformView::OnSelchangeComboStroke() 
{
	m_Stroke = m_CtrlStroke.GetCurSel();
	Invalidate();
}

void CDeformView::MakeStrokeCombo( int stroke )
{
	CDeformDoc *pDoc = (CDeformDoc *)GetDocument();
	while ( m_CtrlStroke.GetCount() )
	{
		m_CtrlStroke.DeleteString(0);
	}
	int size = pDoc->m_ResultNum[stroke];
	CString str;
	for( int i = 0; i < size; i++ )
	{
		str.Format("%d", i);
		m_CtrlStroke.AddString(str);
	}
	m_CtrlStroke.SetCurSel(0);
	m_Stroke = 0;
}

void CDeformView::OnButtonDraw() 
{
	CDeformDoc *pDoc = (CDeformDoc *)GetDocument();
	pDoc->m_DrawMode++;
	pDoc->m_DrawMode %= 7;
	Invalidate(NULL);
	
}

void CDeformView::OnButtonOrder() 
{
	CDeformDoc *pDoc = (CDeformDoc *)GetDocument();
	for( int i = 0; i < pDoc->m_ContourNum; i++ )
	{
		pDoc->m_StrokeList[i].m_TextMode++;
		pDoc->m_StrokeList[i].m_TextMode = pDoc->m_StrokeList[i].m_TextMode%3;
	}
	Invalidate(NULL);
}

void CDeformView::OnButtonPoint() 
{
	CDeformDoc *pDoc = (CDeformDoc *)GetDocument();
	for( int i = 0; i < pDoc->m_ContourNum; i++ ) pDoc->m_ControlPoint[i].SetDrawMode();
	Invalidate(NULL);
}

void CDeformView::OnButtonNew() 
{
	if( m_ShowEdit ) return;
	m_EditInput.EnableWindow(TRUE);
	m_EditInput.SetWindowText("");
	m_EditInput.ShowWindow(SW_SHOW);
	m_EditInput.SetFocus();
	m_ShowEdit = TRUE;
	Invalidate(NULL);
}

void CDeformView::OnSelchangeComboLast() 
{
	Invalidate();
	
}

//
// 획 종류 그리기 (테스트용)
void CDeformView::DrawStrokeType( CDC *pDC)
{
	CDeformDoc *pDoc = (CDeformDoc *)GetDocument();
	int i, j, size, line;
	CRect rect;
	CString str;

	for( i = 0; i < pDoc->m_GroupList.GetSize(); i++)
	{
		size = pDoc->m_StrokeList[i].m_Size;
		for( j = 0; j < size; j++)
		{
			if( pDoc->m_StrokeList[i].m_Buffer[j].use == FALSE ) continue;

			line = pDoc->m_StrokeList[i].m_Buffer[j].MainIndex[0][0];
			if( pDoc->m_StrokeList[i].m_Buffer[j].type == STROKE_TYPE_DOT ) str= "`";
			else if( pDoc->m_StrokeList[i].m_Buffer[j].type == STROKE_TYPE_HORIZONTAL ) str= "--";
			else if( pDoc->m_StrokeList[i].m_Buffer[j].type == STROKE_TYPE_VERTICAL ) str= "||";
			else if( pDoc->m_StrokeList[i].m_Buffer[j].type == STROKE_TYPE_LEFTFALLING ) str= "/";
			else if( pDoc->m_StrokeList[i].m_Buffer[j].type == STROKE_TYPE_RIGHTFALLING ) str= "R";
			else if( pDoc->m_StrokeList[i].m_Buffer[j].type == STROKE_TYPE_HOOK ) str= "?";
			else if( pDoc->m_StrokeList[i].m_Buffer[j].type == STROKE_TYPE_TURNING ) str= ">";
			
			rect.top = pDoc->m_LineList[i].m_Buffer[line].p1.y - 15;
			rect.bottom = pDoc->m_LineList[i].m_Buffer[line].p1.y;
			rect.left = pDoc->m_LineList[i].m_Buffer[line].p1.x - 15;
			rect.right = pDoc->m_LineList[i].m_Buffer[line].p1.x;
			pDC->DrawText(str, rect, 1);
		}
	}
}
/*
#define STROKE_TYPE_RISING			0x0102
#define STROKE_TYPE_TURNING			0x0200*/