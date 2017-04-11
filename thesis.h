#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wincon.h>
#include <conio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <ctype.h>
#include <time.h>

#ifndef TYPE_H_INCLUDED
#define TYPE_H_INCLUDED

#define SCR_ROW 25             /*��Ļ����*/
#define SCR_COL 80             /*��Ļ����*/

/**-------------------------������Ϣ��------------------------*/

/**
 *��У(school)��Ϣ�����ṹ
 */
typedef struct sch_node {
    char sch_id[8];            /**< ѧУ���*/
    char name[30];             /**< ѧУ����*/
    char manager[15];          /**< ������*/
    int  phone[15];            /**< ��ϵ�绰*/
    struct depart_node *dnext;    /**< ָ��Ժϵ֧����ָ��*/
    struct sch_node *next;     /**< ָ����һ����ָ��*/
} SCH_NODE;

/**
 *Ժϵ(department)������Ϣ�����ṹ
 */
typedef struct depart_node {
    char sch_id[8];             /**< ѧУ���*/
    char name[20];              /**< Ժϵ����*/
    int teacher;                /**< ��ʦ����*/
    int professor;              /**< ��������*/
    int aprofessor;             /**< ������associate-professor����*/
    int grastu;                 /**< �о���graduate-student����*/
    struct the_node *tnext;     /**< ָ��������Ϣ֧����ָ��*/
    struct depart_node *next;      /**< ָ����һ����ָ��*/
} DEP_NODE;

/**
 *����(thesis)��Ϣ�����ṹ
 */
typedef struct the_node {
    char title[50];           /**< ������Ŀ*/
    char writer[15];          /**< ����*/
    char sch_id[8];           /**< ����ѧУ���*/
    char depart[20];          /**< ����Ժϵ*/
    char ISSN[20];            /**< ����*/
    char date[10];                /**< �ڴ�*/
    char Rank;                /**< ����*/
    struct the_node *next;   /**< ָ����һ����ָ��*/
} THE_NODE;

/**----------------------����ͳ�ƹ�����-----------------------*/
/*ͳ�Ƹ���У��ʦ������������������ͽ�ʦ�˾������������*/
typedef struct Stat1{
    int num1;/*��У��ʦ����*/
    int num2;/*����������*/
    float num3;/*�˾�������*/
    char sch_id[8];/*��У���*/
    char name[30];/*��У����*/
    struct Stat1 *next;
}Stat1_NODE;

/*ͳ�ơ�a�������ķ���������5����ѧ*/
typedef struct Stat3{
    char sch_id[8];
    char name[30];
    int num;/*a���ķ�����*/
    struct Stat3 *next;
}Stat3_NODE;

/*ͳ��ĳ����о���ƽ��������������ߵ�10��Ժϵ*/
typedef struct Stat4{
    int num1;/*��У�о�������*/
    int num2;/*����������*/
    float num3;/*�˾�������*/
    char sch_id[8];/*��У���*/
    char name[20];/*Ժϵ����*/
    struct Stat4 *next;
}Stat4_NODE;

/**----------------------���潻����Ϣ��-----------------------*/

/**
 *��Ļ������Ϣ�������ṹ
 */
typedef struct layer_node {
    char LayerNo;            /**< �������ڲ���*/
    SMALL_RECT rcArea;       /**< ����������������*/
    CHAR_INFO *pContent;     /**< �������������ַ���Ԫԭ��Ϣ�洢������*/
    char *pScrAtt;           /**< �������������ַ���Ԫԭ����ֵ�洢������*/
    struct layer_node *next; /**< ָ����һ����ָ��*/
} LAYER_NODE;

/**
 *��ǩ���ṹ
 */
typedef struct labe1_bundle {
    char **ppLabel;        /**< ��ǩ�ַ��������׵�ַ*/
    COORD *pLoc;           /**< ��ǩ��λ�����׵�ַ*/
    int num;               /**< ��ǩ����*/
} LABEL_BUNDLE;

/**
 *�����ṹ
 */
typedef struct hot_area {
    SMALL_RECT *pArea;     /**< ������λ�����׵�ַ*/
    char *pSort;           /**< �������(�������ı���ѡ���)�����׵�ַ*/
    char *pTag;            /**< ������������׵�ַ*/
    int num;               /**< ��������*/
} HOT_AREA;

LAYER_NODE *gp_top_layer = NULL;               /*����������Ϣ����ͷ*/
SCH_NODE *gp_head = NULL;                     /*����ͷָ��*/

/**----------------------����&������Ϣ��-----------------------*/

char *gp_sys_name = "���ķ�����Ϣ����ϵͳ Ver.1.00  --Powered by qyx";     /*ϵͳ����*/
char *gp_sch_info_filename = "school.dat";      /*��У������Ϣ�����ļ�*/
char *gp_department_info_filename = "department.dat";  /*Ժϵ��Ϣ�����ļ�*/
char *gp_thesis_info_filename = "thesis.dat";      /*������Ϣ�����ļ�*/

char *ga_main_menu[] = {"�ļ�(F)",             /*ϵͳ���˵���*/
    	                "����ά��(M)",
                        "���ݲ�ѯ(Q)",
                        "����ͳ��(S)",
                        "����(H)"
                       };

char *ga_sub_menu[] = {"[S] ���ݱ���",          /*ϵͳ�Ӳ˵���*/
                       "[C] ���ݱ���",
                       "[B] ���ݻָ�",
                       "[X] �˳�    Alt+X",
                       "[S] ��У������Ϣ",
                       "[D] Ժϵ������Ϣ",
                       "[T] ���ķ�����Ϣ",
                       "[S] ��У������Ϣ",
                       "[D] Ժϵ������Ϣ",
                       "[T] ���ķ�����Ϣ",
                       "[1] ����ʦ�˾�����������ͳ��",
                       "[2] ��Уĳ��ȸ���������ͳ��",
                       "[3] ����Уa��������ǰ5��ͳ��",
                       "[4] ��Ժϵ�о����˾�����ͳ��",
                       "[5] ĳ�������a ������Ϣͳ��",
                       "[H] Э��",
                       "[A] ����..."
                      };


int ga_sub_menu_count[] = {4, 3, 3, 5, 2};  /*�����˵������Ӳ˵��ĸ���*/
int gi_sel_menu = 1;                        /*��ѡ�е����˵����,��ʼΪ1*/
int gi_sel_sub_menu = 0;                    /*��ѡ�е��Ӳ˵����,��ʼΪ0,��ʾδѡ��*/

CHAR_INFO *gp_buff_menubar_info = NULL;     /*��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
CHAR_INFO *gp_buff_stateBar_info = NULL;    /*���״̬����Ļ���ַ���Ϣ�Ļ�����*/

char *gp_scr_att = NULL;    /*�����Ļ���ַ���Ԫ����ֵ�Ļ�����*/

HANDLE gh_std_out;          /*��׼����豸���*/
HANDLE gh_std_in;           /*��׼�����豸���*/

int CreatList(SCH_NODE **pphead);               /*���������ʼ��*/
void InitInterface(void);                       /*ϵͳ�����ʼ��*/
void ClearScreen(void);                         /*����*/
void ShowMenu(void);                            /*��ʾ�˵���*/
void PopMenu(int num);                          /*��ʾ�����˵�*/
void PopPrompt(char **pCh);                        /*��ʾ��������*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*����������Ļ��Ϣά��*/
void PopOff(void);                              /*�رն��㵯������*/
void DrawBox(SMALL_RECT *parea);                /*���Ʊ߿�*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*���˵������˵���λ*/
void ShowState(void);                           /*��ʾ״̬��*/
void TagMainMenu(int num);                      /*��Ǳ�ѡ�е����˵���*/
void TagSubMenu(int num);                       /*��Ǳ�ѡ�е��Ӳ˵���*/
int DealConInput(HOT_AREA *phot_area, int *pihot_num);  /*����̨���봦��*/
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*��������*/
void RunSys(SCH_NODE **pphd);                  /*ϵͳ����ģ���ѡ�������*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*����ģ��ĵ���*/
void CloseSys(SCH_NODE *phd);                  /*�˳�ϵͳ*/
BOOL ShowModule(char **pString, int n);


BOOL LoadData(void);           /*���ݼ���*/
BOOL SaveData(void);           /*��������*/
BOOL BackupData(void);         /*��������*/
BOOL RestoreData(void);        /*�ָ�����*/
BOOL ExitSys(void);            /*�˳�ϵͳ*/
BOOL HelpTopic(void);          /*Э��*/
BOOL About(void);              /*����*/


BOOL MaintainSchInfo(void);    /*ά����У������Ϣ*/
BOOL MaintainDepInfo(void);    /*ά��Ժϵ������Ϣ*/
BOOL MaintainTheInfo(void);    /*ά�����ķ�����Ϣ*/


BOOL QuerySchInfo(void);       /*��ѯ��У������Ϣ*/
BOOL QueryDepInfo(void);       /*��ѯԺϵ������Ϣ*/
BOOL QueryTheInfo(void);       /*��ѯ���ķ�����Ϣ*/


BOOL Stat1(void);        /*������У��ʦ�˾�������ͳ��*/
BOOL Stat2(void);        /*��ĳ����У������������ͳ��*/
BOOL Stat3(void);        /*������Уa��������ǰ5��ͳ��*/
BOOL Stat4(void);        /*����Ժϵ�о��������Ŀͳ��*/
BOOL Stat5(void);        /*ͳ��ĳ�������a������Ŀ*/

BOOL SaveSysData(SCH_NODE *hd);                       /*����ϵͳ����*/
BOOL BackupSysData(SCH_NODE *phd, char *filename);     /*����ϵͳ����*/
BOOL RestoreSysData(SCH_NODE **pphd, char *filename);  /*�ָ�ϵͳ����*/

void schinfo1(SCH_NODE **phd); /*��Ӹ�У������Ϣ*/
void schinfo2(SCH_NODE *hd); /*�޸ĸ�У������Ϣ*/
void schinfo3(SCH_NODE **phd); /*ɾ����У������Ϣ*/
void schquery1(SCH_NODE *hd);  /*����Ų�ѯ��У*/
void schquery2(SCH_NODE *hd);  /*�����Ʋ�ѯ��У*/
void schquery3(SCH_NODE *hd); /*��ʾ���и�У��Ϣ*/

void depinfo1(SCH_NODE *hd);/*���Ժϵ��Ϣ*/
void depinfo2(SCH_NODE *hd); /*�޸�Ժϵ������Ϣ*/
void depinfo3(SCH_NODE *phd); /*ɾ��Ժϵ������Ϣ*/
void depquery1(SCH_NODE *hd);  /*��ѧУ��Ժϵ���Ʋ���*/
void depquery2(SCH_NODE *hd);  /*����ʦ��������*/
void depquery3(SCH_NODE *hd); /*��ʾ��У����Ժϵ��Ϣ*/

void theinfo1(SCH_NODE *hd);/*���������Ϣ*/
void theinfo2(SCH_NODE *hd); /*�޸�������Ϣ*/
void theinfo3(SCH_NODE *phd); /*ɾ��������Ϣ*/
void thequery1(SCH_NODE *hd);  /*������������*/
void thequery2(SCH_NODE *hd);  /*����һ����������*/

DEP_NODE *SeekdepNode1(SCH_NODE *hd,char *name);/*��Ժϵ���Ʋ���Ժϵ��Ϣ�ڵ�*/
int SeekdepNode2(SCH_NODE *hd,int num);/*����ʦ����*/
THE_NODE *SeektheNode1(DEP_NODE *hd,char *title);/*��������Ŀ����������Ϣ�ڵ�*/

void State1(SCH_NODE *hd);/*ͳ�Ƹ���У��ʦ������������������ͽ�ʦ�˾������������*/
void State2(SCH_NODE *hd);/*ͳ��ĳ��Уĳ��ȷ���ġ�a������b������c������d��������������*/
void State3(SCH_NODE *hd);/*ͳ�ơ�a�������ķ���������5����ѧ*/
void State4(SCH_NODE *hd);/*ͳ��ĳ����о���ƽ��������������ߵ�10��Ժϵ*/
void State5(SCH_NODE *hd);/*ͳ��ĳ�������a���ķ�����Ϣ*/
#endif /**< TYPE_H_INCLUDED*/
