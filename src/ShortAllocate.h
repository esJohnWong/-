// ShortAllocate.h: interface for the CShortAllocate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHORTALLOCATE_H__1820AC9D_A8AC_4D57_B381_FDB173EA95AC__INCLUDED_)
#define AFX_SHORTALLOCATE_H__1820AC9D_A8AC_4D57_B381_FDB173EA95AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CShortAllocate  
{
public:
	CShortAllocate();
	bool Allocate(int count);
	~CShortAllocate();
	bool Delete();
public:
	short * m_pData;
};

#endif // !defined(AFX_SHORTALLOCATE_H__1820AC9D_A8AC_4D57_B381_FDB173EA95AC__INCLUDED_)
