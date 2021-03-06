#include "thesis.h"

unsigned long ul;

int main()
{
    COORD size = {SCR_COL, SCR_ROW};              /*窗口缓冲区大小*/

    gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* 获取标准输出设备句柄*/
    gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* 获取标准输入设备句柄*/

    SetConsoleTitle(gp_sys_name);                 /*设置窗口标题*/
    SetConsoleScreenBufferSize(gh_std_out, size); /*设置窗口缓冲区大小80*25*/

    LoadData();                   /*数据加载*/
    InitInterface();          /*界面初始化*/
    RunSys(&gp_head);             /*系统功能模块的选择及运行*/
    CloseSys(gp_head);            /*退出系统*/

    return 0;
}

/**
 * 函数名称: LoadData
 * 函数功能: 将三类基础数据从数据文件载入到内存缓冲区和十字链表中.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: BOOL类型, 功能函数中除了函数ExitSys的返回值可以为FALSE外,
 *           其他函数的返回值必须为TRUE.
 *
 * 调用说明: 为了能够以统一的方式调用各功能函数, 将这些功能函数的原型设为
 *           一致, 即无参数且返回值为BOOL. 返回值为FALSE时, 结束程序运行.
 */
BOOL LoadData()
{
    int Re = 0;
    Re = CreatList(&gp_head);
    if (Re != 28)
    {  /*数据加载提示信息*/
        printf("\n系统基础数据不完整!\n");
        printf("\n按任意键继续...\n");
        getch();
    }
    return TRUE;
}

/**
 * 函数名称: CreatList
 * 函数功能: 从数据文件读取基础数据, 并存放到所创建的十字链表中.
 * 输入参数: 无
 * 输出参数: phead 主链头指针的地址, 用来返回所创建的十字链.
 * 返 回 值: int型数值, 表示链表创建的情况.
 *           0  空链, 无数据
 *           4  已加载高校基本信息数据，无院系基本信息和论文发表信息数据
 *           12 已加载院系基本信息和高校基本信息数据，无论文发表信息数据
 *           28 三类基础数据都已加载
 *
 * 调用说明:
 */
int CreatList(SCH_NODE **phead)
{
    SCH_NODE *hd = NULL, *pSCHNode, tmp1;
    DEP_NODE *pDEPNode, tmp2;
    THE_NODE *pTHENode, tmp3;
    FILE *pFile;
    int find;
    int re = 0;

    if ((pFile = fopen(gp_sch_info_filename, "rb")) == NULL)
    {
        printf("高校基本信息数据文件打开失败!\n");
        return re;
    }
    printf("高校基本信息数据文件打开成功!\n");

    /*从数据文件中读高校基本信息数据，存入以后进先出方式建立的主链中*/
    while (fread(&tmp1, sizeof(SCH_NODE), 1, pFile) == 1)
    {
        pSCHNode = (SCH_NODE *)malloc(sizeof(SCH_NODE));
        *pSCHNode = tmp1;
        pSCHNode->dnext = NULL;
        pSCHNode->next = hd;
        hd = pSCHNode;
    }
    fclose(pFile);

    if (hd == NULL)
    {
        printf("高校基本信息数据文件加载失败!\n");
        return re;
    }
    printf("高校基本信息数据文件加载成功!\n");
    *phead = hd;
    re += 4;

    if ((pFile = fopen(gp_department_info_filename, "rb")) == NULL)
    {
        printf("院系基本信息数据文件打开失败!\n");
        return re;
    }
    printf("院系基本信息数据文件打开成功!\n");
    re += 8;

    /*从数据文件中读取院系基本信息数据，存入主链对应结点的院系基本信息支链中*/
    while (fread(&tmp2, sizeof(DEP_NODE), 1, pFile) == 1)
    {
        /*创建结点，存放从数据文件中读出的院系基本信息*/
        pDEPNode = (DEP_NODE *)malloc(sizeof(DEP_NODE));
        *pDEPNode = tmp2;
        pDEPNode->tnext = NULL;

        /*在主链上查找该院系所在高校对应的主链结点*/
        pSCHNode = hd;
        while (pSCHNode != NULL
               && strcmp(pSCHNode->sch_id, pDEPNode->sch_id) != 0)
        {
            pSCHNode = pSCHNode->next;
        }
        if (pSCHNode != NULL) /*如果找到，则将结点以后进先出方式插入学生信息支链*/
        {
            pDEPNode->next = pSCHNode->dnext;
            pSCHNode->dnext = pDEPNode;
        }
        else  /*如果未找到，则释放所创建结点的内存空间*/
        {
            free(pDEPNode);
        }
    }
    fclose(pFile);

    if ((pFile = fopen(gp_thesis_info_filename, "rb")) == NULL)
    {
        printf("论文发表信息数据文件打开失败!\n");
        return re;
    }
    printf("论文发表信息数据文件打开成功!\n");
    re += 16;

    /*从数据文件中读取论文基本信息数据，存入院系基本信息支链对应结点的论文发表信息支链中*/
    while (fread(&tmp3, sizeof(THE_NODE), 1, pFile) == 1)
    {
        /*创建结点，存放从数据文件中读出的论文信息*/
        pTHENode = (THE_NODE *)malloc(sizeof(THE_NODE));
        *pTHENode = tmp3;

        /*查找院系信息支链上对应论文信息结点*/
        pSCHNode = hd;
        find = 0;
        while (pSCHNode != NULL && find == 0)
        {
            pDEPNode = pSCHNode->dnext;
            while (pDEPNode != NULL && find == 0)
            {
                if (strcmp(pDEPNode->name, pTHENode->depart) == 0) /*查找所在院系*/
                {
                    find = 1;
                    break;
                }
                pDEPNode = pDEPNode->next;
            }
            pSCHNode = pSCHNode->next;
        }
        if (find)  /*如果找到，则将结点以后进先出方式插入论文发表信息支链中*/
        {
            pTHENode->next = pDEPNode->tnext;
            pDEPNode->tnext = pTHENode;
        }
        else /*如果未找到，则释放所创建结点的内存空间*/
        {
            free(pTHENode);
        }
    }
    fclose(pFile);
    printf("%d",re);
    return re;
}

/**
 * 函数名称: InitInterface
 * 函数功能: 初始化界面.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void InitInterface()
{
    WORD att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
               | BACKGROUND_BLUE | BACKGROUND_GREEN;  /*白色前景和绿色背景*/

    SetConsoleTextAttribute(gh_std_out, att);  /*设置控制台屏幕缓冲区字符属性*/

    ClearScreen();  /* 清屏*/

    /*创建弹出窗口信息堆栈，将初始化后的屏幕窗口当作第一层弹出窗口*/
    gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*屏幕字符属性*/
    gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    gp_top_layer->LayerNo = 0;      /*弹出窗口的层号为0*/
    gp_top_layer->rcArea.Left = 0;  /*弹出窗口的区域为整个屏幕窗口*/
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL - 1;
    gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();     /*显示菜单栏*/
    ShowState();    /*显示状态栏*/

    return;
}

/**
 * 函数名称: ClearScreen
 * 函数功能: 清除屏幕信息.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void ClearScreen(void)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD home = {0, 0};
    unsigned long size;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );/*取屏幕缓冲区信息*/
    size =  bInfo.dwSize.X * bInfo.dwSize.Y; /*计算屏幕缓冲区字符单元数*/

    /*将屏幕缓冲区所有单元的字符属性设置为当前屏幕缓冲区字符属性*/
    FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &ul);

    /*将屏幕缓冲区所有单元填充为空格字符*/
    FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &ul);

    return;
}

/**
 * 函数名称: ShowMenu
 * 函数功能: 在屏幕上显示主菜单, 并设置热区, 在主菜单第一项上置选中标记.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void ShowMenu()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    CONSOLE_CURSOR_INFO lpCur;
    COORD size;
    COORD pos = {0, 0};
    int i, j;
    int PosA = 2, PosB;
    char ch;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    SetConsoleCursorPosition(gh_std_out, pos);
    for (i=0; i < 5; i++) /*在窗口第一行第一列处输出主菜单项*/
    {
        printf("  %s  ", ga_main_menu[i]);
    }

    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;
    SetConsoleCursorInfo(gh_std_out, &lpCur);  /*隐藏光标*/

    /*申请动态存储区作为存放菜单条屏幕区字符信息的缓冲区*/
    gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
    SMALL_RECT rcMenu ={0, 0, size.X-1, 0} ;

    /*将窗口第一行的内容读入到存放菜单条屏幕区字符信息的缓冲区中*/
    ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    /*将这一行中的英文字母置为红色，其他字符单元置为白底黑字*/
    for (i=0; i<size.X; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*修改后的菜单条字符信息回写到窗口的第一行*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
    COORD endPos = {0, 1};
    SetConsoleCursorPosition(gh_std_out, endPos);  /*将光标位置设置在第2行第1列*/

    /*将菜单项置为热区，热区编号为菜单项号，热区类型为0(按钮型)*/
    i = 0;
    do
    {
        PosB = PosA + strlen(ga_main_menu[i]);  /*定位第i+1号菜单项的起止位置*/
        for (j=PosA; j<PosB; j++)
        {
            gp_scr_att[j] |= (i+1) << 2; /*设置菜单项所在字符单元的属性值*/
        }
        PosA = PosB + 4;
        i++;
    } while (i<5);

    TagMainMenu(gi_sel_menu);  /*在选中主菜单项上做标记，gi_sel_menu初值为1*/

    return;
}

/**
 * 函数名称: ShowState
 * 函数功能: 显示状态条.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明: 状态条字符属性为白底黑字, 初始状态无状态信息.
 */
void ShowState()
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int i;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;
    SMALL_RECT rcMenu ={0, bInfo.dwSize.Y-1, size.X-1, bInfo.dwSize.Y-1};

    if (gp_buff_stateBar_info == NULL)
    {
        gp_buff_stateBar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
        ReadConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);
    }

    for (i=0; i<size.X; i++)
    {
        (gp_buff_stateBar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                                | BACKGROUND_RED;
/*
        ch = (char)((gp_buff_stateBar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_stateBar_info+i)->Attributes |= FOREGROUND_RED;
        }
*/
    }

    WriteConsoleOutput(gh_std_out, gp_buff_stateBar_info, size, pos, &rcMenu);

    return;
}

/**
 * 函数名称: TagMainMenu
 * 函数功能: 在指定主菜单项上置选中标志.
 * 输入参数: num 选中的主菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void TagMainMenu(int num)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int PosA = 1, PosB;
    char ch;
    int i;

    if (num == 0) /*num为0时，将会去除主菜单项选中标记*/
    {
        PosA = 0;
        PosB = 0;
    }
    else  /*否则，定位选中主菜单项的起止位置: PosA为起始位置, PosB为截止位置*/
    {
        for (i=1; i<num; i++)
        {
            PosA += strlen(ga_main_menu[i-1]) + 4;
        }
        PosB = PosA + strlen(ga_main_menu[num-1]) + 2;
    }

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );
    size.X = bInfo.dwSize.X;
    size.Y = 1;

    /*去除选中菜单项前面的菜单项选中标记*/
    for (i=0; i<PosA; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (gp_buff_menubar_info+i)->Char.AsciiChar;
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*在选中菜单项上做标记，蓝底白字*/
    for (i=PosA; i<PosB; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
                                               | FOREGROUND_RED | BACKGROUND_BLUE;
    }

    /*去除选中菜单项后面的菜单项选中标记*/
    for (i=PosB; i<bInfo.dwSize.X; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = BACKGROUND_BLUE | BACKGROUND_GREEN
                                               | BACKGROUND_RED;
        ch = (char)((gp_buff_menubar_info+i)->Char.AsciiChar);
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
        {
            (gp_buff_menubar_info+i)->Attributes |= FOREGROUND_RED;
        }
    }

    /*将做好标记的菜单条信息写到窗口第一行*/
    SMALL_RECT rcMenu ={0, 0, size.X-1, 0};
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * 函数名称: CloseSys
 * 函数功能: 关闭系统.
 * 输入参数: hd 主链头指针
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void CloseSys(SCH_NODE *hd)
{
    SCH_NODE *pSCHNode1 = hd, *pSCHNode2;
    DEP_NODE *pDEPNode1, *pDEPNode2;
    THE_NODE *pTHENode1, *pTHENode2;

    while (pSCHNode1 != NULL) /*释放十字交叉链表的动态存储区*/
    {
        pSCHNode2 = pSCHNode1->next;
        pDEPNode1 = pSCHNode1->dnext;
        while (pDEPNode1 != NULL) /*释放院系基本信息支链的动态存储区*/
        {
            pDEPNode2 = pDEPNode1->next;
            pTHENode1 = pDEPNode1->tnext;
            while (pTHENode1 != NULL) /*释放论文发表信息支链的动态存储区*/
            {
                pTHENode2 = pTHENode1->next;
                free(pTHENode1);
                pTHENode1 = pTHENode2;
            }
            free(pDEPNode1);
            pDEPNode1 = pDEPNode2;
        }
        free(pSCHNode1);  /*释放主链结点的动态存储区*/
        pSCHNode1 = pSCHNode2;
    }

    ClearScreen();        /*清屏*/

    /*释放存放菜单条、状态条、性别代码和学生类别代码等信息动态存储区*/
    free(gp_buff_menubar_info);
    free(gp_buff_stateBar_info);

    /*关闭标准输入和输出设备句柄*/
    CloseHandle(gh_std_out);
    CloseHandle(gh_std_in);

    /*将窗口标题栏置为运行结束*/
    SetConsoleTitle("运行结束");

    return;
}

/**
 * 函数名称: RunSys
 * 函数功能: 运行系统, 在系统主界面下运行用户所选择的功能模块.
 * 输入参数: 无
 * 输出参数: phead 主链头指针的地址
 * 返 回 值: 无
 *
 * 调用说明:
 */
void RunSys(SCH_NODE **phead)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:字符单元层号, cAtt:字符单元属性*/
    char vkc, asc;      /*vkc:虚拟键代码, asc:字符的ASCII码值*/

    while (bRet)
    {
        /*从控制台输入缓冲区中读一条记录*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == MOUSE_EVENT) /*如果记录由鼠标事件产生*/
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;  /*获取鼠标坐标位置*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*取该位置的层号*/
            cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*取该字符单元属性*/
            if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
            {
                /* cAtt > 0 表明该位置处于热区(主菜单项字符单元)
                 * cAtt != gi_sel_menu 表明该位置的主菜单项未被选中
                 * gp_top_layer->LayerNo > 0 表明当前有子菜单弹出
                 */
                if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0)
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                    PopMenu(cAtt);       /*弹出鼠标所在主菜单项对应的子菜单*/
                }
            }
            else if (cAtt > 0) /*鼠标所在位置为弹出子菜单的菜单项字符单元*/
            {
                TagSubMenu(cAtt); /*在该子菜单项上做选中标记*/
            }

            if (inRec.Event.MouseEvent.dwButtonState
                == FROM_LEFT_1ST_BUTTON_PRESSED) /*如果按下鼠标左边第一键*/
            {
                if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
                {
                    if (cAtt > 0) /*如果该位置处于热区(主菜单项字符单元)*/
                    {
                        PopMenu(cAtt);   /*弹出鼠标所在主菜单项对应的子菜单*/
                    }
                    /*如果该位置不属于主菜单项字符单元，且有子菜单弹出*/
                    else if (gp_top_layer->LayerNo > 0)
                    {
                        PopOff();            /*关闭弹出的子菜单*/
                        gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                    }
                }
                else /*层号不为0，表明该位置被弹出子菜单覆盖*/
                {
                    if (cAtt > 0) /*如果该位置处于热区(子菜单项字符单元)*/
                    {
                        PopOff(); /*关闭弹出的子菜单*/
                        gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/

                        /*执行对应功能函数:gi_sel_menu主菜单项号,cAtt子菜单项号*/
                        bRet = ExeFunction(gi_sel_menu, cAtt);
                    }
                }
            }
            else if (inRec.Event.MouseEvent.dwButtonState
                     == RIGHTMOST_BUTTON_PRESSED) /*如果按下鼠标右键*/
            {
                if (cNo == 0) /*层号为0，表明该位置未被弹出子菜单覆盖*/
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT  /*如果记录由按键产生*/
                 && inRec.Event.KeyEvent.bKeyDown) /*且键被按下*/
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*获取按键的虚拟键码*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*获取按键的ASC码*/

            /*系统快捷键的处理*/
            if (vkc == 112) /*如果按下F1键*/
            {
                if (gp_top_layer->LayerNo != 0) /*如果当前有子菜单弹出*/
                {
                    PopOff();            /*关闭弹出的子菜单*/
                    gi_sel_sub_menu = 0; /*将选中子菜单项的项号置为0*/
                }
                bRet = ExeFunction(5, 1);  /*运行帮助主题功能函数*/
            }
            else if (inRec.Event.KeyEvent.dwControlKeyState
                     & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
            { /*如果按下左或右Alt键*/
                switch (vkc)  /*判断组合键Alt+字母*/
                {
                    case 88:  /*Alt+X 退出*/
                        if (gp_top_layer->LayerNo != 0)
                        {
                            PopOff();
                            gi_sel_sub_menu = 0;
                        }
                        bRet = ExeFunction(1,4);
                        break;
                    case 70:  /*Alt+F*/
                        PopMenu(1);
                        break;
                    case 77: /*Alt+M*/
                        PopMenu(2);
                        break;
                    case 81: /*Alt+Q*/
                        PopMenu(3);
                        break;
                    case 83: /*Alt+S*/
                        PopMenu(4);
                        break;
                    case 72: /*Alt+H*/
                        PopMenu(5);
                        break;
                }
            }
            else if (asc == 0) /*其他控制键的处理*/
            {
                if (gp_top_layer->LayerNo == 0) /*如果未弹出子菜单*/
                {
                    switch (vkc) /*处理方向键(左、右、下)，不响应其他控制键*/
                    {
                        case 37:
                            gi_sel_menu--;
                            if (gi_sel_menu == 0)
                            {
                                gi_sel_menu = 5;
                            }
                            TagMainMenu(gi_sel_menu);
                            break;
                        case 39:
                            gi_sel_menu++;
                            if (gi_sel_menu == 6)
                            {
                                gi_sel_menu = 1;
                            }
                            TagMainMenu(gi_sel_menu);
                            break;
                        case 40:
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else  /*已弹出子菜单时*/
                {
                    for (loc=0,i=1; i<gi_sel_menu; i++)
                    {
                        loc += ga_sub_menu_count[i-1];
                    }  /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
                    switch (vkc) /*方向键(左、右、上、下)的处理*/
                    {
                        case 37:
                            gi_sel_menu--;
                            if (gi_sel_menu < 1)
                            {
                                gi_sel_menu = 5;
                            }
                            TagMainMenu(gi_sel_menu);
                            PopOff();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                        case 38:
                            num = gi_sel_sub_menu - 1;
                            if (num < 1)
                            {
                                num = ga_sub_menu_count[gi_sel_menu-1];
                            }
                            if (strlen(ga_sub_menu[loc+num-1]) == 0)
                            {
                                num--;
                            }
                            TagSubMenu(num);
                            break;
                        case 39:
                            gi_sel_menu++;
                            if (gi_sel_menu > 5)
                            {
                                gi_sel_menu = 1;
                            }
                            TagMainMenu(gi_sel_menu);
                            PopOff();
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                        case 40:
                            num = gi_sel_sub_menu + 1;
                            if (num > ga_sub_menu_count[gi_sel_menu-1])
                            {
                                num = 1;
                            }
                            if (strlen(ga_sub_menu[loc+num-1]) == 0)
                            {
                                num++;
                            }
                            TagSubMenu(num);
                            break;
                    }
                }
            }
            else if ((asc-vkc == 0) || (asc-vkc == 32)){  /*按下普通键*/
                if (gp_top_layer->LayerNo == 0)  /*如果未弹出子菜单*/
                {
                    switch (vkc)
                    {
                        case 70: /*f或F*/
                            PopMenu(1);
                            break;
                        case 77: /*m或M*/
                            PopMenu(2);
                            break;
                        case 81: /*q或Q*/
                            PopMenu(3);
                            break;
                        case 83: /*s或S*/
                            PopMenu(4);
                            break;
                        case 72: /*h或H*/
                            PopMenu(5);
                            break;
                        case 13: /*回车*/
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else /*已弹出子菜单时的键盘输入处理*/
                {
                    if (vkc == 27) /*如果按下ESC键*/
                    {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }
                    else if(vkc == 13) /*如果按下回车键*/
                    {
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        bRet = ExeFunction(gi_sel_menu, num);
                    }
                    else /*其他普通键的处理*/
                    {
                        /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
                        for (loc=0,i=1; i<gi_sel_menu; i++)
                        {
                            loc += ga_sub_menu_count[i-1];
                        }

                        /*依次与当前子菜单中每一项的代表字符进行比较*/
                        for (i=loc; i<loc+ga_sub_menu_count[gi_sel_menu-1]; i++)
                        {
                            if (strlen(ga_sub_menu[i])>0 && vkc==ga_sub_menu[i][1])
                            { /*如果匹配成功*/
                                PopOff();
                                gi_sel_sub_menu = 0;
                                bRet = ExeFunction(gi_sel_menu, i-loc+1);
                            }
                        }
                    }
                }
            }
        }
    }
}

void PopPrompt(char **pCh)
{
    InitInterface();
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    int maxlen,str_len,i;
    int iHot = 1;

    for (i=0,maxlen=0; i<2; i++) {
        str_len = strlen(pCh[i]);
        if (maxlen < str_len) {
            maxlen = str_len;
        }
    }

    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1; /*窗口区域*/

    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[2];

    for (i=0;i<2;i++){
        aLoc[i].X = rcPop.Left + 3;
        aLoc[i].Y = rcPop.Top +2 + i;
    }

    str_len = strlen(pCh[1]);
    aLoc[1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[1].Y = aLoc[1].Y + 2;
    labels.pLoc = aLoc;
    areas.num = 1;
    SMALL_RECT aArea[]= {{aLoc[1].X, aLoc[1].Y,
                           aLoc[1].X + 3, aLoc[1].Y}};

    char aSort[] = {0};
    char aTag[] = {1};

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;

    PopUp(&rcPop, att, &labels, &areas);
    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 + 2;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    DealInput(&areas,&iHot);
    PopOff();
}

/**
 * 函数名称: PopMenu
 * 函数功能: 弹出指定主菜单项对应的子菜单.
 * 输入参数: num 指定的主菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopMenu(int num)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh;
    int i, j, loc = 0;

    if (num != gi_sel_menu)       /*如果指定主菜单不是已选中菜单*/
    {
        if (gp_top_layer->LayerNo != 0) /*如果此前已有子菜单弹出*/
        {
            PopOff();
            gi_sel_sub_menu = 0;
        }
    }
    else if (gp_top_layer->LayerNo != 0) /*若已弹出该子菜单，则返回*/
    {
        return;
    }

    gi_sel_menu = num;    /*将选中主菜单项置为指定的主菜单项*/
    TagMainMenu(gi_sel_menu); /*在选中的主菜单项上做标记*/
    LocSubMenu(gi_sel_menu, &rcPop); /*计算弹出子菜单的区域位置, 存放在rcPop中*/

    /*计算该子菜单中的第一项在子菜单字符串数组中的位置(下标)*/
    for (i=1; i<gi_sel_menu; i++)
    {
        loc += ga_sub_menu_count[i-1];
    }
    /*将该组子菜单项项名存入标签束结构变量*/
    labels.ppLabel = ga_sub_menu + loc;   /*标签束第一个标签字符串的地址*/
    labels.num = ga_sub_menu_count[gi_sel_menu-1]; /*标签束中标签字符串的个数*/
    COORD aLoc[labels.num];/*定义一个坐标数组，存放每个标签字符串输出位置的坐标*/
    for (i=0; i<labels.num; i++) /*确定标签字符串的输出位置，存放在坐标数组中*/
    {
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*使标签束结构变量labels的成员pLoc指向坐标数组的首元素*/
    /*设置热区信息*/
    areas.num = labels.num;       /*热区的个数，等于标签的个数，即子菜单的项数*/
    SMALL_RECT aArea[areas.num];                    /*定义数组存放所有热区位置*/
    char aSort[areas.num];                      /*定义数组存放所有热区对应类别*/
    char aTag[areas.num];                         /*定义数组存放每个热区的编号*/
    for (i=0; i<areas.num; i++)
    {
        aArea[i].Left = rcPop.Left + 2;  /*热区定位*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*热区类别都为0(按钮型)*/
        aTag[i] = i + 1;           /*热区按顺序编号*/
    }
    areas.pArea = aArea;/*使热区结构变量areas的成员pArea指向热区位置数组首元素*/
    areas.pSort = aSort;/*使热区结构变量areas的成员pSort指向热区类别数组首元素*/
    areas.pTag = aTag;   /*使热区结构变量areas的成员pTag指向热区编号数组首元素*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*给弹出窗口画边框*/
    pos.X = rcPop.Left + 2;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    { /*此循环用来在空串子菜项位置画线形成分隔，并取消此菜单项的热区属性*/
        pCh = ga_sub_menu[loc+pos.Y-rcPop.Top-1];
        if (strlen(pCh)==0) /*串长为0，表明为空串*/
        {   /*首先画横线*/
            FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-3, pos, &ul);
            for (j=rcPop.Left+2; j<rcPop.Right-1; j++)
            {   /*取消该区域字符单元的热区属性*/
                gp_scr_att[pos.Y*SCR_COL+j] &= 3; /*按位与的结果保留了低两位*/
            }
        }

    }
    /*将子菜单项的功能键设为白底红字*/
    pos.X = rcPop.Left + 3;
    att =  FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        if (strlen(ga_sub_menu[loc+pos.Y-rcPop.Top-1])==0)
        {
            continue;  /*跳过空串*/
        }
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    return;
}

/**
 * 函数名称: PopUp
 * 函数功能: 在指定区域输出弹出窗口信息, 同时设置热区, 将弹出窗口位置信息入栈.
 * 输入参数: pRc 弹出窗口位置数据存放的地址
 *           att 弹出窗口区域字符属性
 *           pLabel 弹出窗口中标签束信息存放的地址
             pHotArea 弹出窗口中热区信息存放的地址
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*弹出窗口所在位置字符单元信息入栈*/
    size.X = pRc->Right - pRc->Left + 1;    /*弹出窗口的宽度*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*弹出窗口的高度*/
    /*申请存放弹出窗口相关信息的动态存储区*/
    nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*将弹出窗口覆盖区域的字符信息保存，用于在关闭弹出窗口时恢复原样*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*此二重循环将所覆盖字符单元的原先属性值存入动态存储区，便于以后恢复*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }
    gp_top_layer = nextLayer;  /*完成弹出窗口相关信息入栈操作*/
    /*设置弹出窗口区域字符的新属性*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
        pos.Y++;
    }
    /*将标签束中的标签字符串在设定的位置输出*/
    for (i=0; i<pLabel->num; i++)
    {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0)
        {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
                                        pLabel->pLoc[i], &ul);
        }
    }
    /*设置弹出窗口区域字符单元的新属性*/
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*此二重循环设置字符单元的层号*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNo;
        }
    }

    for (i=0; i<pHotArea->num; i++)
    {   /*此二重循环设置所有热区中字符单元的热区类型和热区编号*/
        row = pHotArea->pArea[i].Top;
        for (j=pHotArea->pArea[i].Left; j<=pHotArea->pArea[i].Right; j++)
        {
            gp_scr_att[row*SCR_COL+j] |= (pHotArea->pSort[i] << 6)
                                    | (pHotArea->pTag[i] << 2);
        }
    }
    return;
}

/**
 * 函数名称: PopOff
 * 函数功能: 关闭顶层弹出窗口, 恢复覆盖区域原外观和字符单元原属性.
 * 输入参数: 无
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void PopOff(void)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j;

    if ((gp_top_layer->next==NULL) || (gp_top_layer->pContent==NULL))
    {   /*栈底存放的主界面屏幕信息，不用关闭*/
        return;
    }
    nextLayer = gp_top_layer->next;
    /*恢复弹出窗口区域原外观*/
    size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
    size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
    /*恢复字符单元原属性*/
    pCh = gp_top_layer->pScrAtt;
    for (i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++)
    {
        for (j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }
    free(gp_top_layer->pContent);    /*释放动态存储区*/
    free(gp_top_layer->pScrAtt);
    free(gp_top_layer);
    gp_top_layer = nextLayer;
    gi_sel_sub_menu = 0;
    return;
}

/**
 * 函数名称: DrawBox
 * 函数功能: 在指定区域画边框.
 * 输入参数: pRc 存放区域位置信息的地址
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void DrawBox(SMALL_RECT *pRc)
{
    char chBox[] = {'+','-',' '};  /*画框用的字符*/
    COORD pos = {pRc->Left, pRc->Top};  /*定位在区域的左上角*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框左上角*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {   /*此循环画上边框横线*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框右上角*/
    for (pos.Y = pRc->Top+1; pos.Y < pRc->Bottom; pos.Y++)
    {   /*此循环画边框左边线和右边线*/
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
    }
    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框左下角*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {   /*画下边框横线*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*画边框右下角*/
    return;
}

/**
 * 函数名称: TagSubMenu
 * 函数功能: 在指定子菜单项上做选中标记.
 * 输入参数: num 选中的子菜单项号
 * 输出参数: 无
 * 返 回 值: 无
 *
 * 调用说明:
 */
void TagSubMenu(int num)
{
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    LocSubMenu(gi_sel_menu, &rcPop);  /*计算弹出子菜单的区域位置, 存放在rcPop中*/
    if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1))
    {   /*如果子菜单项号越界，或该项子菜单已被选中，则返回*/
        return;
    }

    pos.X = rcPop.Left + 2;
    width = rcPop.Right - rcPop.Left - 3;
    if (gi_sel_sub_menu != 0) /*首先取消原选中子菜单项上的标记*/
    {
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
        pos.X += 1;
        att |=  FOREGROUND_RED;/*白底红字*/
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    /*在制定子菜单项上做选中标记*/
    pos.X = rcPop.Left + 2;
    pos.Y = rcPop.Top + num;
    att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | BACKGROUND_BLUE;  /*蓝底白字*/
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    gi_sel_sub_menu = num;  /*修改选中子菜单项号*/
    return;
}

/**
 * 函数名称: LocSubMenu
 * 函数功能: 计算弹出子菜单区域左上角和右下角的位置.
 * 输入参数: num 选中的主菜单项号
 * 输出参数: rc 存放区域位置信息的地址
 * 返 回 值: 无
 *
 * 调用说明:
 */
void LocSubMenu(int num, SMALL_RECT *rc)
{
    int i, len, loc = 0;

    rc->Top = 1; /*区域的上边定在第2行，行号为1*/
    rc->Left = 1;
    for (i=1; i<num; i++)
    {   /*计算区域左边界位置, 同时计算第一个子菜单项在子菜单字符串数组中的位置*/
        rc->Left += strlen(ga_main_menu[i-1]) + 4;
        loc += ga_sub_menu_count[i-1];
    }
    rc->Right = strlen(ga_sub_menu[loc]);/*暂时存放第一个子菜单项字符串长度*/
    for (i=1; i<ga_sub_menu_count[num-1]; i++)
    {   /*查找最长子菜单字符串，将其长度存放在rc->Right*/
        len = strlen(ga_sub_menu[loc+i]);
        if (rc->Right < len)
        {
            rc->Right = len;
        }
    }
    rc->Right += rc->Left + 3;  /*计算区域的右边界*/
    rc->Bottom = rc->Top + ga_sub_menu_count[num-1] + 1;/*计算区域下边的行号*/
    if (rc->Right >= SCR_COL)  /*右边界越界的处理*/
    {
        len = rc->Right - SCR_COL + 1;
        rc->Left -= len;
        rc->Right = SCR_COL - 1;
    }
    return;
}

/**
 * 函数名称: DealInput
 * 函数功能: 在弹出窗口区域设置热区, 等待并相应用户输入.
 * 输入参数: pHotArea
 *           piHot 焦点热区编号的存放地址, 即指向焦点热区编号的指针
 * 输出参数: piHot 用鼠标单击、按回车或空格时返回当前热区编号
 * 返 回 值:
 *
 * 调用说明:
 */
int DealInput(HOT_AREA *pHotArea, int *piHot)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    int num, arrow, iRet = 0;
    int cNo, cTag, cSort;/*cNo:层号, cTag:热区编号, cSort: 热区类型*/
    char vkc, asc;       /*vkc:虚拟键代码, asc:字符的ASCII码值*/

    SetHotPoint(pHotArea, *piHot);
    while (TRUE)
    {    /*循环*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);
        if ((inRec.EventType == MOUSE_EVENT) &&
            (inRec.Event.MouseEvent.dwButtonState
             == FROM_LEFT_1ST_BUTTON_PRESSED))
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3;
            cTag = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2) & 15;
            cSort = (gp_scr_att[pos.Y * SCR_COL + pos.X] >> 6) & 3;

            if ((cNo == gp_top_layer->LayerNo) && cTag > 0)
            {
                *piHot = cTag;
                SetHotPoint(pHotArea, *piHot);
                if (cSort == 0)
                {
                    iRet = 13;
                    break;
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT && inRec.Event.KeyEvent.bKeyDown)
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode;
            asc = inRec.Event.KeyEvent.uChar.AsciiChar;;
            if (asc == 0)
            {
                arrow = 0;
                switch (vkc)
                {  /*方向键(左、上、右、下)的处理*/
                    case 37: arrow = 1; break;
                    case 38: arrow = 2; break;
                    case 39: arrow = 3; break;
                    case 40: arrow = 4; break;
                }
                if (arrow > 0)
                {
                    num = *piHot;
                    while (TRUE)
                    {
                        if (arrow < 3)
                        {
                            num--;
                        }
                        else
                        {
                            num++;
                        }
                        if ((num < 1) || (num > pHotArea->num) ||
                            ((arrow % 2) && (pHotArea->pArea[num-1].Top
                            == pHotArea->pArea[*piHot-1].Top)) || ((!(arrow % 2))
                            && (pHotArea->pArea[num-1].Top
                            != pHotArea->pArea[*piHot-1].Top)))
                        {
                            break;
                        }
                    }
                    if (num > 0 && num <= pHotArea->num)
                    {
                        *piHot = num;
                        SetHotPoint(pHotArea, *piHot);
                    }
                }
            }
            else if (vkc == 27)
            {  /*ESC键*/
                iRet = 27;
                break;
            }
            else if (vkc == 13 || vkc == 32)
            {  /*回车键或空格表示按下当前按钮*/
                iRet = 13;
                break;
            }
        }
    }
    return iRet;
}

void SetHotPoint(HOT_AREA *pHotArea, int iHot)
{
    CONSOLE_CURSOR_INFO lpCur;
    COORD pos = {0, 0};
    WORD att1, att2;
    int i, width;

    att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*黑底白字*/
    att2 = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    for (i=0; i<pHotArea->num; i++)
    {  /*将按钮类热区置为白底黑字*/
        pos.X = pHotArea->pArea[i].Left;
        pos.Y = pHotArea->pArea[i].Top;
        width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
        if (pHotArea->pSort[i] == 0)
        {  /*热区是按钮类*/
            FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
        }
    }

    pos.X = pHotArea->pArea[iHot-1].Left;
    pos.Y = pHotArea->pArea[iHot-1].Top;
    width = pHotArea->pArea[iHot-1].Right - pHotArea->pArea[iHot-1].Left + 1;
    if (pHotArea->pSort[iHot-1] == 0)
    {  /*被激活热区是按钮类*/
        FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
    }
    else if (pHotArea->pSort[iHot-1] == 1)
    {  /*被激活热区是文本框类*/
        SetConsoleCursorPosition(gh_std_out, pos);
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);
    }
}

/**
 * 函数名称: ExeFunction
 * 函数功能: 执行由主菜单号和子菜单号确定的功能函数.
 * 输入参数: m 主菜单项号
 *           s 子菜单项号
 * 输出参数: 无
 * 返 回 值: BOOL类型, TRUE 或 FALSE
 *
 * 调用说明: 仅在执行函数ExitSys时, 才可能返回FALSE, 其他情况下总是返回TRUE
 */
BOOL ExeFunction(int m, int s)
{
    BOOL bRet = TRUE;
    /*函数指针数组，用来存放所有功能函数的入口地址*/
    BOOL (*pFunction[ga_sub_menu_count[0]+ga_sub_menu_count[1]+ga_sub_menu_count[2]+ga_sub_menu_count[3]+ga_sub_menu_count[4]])(void);
    int i, loc;

    /*将功能函数入口地址存入与功能函数所在主菜单号和子菜单号对应下标的数组元素*/
    pFunction[0] = SaveData;
    pFunction[1] = BackupData;
    pFunction[2] = RestoreData;
    pFunction[3] = ExitSys;
    pFunction[4] = MaintainSchInfo;
    pFunction[5] = MaintainDepInfo;
    pFunction[6] = MaintainTheInfo;
    pFunction[7] = QuerySchInfo;
    pFunction[8] = QueryDepInfo;
    pFunction[9] = QueryTheInfo;
    pFunction[10] = Stat1;
    pFunction[11] = Stat2;
    pFunction[12] = Stat3;
    pFunction[13] = Stat4;
    pFunction[14] = Stat5;
    pFunction[15] = HelpTopic;
    pFunction[16] = About;

    for (i=1,loc=0; i<m; i++)  /*根据主菜单号和子菜单号计算对应下标*/
    {
        loc += ga_sub_menu_count[i-1];
    }
    loc += s - 1;

    if (pFunction[loc] != NULL)
    {
        bRet = (*pFunction[loc])();  /*用函数指针调用所指向的功能函数*/
    }

    return bRet;
}


BOOL SaveData()
{
    BOOL bRet = TRUE;
    SaveSysData(gp_head);
    return bRet;
}

BOOL SaveSysData(SCH_NODE *hd)
{
    BOOL bRet = TRUE;
    SCH_NODE *psch_node;
    DEP_NODE *pdep_node;
    THE_NODE *pthe_node;
    FILE *pfout;

    pfout=fopen(gp_sch_info_filename,"wb");
    for(psch_node=hd;psch_node!=NULL;psch_node=psch_node->next)/*保存学校基本信息*/
    {
        fwrite(psch_node,sizeof(SCH_NODE),1,pfout);
    }
    fclose(pfout);

    pfout=fopen(gp_department_info_filename,"wb");
    for(psch_node=hd;psch_node!=NULL;psch_node=psch_node->next)/*保存院系基本信息*/
    {
        pdep_node=psch_node->dnext;
        while(pdep_node!=NULL){
            fwrite(pdep_node,sizeof(DEP_NODE),1,pfout);
            pdep_node=pdep_node->next;
        }
    }
    fclose(pfout);

    pfout=fopen(gp_thesis_info_filename,"wb");
    for(psch_node=hd;psch_node!=NULL;psch_node=psch_node->next)/*保存论文发表基本信息*/
    {
        pdep_node=psch_node->dnext;
        while(pdep_node!=NULL){
                pthe_node=pdep_node->tnext;
                while(pthe_node!=NULL){
                fwrite(pthe_node,sizeof(THE_NODE),1,pfout);
                pthe_node=pthe_node->next;
          }
         pdep_node=pdep_node->next;
       }
    }
    fclose(pfout);

    char *pCh[]={"数据保存成功！","确认"};
    PopPrompt(pCh);
    return bRet;
}

BOOL BackupData(void)
{
    BOOL bRet = TRUE;
    char s[30];
    printf("请输入文件名称：  例如8.26.bck\n");
    scanf("%s",s);
    FILE *f;
    f=fopen(s,"w+");
    fclose(f);
    BackupSysData(gp_head,s);
    return bRet;
}

BOOL BackupSysData(SCH_NODE *phd, char *filename)
{
    SCH_NODE *psch_node;
    DEP_NODE *pdep_node;
    THE_NODE *pthe_node;
    unsigned long sch_node_num = 0;
    unsigned long dep_node_num = 0;
    unsigned long the_node_num = 0;
    int handle;

    for(psch_node=phd;psch_node!=NULL;psch_node=psch_node->next)  /*检索各链表节点数*/
    {
        sch_node_num++;
        pdep_node=psch_node->dnext;
        while(pdep_node!=NULL){
            dep_node_num++;
            pthe_node=pdep_node->tnext;
            while(pthe_node!=NULL){
                the_node_num++;
                pthe_node=pthe_node->next;
            }
            pdep_node=pdep_node->next;
        }
    }

    handle=open(filename,O_WRONLY|O_BINARY,S_IWRITE);

   /*保存三类链表总数*/
    write(handle,&sch_node_num,sizeof(sch_node_num));
    write(handle,&dep_node_num,sizeof(dep_node_num));
    write(handle,&the_node_num,sizeof(the_node_num));

    for(psch_node=phd;psch_node!=NULL;psch_node=psch_node->next){
        write(handle,(char *)psch_node,sizeof(*psch_node));
    }

    for(psch_node=phd;psch_node!=NULL;psch_node=psch_node->next){
        pdep_node=psch_node->dnext;
        while(pdep_node!=NULL){
            write(handle,(char *)pdep_node,sizeof(*pdep_node));
            pdep_node=pdep_node->next;
        }
    }

    for(psch_node=phd;psch_node!=NULL;psch_node=psch_node->next){
        pdep_node=psch_node->dnext;
        while(pdep_node!=NULL){
            pthe_node=pdep_node->tnext;
            while(pthe_node!=NULL){
                write(handle,(char *)pthe_node,sizeof(*pthe_node));
                pthe_node=pthe_node->next;
            }
            pdep_node=pdep_node->next;
        }
    }

    close(handle);

    char *pCh[]={"数据已经备份至程序目录下！","确认"};
    PopPrompt(pCh);

    return TRUE;
}

BOOL RestoreData(void)
{


    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"此操作将覆盖当前未保存的信息，继续吗？", "继续    取消"};
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
                    {rcPop.Left+5, rcPop.Top+5}};
    labels.pLoc = aLoc;

    areas.num = 2;
    SMALL_RECT aArea[] = {{rcPop.Left + 5, rcPop.Top + 5,
                           rcPop.Left + 8, rcPop.Top + 5},
                          {rcPop.Left + 13, rcPop.Top + 5,
                           rcPop.Left + 16, rcPop.Top + 5}};
    char aSort[] = {0, 0};
    char aTag[] = {1, 2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    if (DealInput(&areas, &iHot) == 13 && iHot == 1)
    {
        PopOff();
        char filename[20];
        printf("请输入需要恢复的文件名称：\n");
        scanf("%s",filename);
        if(fopen(filename,"r")==0) {
            char *pCh[]={"输入文件名有误，打开失败！","返回"};
            PopPrompt(pCh);
        }
        else RestoreSysData(&gp_head,filename);
    }

    InitInterface();

    return bRet;
}

BOOL RestoreSysData(SCH_NODE **pphd, char *filename)
{
    SCH_NODE *hd=NULL;
    SCH_NODE *psch_node;
    DEP_NODE *pdep_node;
    THE_NODE *pthe_node;
    unsigned long sch_node_num = 0;
    unsigned long dep_node_num = 0;
    unsigned long the_node_num = 0;
    unsigned long ulloop;
    int handle;
    int find;

    if((handle=open(filename,O_RDONLY|O_BINARY))==-1){
        handle=open(filename,O_CREAT|O_BINARY,S_IREAD);
    }

    read(handle,(char *)&sch_node_num,sizeof(sch_node_num));
    read(handle,(char *)&dep_node_num,sizeof(dep_node_num));
    read(handle,(char *)&the_node_num,sizeof(the_node_num));

    for(ulloop=1;ulloop<=sch_node_num;ulloop++){
        psch_node=(SCH_NODE *)malloc(sizeof(SCH_NODE));
        read(handle,(char *)psch_node,sizeof(SCH_NODE));
        psch_node->dnext=NULL;
        psch_node->next=hd;
        hd=psch_node;
    }
    *pphd=hd;

    for(ulloop=1;ulloop<=dep_node_num;ulloop++)
    {
        pdep_node=(DEP_NODE *)malloc(sizeof(DEP_NODE));
        read(handle,(char *)pdep_node,sizeof(DEP_NODE));
        pdep_node->tnext=NULL;
        psch_node=hd;
        while(psch_node!=NULL&&strcmp(psch_node->sch_id,pdep_node->sch_id)!=0){
            psch_node=psch_node->next;
        }

        if(psch_node!=NULL){
            pdep_node->next=psch_node->dnext;
            psch_node->dnext=pdep_node;
        }
        else{free(pdep_node);}
    }

    for(ulloop=1;ulloop<=the_node_num;ulloop++){
        pthe_node=(THE_NODE *)malloc(sizeof(THE_NODE));
        read(handle,(char *)pthe_node,sizeof(THE_NODE));
        psch_node=hd;
        find=0;

        while(psch_node!=NULL&&find==0){
            pdep_node=psch_node->dnext;
            while(pdep_node!=NULL&&find==0){
                if(strcmp(pdep_node->sch_id,pthe_node->sch_id)==0)
                {
                    find=1;
                    break;
                }
                pdep_node=pdep_node->next;
            }
            psch_node=psch_node->next;
        }

        if(find){
            pthe_node->next=pdep_node->tnext;
            pdep_node->tnext=pthe_node;
        }
        else{
            free(pthe_node);
        }
    }

    close(handle);
    SaveSysData(hd);
    return TRUE;
}

BOOL ExitSys(void)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    char *pCh[] = {"确认退出系统吗？", "确定    取消"};
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = 2;
    labels.ppLabel = pCh;
    COORD aLoc[] = {{rcPop.Left+3, rcPop.Top+2},
                    {rcPop.Left+5, rcPop.Top+5}};
    labels.pLoc = aLoc;

    areas.num = 2;
    SMALL_RECT aArea[] = {{rcPop.Left + 5, rcPop.Top + 5,
                           rcPop.Left + 8, rcPop.Top + 5},
                          {rcPop.Left + 13, rcPop.Top + 5,
                           rcPop.Left + 16, rcPop.Top + 5}};
    char aSort[] = {0, 0};
    char aTag[] = {1, 2};
    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 4;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    if (DealInput(&areas, &iHot) == 13 && iHot == 1)
    {
        bRet = FALSE;
    }
    else
    {
        bRet = TRUE;
    }
    PopOff();

    return bRet;
}


BOOL MaintainSchInfo(void)
{
    BOOL bRet = TRUE;
    printf("请输入功能序号:\n");
    printf("1.添加高校基本信息\n");
    printf("2.修改高校基本信息\n");
    printf("3.删除高校基本信息\n");
    printf("4.返回\n\n");
    int num;
    scanf("%d",&num);
    switch(num){
        case 1 : schinfo1(&gp_head);break;
        case 2 : schinfo2(gp_head);break;
        case 3 : schinfo3(&gp_head);break;
        case 4 : break;
        default : {char*pCh[]={"输入有误！","返回"};PopPrompt(pCh);}
    }
    InitInterface();
    return bRet;
}

void schinfo1(SCH_NODE **phd) /*用于添加高校信息*/
{
    SCH_NODE *p1,*p=*phd;
    int num;
    printf("请输入添加高校的数目：");
    scanf("%d",&num);
    printf("请按照如下格式录入基本信息：\n");
    printf("学校编号|           学校名称          |    负责人    |   联系电话   |\n");
    while(num--){
        p1=(SCH_NODE *)malloc(sizeof(SCH_NODE));
        scanf("%s %s %s %s",p1->sch_id,p1->name,p1->manager,p1->phone);
        p1->dnext=NULL;
        p1->next=p;
        p=p1;
    }
    *phd=p;
    char *pCh[]={"添加成功！","返回"};PopPrompt(pCh);
}

void schinfo2(SCH_NODE *hd) /*用于修改高校信息*//*无需修改主链头位置，故使用*hd指针即可*/
{
    printf("请输入需要修改的高校编号：");
    SCH_NODE *p=hd;
    char s[8];
    scanf("%s",s);
    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
        printf("原信息如下：\n\n");
        printf("学校编号|                     学校名称|        负责人|      联系电话|\n");
        printf("%8s|%29s|%14s|%14s|\n",p->sch_id,p->name,p->manager,p->phone);
        printf("请重新录入该校基本信息：\n");
        scanf("%s %s %s %s",p->sch_id,p->name,p->manager,p->phone);
        char*pCh[]={"修改成功！","返回"};PopPrompt(pCh);
    }
    else {char*pCh[]={"查找失败！","返回"};PopPrompt(pCh);}

}

void schinfo3(SCH_NODE **phd)/*删除高校基本信息*/
{
    printf("请输入需要删除的高校编号：");
    char s[8];
    scanf("%s",s);
        SCH_NODE *pprior;
        SCH_NODE *pcurrent;
        DEP_NODE *p1prior;
        DEP_NODE *p1current;
        THE_NODE *p2prior;
        THE_NODE *p2current;

        for(pcurrent=*phd,pprior = NULL;pcurrent!=NULL;pprior=pcurrent,pcurrent=pcurrent->next){
            if(strcmp(pcurrent->sch_id,s)==0) break;
        }
        if(pcurrent!=NULL){
            if(pprior==NULL){  /*如果是链头*/
                *phd=pcurrent->next;
            }
            else{
                pprior->next=pcurrent->next;
            }
            p1current=pcurrent->dnext;
            free(pcurrent); /*释放该高校信息*/
            while(p1current!=NULL){
                p2current=p1current->tnext;
                p1prior=p1current;
                while(p2current!=NULL){
                    p2prior=p2current;
                    p2current=p2current->next;
                    free(p2prior);/*施放论文链信息*/
                }
                p1current=p1current->next;
                free(p1prior);/*释放院系链信息*/
            }
        char*pCh[]={"删除成功！","返回"};PopPrompt(pCh);
    }
    else {char*pCh[]={"查找失败！","返回"};PopPrompt(pCh);}

}

BOOL MaintainDepInfo(void)
{
    BOOL bRet = TRUE;
    printf("请输入功能序号:\n");
    printf("1.添加院系基本信息\n");
    printf("2.修改院系基本信息\n");
    printf("3.删除院系基本信息\n");
    printf("4.返回\n\n");
    int num;
    scanf("%d",&num);
    switch(num){
        case 1 : depinfo1(gp_head);break;
        case 2 : depinfo2(gp_head);break;
        case 3 : depinfo3(gp_head);break;
        case 4 : break;
        default : {char*pCh[]={"输入有误！","返回"};PopPrompt(pCh);}
    }
    InitInterface();
    return bRet;
}

void depinfo1(SCH_NODE *hd) /*添加院系信息*/
{
    printf("Process 1: 请输入院系所在学校的编号：");
    char s[8];
    int num;
    scanf("%s",s);
    SCH_NODE *p=hd;
    DEP_NODE *p1,*pt;
    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
        printf("高校信息如下：\n\n");
        printf("学校编号|                     学校名称|        负责人|      联系电话|\n");
        printf("%8s|%29s|%14s|%14s|\n",p->sch_id,p->name,p->manager,p->phone);
        printf("Process 2: 请输入添加院系的数目：");
        scanf("%d",&num);
        printf("Process Final: 请输入需要添加的院系信息：\n");
        printf("    院系名称|    教师人数|    教授人数|    副教授人数|    研究生人数|\n");
        while(num--){
            pt=(DEP_NODE *)malloc(sizeof(DEP_NODE));
            scanf("%s %d %d %d %d",pt->name,&pt->teacher,&pt->professor,&pt->aprofessor,&pt->grastu);
            strcpy(pt->sch_id,s);
            pt->tnext = NULL;
            p1=SeekdepNode1(p,pt->name);
            if(p1==NULL){
                    printf("-------OK!\n");
                    pt->next=p->dnext;
                    p->dnext=pt;
            }
            else{
                printf("-------FALSE!\n");
                free(pt);
            }
        }
        printf("\n请按任意键返回！");
        getch();
    }
    else {char*pCh[]={"查找失败！","返回"};PopPrompt(pCh);}
}

void depinfo2(SCH_NODE *hd) /*修改院系信息*/
{
    printf("Process 1: 请输入院系所在学校的编号：");
    char s[8];
    char name[20];
    scanf("%s",s);
    SCH_NODE *p=hd;
    DEP_NODE *p1;
    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
        printf("\nProcess 2: 请输入需要修改的院系名称：");
        scanf("%s",name);
        p1=SeekdepNode1(p,name);
        printf("原本院系信息如下：\n\n");
        printf("学校编号|          院系名称|  教师人数|  教授人数|    副教授人数|    研究生人数|");
        if(p1!=NULL){
             printf("%8s|%18s|%10d|%10d|%14d|%14d|\n",p1->sch_id,p1->name,p1->teacher,p1->professor,p1->aprofessor,p1->grastu);
             printf("请输入修改后的院系信息：\n");
             printf("          院系名称|  教师人数|  教授人数|    副教授人数|    研究生人数|\n");
             scanf("%s %d %d %d %d",p1->name,&p1->teacher,&p1->professor,&p1->aprofessor,&p1->grastu);
        }
        else{
            printf("无相关院系信息可以修改！\n");
        }
        printf("\n请按任意键返回！");
        getch();
    }
    else {char*pCh[]={"查找失败！","返回"};PopPrompt(pCh);}
}

void depinfo3(SCH_NODE *hd)/*删除院系信息*/
{
    printf("Process 1: 请输入院系所在学校的编号：");
    char s[8];
    char name[20];
    scanf("%s",s);
    SCH_NODE *p=hd;
    DEP_NODE *pdprior,*pdcurrent,*pt;
    THE_NODE *ptprior,*ptcurrent;

    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
        printf("\n原本院系信息如下：\n\n");
        printf("学校编号|          院系名称|  教师人数|  教授人数|    副教授人数|    研究生人数|");
        pt=p->dnext;
        while(pt!=NULL){
            printf("%8s|%18s|%10d|%10d|%14d|%14d|",pt->sch_id,pt->name,pt->teacher,pt->professor,pt->aprofessor,pt->grastu);
            pt=pt->next;
        }
        printf("\nProcess 2: 请输入需要删除的院系名称：");
        scanf("%s",name);
        getchar();

    /*查找删除节点*/
        pdcurrent=p->dnext;
        pdprior = NULL;
        while(pdcurrent!=NULL&&strcmp(pdcurrent->name,name)){
            pdprior=pdcurrent;
            pdcurrent=pdcurrent->next;
        }

        if(pdcurrent!=NULL){
             printf("确认删除？按Y确认\n");
             char c;
             scanf("%c",&c);
             if(c=='Y'){/*开始执行删除任务*/
                if(pdprior==NULL){/*若删除的是链头*/
                    p->dnext=pdcurrent->next;
                }
                else pdprior->next=pdcurrent->next;

                ptcurrent=pdcurrent->tnext;
                free(pdcurrent);/*释放院系信息链*/

                while(ptcurrent!=NULL){/*循环释放该院系链上的论文信息链*/
                    ptprior=ptcurrent;
                    ptcurrent=ptcurrent->next;
                    free(ptprior);
                }
                printf("---------OK!");
             }
             else printf("---------False");
        }
        else{
            printf("无相关院系信息可以删除！\n");
        }
        printf("\n请按任意键返回！");
        getch();
    }
    else {char*pCh[]={"查找失败！","返回"};PopPrompt(pCh);}
}

DEP_NODE *SeekdepNode1(SCH_NODE *hd,char *name)/*搜索指定高校节点中名称为name的院系信息*/
{
    SCH_NODE *psch_node=hd;
    DEP_NODE *pdep_node;
    int find = 0;

        pdep_node=psch_node->dnext;

        while(pdep_node!=NULL){
            if(strcmp(pdep_node->name,name)==0){
                find = 1;
                break;
            }
            pdep_node=pdep_node->next;
        }

    if(find){
        return pdep_node;
    }
    else{
        return NULL;
    }
}

int SeekdepNode2(SCH_NODE *hd,int num)/*列举所有高校节点中教师人数超过为num的院系信息并返回搜寻的数目*/
{
    SCH_NODE *psch_node=hd;
    DEP_NODE *pdep_node;
    int find = 0;

    for(psch_node=hd;psch_node!=NULL;psch_node=psch_node->next){
        pdep_node=psch_node->dnext;
        while(pdep_node!=NULL){
            if(pdep_node->teacher>num){
                find++;
                printf("%8s|%18s|%10d|%10d|%14d|%14d|",pdep_node->sch_id,pdep_node->name,pdep_node->teacher,pdep_node->professor,pdep_node->aprofessor,pdep_node->grastu);
            }
            pdep_node=pdep_node->next;
        }
    }

   return find;
}

BOOL MaintainTheInfo(void)
{
    BOOL bRet = TRUE;
    printf("请输入功能序号:\n");
    printf("1.添加论文发表信息\n");
    printf("2.修改论文发表信息\n");
    printf("3.删除论文发表信息\n");
    printf("4.返回\n\n");
    int num;
    scanf("%d",&num);
    switch(num){
        case 1 : theinfo1(gp_head);break;
        case 2 : theinfo2(gp_head);break;
        case 3 : theinfo3(gp_head);break;
        case 4 : break;
        default : {char*pCh[]={"输入有误！","返回"};PopPrompt(pCh);}
    }
    InitInterface();

    return bRet;
}

void theinfo1(SCH_NODE *hd)/*添加论文发表信息*/
{
    printf("Process 1: 请输入院系所在学校的编号：");
    char s[8];
    char name[20];
    scanf("%s",s);
    SCH_NODE *p=hd;
    DEP_NODE *p1;
    THE_NODE *p2,*pt;
    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
        printf("Process 2: 请输入院系名称：");
        scanf("%s",name);
        p1=SeekdepNode1(p,name);
        if(p1!=NULL){
             printf("Process 3: 请输入添加论文发表信息数目：");
             int n;
             scanf("%d",&n);
             printf("Process Final: 请输入论文发表信息：\n");
             printf("         论文题目          |   第一作者   |        刊名        |   期次   |分类|");
             while(n--){
                pt=(THE_NODE *)malloc(sizeof(THE_NODE));
                scanf("%s %s %s %s %c",pt->title,pt->writer,pt->ISSN,pt->date,&pt->Rank);
                strcpy(pt->sch_id,s);
                strcpy(pt->depart,name);
                p2=SeektheNode1(p1,pt->title); /*检查是否有重名论文发表信息*/
                if(p2==NULL){
                    printf("-------OK!\n");
                    pt->next=p1->tnext;
                    p1->tnext=pt;
                }
                else{
                printf("-------FALSE!\n");
                free(pt);
                }
             }
        }
        else{
            printf("无相关院系信息！\n");
        }
        printf("\n请按任意键返回！");
        getch();
    }
    else {char*pCh[]={"查找失败！","返回"};PopPrompt(pCh);}

}

void theinfo2(SCH_NODE *hd) /*修改指定论文发表信息*/
{
    printf("Process 1: 请输入院系所在学校的编号：");
    char s[8];
    char name[20];
    scanf("%s",s);
    SCH_NODE *p=hd;
    DEP_NODE *p1;
    THE_NODE *p2;
    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
        printf("Process 2: 请输入院系名称：");
        scanf("%s",name);
        p1=SeekdepNode1(p,name);
        if(p1!=NULL){
             printf("Process 3: 请输入需要修改的论文发表信息题目：");
             char title[50];
             scanf("%s",title);
             p2=SeektheNode1(p1,title);/*搜索论文信息*/
             if(p2!=NULL){
                printf("原信息如下：\n");
                printf("         论文题目          |   第一作者   |        刊名        |   期次   |分类|");
                printf("%27s|%14s|%20s|%10s|   %c|",p2->title,p2->writer,p2->ISSN,p2->date,p2->Rank);
                printf("Process Final: 请输入新的论文发表信息：\n");
                printf("         论文题目          |   第一作者   |        刊名        |   期次   |分类|");
                scanf("%s %s %s %s %c",p2->title,p2->writer,p2->ISSN,p2->date,&p2->Rank);
                strcpy(p2->sch_id,s);
                strcpy(p2->depart,name);
                printf("------OK!\n");
             }
             else printf("查找论文信息失败！\n");
          }
        else{
            printf("无相关院系信息！\n");
        }
        printf("\n请按任意键返回！");
        getch();
    }
    else {char*pCh[]={"查找失败！","返回"};PopPrompt(pCh);}
}

void theinfo3(SCH_NODE *hd)/*删除指定论文信息链*/
{
    printf("Process 1: 请输入院系所在学校的编号：");
    char s[8];
    char name[20];
    scanf("%s",s);
    SCH_NODE *p=hd;
    DEP_NODE *p1;
    THE_NODE *ptcurrent,*ptprior = NULL;
    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
        printf("Process 2: 请输入院系名称：");
        scanf("%s",name);
        p1=SeekdepNode1(p,name);
        if(p1!=NULL){
             printf("Process Final: 请输入需要删除的论文发表信息题目：");
             char title[50];
             scanf("%s",title);
             ptcurrent=p1->tnext;
             while(ptcurrent!=NULL&&strcmp(ptcurrent->title,title)!=0){
                ptprior=ptcurrent;
                ptcurrent=ptcurrent->next;
             }
             if(ptcurrent!=NULL){
                if(ptprior==NULL){/*如果是链头*/
                    p1->tnext=ptcurrent->next;
                }
                else{
                    ptprior->next=ptcurrent->next;
                }
                free(ptcurrent);
                printf("------OK!\n");
             }
             else printf("查找论文信息失败！\n");
          }
        else{
            printf("无相关院系信息！\n");
        }
        printf("\n请按任意键返回！");
        getch();
    }
    else {char*pCh[]={"查找失败！","返回"};PopPrompt(pCh);}
}

THE_NODE *SeektheNode1(DEP_NODE *pd,char *title)/*搜索指定院系节点中论文题目为title的论文信息*/
{
    THE_NODE *pt;
    pt=pd->tnext;
    int find = 0;
    while(pt!=NULL){
        if(strcmp(pt->title,title)==0){
            find = 1;
            break;
        }
        pt=pt->next;
    }

    if(find){
        return pt;
    }

    else {
        return NULL;
    }
}

BOOL QuerySchInfo(void)
{
    printf("请输入查询方式:\n");
    printf("1.按高校编号查询\n");
    printf("2.按高校名称查询\n");
    printf("3.显示所有高校信息\n");
    printf("4.返回\n");
    int num;
    scanf("%d",&num);
    switch(num){
        case 1 : schquery1(gp_head);break;
        case 2 : schquery2(gp_head);break;
        case 3 : schquery3(gp_head);break;
        case 4 : break;
        default : {char*pCh[]={"输入有误！","返回"};PopPrompt(pCh);}
    }
    InitInterface();
    return TRUE;
}

void schquery1(SCH_NODE *hd)/*按高校编号*/
{
    printf("请输入需要查询的高校编号：");
    SCH_NODE *p=hd;
    char s[8];
    scanf("%s",s);

    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
        printf("查询结果如下:\n\n");
        printf("学校编号|                     学校名称|        负责人|      联系电话|\n");
        printf("%8s|%29s|%14s|%14s|\n",p->sch_id,p->name,p->manager,p->phone);
        printf("\n请按任意键返回！");
        getch();
    }
    else {char*pCh[]={"查找失败！","返回"};PopPrompt(pCh);}

}

void schquery2(SCH_NODE *hd)  /*模糊查询功能*/
{
    printf("请输入需要查询的高校名称关键字：");
    SCH_NODE *p=hd;
    char Keywords[30];
    scanf("%s",Keywords);
    int flag = 0;
    printf("查询结果如下:\n\n");
    printf("学校编号|                     学校名称|        负责人|      联系电话|\n");
    while(p!=NULL){
        if(strstr(p->name,Keywords)){
            printf("%8s|%29s|%14s|%14s|\n",p->sch_id,p->name,p->manager,p->phone);
            flag = 1;
        }
        p=p->next;
    }
    if(flag){
        printf("\n请按任意键返回！");
        getch();
    }
    else {char*pCh[]={"查找失败！","返回"};PopPrompt(pCh);}


}

void schquery3(SCH_NODE *hd) /*全部显示*/
{
    printf("所有结果如下:\n\n");
    printf("学校编号|                     学校名称|        负责人|      联系电话|\n");
    SCH_NODE *p=hd;
    if(p==NULL){printf("无信息！");printf("\n请按任意键返回！");
        getch();}
    else{
        while(p!=NULL){
            printf("%8s|%29s|%14s|%14s|\n",p->sch_id,p->name,p->manager,p->phone);
            p=p->next;
        }
        printf("\n请按任意键返回！");
        getch();
    }
}


BOOL QueryDepInfo(void)
{
    printf("请输入查询方式:\n");
    printf("1.按高校编号以及院系名称查询\n");
    printf("2.按教师人数查询\n");
    printf("3.显示所有院系信息\n");
    printf("4.返回\n");
    int num;
    scanf("%d",&num);
    switch(num){
        case 1 : depquery1(gp_head);break;
        case 2 : depquery2(gp_head);break;
        case 3 : depquery3(gp_head);break;
        case 4 : break;
        default : {char*pCh[]={"输入有误！","返回"};PopPrompt(pCh);}
    }
    InitInterface();
    return TRUE;
}

void depquery1(SCH_NODE *hd)/*按高校编号及院系名称查询*/
{
    printf("Process 1: 请输入院系所在学校的编号：");
    char s[8];
    char name[20];
    scanf("%s",s);
    SCH_NODE *p=hd;
    DEP_NODE *p1;
    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
        printf("Process 2: 请输入需要查询的院系名称：");
        scanf("%s",name);
        p1=SeekdepNode1(p,name);
        printf("院系信息如下：\n\n");
        printf("学校编号|          院系名称|  教师人数|  教授人数|    副教授人数|    研究生人数|");
        if(p1!=NULL){
             printf("%8s|%18s|%10d|%10d|%14d|%14d|\n",p1->sch_id,p1->name,p1->teacher,p1->professor,p1->aprofessor,p1->grastu);
        }
        else{
            printf("无相关院系信息！\n");
        }
        printf("\n请按任意键返回！");
        getch();
    }
    else {char*pCh[]={"查找失败！","返回"};PopPrompt(pCh);}
}

void depquery2(SCH_NODE *hd)/*按教师人数查询*/
{
    printf("此操作将显示所有超过指定教师人数的院系！\n");
    printf("请输入教师人数：");
    int teacher,find;
    scanf("%d",&teacher);
    printf("院系信息如下：\n\n");
    printf("学校编号|          院系名称|  教师人数|  教授人数|    副教授人数|    研究生人数|");
    find=SeekdepNode2(gp_head,teacher);
    printf("\n----------共%d条结果！",find);
    printf("\n请按任意键返回！");
    getch();
}

void depquery3(SCH_NODE *hd) /*显示所有院系信息*/
{
    SCH_NODE *psch_node;
    DEP_NODE *pdep_node;
    printf("所有院系信息如下：\n\n");
    printf("学校编号|          院系名称|  教师人数|  教授人数|    副教授人数|    研究生人数|"); /*正好80宽度，自动换行，不加换行符*/
    for(psch_node=hd;psch_node!=NULL;psch_node=psch_node->next){
        pdep_node=psch_node->dnext;
        while(pdep_node!=NULL){
                printf("%8s|%18s|%10d|%10d|%14d|%14d|",pdep_node->sch_id,pdep_node->name,pdep_node->teacher,pdep_node->professor,pdep_node->aprofessor,pdep_node->grastu);
                pdep_node=pdep_node->next;
            }
        }
    printf("\n请按任意键返回！");
    getch();
}

BOOL QueryTheInfo(void)
{
    BOOL bRet = TRUE;
    printf("请输入查询方式:\n");
    printf("1.按论文题目查询\n");
    printf("2.按第一作者名查询\n");
    printf("3.返回\n");
    int num;
    scanf("%d",&num);
    switch(num){
        case 1 : thequery1(gp_head);break;
        case 2 : thequery2(gp_head);break;
        case 3 : break;
        default : {char*pCh[]={"输入有误！","返回"};PopPrompt(pCh);}
    }
    InitInterface();
    return bRet;
}

void thequery1(SCH_NODE *hd)/*按论文名*/
{
    printf("请输入论文名称：");
    char title[50];
    scanf("%s",title);
    SCH_NODE *p;
    DEP_NODE *pd;
    THE_NODE *pt;
    int find = 0;

    for(p=hd;p!=NULL;p=p->next){/*遍历主链*/
        for(pd=p->dnext;pd!=NULL;pd=pd->next){/*遍历院系链*/
            pt=pd->tnext;
            while(pt!=NULL){/*遍历论文链*/
            if(strcmp(pt->title,title)==0){
            printf("高校编号：  %s   高校名称：  %s   ",p->sch_id,p->name);
            printf("院系名称：  %s\n",pd->name);
            printf("         论文题目          |   第一作者   |        刊名        |   期次   |分类|");
            printf("%27s|%14s|%20s|%10s|   %c|",pt->title,pt->writer,pt->ISSN,pt->date,pt->Rank);
            find ++;
            }
            pt=pt->next;
           }
        }
    }
    printf("------共%d条结果！\n",find);
    printf("\n请按任意键返回！");
    getch();
}

void thequery2(SCH_NODE *hd)/*按第一作者名*/
{
    printf("请输入第一作者名称：");
    char writer[15];
    scanf("%s",writer);
    SCH_NODE *p;
    DEP_NODE *pd;
    THE_NODE *pt;
    int find = 0;

    for(p=hd;p!=NULL;p=p->next){/*遍历主链*/
        for(pd=p->dnext;pd!=NULL;pd=pd->next){/*遍历院系链*/
            pt=pd->tnext;
            while(pt!=NULL){/*遍历论文链*/
            if(strcmp(pt->writer,writer)==0){
            printf("高校编号：  %s   高校名称：  %s   ",p->sch_id,p->name);
            printf("院系名称：  %s\n",pd->name);
            printf("         论文题目          |   第一作者   |        刊名        |   期次   |分类|");
            printf("%27s|%14s|%20s|%10s|   %c|",pt->title,pt->writer,pt->ISSN,pt->date,pt->Rank);
            find ++;
            }
            pt=pt->next;
           }
        }
    }
    printf("------共%d条结果！\n",find);
    printf("\n请按任意键返回！");
    getch();
}

BOOL Stat1(void)/*统计各高校教师人数、发表的论文数和教师人均发表的论文数（保留1位小数），按教师人均发表的论文数降序排序后，输出统计结果。*/
{
    BOOL bRet = TRUE;
    State1(gp_head);
    printf("\n请按任意键返回！");
    getch();
    InitInterface();
    return bRet;
}

void State1(SCH_NODE *hd)
{
    SCH_NODE *p;
    DEP_NODE *pd;
    THE_NODE *pt;
    Stat1_NODE *p1=NULL,*ptemp;
    int num1;
    int num2;
    float num3;
    for(p=hd;p!=NULL;p=p->next){
            num1 = 0;
            num2 = 0;
            num3 = 0;
        for(pd=p->dnext;pd!=NULL;pd=pd->next){
            num1 = pd->teacher;/*教师人数*/
            for(pt=pd->tnext;pt!=NULL;pt=pt->next){
                num2++;/*论文数*/
            }
            num3=(float)num2/(float)num1; /*强制转型，计算小数*/
        }

        ptemp=(Stat1_NODE *)malloc(sizeof(Stat1_NODE));

        ptemp->next=p1;

        strcpy(ptemp->name,p->name);
        strcpy(ptemp->sch_id,p->sch_id);
        ptemp->num1=num1;
        ptemp->num2=num2;
        ptemp->num3=num3;

        p1=ptemp;
    }

    printf("学校编号|                      学校名称|  教师人数|    论文数|  人均发表数|\n");

    /*选择法排序*/
    Stat1_NODE *p1cur,*p1prior,*p1after,*p1temp;
    p1prior=p1;
    if(p1prior==NULL){
        printf("------无信息！");
    }
    else{
        p1temp=(Stat1_NODE *)malloc(sizeof(Stat1_NODE));
        while(p1prior->next!=NULL){
            p1cur=p1prior;
            p1after=p1prior->next;
            while(p1after!=NULL){
                if(p1cur->num3<p1after->num3){
                    p1cur=p1after;
                }
                p1after=p1after->next;
            }

            if(p1cur!=p1prior){/*交换内部信息而不交换指针域*/
                *p1temp=*p1prior;
                *p1prior=*p1cur;
                p1prior->next=p1temp->next;
                p1temp->next=p1cur->next;
                *p1cur=*p1temp;
            }
            p1prior=p1prior->next;
        }
        free(p1temp);

        while(p1!=NULL){

        printf("%8s|%30s|%10d|%10d|      %.4f|\n",p1->sch_id,p1->name,p1->num1,p1->num2,p1->num3);
        p1=p1->next;
    }
    }
}

BOOL Stat2(void)
{
    BOOL bRet = TRUE;
    State2(gp_head);
    printf("\n请按任意键返回！");
    getch();
    InitInterface();
    return bRet;
}

void State2(SCH_NODE *hd)/*统计某高校某年度发表的‘a’、‘b’、‘c’、‘d’四类论文数。*/
{
    printf("Process 1: 请输入高校编号：");
    char s[8];
    scanf("%s",s);
    SCH_NODE *p=hd;
    DEP_NODE *pd;
    THE_NODE *pt;
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
    printf("Process 2: 请输入论文年份：");
    char date[10];
    scanf("%s",date);

        for(pd=p->dnext;pd!=NULL;pd=pd->next){
            for(pt=pd->tnext;pt!=NULL;pt=pt->next){
                if(strstr(pt->date,date)&&(pt->Rank=='a')) a++;
                else if(strstr(pt->date,date)&&(pt->Rank=='b')) b++;
                else if(strstr(pt->date,date)&&(pt->Rank=='c')) c++;
                else if(strstr(pt->date,date)&&(pt->Rank=='d')) d++;
            }
        }
        printf("\n统计年份：   %s   高校名称：   %s\n",date,p->name);
        printf("分类统计：   a类： %d篇\n             b类： %d篇\n             c类： %d篇\n             d类： %d篇\n",a,b,c,d);
    }
    else printf("输入高校编号有误！\n");
}

BOOL Stat3(void)
{
    BOOL bRet = TRUE;
    State3(gp_head);
    printf("\n请按任意键返回！");
    getch();
    InitInterface();
    return bRet;
}

void State3(SCH_NODE *hd)
{
    SCH_NODE *p;
    DEP_NODE *pd;
    THE_NODE *pt;
    Stat3_NODE *p1=NULL,*ptemp;
    int num;
    for(p=hd;p!=NULL;p=p->next){
            num = 0;
        for(pd=p->dnext;pd!=NULL;pd=pd->next){
            for(pt=pd->tnext;pt!=NULL;pt=pt->next){
                if(pt->Rank=='a') num++;/*统计a类论文数*/
            }
        }

        ptemp=(Stat3_NODE *)malloc(sizeof(Stat3_NODE));

        ptemp->next=p1;

        strcpy(ptemp->name,p->name);
        strcpy(ptemp->sch_id,p->sch_id);
        ptemp->num=num;
        p1=ptemp;
    }

    /*选择法排序*/
    Stat3_NODE *p1cur,*p1prior,*p1after,*p1temp;
    p1prior=p1;
    printf("统计结果如下：\n");
    printf("学校编号|                      学校名称|                       a类文数目|\n");

    if(p1prior==NULL){
        printf("------无高校信息！");
    }
    else{
        p1temp=(Stat3_NODE *)malloc(sizeof(Stat3_NODE));
        while(p1prior->next!=NULL){
            p1cur=p1prior;
            p1after=p1prior->next;
            while(p1after!=NULL){
                if(p1cur->num<p1after->num){
                    p1cur=p1after;
                }
                p1after=p1after->next;
            }

            if(p1cur!=p1prior){/*交换内部信息而不交换指针域*/
                *p1temp=*p1prior;
                *p1prior=*p1cur;
                p1prior->next=p1temp->next;
                p1temp->next=p1cur->next;
                *p1cur=*p1temp;
            }
            p1prior=p1prior->next;
        }
        free(p1temp);

        num = 5;/*只统计前5个*/

        while(p1!=NULL){
        num--;
        printf("%8s|%30s|                      %10d|\n",p1->sch_id,p1->name,p1->num);
        p1=p1->next;
        if(!num) break;
    }
    }
}

BOOL Stat4(void)
{
    BOOL bRet = TRUE;
    State4(gp_head);
    printf("\n请按任意键返回！");
    getch();
    InitInterface();
    return bRet;
}

void State4(SCH_NODE *hd)
{
    SCH_NODE *p;
    DEP_NODE *pd;
    THE_NODE *pt;
    Stat4_NODE *p1=NULL,*ptemp;
    char date[10];
    printf("请输入年份：");
    scanf("%s",date);
    int num1; /*研究生数*/
    int num2; /*论文数*/
    float num3;
    for(p=hd;p!=NULL;p=p->next){
            num1 = 0;
            num2 = 0;
            num3 = 0;
        for(pd=p->dnext;pd!=NULL;pd=pd->next){
            num1 = pd->grastu;/*研究生人数*/
            for(pt=pd->tnext;pt!=NULL;pt=pt->next){
                if(strstr(pt->date,date)) num2++;/*论文数*/
            }
            num3=(float)num2/(float)num1; /*强制转型，计算小数*/
            ptemp=(Stat4_NODE *)malloc(sizeof(Stat4_NODE));
            ptemp->next=p1;
            strcpy(ptemp->name,pd->name);
            strcpy(ptemp->sch_id,pd->sch_id);
            ptemp->num1=num1;
            ptemp->num2=num2;
            ptemp->num3=num3;
            p1=ptemp;
        }
    }

    printf("学校编号|            院系名称|  研究生人数|    论文数|  人均发表数|\n");

    /*选择法排序*/
    Stat4_NODE *p1cur,*p1prior,*p1after,*p1temp;
    p1prior=p1;
    if(p1prior==NULL){
        printf("------无信息！");
    }
    else{
        p1temp=(Stat4_NODE *)malloc(sizeof(Stat4_NODE));
        while(p1prior->next!=NULL){
            p1cur=p1prior;
            p1after=p1prior->next;
            while(p1after!=NULL){
                if(p1cur->num3<p1after->num3){
                    p1cur=p1after;
                }
                p1after=p1after->next;
            }

            if(p1cur!=p1prior){/*交换内部信息而不交换指针域*/
                *p1temp=*p1prior;
                *p1prior=*p1cur;
                p1prior->next=p1temp->next;
                p1temp->next=p1cur->next;
                *p1cur=*p1temp;
            }
            p1prior=p1prior->next;
        }
        free(p1temp);

        int n =10;/*只统计前10个*/

        while(p1!=NULL){
        n--;
        printf("%8s|%20s|  %10d|%10d|      %.4f|\n",p1->sch_id,p1->name,p1->num1,p1->num2,p1->num3);
        p1=p1->next;
        if(!n) break;
    }
    }
}

BOOL Stat5(void)
{
    BOOL bRet = TRUE;
    State5(gp_head);
    printf("\n请按任意键返回！");
    getch();
    InitInterface();
    return bRet;
}

void State5(SCH_NODE *hd)
{
    printf("请输入年份信息：");
    char date[10];
    scanf("%s",date);
    SCH_NODE *p;
    DEP_NODE *pd;
    THE_NODE *pt;
    int find = 0;
    printf("                   论文题目|      第一作者|                刊名|      期次|分类|");
    for(p=hd;p!=NULL;p=p->next){
        for(pd=p->dnext;pd!=NULL;pd=pd->next){
            for(pt=pd->tnext;pt!=NULL;pt=pt->next){
                if(strstr(pt->date,date)&&(pt->Rank='a')){
                    printf("%27s|%14s|%20s|%10s|   %c|",pt->title,pt->writer,pt->ISSN,pt->date,pt->Rank);
                    find++;
                }
            }
        }
    }
    printf("\n------共计%d条记录",find);
}

BOOL HelpTopic(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"如有任何问题需要帮助,请联系",
                           "   邮箱:382825415@qq.com",
                           "确认"
                          };

    ShowModule(plabel_name, 3);

    return bRet;
}

BOOL About(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"华中科技大学计算机系",
                           " CS1401班覃宇骁制作",
                           "确认"
                          };

    ShowModule(plabel_name, 3);

    return bRet;
}


BOOL ShowModule(char **pString, int n)
{
    LABEL_BUNDLE labels;
    HOT_AREA areas;
    BOOL bRet = TRUE;
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int iHot = 1;
    int i, maxlen, str_len;

    for (i=0,maxlen=0; i<n; i++) {
        str_len = strlen(pString[i]);
        if (maxlen < str_len) {
            maxlen = str_len;
        }
    }

    pos.X = maxlen + 6;
    pos.Y = n + 5;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*白底黑字*/
    labels.num = n;
    labels.ppLabel = pString;
    COORD aLoc[n];

    for (i=0; i<n; i++) {
        aLoc[i].X = rcPop.Left + 3;
        aLoc[i].Y = rcPop.Top + 2 + i;

    }
    str_len = strlen(pString[n-1]);
    aLoc[n-1].X = rcPop.Left + 3 + (maxlen-str_len)/2;
    aLoc[n-1].Y = aLoc[n-1].Y + 2;

    labels.pLoc = aLoc;

    areas.num = 1;
    SMALL_RECT aArea[] = {{aLoc[n-1].X, aLoc[n-1].Y,
                           aLoc[n-1].X + 3, aLoc[n-1].Y}};

    char aSort[] = {0};
    char aTag[] = {1};

    areas.pArea = aArea;
    areas.pSort = aSort;
    areas.pTag = aTag;
    PopUp(&rcPop, att, &labels, &areas);

    pos.X = rcPop.Left + 1;
    pos.Y = rcPop.Top + 2 + n;
    FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-1, pos, &ul);

    DealInput(&areas, &iHot);
    PopOff();

    return bRet;

}
