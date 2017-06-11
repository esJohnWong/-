#include "imagebuf.h"
#include <cstdio>

ImageBuf::ImageBuf(void)
{
	m_pBuf = NULL;
}

bool ImageBuf::Delete()
{
	if (NULL != m_pBuf)
	{
		delete [] m_pBuf;
		m_pBuf = NULL;
		return true;
	}
	return false;
}

bool ImageBuf::Allocate(int count)
{
	m_pBuf = new unsigned char[count];
	if (NULL == m_pBuf)
	{
		return false;
	}
	return true;
}

ImageBuf::~ImageBuf(void)
{
	if (NULL != m_pBuf)
	{
		delete [] m_pBuf;
	}
}
