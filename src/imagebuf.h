/************************************************************************/
/* ΪͼƬ��ʾ�����ڴ�                                                      */
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
	unsigned char * m_pBuf;//����
};

