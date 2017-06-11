/************************************************************************/
/* 为图片显示分配内存                                                      */
/************************************************************************/

#pragma once
class ImageBuf
{
public:
	ImageBuf(void);
	~ImageBuf(void);
	bool Allocate(int count);
	bool Delete();
public:
	unsigned char * m_pBuf;//缓存
};

