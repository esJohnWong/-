// ShortAllocate.cpp: implementation of the CShortAllocate class.
//
//////////////////////////////////////////////////////////////////////

#include "cstdio"
#include "ShortAllocate.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShortAllocate::CShortAllocate()
{
	m_pData = (short *)NULL;
}
bool CShortAllocate::Allocate(int count)
{
	m_pData = new short[count];
	if (NULL==m_pData)
	{
		return false;
	}
	return true;
}

bool CShortAllocate::Delete()
{
	if (m_pData != NULL)
	{
		delete [] m_pData;
		m_pData = NULL;
	}
	return true;
}

CShortAllocate::~CShortAllocate()
{
	if (NULL!=m_pData)
	{
		delete []m_pData;
	}
}
