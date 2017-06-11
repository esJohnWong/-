#pragma once

#include "DataStruct.h"

// ����MSXML������
#import <msxml4.dll>
//#import <msxml6.dll>
using namespace MSXML2;

class XmlParse
{
	class InitializeCom
	{
	public:
		InitializeCom()    
		{        
			CoInitialize(NULL); // Initializes the COM library  
		}
		~InitializeCom() 
		{       
			CoUninitialize(); // Closes the COM library   
		}
	}InitCom;

public:
	XmlParse(void);
	~XmlParse(void);
    bool LoadXmlFile(char *fileName);
	bool readConstrain(char *fileName,vector<iPolygon> &PolygonVector);
	bool readParam(char *fileName,double &minl,double &maxl,double &maxhangle,double &maxvangle);
	bool readETPoint(char *fileName,GroundPoint &startPoint,GroundPoint &endPoint);
	bool readResultFromXml(char *fileName,vector<GroundPoint>& NodeVector);
	bool writeResultToXml(char *fileName,vector<GroundPoint>& NodeVector);
	bool readGuidePointFromXml(char *fileName,vector<GroundPoint>& NodeVector);
	bool readGuidePointForPlan(char *fileName,vector<GroundPoint>& NodeVector);
	bool writeGuidePointToXml(char *fileName,vector<GroundPoint>& NodeVector);
	void strConstrainParse(char *str, vector<iPoint> &ptPolygon);
	void strETParse(char *str, GroundPoint &p);

	MSXML2::IXMLDOMDocumentPtr pDoc;      // xml�ĵ�
	MSXML2::IXMLDOMElementPtr pRootElement; //���ڵ�
	MSXML2::IXMLDOMElementPtr pElement;   //Ԫ��
	MSXML2::IXMLDOMNodeListPtr pNodeList; // �ڵ�����
	MSXML2::IXMLDOMNodePtr pNode;         // �ڵ�
	MSXML2::IXMLDOMNamedNodeMapPtr pAttrList; // ��������
	MSXML2::IXMLDOMAttributePtr pAttrNode;    // ����
	_variant_t variantvalue;  //����ֵ
	HRESULT hr;
	long lChilds, lAttr, i;
	char *szXmlFile;
};

