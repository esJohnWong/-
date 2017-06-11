#pragma once

#include "DataStruct.h"

// 引入MSXML解析器
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

	MSXML2::IXMLDOMDocumentPtr pDoc;      // xml文档
	MSXML2::IXMLDOMElementPtr pRootElement; //根节点
	MSXML2::IXMLDOMElementPtr pElement;   //元素
	MSXML2::IXMLDOMNodeListPtr pNodeList; // 节点链表
	MSXML2::IXMLDOMNodePtr pNode;         // 节点
	MSXML2::IXMLDOMNamedNodeMapPtr pAttrList; // 属性链表
	MSXML2::IXMLDOMAttributePtr pAttrNode;    // 属性
	_variant_t variantvalue;  //属性值
	HRESULT hr;
	long lChilds, lAttr, i;
	char *szXmlFile;
};

