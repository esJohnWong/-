#include "DataConvertThread.h"
#include "DataStruct.h"

DataConvertThread::DataConvertThread(void)
{
	errcode = 0;
}


DataConvertThread::~DataConvertThread(void)
{
}

void DataConvertThread::run()
{
	//���߳̽�����־��Ϊ��
	isEndThread = false;

	if (!dataConvert.Convert((char *)fileNameOrg.toStdString().c_str() , (char *)fileNameNew.toStdString().c_str()) )
	{
		errcode = 1;
	}
}
