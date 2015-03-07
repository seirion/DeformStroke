// GroupList.cpp: implementation of the CGroupList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Deform.h"
#include "GroupList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGroupList::CGroupList()
{
	m_Size = 0;
	m_Buffer = NULL;
	m_BufferSize = 0;
}

CGroupList::~CGroupList()
{
	if( m_Buffer ) delete [] m_Buffer;
}

//////////////////////////////////////////////////////////////////////////
// operations
void CGroupList::Init()
{
	if( m_Buffer ) delete [] m_Buffer;
	m_Buffer = NULL;
	m_Size = 0;
}

void CGroupList::Init(int size)
{
	if( m_Buffer ) delete [] m_Buffer;
	m_Buffer = new MyGroup[size];
	m_BufferSize = size;
	m_Size = 0;
}

int CGroupList::AddRoot(int index, CPoint &min, CPoint &max)
{
	m_Buffer[m_Size].parent = index;
	m_Buffer[m_Size].min = min;
	m_Buffer[m_Size].max = max;
	m_Buffer[m_Size].childNum = 0;
	return ++m_Size;
}

int CGroupList::AddSub(int index, CPoint &min, CPoint &max)
{
	int i;
	for( i = m_Size-1; i >= 0; i--)
	{
		if(m_Buffer[i].min.x < min.x && m_Buffer[i].min.y < min.y && 
			m_Buffer[i].max.x > max.x && m_Buffer[i].max.y > max.y )
		{
			m_Buffer[i].child[m_Buffer[i].childNum] = index;
			m_Buffer[i].childNum++;
			return i;
		}
	}

	// if fail 
	return AddRoot( index, min, max);
}

