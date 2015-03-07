// StrokeList.h: interface for the CStrokeList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STROKELIST_H__1188CE35_9E16_4FF6_91C0_7587598DDEE4__INCLUDED_)
#define AFX_STROKELIST_H__1188CE35_9E16_4FF6_91C0_7587598DDEE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SORT_ANGLE 45 

//////////////////////////////////////////////////////////////////////////
// type define
//#define STROKE_TYPE_NONE			0x0000
//#define STROKE_TYPE_DOT				0x0001
//
//#define STROKE_TYPE_HORIZONTAL		0x0010
//#define STROKE_TYPE_VERTICAL		0x0020
//#define STROKE_TYPE_LEFTFALLING		0x0110
//#define STROKE_TYPE_RIGHTFALLING	0x0120
//
//#define STROKE_TYPE_RISING			0x0130
//#define STROKE_TYPE_HOOK			0x0021
//#define STROKE_TYPE_TURNING			0x0200

//////////////////////////////////////////////////////////////////////////
// ����� ���� define ... 
/*#define STROKE_TYPE_NONE			0x0000
#define STROKE_TYPE_DOT				0x0001


#define STROKE_TYPE_HORIZONTAL			0x0010		// �ƹ� �͵� �ƴ�
#define STROKE_TYPE_HORIZONTAL1			0x0011		// vertical + hook
//#define STROKE_TYPE_HORIZONTAL_CROSS	0x0012		// +
//#define STROKE_TYPE_HORIZONTAL_CROSS_JA 0x0013		// �
//#define STROKE_TYPE_HORIZONTAL_RIGHT_TOP	0x0014
//#define STROKE_TYPE_HORIZONTAL_RIGHT_MID	0x0015	// �� 
//#define STROKE_TYPE_HORIZONTAL_RIGHT_MID2	0x0016	// ���� �� 
#define STROKE_TYPE_HORIZONTAL_RIGHT_BOT	0x0017
#define STROKE_TYPE_HORIZONTAL_RIGHT_BOT2	0x0018 // L
//#define STROKE_TYPE_HORIZONTAL_LEFT			0x0019	// ��


#define STROKE_TYPE_VERTICAL		0x0020		// �ƹ� �͵� �ƴ�
#define STROKE_TYPE_VERTICAL1		0x0021		
#define STROKE_TYPE_VERTICAL_T2		0x0029	// ��
//#define STROKE_TYPE_VERTICAL_CROSS		0x0022	// +
#define STROKE_TYPE_VERTICAL_CROSS2		0x0023	// �
//#define STROKE_TYPE_VERTICAL_RIGHT	0x0024	// �� (�ܼ�)
//#define STROKE_TYPE_VERTICAL_RIGHT2	0x0025	// �� (����)
//#define STROKE_TYPE_VERTICAL_L		0x0026		// ��
#define STROKE_TYPE_VERTICAL_L2		0x0027		// L
#define STROKE_TYPE_VERTICAL_F		0x0028	// ��

#define STROKE_TYPE_HOOK			0x0030
#define STROKE_TYPE_HOOK1			0x0031
//#define STROKE_TYPE_HOOK_CROSS		0x0032		// +
#define STROKE_TYPE_HOOK_CROSS2		0x0033		// �
//#define STROKE_TYPE_HOOK_RIGHT		0x0043	// �� (�ܼ�)	
//#define STROKE_TYPE_HOOK_RIGHT2		0x0035	// �� (����)
#define STROKE_TYPE_HOOK_F			0x0038		// ��

#define STROKE_TYPE_LEFTFALLING		0x0110
#define STROKE_TYPE_LEFTFALLING1	0x0111	// ��
//#define STROKE_TYPE_LEFTFALLING_LEFT	0x0112	// ��    (vertical + hook)
//#define STROKE_TYPE_LEFTFALLING_RIGHT	0x0113	// ��
#define STROKE_TYPE_LEFTFALLING_DOUBLE	0x0114	// ��
#define STROKE_TYPE_LEFTFALLING_DOUBLE2	0x0115	// ڷ
//#define STROKE_TYPE_LEFTFALLING_CROSS	0x0116	// +


#define STROKE_TYPE_RIGHTFALLING	0x0120
#define STROKE_TYPE_RIGHTFALLING1	0x0121
//#define STROKE_TYPE_RIGHTFALLING_LEFT	0x0122	// ��
//#define STROKE_TYPE_RIGHTFALLING_RIGHT	0x0123	// ��
#define STROKE_TYPE_RIGHTFALLING_DOUBLE		0x0124 // ��
#define STROKE_TYPE_RIGHTFALLING_DOUBLE2	0x0125 // ڷ


#define STROKE_TYPE_RISING			0x0130 // ��� �� ��


#define STROKE_TYPE_TURNING			0x0200
#define STROKE_TYPE_TURNING1		0x0201	// �� 
#define STROKE_TYPE_TURNING2		0x0202	// ��
*/

//////////////////////////////////////////////////////////////////////////
// ����� ���� define ... 
#define STROKE_TYPE_NONE			0x0000
#define STROKE_TYPE_DOT				0x0001


#define STROKE_TYPE_HORIZONTAL			0x0010		// �ƹ� �͵� �ƴ�
#define STROKE_TYPE_HORIZONTAL1			0x0011		// vertical + hook
#define STROKE_TYPE_HORIZONTAL_RIGHT_BOT	0x0017	// CLOSE
#define STROKE_TYPE_HORIZONTAL_RIGHT_BOT2	0x0018 // L


#define STROKE_TYPE_VERTICAL		0x0020		// �ƹ� �͵� �ƴ�
#define STROKE_TYPE_VERTICAL1		0x0021		
#define STROKE_TYPE_VERTICAL_T2		0x0029	// ��
#define STROKE_TYPE_VERTICAL_CROSS2		0x0023	// �
#define STROKE_TYPE_VERTICAL_L2		0x0027		// L
#define STROKE_TYPE_VERTICAL_F		0x0028	// ��

#define STROKE_TYPE_HOOK			0x0030
#define STROKE_TYPE_HOOK1			0x0031
#define STROKE_TYPE_HOOK_CROSS2		0x0033		// �
#define STROKE_TYPE_HOOK_F			0x0038		// ��

#define STROKE_TYPE_LEFTFALLING		0x0110
#define STROKE_TYPE_LEFTFALLING1	0x0111	// ��
#define STROKE_TYPE_LEFTFALLING_DOUBLE	0x0114	// ��
#define STROKE_TYPE_LEFTFALLING_DOUBLE2	0x0115	// ڷ


#define STROKE_TYPE_RIGHTFALLING	0x0120
#define STROKE_TYPE_RIGHTFALLING1	0x0121
#define STROKE_TYPE_RIGHTFALLING_DOUBLE		0x0124 // ��
#define STROKE_TYPE_RIGHTFALLING_DOUBLE2	0x0125 // ڷ


#define STROKE_TYPE_RISING			0x0130 // ��� �� ��


#define STROKE_TYPE_TURNING			0x0200
#define STROKE_TYPE_TURNING1		0x0201	// �� 
#define STROKE_TYPE_TURNING2		0x0202	// ��
#define STROKE_TYPE_TURNING_F		0x0203	// F

//////////////////////////////////////////////////////////////////////////

#include "LineList.h"

struct MyStroke
{
	int MainIndex[2][128];		// CLineList index 
								// 0 is main
								// 1 is opposite
//	int SideIndex[2][32];		// CLineList index
	
	int type;				// stroke type
	CRect bound;			// bounding box
	int num_main;
	int num_oppsite;
	int num_side1;
	int num_side2;

	int angle;				// mean angle (main line)
	CPoint vector1;		// ȹ ��ġ�⿡ �� ������ (ùȹ�� ������ ȹ�� ������ ���� )
	int open1;
	int open2;
	int direct;	// 0 - ���� // 1 - ����

	BOOL check;				// ȹ ��¡ 
	BOOL use;				// ȹ ���� �� ��� ���� 

	//////////////////////////////////////////////////////////////////////////
	// ���� ����� ���� (�������� ��� ��ü���� ������ ����)
	int r_key;				// ��Ʈ�� ���� Ű ��
	int r_group;				// ��� ����� ���� 
	int r_index;				// ��� ����� ���� 
	int r_orgIndex;				// StrokeList ���� ���� �ε���

	int r_parentGroup;			// ȹ (���� ȹ)
	int r_parentIndex;			// ȹ (���� ȹ index)
};
 class CStrokeList  
{
public:
	void Sort(){ for( int i = 0; i < m_Size; i++) Sort( i); };
	void SetContinuity(){for( int i = 0; i < m_Size; i++) SetContinuity(i);};
	void ConnectStroke();
	void MakeTurning(); // ������ ȹ ����� 
	void Ordering(); // ���� ���ϱ�
	/*ordering*/
	BOOL FindDotStroke(); // dot ȹ ã��
	int GetLength( int line); // ���� �ϳ��� ����
	BOOL IsHookStroke(int stroke); // HOOK ȹ�� �� ���� 
	BOOL IsVerticalStroke(int stroke); // VERTICAL ���� ���� 
	BOOL IsHorizontalStroke(int stroke); // HORIZONTAL ���� ����
	/*ordering*/

	void Draw(CDC *pDC);
	void DrawStroke(CDC *pDC, int index, COLORREF color );
	void DrawStrokeOrder(CDC *pDC, int index, COLORREF color );
	void Init();
	CStrokeList();
	virtual ~CStrokeList();
	void SetLineList(CLineList *pLineList){m_LineList = pLineList;};
	void MakeStroke();

	MyStroke m_Buffer[128];
	int m_Size;
	CLineList* m_LineList;

	void SetPen( CDC *pDC, int width, COLORREF color);
	void SetBrush(CDC *pDC, COLORREF color);
	
	int m_TextMode; // ���� �׸��� 0 ������ ����
	int m_GroupNum; // ���� �� ��, �׷� ���� ��ȣ 
private:
	int BelongTo(int index);
	void MergeStroke(int stroke1, int stroke2 );
	void MergeStroke2(int stroke1, int stroke2 );
	int MergeAngle(int stroke1, int stroke2);
	//////////////////////////////////////////////////////////////////////////
	// sorting ����
	void Sort(int index);	
	void SortX(int index);
	void SortY(int index);
	void SortXY(int index);
	void SortX_Y(int index);
	void Reverse(int index, int flag);
	
	void MakeOrderX(int index);
	void MakeOrderY(int index);
	void SetContinuity(int index);

	CPoint GetVector(int index); // ȹ ��ġ�� ����
	int DotProductAngle(int index1, int index2); // ȹ ��ġ���� ���� ���ϱ�
	int DotProductAngle2(int index1, int index2);
	void MakeRightWay(int index);
	void SetSortAngle( int index);
	////////////////////////////////////////////////////////////////////////
	// �׸��� ����
	CPen *m_pen, *m_oldPen;
	CBrush *m_brush, *m_oldBrush;

	void ReleaseBrush(CDC *pDC);
	void ReleasePen(CDC *pDC);
};

#endif // !defined(AFX_STROKELIST_H__1188CE35_9E16_4FF6_91C0_7587598DDEE4__INCLUDED_)
