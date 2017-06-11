/************************************************************************/
/*初始化组合图文件*/
/*add by hj 2013.11.3*/
/************************************************************************/
#pragma once
class WriteComMapFile
{
public:
	WriteComMapFile(void);
	~WriteComMapFile(void);

private:
	unsigned short * m_pDataLine;
public:
	bool Allocate(__int64 length);
	bool WrtieAllData(const char * szFileName , __int64 height , __int64 width);
};

