// GroupList.h: interface for the CGroupList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GROUPLIST_H__EE8A0237_F775_4BE3_9EA2_9F58173C4FAD__INCLUDED_)
#define AFX_GROUPLIST_H__EE8A0237_F775_4BE3_9EA2_9F58173C4FAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct MyGroup
{
	int childNum; // 0이면 단독 그룹
	int parent;
	int child[16];
	CPoint min;
	CPoint max;
	BOOL center;
};

class CGroupList  
{
public:
	int AddRoot(int index, CPoint& min, CPoint& max);
	int AddSub(int index, CPoint& min, CPoint& max);
	void Init();
	void Init(int size);
	CGroupList();
	virtual ~CGroupList();

	int GetSize() { return m_Size;};
	MyGroup *m_Buffer;
private:
	int m_Size;
	int m_BufferSize;
	

};

#endif // !defined(AFX_GROUPLIST_H__EE8A0237_F775_4BE3_9EA2_9F58173C4FAD__INCLUDED_)
