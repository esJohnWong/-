// GolbalPlan.h: interface for the GolbalPlan class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _GL_GLOBAL_PLAN_
#define _GL_GLOBAL_PLAN_

#include "Datastruct.h"


#define NOOFROUTES 40    //��ʼ��Ⱥ��С
#define MAXGEN   10000  //�Ӵ�������������
#define MAXNUM  INT_MAX


//�滮����ľ��η�Χ
typedef struct _TransInfo
{
	int leftupX;    //�滮�������Ͻ�X����
	int leftupY;    //�滮�������Ͻ�Y����
	int width;      //�滮���εĿ�����
	int height;     //�滮���εĸߣ�����
}transInfo; 


typedef struct _genoType
{
	EarthPoint*  genes;  //��Ӧһ������������
	double totalcost;
}genoType;
 

class GL_GolbalPlan 
{
public:
	GL_GolbalPlan();
	~GL_GolbalPlan();


	int SEED;            //ÿ���������������
	int num;             //��¼���·��������
	transInfo transinfo;   //�滮����
	genoType *planningRoutes[NOOFROUTES]; //��������������
	NavigationNode emitPoint;       //���
	NavigationNode targetPoint;		//Ŀ���
	

	//ȫ�ֹ滮��غ�������...........................
	//�����������ж��뷢���Ŀ�����Ϣ
	bool run();   //��ں���

	//���뷢���Ŀ�����Ϣ���ж������Ƿ�Ϸ�
	bool loadTasks_GL(std::list<NavigationNode> &ETList);

	//�ӷ���Ŀ����������ε���Ϣ
	void GetTransInfo_GL();

	//��ɶԳ�ʼ�ļ����������еĳ�ʼ��  
	void initialize_GL();

	double calAngle_GL(EarthPoint * frontNode,EarthPoint* centreNode,EarthPoint* lastNode);//ֱ����õ�ǰ��нǵĻ���

	//�������������
	void evaluateCostOfARuote_GL(genoType *planningRoute);

	//�����۶�ÿ�����������ܴ��ۣ����մ���ֵ�Ժ�����Ⱥ���ӵ͵�������
	void evaluate_GL();

	//���갴X��������
	void orderByX_GL(genoType *Track);
	
	//��������
	void crossover_GL();
	void simpleCrossover_GL(genoType *TrackOne,genoType *TrackTwo,genoType *LastOne,genoType *LastTwo);
	void segmentCrossover_GL(genoType *TrackOne,genoType *TrackTwo,genoType *LastOne,genoType *LastTwo);
	
	//��������
	void mutate_GL();
	void distrub_GL(genoType *Track,int i);
	void disturbWithinACircle_GL(EarthPoint *centrePoint,int r);
	//����:ƽ������
	void smooth_GL(genoType *Track);
	//�������ӣ�
	void stretch_GL(genoType *Track);
	bool applyProjection_GL(int beginPoint,int endPoint,genoType *Track); //����ֱ��
	
	//���������
	double distanc_GL(EarthPoint point1,EarthPoint point2);

	//����Ⱥ��������
	void reOrder_GL();

	//�������
	void OutPutData_GL();

};

#endif // !defined(AFX_GOLBALPLAN_H__AB013ECA_3CE7_43B4_9665_8BA8CE266E98__INCLUDED_)
