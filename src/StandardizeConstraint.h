/**********************************
*�����ļ���������Լ����׼����ת�����꣩
***********************************/
#pragma once
#include "DataStruct.h"
class StandardizeConstraint
{
public:
	StandardizeConstraint(void);
	~StandardizeConstraint(void);
	//������ε�����ת��Ϊ��Ļ���� �����
	bool PolygonConvertToScreenCoordinate(iPolygon & polygon);
	//�����ε���Ӿ���
	bool MakeRectForPolygon(iPolygon & polygon);
	//ת�������� �����
	bool Standardize(iPolygon & polygon);
	//�ж�һ�����Ƿ��ڶ������
	bool APointIsInPolygon(iPoint point , const iPolygon polygon);
	//��һ���������ת������Ļ����
	bool PointLBToSC(GroundPoint ptOrg , iPoint &ptNew);
	//��һ�������Ļ����ת���ɾ�γ��
	bool PointSCToLB(iPoint ptOrg , GroundPoint & ptNew);
	//�������㾭γ��ת��Ϊ��Ļ���� ��������� �滮���
	bool GuidOrResConvertToScreenCoordinate(vector<GroundPoint> &vctOrg , vector<iPoint> &vctNew);
	//����������Ļ����ת��Ϊ��γ��
	bool GuidOrResConvertToLBCoordinate(vector<iPoint> &vctOrg , vector<GroundPoint> & vctNew);
};

