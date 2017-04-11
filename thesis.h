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

#define SCR_ROW 25             /*屏幕行数*/
#define SCR_COL 80             /*屏幕列数*/

/**-------------------------基本信息链------------------------*/

/**
 *高校(school)信息链结点结构
 */
typedef struct sch_node {
    char sch_id[8];            /**< 学校编号*/
    char name[30];             /**< 学校名称*/
    char manager[15];          /**< 负责人*/
    int  phone[15];            /**< 联系电话*/
    struct depart_node *dnext;    /**< 指向院系支链的指针*/
    struct sch_node *next;     /**< 指向下一结点的指针*/
} SCH_NODE;

/**
 *院系(department)基本信息链结点结构
 */
typedef struct depart_node {
    char sch_id[8];             /**< 学校编号*/
    char name[20];              /**< 院系名称*/
    int teacher;                /**< 教师人数*/
    int professor;              /**< 教授人数*/
    int aprofessor;             /**< 副教授associate-professor人数*/
    int grastu;                 /**< 研究生graduate-student人数*/
    struct the_node *tnext;     /**< 指向论文信息支链的指针*/
    struct depart_node *next;      /**< 指向下一结点的指针*/
} DEP_NODE;

/**
 *论文(thesis)信息链结点结构
 */
typedef struct the_node {
    char title[50];           /**< 论文题目*/
    char writer[15];          /**< 作者*/
    char sch_id[8];           /**< 所属学校编号*/
    char depart[20];          /**< 所属院系*/
    char ISSN[20];            /**< 刊名*/
    char date[10];                /**< 期次*/
    char Rank;                /**< 分类*/
    struct the_node *next;   /**< 指向下一结点的指针*/
} THE_NODE;

/**----------------------数据统计功能链-----------------------*/
/*统计各高校教师人数、发表的论文数和教师人均发表的论文数*/
typedef struct Stat1{
    int num1;/*高校教师人数*/
    int num2;/*发表论文数*/
    float num3;/*人均发表数*/
    char sch_id[8];/*高校编号*/
    char name[30];/*高校名称*/
    struct Stat1 *next;
}Stat1_NODE;

/*统计‘a’类论文发表数最多的5所大学*/
typedef struct Stat3{
    char sch_id[8];
    char name[30];
    int num;/*a类文发表数*/
    struct Stat3 *next;
}Stat3_NODE;

/*统计某年度研究生平均发表论文数最高的10个院系*/
typedef struct Stat4{
    int num1;/*高校研究生人数*/
    int num2;/*发表论文数*/
    float num3;/*人均发表数*/
    char sch_id[8];/*高校编号*/
    char name[20];/*院系名称*/
    struct Stat4 *next;
}Stat4_NODE;

/**----------------------界面交互信息链-----------------------*/

/**
 *屏幕窗口信息链结点结点结构
 */
typedef struct layer_node {
    char LayerNo;            /**< 弹出窗口层数*/
    SMALL_RECT rcArea;       /**< 弹出窗口区域坐标*/
    CHAR_INFO *pContent;     /**< 弹出窗口区域字符单元原信息存储缓冲区*/
    char *pScrAtt;           /**< 弹出窗口区域字符单元原属性值存储缓冲区*/
    struct layer_node *next; /**< 指向下一结点的指针*/
} LAYER_NODE;

/**
 *标签束结构
 */
typedef struct labe1_bundle {
    char **ppLabel;        /**< 标签字符串数组首地址*/
    COORD *pLoc;           /**< 标签定位数组首地址*/
    int num;               /**< 标签个数*/
} LABEL_BUNDLE;

/**
 *热区结构
 */
typedef struct hot_area {
    SMALL_RECT *pArea;     /**< 热区定位数组首地址*/
    char *pSort;           /**< 热区类别(按键、文本框、选项框)数组首地址*/
    char *pTag;            /**< 热区序号数组首地址*/
    int num;               /**< 热区个数*/
} HOT_AREA;

LAYER_NODE *gp_top_layer = NULL;               /*弹出窗口信息链链头*/
SCH_NODE *gp_head = NULL;                     /*主链头指针*/

/**----------------------功能&函数信息链-----------------------*/

char *gp_sys_name = "论文发表信息管理系统 Ver.1.00  --Powered by qyx";     /*系统名称*/
char *gp_sch_info_filename = "school.dat";      /*高校基本信息数据文件*/
char *gp_department_info_filename = "department.dat";  /*院系信息数据文件*/
char *gp_thesis_info_filename = "thesis.dat";      /*论文信息数据文件*/

char *ga_main_menu[] = {"文件(F)",             /*系统主菜单名*/
    	                "数据维护(M)",
                        "数据查询(Q)",
                        "数据统计(S)",
                        "帮助(H)"
                       };

char *ga_sub_menu[] = {"[S] 数据保存",          /*系统子菜单名*/
                       "[C] 数据备份",
                       "[B] 数据恢复",
                       "[X] 退出    Alt+X",
                       "[S] 高校基本信息",
                       "[D] 院系基本信息",
                       "[T] 论文发表信息",
                       "[S] 高校基本信息",
                       "[D] 院系基本信息",
                       "[T] 论文发表信息",
                       "[1] 按教师人均发表数降序统计",
                       "[2] 高校某年度各类论文数统计",
                       "[3] 各高校a类文总数前5名统计",
                       "[4] 各院系研究生人均发表统计",
                       "[5] 某年度所有a 类文信息统计",
                       "[H] 协助",
                       "[A] 关于..."
                      };


int ga_sub_menu_count[] = {4, 3, 3, 5, 2};  /*各主菜单项下子菜单的个数*/
int gi_sel_menu = 1;                        /*被选中的主菜单项号,初始为1*/
int gi_sel_sub_menu = 0;                    /*被选中的子菜单项号,初始为0,表示未选中*/

CHAR_INFO *gp_buff_menubar_info = NULL;     /*存放菜单条屏幕区字符信息的缓冲区*/
CHAR_INFO *gp_buff_stateBar_info = NULL;    /*存放状态条屏幕区字符信息的缓冲区*/

char *gp_scr_att = NULL;    /*存放屏幕上字符单元属性值的缓冲区*/

HANDLE gh_std_out;          /*标准输出设备句柄*/
HANDLE gh_std_in;           /*标准输入设备句柄*/

int CreatList(SCH_NODE **pphead);               /*数据链表初始化*/
void InitInterface(void);                       /*系统界面初始化*/
void ClearScreen(void);                         /*清屏*/
void ShowMenu(void);                            /*显示菜单栏*/
void PopMenu(int num);                          /*显示下拉菜单*/
void PopPrompt(char **pCh);                        /*显示弹出窗口*/
void PopUp(SMALL_RECT *, WORD, LABEL_BUNDLE *, HOT_AREA *);  /*弹出窗口屏幕信息维护*/
void PopOff(void);                              /*关闭顶层弹出窗口*/
void DrawBox(SMALL_RECT *parea);                /*绘制边框*/
void LocSubMenu(int num, SMALL_RECT *parea);    /*主菜单下拉菜单定位*/
void ShowState(void);                           /*显示状态栏*/
void TagMainMenu(int num);                      /*标记被选中的主菜单项*/
void TagSubMenu(int num);                       /*标记被选中的子菜单项*/
int DealConInput(HOT_AREA *phot_area, int *pihot_num);  /*控制台输入处理*/
void SetHotPoint(HOT_AREA *phot_area, int hot_num);     /*设置热区*/
void RunSys(SCH_NODE **pphd);                  /*系统功能模块的选择和运行*/
BOOL ExeFunction(int main_menu_num, int sub_menu_num);  /*功能模块的调用*/
void CloseSys(SCH_NODE *phd);                  /*退出系统*/
BOOL ShowModule(char **pString, int n);


BOOL LoadData(void);           /*数据加载*/
BOOL SaveData(void);           /*保存数据*/
BOOL BackupData(void);         /*备份数据*/
BOOL RestoreData(void);        /*恢复数据*/
BOOL ExitSys(void);            /*退出系统*/
BOOL HelpTopic(void);          /*协助*/
BOOL About(void);              /*关于*/


BOOL MaintainSchInfo(void);    /*维护高校基本信息*/
BOOL MaintainDepInfo(void);    /*维护院系基本信息*/
BOOL MaintainTheInfo(void);    /*维护论文发表信息*/


BOOL QuerySchInfo(void);       /*查询高校基本信息*/
BOOL QueryDepInfo(void);       /*查询院系基本信息*/
BOOL QueryTheInfo(void);       /*查询论文发表信息*/


BOOL Stat1(void);        /*按各高校教师人均发表数统计*/
BOOL Stat2(void);        /*对某所高校按各类论文数统计*/
BOOL Stat3(void);        /*按各高校a类文总数前5名统计*/
BOOL Stat4(void);        /*按各院系研究生年均数目统计*/
BOOL Stat5(void);        /*统计某年度所有a类文数目*/

BOOL SaveSysData(SCH_NODE *hd);                       /*保存系统数据*/
BOOL BackupSysData(SCH_NODE *phd, char *filename);     /*备份系统数据*/
BOOL RestoreSysData(SCH_NODE **pphd, char *filename);  /*恢复系统数据*/

void schinfo1(SCH_NODE **phd); /*添加高校基本信息*/
void schinfo2(SCH_NODE *hd); /*修改高校基本信息*/
void schinfo3(SCH_NODE **phd); /*删除高校基本信息*/
void schquery1(SCH_NODE *hd);  /*按编号查询高校*/
void schquery2(SCH_NODE *hd);  /*按名称查询高校*/
void schquery3(SCH_NODE *hd); /*显示所有高校信息*/

void depinfo1(SCH_NODE *hd);/*添加院系信息*/
void depinfo2(SCH_NODE *hd); /*修改院系基本信息*/
void depinfo3(SCH_NODE *phd); /*删除院系基本信息*/
void depquery1(SCH_NODE *hd);  /*按学校和院系名称查找*/
void depquery2(SCH_NODE *hd);  /*按教师人数查找*/
void depquery3(SCH_NODE *hd); /*显示高校所有院系信息*/

void theinfo1(SCH_NODE *hd);/*添加论文信息*/
void theinfo2(SCH_NODE *hd); /*修改论文信息*/
void theinfo3(SCH_NODE *phd); /*删除论文信息*/
void thequery1(SCH_NODE *hd);  /*按论文名查找*/
void thequery2(SCH_NODE *hd);  /*按第一作者名查找*/

DEP_NODE *SeekdepNode1(SCH_NODE *hd,char *name);/*按院系名称查找院系信息节点*/
int SeekdepNode2(SCH_NODE *hd,int num);/*按教师人数*/
THE_NODE *SeektheNode1(DEP_NODE *hd,char *title);/*按论文题目查找论文信息节点*/

void State1(SCH_NODE *hd);/*统计各高校教师人数、发表的论文数和教师人均发表的论文数*/
void State2(SCH_NODE *hd);/*统计某高校某年度发表的‘a’、‘b’、‘c’、‘d’四类论文数。*/
void State3(SCH_NODE *hd);/*统计‘a’类论文发表数最多的5所大学*/
void State4(SCH_NODE *hd);/*统计某年度研究生平均发表论文数最高的10个院系*/
void State5(SCH_NODE *hd);/*统计某年度所有a类文发表信息*/
#endif /**< TYPE_H_INCLUDED*/
