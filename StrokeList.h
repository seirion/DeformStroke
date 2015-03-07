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
// 변경된 후의 define ... 
/*#define STROKE_TYPE_NONE			0x0000
#define STROKE_TYPE_DOT				0x0001


#define STROKE_TYPE_HORIZONTAL			0x0010		// 아무 것도 아님
#define STROKE_TYPE_HORIZONTAL1			0x0011		// vertical + hook
//#define STROKE_TYPE_HORIZONTAL_CROSS	0x0012		// +
//#define STROKE_TYPE_HORIZONTAL_CROSS_JA 0x0013		// 子
//#define STROKE_TYPE_HORIZONTAL_RIGHT_TOP	0x0014
//#define STROKE_TYPE_HORIZONTAL_RIGHT_MID	0x0015	// ㅏ 
//#define STROKE_TYPE_HORIZONTAL_RIGHT_MID2	0x0016	// 복수 ㅑ 
#define STROKE_TYPE_HORIZONTAL_RIGHT_BOT	0x0017
#define STROKE_TYPE_HORIZONTAL_RIGHT_BOT2	0x0018 // L
//#define STROKE_TYPE_HORIZONTAL_LEFT			0x0019	// ㅓ


#define STROKE_TYPE_VERTICAL		0x0020		// 아무 것도 아님
#define STROKE_TYPE_VERTICAL1		0x0021		
#define STROKE_TYPE_VERTICAL_T2		0x0029	// 王
//#define STROKE_TYPE_VERTICAL_CROSS		0x0022	// +
#define STROKE_TYPE_VERTICAL_CROSS2		0x0023	// 子
//#define STROKE_TYPE_VERTICAL_RIGHT	0x0024	// ㅏ (단수)
//#define STROKE_TYPE_VERTICAL_RIGHT2	0x0025	// ㅑ (복수)
//#define STROKE_TYPE_VERTICAL_L		0x0026		// ㅁ
#define STROKE_TYPE_VERTICAL_L2		0x0027		// L
#define STROKE_TYPE_VERTICAL_F		0x0028	// 民

#define STROKE_TYPE_HOOK			0x0030
#define STROKE_TYPE_HOOK1			0x0031
//#define STROKE_TYPE_HOOK_CROSS		0x0032		// +
#define STROKE_TYPE_HOOK_CROSS2		0x0033		// 子
//#define STROKE_TYPE_HOOK_RIGHT		0x0043	// ㅏ (단수)	
//#define STROKE_TYPE_HOOK_RIGHT2		0x0035	// ㅑ (복수)
#define STROKE_TYPE_HOOK_F			0x0038		// 民

#define STROKE_TYPE_LEFTFALLING		0x0110
#define STROKE_TYPE_LEFTFALLING1	0x0111	// ㅠ
//#define STROKE_TYPE_LEFTFALLING_LEFT	0x0112	// ㅓ    (vertical + hook)
//#define STROKE_TYPE_LEFTFALLING_RIGHT	0x0113	// ㅏ
#define STROKE_TYPE_LEFTFALLING_DOUBLE	0x0114	// 木
#define STROKE_TYPE_LEFTFALLING_DOUBLE2	0x0115	// 米
//#define STROKE_TYPE_LEFTFALLING_CROSS	0x0116	// +


#define STROKE_TYPE_RIGHTFALLING	0x0120
#define STROKE_TYPE_RIGHTFALLING1	0x0121
//#define STROKE_TYPE_RIGHTFALLING_LEFT	0x0122	// ㅓ
//#define STROKE_TYPE_RIGHTFALLING_RIGHT	0x0123	// ㅏ
#define STROKE_TYPE_RIGHTFALLING_DOUBLE		0x0124 // 木
#define STROKE_TYPE_RIGHTFALLING_DOUBLE2	0x0125 // 米


#define STROKE_TYPE_RISING			0x0130 // 사용 안 함


#define STROKE_TYPE_TURNING			0x0200
#define STROKE_TYPE_TURNING1		0x0201	// ㅁ 
#define STROKE_TYPE_TURNING2		0x0202	// ㄱ
*/

//////////////////////////////////////////////////////////////////////////
// 변경된 후의 define ... 
#define STROKE_TYPE_NONE			0x0000
#define STROKE_TYPE_DOT				0x0001


#define STROKE_TYPE_HORIZONTAL			0x0010		// 아무 것도 아님
#define STROKE_TYPE_HORIZONTAL1			0x0011		// vertical + hook
#define STROKE_TYPE_HORIZONTAL_RIGHT_BOT	0x0017	// CLOSE
#define STROKE_TYPE_HORIZONTAL_RIGHT_BOT2	0x0018 // L


#define STROKE_TYPE_VERTICAL		0x0020		// 아무 것도 아님
#define STROKE_TYPE_VERTICAL1		0x0021		
#define STROKE_TYPE_VERTICAL_T2		0x0029	// 王
#define STROKE_TYPE_VERTICAL_CROSS2		0x0023	// 子
#define STROKE_TYPE_VERTICAL_L2		0x0027		// L
#define STROKE_TYPE_VERTICAL_F		0x0028	// 民

#define STROKE_TYPE_HOOK			0x0030
#define STROKE_TYPE_HOOK1			0x0031
#define STROKE_TYPE_HOOK_CROSS2		0x0033		// 子
#define STROKE_TYPE_HOOK_F			0x0038		// 民

#define STROKE_TYPE_LEFTFALLING		0x0110
#define STROKE_TYPE_LEFTFALLING1	0x0111	// ㅠ
#define STROKE_TYPE_LEFTFALLING_DOUBLE	0x0114	// 木
#define STROKE_TYPE_LEFTFALLING_DOUBLE2	0x0115	// 米


#define STROKE_TYPE_RIGHTFALLING	0x0120
#define STROKE_TYPE_RIGHTFALLING1	0x0121
#define STROKE_TYPE_RIGHTFALLING_DOUBLE		0x0124 // 木
#define STROKE_TYPE_RIGHTFALLING_DOUBLE2	0x0125 // 米


#define STROKE_TYPE_RISING			0x0130 // 사용 안 함


#define STROKE_TYPE_TURNING			0x0200
#define STROKE_TYPE_TURNING1		0x0201	// ㅁ 
#define STROKE_TYPE_TURNING2		0x0202	// ㄱ
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
	CPoint vector1;		// 획 합치기에 쓸 데이터 (첫획과 마지막 획을 가지고 더함 )
	int open1;
	int open2;
	int direct;	// 0 - 가로 // 1 - 세로

	BOOL check;				// 획 머징 
	BOOL use;				// 획 머지 후 사용 여부 

	//////////////////////////////////////////////////////////////////////////
	// 최종 결과를 위한 (데이터임 계산 자체에는 쓰이지 않음)
	int r_key;				// 소트를 위한 키 값
	int r_group;				// 결과 출력을 위함 
	int r_index;				// 결과 출력을 위함 
	int r_orgIndex;				// StrokeList 내의 본래 인덱스

	int r_parentGroup;			// 획 (접한 획)
	int r_parentIndex;			// 획 (접한 획 index)
};
 class CStrokeList  
{
public:
	void Sort(){ for( int i = 0; i < m_Size; i++) Sort( i); };
	void SetContinuity(){for( int i = 0; i < m_Size; i++) SetContinuity(i);};
	void ConnectStroke();
	void MakeTurning(); // 꺾어진 획 만들기 
	void Ordering(); // 순서 정하기
	/*ordering*/
	BOOL FindDotStroke(); // dot 획 찾기
	int GetLength( int line); // 라인 하나의 길이
	BOOL IsHookStroke(int stroke); // HOOK 획인 지 여부 
	BOOL IsVerticalStroke(int stroke); // VERTICAL 인지 여부 
	BOOL IsHorizontalStroke(int stroke); // HORIZONTAL 인지 여부
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
	
	int m_TextMode; // 글자 그리기 0 번으로 조절
	int m_GroupNum; // 소팅 할 때, 그룹 소팅 번호 
private:
	int BelongTo(int index);
	void MergeStroke(int stroke1, int stroke2 );
	void MergeStroke2(int stroke1, int stroke2 );
	int MergeAngle(int stroke1, int stroke2);
	//////////////////////////////////////////////////////////////////////////
	// sorting 관련
	void Sort(int index);	
	void SortX(int index);
	void SortY(int index);
	void SortXY(int index);
	void SortX_Y(int index);
	void Reverse(int index, int flag);
	
	void MakeOrderX(int index);
	void MakeOrderY(int index);
	void SetContinuity(int index);

	CPoint GetVector(int index); // 획 합치기 관련
	int DotProductAngle(int index1, int index2); // 획 합치기의 각도 구하기
	int DotProductAngle2(int index1, int index2);
	void MakeRightWay(int index);
	void SetSortAngle( int index);
	////////////////////////////////////////////////////////////////////////
	// 그리기 관련
	CPen *m_pen, *m_oldPen;
	CBrush *m_brush, *m_oldBrush;

	void ReleaseBrush(CDC *pDC);
	void ReleasePen(CDC *pDC);
};

#endif // !defined(AFX_STROKELIST_H__1188CE35_9E16_4FF6_91C0_7587598DDEE4__INCLUDED_)
