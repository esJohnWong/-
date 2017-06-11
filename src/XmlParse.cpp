#include "XmlParse.h"


XmlParse::XmlParse(void)
{
	pDoc = NULL; // xml文档
	pRootElement = NULL; //根节点
	pElement = NULL;  //元素
	pNodeList = NULL; // 节点链表
	pNode = NULL;     //节点
	pAttrList = NULL; // 属性链表
	pAttrNode = NULL; // 属性
}


XmlParse::~XmlParse(void)
{  

}

bool XmlParse::LoadXmlFile(char *fileName)
{
	szXmlFile = fileName;
	hr = pDoc.CreateInstance(__uuidof(DOMDocument40));
	if (FAILED(hr))
	{
		return false;
	}

	VARIANT_BOOL bXmlLoad = pDoc->load((_variant_t)szXmlFile);
	if (!bXmlLoad) // 加载失败
	{
		return false;
	}
	return true;
}

bool XmlParse::readParam(char *fileName,double &minl,double &maxl,double &maxhangle,double &maxvangle)
{
	if(!LoadXmlFile(fileName))
	{
		return false;
	}

	// 在树中查找名为TowersMinDist的节点, "//"表示在任意一层查找  
	pNode = pDoc -> selectSingleNode( "//TowersMinDist");
	pAttrList = pNode ->Getattributes();
	pAttrNode = pAttrList->Getitem(1);
	variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
	minl =atof((char *)(_bstr_t)variantvalue);

	// 在树中查找名为TowersMaxDist的节点, "//"表示在任意一层查找  
	pNode = pDoc -> selectSingleNode( "//TowersMaxDist");
	pAttrList = pNode->Getattributes();
	pAttrNode = pAttrList->Getitem(1);
	variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
	maxl = atof((char *)(_bstr_t)variantvalue);

	// 在树中查找名为TowersMaxAngle的节点, "//"表示在任意一层查找  
	pNode = pDoc -> selectSingleNode( "//TowersMaxAngle");
	pAttrList = pNode ->Getattributes();
	pAttrNode = pAttrList->Getitem(1);
	variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
	maxhangle = atof((char *)(_bstr_t)variantvalue);

	// 在树中查找名为TowersMaxSlope的节点, "//"表示在任意一层查找  
	pNode = pDoc -> selectSingleNode( "//TowersMaxSlope");
	pAttrList = pNode ->Getattributes();
	pAttrNode = pAttrList->Getitem(1);
	variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
	maxvangle = atof((char *)(_bstr_t)variantvalue);

	return true;
}

bool XmlParse::readETPoint(char *fileName,GroundPoint &startPoint,GroundPoint &endPoint)
{
	if(!LoadXmlFile(fileName))
	{
		return false;
	}

	// 在树中查找名为StartPoint的节点, "//"表示在任意一层查找  
	pNode = pDoc -> selectSingleNode( "//StartPoint");
	pAttrList = pNode ->Getattributes();
	pAttrNode = pAttrList->Getitem(1);
	variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
	strETParse((char *)(_bstr_t)variantvalue,startPoint);

	// 在树中查找名为EndPoint的节点, "//"表示在任意一层查找  
	pNode = pDoc -> selectSingleNode( "//EndPoint");
	pAttrList = pNode->Getattributes();
	pAttrNode = pAttrList->Getitem(1);
	variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
	strETParse((char *)(_bstr_t)variantvalue,endPoint);

	return true;
}

bool XmlParse::readConstrain(char *fileName,vector<iPolygon> &PolygonVector)
{
	if(!LoadXmlFile(fileName))
	{
		return false;
	}

	// 在树中查找名为ConstrainArea的节点, "//"表示在任意一层查找  
	pNodeList = pDoc ->selectNodes( "//ConstrainArea");
	lChilds = pNodeList->Getlength();
	iPolygon plg;
	for(i = 0; i < lChilds; i++)
	{
		(plg.ptPolygon).clear();
		pNode = pNodeList->Getitem(i);
		pAttrList = pNode ->Getattributes();

		//取出等级
		pAttrNode = pAttrList->Getitem(1);
		variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
		plg.level = atoi((char *)(_bstr_t)variantvalue);

		//取出顶点坐标
		pAttrNode = pAttrList->Getitem(3);
		variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
		//polygon.level = atoi(( char *)(_bstr_t)variantvalue);
		strConstrainParse((char *)(_bstr_t)variantvalue, plg.ptPolygon);
		PolygonVector.push_back(plg);
	}
	return true;
}

void XmlParse::strConstrainParse(char *str, vector<iPoint> &ptPolygon)
{
	char pxy[40];
	double tmp;
	int iCount = 0;
	iPoint p;
	int len = strlen(str);
	for(int j = 10;j<len;j++)
	{
		if(str[j] == ' ')
		{
			tmp = atof(pxy)+0.5;
			p.x = tmp;
			iCount = 0;
		}

		if(str[j] == ',')
		{
			tmp = atof(pxy)+0.5;
			p.y = tmp;
			ptPolygon.push_back(p);
			iCount = 0;
			j++;  //跳过一个字符
		}

		if(str[j] == ')')
		{
			tmp = atof(pxy)+0.5;
			p.y = tmp;
			ptPolygon.push_back(p);
			break;
		}	

		if((str[j] >= '0' && str[j] <= '9') || (str[j] == '.'))
		{
			pxy[iCount ++] = str[j];
		}
	}
}

void XmlParse::strETParse(char *str, GroundPoint &p)
{
	char pxy[40];
	double tmp;
	int iCount = 0;
	int len = strlen(str);
	for(int j = 0;j<len;j++)
	{
		if(str[j] == ',')
		{
			tmp = atof(pxy);
			p.X = tmp;
			iCount = 0;
		}

		if((str[j] >= '0' && str[j] <= '9') || (str[j] == '.'))
		{
			pxy[iCount ++] = str[j];
		}
	}

	tmp = atof(pxy);
	p.Y = tmp;
}

bool XmlParse::writeResultToXml(char *fileName,vector<GroundPoint>& NodeVector)
{
	if(!LoadXmlFile(fileName))
	{
		return false;
	}
	if(NodeVector.size() > 0)
	{
		pRootElement = pDoc->GetdocumentElement();
		pNode = pDoc->createNode((_variant_t)(long)MSXML2::NODE_ELEMENT, (_bstr_t)(char*)"Result", (_bstr_t)(char*)"");
		pRootElement->appendChild(pNode); // 节点
		pRootElement->appendChild(pDoc->createTextNode("\n"));

		char buf[40];
		for(int i=0; i<NodeVector.size(); i++)
		{   
			pNode->appendChild(pDoc->createTextNode("\n\t\t"));
			pElement = pDoc->createElement((_bstr_t)(char*)"Node");
			sprintf(buf,"%.2f",NodeVector[i].X);
			pElement->setAttribute((_bstr_t)(char*)"X", (_variant_t)buf); 
			sprintf(buf,"%.2f",NodeVector[i].Y);
			pElement->setAttribute((_bstr_t)(char*)"Y", (_variant_t)buf); 
			sprintf(buf,"%.2f",NodeVector[i].Z);
			pElement->setAttribute((_bstr_t)(char*)"Z", (_variant_t)buf); 
			pNode->appendChild(pElement); // 加入节点
		}
		pNode->appendChild(pDoc->createTextNode("\n\t"));
		pDoc->save((_variant_t)szXmlFile);
	}
	return true;
}

bool XmlParse::readResultFromXml(char *fileName,vector<GroundPoint>& NodeVector)
{
	if(!LoadXmlFile(fileName))
	{
		return false;
	}

	// 在树中查找名为Node的节点, "//"表示在任意一层查找  
	pNodeList = pDoc ->selectNodes( "//Node");
	lChilds = pNodeList->Getlength();
	GroundPoint tmpNode;
	for(i = 0; i < lChilds; i++)
	{
		pNode = pNodeList->Getitem(i);
		pAttrList = pNode ->Getattributes();

		pAttrNode = pAttrList->Getitem(0);
		variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
		tmpNode.X = atof((char *)(_bstr_t)variantvalue);

		pAttrNode = pAttrList->Getitem(1);
		variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
		tmpNode.Y = atof((char *)(_bstr_t)variantvalue);

		pAttrNode = pAttrList->Getitem(2);
		variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
		tmpNode.Z = atof((char *)(_bstr_t)variantvalue);
		NodeVector.push_back(tmpNode);
	}

	return true;
}

bool XmlParse::writeGuidePointToXml(char *fileName,vector<GroundPoint>& NodeVector)
{
	if(!LoadXmlFile(fileName))
	{
		return false;
	}

	if(NodeVector.size() > 0)
	{
		pRootElement = pDoc->GetdocumentElement();
		pNode = pDoc->createNode((_variant_t)(long)MSXML2::NODE_ELEMENT, (_bstr_t)(char*)"GuidePoint", (_bstr_t)(char*)"");
		pRootElement->appendChild(pNode); // 节点
		pRootElement->appendChild(pDoc->createTextNode("\n"));

		char buf[40];
		for(int i=0; i<NodeVector.size(); i++)
		{   
			pNode->appendChild(pDoc->createTextNode("\n\t\t"));
			pElement = pDoc->createElement((_bstr_t)(char*)"Mark");
			sprintf(buf,"%.2f",NodeVector[i].X);
			pElement->setAttribute((_bstr_t)(char*)"X", (_variant_t)buf); 
			sprintf(buf,"%.2f",NodeVector[i].Y);
			pElement->setAttribute((_bstr_t)(char*)"Y", (_variant_t)buf); 
			sprintf(buf,"%.2f",NodeVector[i].Z);
			pElement->setAttribute((_bstr_t)(char*)"Z", (_variant_t)buf); 
			pNode->appendChild(pElement); // 加入节点
		}
		pNode->appendChild(pDoc->createTextNode("\n\t"));
		pDoc->save((_variant_t)szXmlFile);
	}
	return true;
}

bool XmlParse::readGuidePointFromXml(char *fileName,vector<GroundPoint>& NodeVector)
{
	if(!LoadXmlFile(fileName))
	{
		return false;
	}

	// 在树中查找名为Mark的节点, "//"表示在任意一层查找  
	pNodeList = pDoc ->selectNodes( "//Mark");
	lChilds = pNodeList->Getlength();
	GroundPoint tmpNode;
	for(i = 0; i < lChilds; i++)
	{
		pNode = pNodeList->Getitem(i);
		pAttrList = pNode ->Getattributes();

		pAttrNode = pAttrList->Getitem(0);
		variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
		tmpNode.X = atof((char *)(_bstr_t)variantvalue);

		pAttrNode = pAttrList->Getitem(1);
		variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
		tmpNode.Y = atof((char *)(_bstr_t)variantvalue);

		pAttrNode = pAttrList->Getitem(2);
		variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
		tmpNode.Z = atof((char *)(_bstr_t)variantvalue);
		NodeVector.push_back(tmpNode);
	}
	return true;
}

bool XmlParse::readGuidePointForPlan(char *fileName,vector<GroundPoint>& NodeVector)
{
	if(!LoadXmlFile(fileName))
	{
		return false;
	}

	// 在树中查找名为GuidePoint的节点, "//"表示在任意一层查找  
	pNodeList = pDoc ->selectNodes( "//GuidePoint");
	lChilds = pNodeList->Getlength();
	pNode = pNodeList->Getitem(lChilds - 1);  //取出最后一次画的GuidePoint
	pNodeList = pNode ->selectNodes("Mark");  //取出该节点的元素
	lAttr = pNodeList->Getlength();
	GroundPoint tmpNode;
	for(i = 0; i < lAttr; i++)
	{
		pNode = pNodeList->Getitem(i);
		pAttrList = pNode ->Getattributes();

		pAttrNode = pAttrList->Getitem(0);
		variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
		tmpNode.X = atof((char *)(_bstr_t)variantvalue);

		pAttrNode = pAttrList->Getitem(1);
		variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
		tmpNode.Y = atof((char *)(_bstr_t)variantvalue);

		pAttrNode = pAttrList->Getitem(2);
		variantvalue = pAttrNode -> GetnodeTypedValue();   //取得节点的值 
		tmpNode.Z = atof((char *)(_bstr_t)variantvalue);
		NodeVector.push_back(tmpNode);
	}

	return true;
}