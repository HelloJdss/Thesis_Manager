#include "thesis.h"

unsigned long ul;

int main()
{
    COORD size = {SCR_COL, SCR_ROW};              /*���ڻ�������С*/

    gh_std_out = GetStdHandle(STD_OUTPUT_HANDLE); /* ��ȡ��׼����豸���*/
    gh_std_in = GetStdHandle(STD_INPUT_HANDLE);   /* ��ȡ��׼�����豸���*/

    SetConsoleTitle(gp_sys_name);                 /*���ô��ڱ���*/
    SetConsoleScreenBufferSize(gh_std_out, size); /*���ô��ڻ�������С80*25*/

    LoadData();                   /*���ݼ���*/
    InitInterface();          /*�����ʼ��*/
    RunSys(&gp_head);             /*ϵͳ����ģ���ѡ������*/
    CloseSys(gp_head);            /*�˳�ϵͳ*/

    return 0;
}

/**
 * ��������: LoadData
 * ��������: ������������ݴ������ļ����뵽�ڴ滺������ʮ��������.
 * �������: ��
 * �������: ��
 * �� �� ֵ: BOOL����, ���ܺ����г��˺���ExitSys�ķ���ֵ����ΪFALSE��,
 *           ���������ķ���ֵ����ΪTRUE.
 *
 * ����˵��: Ϊ���ܹ���ͳһ�ķ�ʽ���ø����ܺ���, ����Щ���ܺ�����ԭ����Ϊ
 *           һ��, ���޲����ҷ���ֵΪBOOL. ����ֵΪFALSEʱ, ������������.
 */
BOOL LoadData()
{
    int Re = 0;
    Re = CreatList(&gp_head);
    if (Re != 28)
    {  /*���ݼ�����ʾ��Ϣ*/
        printf("\nϵͳ�������ݲ�����!\n");
        printf("\n�����������...\n");
        getch();
    }
    return TRUE;
}

/**
 * ��������: CreatList
 * ��������: �������ļ���ȡ��������, ����ŵ���������ʮ��������.
 * �������: ��
 * �������: phead ����ͷָ��ĵ�ַ, ����������������ʮ����.
 * �� �� ֵ: int����ֵ, ��ʾ�����������.
 *           0  ����, ������
 *           4  �Ѽ��ظ�У������Ϣ���ݣ���Ժϵ������Ϣ�����ķ�����Ϣ����
 *           12 �Ѽ���Ժϵ������Ϣ�͸�У������Ϣ���ݣ������ķ�����Ϣ����
 *           28 ����������ݶ��Ѽ���
 *
 * ����˵��:
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
        printf("��У������Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("��У������Ϣ�����ļ��򿪳ɹ�!\n");

    /*�������ļ��ж���У������Ϣ���ݣ������Ժ���ȳ���ʽ������������*/
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
        printf("��У������Ϣ�����ļ�����ʧ��!\n");
        return re;
    }
    printf("��У������Ϣ�����ļ����سɹ�!\n");
    *phead = hd;
    re += 4;

    if ((pFile = fopen(gp_department_info_filename, "rb")) == NULL)
    {
        printf("Ժϵ������Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("Ժϵ������Ϣ�����ļ��򿪳ɹ�!\n");
    re += 8;

    /*�������ļ��ж�ȡԺϵ������Ϣ���ݣ�����������Ӧ����Ժϵ������Ϣ֧����*/
    while (fread(&tmp2, sizeof(DEP_NODE), 1, pFile) == 1)
    {
        /*������㣬��Ŵ������ļ��ж�����Ժϵ������Ϣ*/
        pDEPNode = (DEP_NODE *)malloc(sizeof(DEP_NODE));
        *pDEPNode = tmp2;
        pDEPNode->tnext = NULL;

        /*�������ϲ��Ҹ�Ժϵ���ڸ�У��Ӧ���������*/
        pSCHNode = hd;
        while (pSCHNode != NULL
               && strcmp(pSCHNode->sch_id, pDEPNode->sch_id) != 0)
        {
            pSCHNode = pSCHNode->next;
        }
        if (pSCHNode != NULL) /*����ҵ����򽫽���Ժ���ȳ���ʽ����ѧ����Ϣ֧��*/
        {
            pDEPNode->next = pSCHNode->dnext;
            pSCHNode->dnext = pDEPNode;
        }
        else  /*���δ�ҵ������ͷ������������ڴ�ռ�*/
        {
            free(pDEPNode);
        }
    }
    fclose(pFile);

    if ((pFile = fopen(gp_thesis_info_filename, "rb")) == NULL)
    {
        printf("���ķ�����Ϣ�����ļ���ʧ��!\n");
        return re;
    }
    printf("���ķ�����Ϣ�����ļ��򿪳ɹ�!\n");
    re += 16;

    /*�������ļ��ж�ȡ���Ļ�����Ϣ���ݣ�����Ժϵ������Ϣ֧����Ӧ�������ķ�����Ϣ֧����*/
    while (fread(&tmp3, sizeof(THE_NODE), 1, pFile) == 1)
    {
        /*������㣬��Ŵ������ļ��ж�����������Ϣ*/
        pTHENode = (THE_NODE *)malloc(sizeof(THE_NODE));
        *pTHENode = tmp3;

        /*����Ժϵ��Ϣ֧���϶�Ӧ������Ϣ���*/
        pSCHNode = hd;
        find = 0;
        while (pSCHNode != NULL && find == 0)
        {
            pDEPNode = pSCHNode->dnext;
            while (pDEPNode != NULL && find == 0)
            {
                if (strcmp(pDEPNode->name, pTHENode->depart) == 0) /*��������Ժϵ*/
                {
                    find = 1;
                    break;
                }
                pDEPNode = pDEPNode->next;
            }
            pSCHNode = pSCHNode->next;
        }
        if (find)  /*����ҵ����򽫽���Ժ���ȳ���ʽ�������ķ�����Ϣ֧����*/
        {
            pTHENode->next = pDEPNode->tnext;
            pDEPNode->tnext = pTHENode;
        }
        else /*���δ�ҵ������ͷ������������ڴ�ռ�*/
        {
            free(pTHENode);
        }
    }
    fclose(pFile);
    printf("%d",re);
    return re;
}

/**
 * ��������: InitInterface
 * ��������: ��ʼ������.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void InitInterface()
{
    WORD att = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
               | BACKGROUND_BLUE | BACKGROUND_GREEN;  /*��ɫǰ������ɫ����*/

    SetConsoleTextAttribute(gh_std_out, att);  /*���ÿ���̨��Ļ�������ַ�����*/

    ClearScreen();  /* ����*/

    /*��������������Ϣ��ջ������ʼ�������Ļ���ڵ�����һ�㵯������*/
    gp_scr_att = (char *)calloc(SCR_COL * SCR_ROW, sizeof(char));/*��Ļ�ַ�����*/
    gp_top_layer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    gp_top_layer->LayerNo = 0;      /*�������ڵĲ��Ϊ0*/
    gp_top_layer->rcArea.Left = 0;  /*�������ڵ�����Ϊ������Ļ����*/
    gp_top_layer->rcArea.Top = 0;
    gp_top_layer->rcArea.Right = SCR_COL - 1;
    gp_top_layer->rcArea.Bottom = SCR_ROW - 1;
    gp_top_layer->pContent = NULL;
    gp_top_layer->pScrAtt = gp_scr_att;
    gp_top_layer->next = NULL;

    ShowMenu();     /*��ʾ�˵���*/
    ShowState();    /*��ʾ״̬��*/

    return;
}

/**
 * ��������: ClearScreen
 * ��������: �����Ļ��Ϣ.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void ClearScreen(void)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD home = {0, 0};
    unsigned long size;

    GetConsoleScreenBufferInfo( gh_std_out, &bInfo );/*ȡ��Ļ��������Ϣ*/
    size =  bInfo.dwSize.X * bInfo.dwSize.Y; /*������Ļ�������ַ���Ԫ��*/

    /*����Ļ���������е�Ԫ���ַ���������Ϊ��ǰ��Ļ�������ַ�����*/
    FillConsoleOutputAttribute(gh_std_out, bInfo.wAttributes, size, home, &ul);

    /*����Ļ���������е�Ԫ���Ϊ�ո��ַ�*/
    FillConsoleOutputCharacter(gh_std_out, ' ', size, home, &ul);

    return;
}

/**
 * ��������: ShowMenu
 * ��������: ����Ļ����ʾ���˵�, ����������, �����˵���һ������ѡ�б��.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
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
    for (i=0; i < 5; i++) /*�ڴ��ڵ�һ�е�һ�д�������˵���*/
    {
        printf("  %s  ", ga_main_menu[i]);
    }

    GetConsoleCursorInfo(gh_std_out, &lpCur);
    lpCur.bVisible = FALSE;
    SetConsoleCursorInfo(gh_std_out, &lpCur);  /*���ع��*/

    /*���붯̬�洢����Ϊ��Ų˵�����Ļ���ַ���Ϣ�Ļ�����*/
    gp_buff_menubar_info = (CHAR_INFO *)malloc(size.X * size.Y * sizeof(CHAR_INFO));
    SMALL_RECT rcMenu ={0, 0, size.X-1, 0} ;

    /*�����ڵ�һ�е����ݶ��뵽��Ų˵�����Ļ���ַ���Ϣ�Ļ�������*/
    ReadConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    /*����һ���е�Ӣ����ĸ��Ϊ��ɫ�������ַ���Ԫ��Ϊ�׵׺���*/
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

    /*�޸ĺ�Ĳ˵����ַ���Ϣ��д�����ڵĵ�һ��*/
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);
    COORD endPos = {0, 1};
    SetConsoleCursorPosition(gh_std_out, endPos);  /*�����λ�������ڵ�2�е�1��*/

    /*���˵�����Ϊ�������������Ϊ�˵���ţ���������Ϊ0(��ť��)*/
    i = 0;
    do
    {
        PosB = PosA + strlen(ga_main_menu[i]);  /*��λ��i+1�Ų˵������ֹλ��*/
        for (j=PosA; j<PosB; j++)
        {
            gp_scr_att[j] |= (i+1) << 2; /*���ò˵��������ַ���Ԫ������ֵ*/
        }
        PosA = PosB + 4;
        i++;
    } while (i<5);

    TagMainMenu(gi_sel_menu);  /*��ѡ�����˵���������ǣ�gi_sel_menu��ֵΪ1*/

    return;
}

/**
 * ��������: ShowState
 * ��������: ��ʾ״̬��.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��: ״̬���ַ�����Ϊ�׵׺���, ��ʼ״̬��״̬��Ϣ.
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
 * ��������: TagMainMenu
 * ��������: ��ָ�����˵�������ѡ�б�־.
 * �������: num ѡ�е����˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagMainMenu(int num)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;
    COORD size;
    COORD pos = {0, 0};
    int PosA = 1, PosB;
    char ch;
    int i;

    if (num == 0) /*numΪ0ʱ������ȥ�����˵���ѡ�б��*/
    {
        PosA = 0;
        PosB = 0;
    }
    else  /*���򣬶�λѡ�����˵������ֹλ��: PosAΪ��ʼλ��, PosBΪ��ֹλ��*/
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

    /*ȥ��ѡ�в˵���ǰ��Ĳ˵���ѡ�б��*/
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

    /*��ѡ�в˵���������ǣ����װ���*/
    for (i=PosA; i<PosB; i++)
    {
        (gp_buff_menubar_info+i)->Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN
                                               | FOREGROUND_RED | BACKGROUND_BLUE;
    }

    /*ȥ��ѡ�в˵������Ĳ˵���ѡ�б��*/
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

    /*�����ñ�ǵĲ˵�����Ϣд�����ڵ�һ��*/
    SMALL_RECT rcMenu ={0, 0, size.X-1, 0};
    WriteConsoleOutput(gh_std_out, gp_buff_menubar_info, size, pos, &rcMenu);

    return;
}

/**
 * ��������: CloseSys
 * ��������: �ر�ϵͳ.
 * �������: hd ����ͷָ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void CloseSys(SCH_NODE *hd)
{
    SCH_NODE *pSCHNode1 = hd, *pSCHNode2;
    DEP_NODE *pDEPNode1, *pDEPNode2;
    THE_NODE *pTHENode1, *pTHENode2;

    while (pSCHNode1 != NULL) /*�ͷ�ʮ�ֽ�������Ķ�̬�洢��*/
    {
        pSCHNode2 = pSCHNode1->next;
        pDEPNode1 = pSCHNode1->dnext;
        while (pDEPNode1 != NULL) /*�ͷ�Ժϵ������Ϣ֧���Ķ�̬�洢��*/
        {
            pDEPNode2 = pDEPNode1->next;
            pTHENode1 = pDEPNode1->tnext;
            while (pTHENode1 != NULL) /*�ͷ����ķ�����Ϣ֧���Ķ�̬�洢��*/
            {
                pTHENode2 = pTHENode1->next;
                free(pTHENode1);
                pTHENode1 = pTHENode2;
            }
            free(pDEPNode1);
            pDEPNode1 = pDEPNode2;
        }
        free(pSCHNode1);  /*�ͷ��������Ķ�̬�洢��*/
        pSCHNode1 = pSCHNode2;
    }

    ClearScreen();        /*����*/

    /*�ͷŴ�Ų˵�����״̬�����Ա�����ѧ�����������Ϣ��̬�洢��*/
    free(gp_buff_menubar_info);
    free(gp_buff_stateBar_info);

    /*�رձ�׼���������豸���*/
    CloseHandle(gh_std_out);
    CloseHandle(gh_std_in);

    /*�����ڱ�������Ϊ���н���*/
    SetConsoleTitle("���н���");

    return;
}

/**
 * ��������: RunSys
 * ��������: ����ϵͳ, ��ϵͳ�������������û���ѡ��Ĺ���ģ��.
 * �������: ��
 * �������: phead ����ͷָ��ĵ�ַ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void RunSys(SCH_NODE **phead)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    BOOL bRet = TRUE;
    int i, loc, num;
    int cNo, cAtt;      /*cNo:�ַ���Ԫ���, cAtt:�ַ���Ԫ����*/
    char vkc, asc;      /*vkc:���������, asc:�ַ���ASCII��ֵ*/

    while (bRet)
    {
        /*�ӿ���̨���뻺�����ж�һ����¼*/
        ReadConsoleInput(gh_std_in, &inRec, 1, &res);

        if (inRec.EventType == MOUSE_EVENT) /*�����¼������¼�����*/
        {
            pos = inRec.Event.MouseEvent.dwMousePosition;  /*��ȡ�������λ��*/
            cNo = gp_scr_att[pos.Y * SCR_COL + pos.X] & 3; /*ȡ��λ�õĲ��*/
            cAtt = gp_scr_att[pos.Y * SCR_COL + pos.X] >> 2;/*ȡ���ַ���Ԫ����*/
            if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
            {
                /* cAtt > 0 ������λ�ô�������(���˵����ַ���Ԫ)
                 * cAtt != gi_sel_menu ������λ�õ����˵���δ��ѡ��
                 * gp_top_layer->LayerNo > 0 ������ǰ���Ӳ˵�����
                 */
                if (cAtt > 0 && cAtt != gi_sel_menu && gp_top_layer->LayerNo > 0)
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                    PopMenu(cAtt);       /*��������������˵����Ӧ���Ӳ˵�*/
                }
            }
            else if (cAtt > 0) /*�������λ��Ϊ�����Ӳ˵��Ĳ˵����ַ���Ԫ*/
            {
                TagSubMenu(cAtt); /*�ڸ��Ӳ˵�������ѡ�б��*/
            }

            if (inRec.Event.MouseEvent.dwButtonState
                == FROM_LEFT_1ST_BUTTON_PRESSED) /*������������ߵ�һ��*/
            {
                if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                {
                    if (cAtt > 0) /*�����λ�ô�������(���˵����ַ���Ԫ)*/
                    {
                        PopMenu(cAtt);   /*��������������˵����Ӧ���Ӳ˵�*/
                    }
                    /*�����λ�ò��������˵����ַ���Ԫ�������Ӳ˵�����*/
                    else if (gp_top_layer->LayerNo > 0)
                    {
                        PopOff();            /*�رյ������Ӳ˵�*/
                        gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                    }
                }
                else /*��Ų�Ϊ0��������λ�ñ������Ӳ˵�����*/
                {
                    if (cAtt > 0) /*�����λ�ô�������(�Ӳ˵����ַ���Ԫ)*/
                    {
                        PopOff(); /*�رյ������Ӳ˵�*/
                        gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/

                        /*ִ�ж�Ӧ���ܺ���:gi_sel_menu���˵����,cAtt�Ӳ˵����*/
                        bRet = ExeFunction(gi_sel_menu, cAtt);
                    }
                }
            }
            else if (inRec.Event.MouseEvent.dwButtonState
                     == RIGHTMOST_BUTTON_PRESSED) /*�����������Ҽ�*/
            {
                if (cNo == 0) /*���Ϊ0��������λ��δ�������Ӳ˵�����*/
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                }
            }
        }
        else if (inRec.EventType == KEY_EVENT  /*�����¼�ɰ�������*/
                 && inRec.Event.KeyEvent.bKeyDown) /*�Ҽ�������*/
        {
            vkc = inRec.Event.KeyEvent.wVirtualKeyCode; /*��ȡ�������������*/
            asc = inRec.Event.KeyEvent.uChar.AsciiChar; /*��ȡ������ASC��*/

            /*ϵͳ��ݼ��Ĵ���*/
            if (vkc == 112) /*�������F1��*/
            {
                if (gp_top_layer->LayerNo != 0) /*�����ǰ���Ӳ˵�����*/
                {
                    PopOff();            /*�رյ������Ӳ˵�*/
                    gi_sel_sub_menu = 0; /*��ѡ���Ӳ˵���������Ϊ0*/
                }
                bRet = ExeFunction(5, 1);  /*���а������⹦�ܺ���*/
            }
            else if (inRec.Event.KeyEvent.dwControlKeyState
                     & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED))
            { /*������������Alt��*/
                switch (vkc)  /*�ж���ϼ�Alt+��ĸ*/
                {
                    case 88:  /*Alt+X �˳�*/
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
            else if (asc == 0) /*�������Ƽ��Ĵ���*/
            {
                if (gp_top_layer->LayerNo == 0) /*���δ�����Ӳ˵�*/
                {
                    switch (vkc) /*�������(���ҡ���)������Ӧ�������Ƽ�*/
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
                else  /*�ѵ����Ӳ˵�ʱ*/
                {
                    for (loc=0,i=1; i<gi_sel_menu; i++)
                    {
                        loc += ga_sub_menu_count[i-1];
                    }  /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                    switch (vkc) /*�����(���ҡ��ϡ���)�Ĵ���*/
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
            else if ((asc-vkc == 0) || (asc-vkc == 32)){  /*������ͨ��*/
                if (gp_top_layer->LayerNo == 0)  /*���δ�����Ӳ˵�*/
                {
                    switch (vkc)
                    {
                        case 70: /*f��F*/
                            PopMenu(1);
                            break;
                        case 77: /*m��M*/
                            PopMenu(2);
                            break;
                        case 81: /*q��Q*/
                            PopMenu(3);
                            break;
                        case 83: /*s��S*/
                            PopMenu(4);
                            break;
                        case 72: /*h��H*/
                            PopMenu(5);
                            break;
                        case 13: /*�س�*/
                            PopMenu(gi_sel_menu);
                            TagSubMenu(1);
                            break;
                    }
                }
                else /*�ѵ����Ӳ˵�ʱ�ļ������봦��*/
                {
                    if (vkc == 27) /*�������ESC��*/
                    {
                        PopOff();
                        gi_sel_sub_menu = 0;
                    }
                    else if(vkc == 13) /*������»س���*/
                    {
                        num = gi_sel_sub_menu;
                        PopOff();
                        gi_sel_sub_menu = 0;
                        bRet = ExeFunction(gi_sel_menu, num);
                    }
                    else /*������ͨ���Ĵ���*/
                    {
                        /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
                        for (loc=0,i=1; i<gi_sel_menu; i++)
                        {
                            loc += ga_sub_menu_count[i-1];
                        }

                        /*�����뵱ǰ�Ӳ˵���ÿһ��Ĵ����ַ����бȽ�*/
                        for (i=loc; i<loc+ga_sub_menu_count[gi_sel_menu-1]; i++)
                        {
                            if (strlen(ga_sub_menu[i])>0 && vkc==ga_sub_menu[i][1])
                            { /*���ƥ��ɹ�*/
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
    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
    rcPop.Bottom = rcPop.Top + pos.Y - 1; /*��������*/

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
 * ��������: PopMenu
 * ��������: ����ָ�����˵����Ӧ���Ӳ˵�.
 * �������: num ָ�������˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
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

    if (num != gi_sel_menu)       /*���ָ�����˵�������ѡ�в˵�*/
    {
        if (gp_top_layer->LayerNo != 0) /*�����ǰ�����Ӳ˵�����*/
        {
            PopOff();
            gi_sel_sub_menu = 0;
        }
    }
    else if (gp_top_layer->LayerNo != 0) /*���ѵ������Ӳ˵����򷵻�*/
    {
        return;
    }

    gi_sel_menu = num;    /*��ѡ�����˵�����Ϊָ�������˵���*/
    TagMainMenu(gi_sel_menu); /*��ѡ�е����˵����������*/
    LocSubMenu(gi_sel_menu, &rcPop); /*���㵯���Ӳ˵�������λ��, �����rcPop��*/

    /*������Ӳ˵��еĵ�һ�����Ӳ˵��ַ��������е�λ��(�±�)*/
    for (i=1; i<gi_sel_menu; i++)
    {
        loc += ga_sub_menu_count[i-1];
    }
    /*�������Ӳ˵������������ǩ���ṹ����*/
    labels.ppLabel = ga_sub_menu + loc;   /*��ǩ����һ����ǩ�ַ����ĵ�ַ*/
    labels.num = ga_sub_menu_count[gi_sel_menu-1]; /*��ǩ���б�ǩ�ַ����ĸ���*/
    COORD aLoc[labels.num];/*����һ���������飬���ÿ����ǩ�ַ������λ�õ�����*/
    for (i=0; i<labels.num; i++) /*ȷ����ǩ�ַ��������λ�ã����������������*/
    {
        aLoc[i].X = rcPop.Left + 2;
        aLoc[i].Y = rcPop.Top + i + 1;
    }
    labels.pLoc = aLoc; /*ʹ��ǩ���ṹ����labels�ĳ�ԱpLocָ�������������Ԫ��*/
    /*����������Ϣ*/
    areas.num = labels.num;       /*�����ĸ��������ڱ�ǩ�ĸ��������Ӳ˵�������*/
    SMALL_RECT aArea[areas.num];                    /*������������������λ��*/
    char aSort[areas.num];                      /*��������������������Ӧ���*/
    char aTag[areas.num];                         /*����������ÿ�������ı��*/
    for (i=0; i<areas.num; i++)
    {
        aArea[i].Left = rcPop.Left + 2;  /*������λ*/
        aArea[i].Top = rcPop.Top + i + 1;
        aArea[i].Right = rcPop.Right - 2;
        aArea[i].Bottom = aArea[i].Top;
        aSort[i] = 0;       /*�������Ϊ0(��ť��)*/
        aTag[i] = i + 1;           /*������˳����*/
    }
    areas.pArea = aArea;/*ʹ�����ṹ����areas�ĳ�ԱpAreaָ������λ��������Ԫ��*/
    areas.pSort = aSort;/*ʹ�����ṹ����areas�ĳ�ԱpSortָ���������������Ԫ��*/
    areas.pTag = aTag;   /*ʹ�����ṹ����areas�ĳ�ԱpTagָ���������������Ԫ��*/

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    PopUp(&rcPop, att, &labels, &areas);
    DrawBox(&rcPop);  /*���������ڻ��߿�*/
    pos.X = rcPop.Left + 2;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    { /*��ѭ�������ڿմ��Ӳ���λ�û����γɷָ�����ȡ���˲˵������������*/
        pCh = ga_sub_menu[loc+pos.Y-rcPop.Top-1];
        if (strlen(pCh)==0) /*����Ϊ0������Ϊ�մ�*/
        {   /*���Ȼ�����*/
            FillConsoleOutputCharacter(gh_std_out, '-', rcPop.Right-rcPop.Left-3, pos, &ul);
            for (j=rcPop.Left+2; j<rcPop.Right-1; j++)
            {   /*ȡ���������ַ���Ԫ����������*/
                gp_scr_att[pos.Y*SCR_COL+j] &= 3; /*��λ��Ľ�������˵���λ*/
            }
        }

    }
    /*���Ӳ˵���Ĺ��ܼ���Ϊ�׵׺���*/
    pos.X = rcPop.Left + 3;
    att =  FOREGROUND_RED | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;
    for (pos.Y=rcPop.Top+1; pos.Y<rcPop.Bottom; pos.Y++)
    {
        if (strlen(ga_sub_menu[loc+pos.Y-rcPop.Top-1])==0)
        {
            continue;  /*�����մ�*/
        }
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    return;
}

/**
 * ��������: PopUp
 * ��������: ��ָ�������������������Ϣ, ͬʱ��������, ����������λ����Ϣ��ջ.
 * �������: pRc ��������λ�����ݴ�ŵĵ�ַ
 *           att �������������ַ�����
 *           pLabel ���������б�ǩ����Ϣ��ŵĵ�ַ
             pHotArea ����������������Ϣ��ŵĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopUp(SMALL_RECT *pRc, WORD att, LABEL_BUNDLE *pLabel, HOT_AREA *pHotArea)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j, row;

    /*������������λ���ַ���Ԫ��Ϣ��ջ*/
    size.X = pRc->Right - pRc->Left + 1;    /*�������ڵĿ��*/
    size.Y = pRc->Bottom - pRc->Top + 1;    /*�������ڵĸ߶�*/
    /*�����ŵ������������Ϣ�Ķ�̬�洢��*/
    nextLayer = (LAYER_NODE *)malloc(sizeof(LAYER_NODE));
    nextLayer->next = gp_top_layer;
    nextLayer->LayerNo = gp_top_layer->LayerNo + 1;
    nextLayer->rcArea = *pRc;
    nextLayer->pContent = (CHAR_INFO *)malloc(size.X*size.Y*sizeof(CHAR_INFO));
    nextLayer->pScrAtt = (char *)malloc(size.X*size.Y*sizeof(char));
    pCh = nextLayer->pScrAtt;
    /*���������ڸ���������ַ���Ϣ���棬�����ڹرյ�������ʱ�ָ�ԭ��*/
    ReadConsoleOutput(gh_std_out, nextLayer->pContent, size, pos, pRc);
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*�˶���ѭ�����������ַ���Ԫ��ԭ������ֵ���붯̬�洢���������Ժ�ָ�*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            *pCh = gp_scr_att[i*SCR_COL+j];
            pCh++;
        }
    }
    gp_top_layer = nextLayer;  /*��ɵ������������Ϣ��ջ����*/
    /*���õ������������ַ���������*/
    pos.X = pRc->Left;
    pos.Y = pRc->Top;
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {
        FillConsoleOutputAttribute(gh_std_out, att, size.X, pos, &ul);
        pos.Y++;
    }
    /*����ǩ���еı�ǩ�ַ������趨��λ�����*/
    for (i=0; i<pLabel->num; i++)
    {
        pCh = pLabel->ppLabel[i];
        if (strlen(pCh) != 0)
        {
            WriteConsoleOutputCharacter(gh_std_out, pCh, strlen(pCh),
                                        pLabel->pLoc[i], &ul);
        }
    }
    /*���õ������������ַ���Ԫ��������*/
    for (i=pRc->Top; i<=pRc->Bottom; i++)
    {   /*�˶���ѭ�������ַ���Ԫ�Ĳ��*/
        for (j=pRc->Left; j<=pRc->Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = gp_top_layer->LayerNo;
        }
    }

    for (i=0; i<pHotArea->num; i++)
    {   /*�˶���ѭ�����������������ַ���Ԫ���������ͺ��������*/
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
 * ��������: PopOff
 * ��������: �رն��㵯������, �ָ���������ԭ��ۺ��ַ���Ԫԭ����.
 * �������: ��
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void PopOff(void)
{
    LAYER_NODE *nextLayer;
    COORD size;
    COORD pos = {0, 0};
    char *pCh;
    int i, j;

    if ((gp_top_layer->next==NULL) || (gp_top_layer->pContent==NULL))
    {   /*ջ�״�ŵ���������Ļ��Ϣ�����ùر�*/
        return;
    }
    nextLayer = gp_top_layer->next;
    /*�ָ�������������ԭ���*/
    size.X = gp_top_layer->rcArea.Right - gp_top_layer->rcArea.Left + 1;
    size.Y = gp_top_layer->rcArea.Bottom - gp_top_layer->rcArea.Top + 1;
    WriteConsoleOutput(gh_std_out, gp_top_layer->pContent, size, pos, &(gp_top_layer->rcArea));
    /*�ָ��ַ���Ԫԭ����*/
    pCh = gp_top_layer->pScrAtt;
    for (i=gp_top_layer->rcArea.Top; i<=gp_top_layer->rcArea.Bottom; i++)
    {
        for (j=gp_top_layer->rcArea.Left; j<=gp_top_layer->rcArea.Right; j++)
        {
            gp_scr_att[i*SCR_COL+j] = *pCh;
            pCh++;
        }
    }
    free(gp_top_layer->pContent);    /*�ͷŶ�̬�洢��*/
    free(gp_top_layer->pScrAtt);
    free(gp_top_layer);
    gp_top_layer = nextLayer;
    gi_sel_sub_menu = 0;
    return;
}

/**
 * ��������: DrawBox
 * ��������: ��ָ�����򻭱߿�.
 * �������: pRc �������λ����Ϣ�ĵ�ַ
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void DrawBox(SMALL_RECT *pRc)
{
    char chBox[] = {'+','-',' '};  /*�����õ��ַ�*/
    COORD pos = {pRc->Left, pRc->Top};  /*��λ����������Ͻ�*/

    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {   /*��ѭ�����ϱ߿����*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����Ͻ�*/
    for (pos.Y = pRc->Top+1; pos.Y < pRc->Bottom; pos.Y++)
    {   /*��ѭ�����߿�����ߺ��ұ���*/
        pos.X = pRc->Left;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
        pos.X = pRc->Right;
        WriteConsoleOutputCharacter(gh_std_out, &chBox[2], 1, pos, &ul);
    }
    pos.X = pRc->Left;
    pos.Y = pRc->Bottom;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/
    for (pos.X = pRc->Left + 1; pos.X < pRc->Right; pos.X++)
    {   /*���±߿����*/
        WriteConsoleOutputCharacter(gh_std_out, &chBox[1], 1, pos, &ul);
    }
    pos.X = pRc->Right;
    WriteConsoleOutputCharacter(gh_std_out, &chBox[0], 1, pos, &ul);/*���߿����½�*/
    return;
}

/**
 * ��������: TagSubMenu
 * ��������: ��ָ���Ӳ˵�������ѡ�б��.
 * �������: num ѡ�е��Ӳ˵����
 * �������: ��
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void TagSubMenu(int num)
{
    SMALL_RECT rcPop;
    COORD pos;
    WORD att;
    int width;

    LocSubMenu(gi_sel_menu, &rcPop);  /*���㵯���Ӳ˵�������λ��, �����rcPop��*/
    if ((num<1) || (num == gi_sel_sub_menu) || (num>rcPop.Bottom-rcPop.Top-1))
    {   /*����Ӳ˵����Խ�磬������Ӳ˵��ѱ�ѡ�У��򷵻�*/
        return;
    }

    pos.X = rcPop.Left + 2;
    width = rcPop.Right - rcPop.Left - 3;
    if (gi_sel_sub_menu != 0) /*����ȡ��ԭѡ���Ӳ˵����ϵı��*/
    {
        pos.Y = rcPop.Top + gi_sel_sub_menu;
        att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
        FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
        pos.X += 1;
        att |=  FOREGROUND_RED;/*�׵׺���*/
        FillConsoleOutputAttribute(gh_std_out, att, 1, pos, &ul);
    }
    /*���ƶ��Ӳ˵�������ѡ�б��*/
    pos.X = rcPop.Left + 2;
    pos.Y = rcPop.Top + num;
    att = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | BACKGROUND_BLUE;  /*���װ���*/
    FillConsoleOutputAttribute(gh_std_out, att, width, pos, &ul);
    gi_sel_sub_menu = num;  /*�޸�ѡ���Ӳ˵����*/
    return;
}

/**
 * ��������: LocSubMenu
 * ��������: ���㵯���Ӳ˵��������ϽǺ����½ǵ�λ��.
 * �������: num ѡ�е����˵����
 * �������: rc �������λ����Ϣ�ĵ�ַ
 * �� �� ֵ: ��
 *
 * ����˵��:
 */
void LocSubMenu(int num, SMALL_RECT *rc)
{
    int i, len, loc = 0;

    rc->Top = 1; /*������ϱ߶��ڵ�2�У��к�Ϊ1*/
    rc->Left = 1;
    for (i=1; i<num; i++)
    {   /*����������߽�λ��, ͬʱ�����һ���Ӳ˵������Ӳ˵��ַ��������е�λ��*/
        rc->Left += strlen(ga_main_menu[i-1]) + 4;
        loc += ga_sub_menu_count[i-1];
    }
    rc->Right = strlen(ga_sub_menu[loc]);/*��ʱ��ŵ�һ���Ӳ˵����ַ�������*/
    for (i=1; i<ga_sub_menu_count[num-1]; i++)
    {   /*������Ӳ˵��ַ��������䳤�ȴ����rc->Right*/
        len = strlen(ga_sub_menu[loc+i]);
        if (rc->Right < len)
        {
            rc->Right = len;
        }
    }
    rc->Right += rc->Left + 3;  /*����������ұ߽�*/
    rc->Bottom = rc->Top + ga_sub_menu_count[num-1] + 1;/*���������±ߵ��к�*/
    if (rc->Right >= SCR_COL)  /*�ұ߽�Խ��Ĵ���*/
    {
        len = rc->Right - SCR_COL + 1;
        rc->Left -= len;
        rc->Right = SCR_COL - 1;
    }
    return;
}

/**
 * ��������: DealInput
 * ��������: �ڵ�������������������, �ȴ�����Ӧ�û�����.
 * �������: pHotArea
 *           piHot ����������ŵĴ�ŵ�ַ, ��ָ�򽹵�������ŵ�ָ��
 * �������: piHot ����굥�������س���ո�ʱ���ص�ǰ�������
 * �� �� ֵ:
 *
 * ����˵��:
 */
int DealInput(HOT_AREA *pHotArea, int *piHot)
{
    INPUT_RECORD inRec;
    DWORD res;
    COORD pos = {0, 0};
    int num, arrow, iRet = 0;
    int cNo, cTag, cSort;/*cNo:���, cTag:�������, cSort: ��������*/
    char vkc, asc;       /*vkc:���������, asc:�ַ���ASCII��ֵ*/

    SetHotPoint(pHotArea, *piHot);
    while (TRUE)
    {    /*ѭ��*/
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
                {  /*�����(���ϡ��ҡ���)�Ĵ���*/
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
            {  /*ESC��*/
                iRet = 27;
                break;
            }
            else if (vkc == 13 || vkc == 32)
            {  /*�س�����ո��ʾ���µ�ǰ��ť*/
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

    att1 = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  /*�ڵװ���*/
    att2 = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
    for (i=0; i<pHotArea->num; i++)
    {  /*����ť��������Ϊ�׵׺���*/
        pos.X = pHotArea->pArea[i].Left;
        pos.Y = pHotArea->pArea[i].Top;
        width = pHotArea->pArea[i].Right - pHotArea->pArea[i].Left + 1;
        if (pHotArea->pSort[i] == 0)
        {  /*�����ǰ�ť��*/
            FillConsoleOutputAttribute(gh_std_out, att2, width, pos, &ul);
        }
    }

    pos.X = pHotArea->pArea[iHot-1].Left;
    pos.Y = pHotArea->pArea[iHot-1].Top;
    width = pHotArea->pArea[iHot-1].Right - pHotArea->pArea[iHot-1].Left + 1;
    if (pHotArea->pSort[iHot-1] == 0)
    {  /*�����������ǰ�ť��*/
        FillConsoleOutputAttribute(gh_std_out, att1, width, pos, &ul);
    }
    else if (pHotArea->pSort[iHot-1] == 1)
    {  /*�������������ı�����*/
        SetConsoleCursorPosition(gh_std_out, pos);
        GetConsoleCursorInfo(gh_std_out, &lpCur);
        lpCur.bVisible = TRUE;
        SetConsoleCursorInfo(gh_std_out, &lpCur);
    }
}

/**
 * ��������: ExeFunction
 * ��������: ִ�������˵��ź��Ӳ˵���ȷ���Ĺ��ܺ���.
 * �������: m ���˵����
 *           s �Ӳ˵����
 * �������: ��
 * �� �� ֵ: BOOL����, TRUE �� FALSE
 *
 * ����˵��: ����ִ�к���ExitSysʱ, �ſ��ܷ���FALSE, ������������Ƿ���TRUE
 */
BOOL ExeFunction(int m, int s)
{
    BOOL bRet = TRUE;
    /*����ָ�����飬����������й��ܺ�������ڵ�ַ*/
    BOOL (*pFunction[ga_sub_menu_count[0]+ga_sub_menu_count[1]+ga_sub_menu_count[2]+ga_sub_menu_count[3]+ga_sub_menu_count[4]])(void);
    int i, loc;

    /*�����ܺ�����ڵ�ַ�����빦�ܺ����������˵��ź��Ӳ˵��Ŷ�Ӧ�±������Ԫ��*/
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

    for (i=1,loc=0; i<m; i++)  /*�������˵��ź��Ӳ˵��ż����Ӧ�±�*/
    {
        loc += ga_sub_menu_count[i-1];
    }
    loc += s - 1;

    if (pFunction[loc] != NULL)
    {
        bRet = (*pFunction[loc])();  /*�ú���ָ�������ָ��Ĺ��ܺ���*/
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
    for(psch_node=hd;psch_node!=NULL;psch_node=psch_node->next)/*����ѧУ������Ϣ*/
    {
        fwrite(psch_node,sizeof(SCH_NODE),1,pfout);
    }
    fclose(pfout);

    pfout=fopen(gp_department_info_filename,"wb");
    for(psch_node=hd;psch_node!=NULL;psch_node=psch_node->next)/*����Ժϵ������Ϣ*/
    {
        pdep_node=psch_node->dnext;
        while(pdep_node!=NULL){
            fwrite(pdep_node,sizeof(DEP_NODE),1,pfout);
            pdep_node=pdep_node->next;
        }
    }
    fclose(pfout);

    pfout=fopen(gp_thesis_info_filename,"wb");
    for(psch_node=hd;psch_node!=NULL;psch_node=psch_node->next)/*�������ķ��������Ϣ*/
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

    char *pCh[]={"���ݱ���ɹ���","ȷ��"};
    PopPrompt(pCh);
    return bRet;
}

BOOL BackupData(void)
{
    BOOL bRet = TRUE;
    char s[30];
    printf("�������ļ����ƣ�  ����8.26.bck\n");
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

    for(psch_node=phd;psch_node!=NULL;psch_node=psch_node->next)  /*����������ڵ���*/
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

   /*����������������*/
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

    char *pCh[]={"�����Ѿ�����������Ŀ¼�£�","ȷ��"};
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
    char *pCh[] = {"�˲��������ǵ�ǰδ�������Ϣ��������", "����    ȡ��"};
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
        printf("��������Ҫ�ָ����ļ����ƣ�\n");
        scanf("%s",filename);
        if(fopen(filename,"r")==0) {
            char *pCh[]={"�����ļ������󣬴�ʧ�ܣ�","����"};
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
    char *pCh[] = {"ȷ���˳�ϵͳ��", "ȷ��    ȡ��"};
    int iHot = 1;

    pos.X = strlen(pCh[0]) + 6;
    pos.Y = 7;
    rcPop.Left = (SCR_COL - pos.X) / 2;
    rcPop.Right = rcPop.Left + pos.X - 1;
    rcPop.Top = (SCR_ROW - pos.Y) / 2;
    rcPop.Bottom = rcPop.Top + pos.Y - 1;

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
    printf("�����빦�����:\n");
    printf("1.��Ӹ�У������Ϣ\n");
    printf("2.�޸ĸ�У������Ϣ\n");
    printf("3.ɾ����У������Ϣ\n");
    printf("4.����\n\n");
    int num;
    scanf("%d",&num);
    switch(num){
        case 1 : schinfo1(&gp_head);break;
        case 2 : schinfo2(gp_head);break;
        case 3 : schinfo3(&gp_head);break;
        case 4 : break;
        default : {char*pCh[]={"��������","����"};PopPrompt(pCh);}
    }
    InitInterface();
    return bRet;
}

void schinfo1(SCH_NODE **phd) /*������Ӹ�У��Ϣ*/
{
    SCH_NODE *p1,*p=*phd;
    int num;
    printf("��������Ӹ�У����Ŀ��");
    scanf("%d",&num);
    printf("�밴�����¸�ʽ¼�������Ϣ��\n");
    printf("ѧУ���|           ѧУ����          |    ������    |   ��ϵ�绰   |\n");
    while(num--){
        p1=(SCH_NODE *)malloc(sizeof(SCH_NODE));
        scanf("%s %s %s %s",p1->sch_id,p1->name,p1->manager,p1->phone);
        p1->dnext=NULL;
        p1->next=p;
        p=p1;
    }
    *phd=p;
    char *pCh[]={"��ӳɹ���","����"};PopPrompt(pCh);
}

void schinfo2(SCH_NODE *hd) /*�����޸ĸ�У��Ϣ*//*�����޸�����ͷλ�ã���ʹ��*hdָ�뼴��*/
{
    printf("��������Ҫ�޸ĵĸ�У��ţ�");
    SCH_NODE *p=hd;
    char s[8];
    scanf("%s",s);
    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
        printf("ԭ��Ϣ���£�\n\n");
        printf("ѧУ���|                     ѧУ����|        ������|      ��ϵ�绰|\n");
        printf("%8s|%29s|%14s|%14s|\n",p->sch_id,p->name,p->manager,p->phone);
        printf("������¼���У������Ϣ��\n");
        scanf("%s %s %s %s",p->sch_id,p->name,p->manager,p->phone);
        char*pCh[]={"�޸ĳɹ���","����"};PopPrompt(pCh);
    }
    else {char*pCh[]={"����ʧ�ܣ�","����"};PopPrompt(pCh);}

}

void schinfo3(SCH_NODE **phd)/*ɾ����У������Ϣ*/
{
    printf("��������Ҫɾ���ĸ�У��ţ�");
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
            if(pprior==NULL){  /*�������ͷ*/
                *phd=pcurrent->next;
            }
            else{
                pprior->next=pcurrent->next;
            }
            p1current=pcurrent->dnext;
            free(pcurrent); /*�ͷŸø�У��Ϣ*/
            while(p1current!=NULL){
                p2current=p1current->tnext;
                p1prior=p1current;
                while(p2current!=NULL){
                    p2prior=p2current;
                    p2current=p2current->next;
                    free(p2prior);/*ʩ����������Ϣ*/
                }
                p1current=p1current->next;
                free(p1prior);/*�ͷ�Ժϵ����Ϣ*/
            }
        char*pCh[]={"ɾ���ɹ���","����"};PopPrompt(pCh);
    }
    else {char*pCh[]={"����ʧ�ܣ�","����"};PopPrompt(pCh);}

}

BOOL MaintainDepInfo(void)
{
    BOOL bRet = TRUE;
    printf("�����빦�����:\n");
    printf("1.���Ժϵ������Ϣ\n");
    printf("2.�޸�Ժϵ������Ϣ\n");
    printf("3.ɾ��Ժϵ������Ϣ\n");
    printf("4.����\n\n");
    int num;
    scanf("%d",&num);
    switch(num){
        case 1 : depinfo1(gp_head);break;
        case 2 : depinfo2(gp_head);break;
        case 3 : depinfo3(gp_head);break;
        case 4 : break;
        default : {char*pCh[]={"��������","����"};PopPrompt(pCh);}
    }
    InitInterface();
    return bRet;
}

void depinfo1(SCH_NODE *hd) /*���Ժϵ��Ϣ*/
{
    printf("Process 1: ������Ժϵ����ѧУ�ı�ţ�");
    char s[8];
    int num;
    scanf("%s",s);
    SCH_NODE *p=hd;
    DEP_NODE *p1,*pt;
    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
        printf("��У��Ϣ���£�\n\n");
        printf("ѧУ���|                     ѧУ����|        ������|      ��ϵ�绰|\n");
        printf("%8s|%29s|%14s|%14s|\n",p->sch_id,p->name,p->manager,p->phone);
        printf("Process 2: ���������Ժϵ����Ŀ��");
        scanf("%d",&num);
        printf("Process Final: ��������Ҫ��ӵ�Ժϵ��Ϣ��\n");
        printf("    Ժϵ����|    ��ʦ����|    ��������|    ����������|    �о�������|\n");
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
        printf("\n�밴��������أ�");
        getch();
    }
    else {char*pCh[]={"����ʧ�ܣ�","����"};PopPrompt(pCh);}
}

void depinfo2(SCH_NODE *hd) /*�޸�Ժϵ��Ϣ*/
{
    printf("Process 1: ������Ժϵ����ѧУ�ı�ţ�");
    char s[8];
    char name[20];
    scanf("%s",s);
    SCH_NODE *p=hd;
    DEP_NODE *p1;
    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
        printf("\nProcess 2: ��������Ҫ�޸ĵ�Ժϵ���ƣ�");
        scanf("%s",name);
        p1=SeekdepNode1(p,name);
        printf("ԭ��Ժϵ��Ϣ���£�\n\n");
        printf("ѧУ���|          Ժϵ����|  ��ʦ����|  ��������|    ����������|    �о�������|");
        if(p1!=NULL){
             printf("%8s|%18s|%10d|%10d|%14d|%14d|\n",p1->sch_id,p1->name,p1->teacher,p1->professor,p1->aprofessor,p1->grastu);
             printf("�������޸ĺ��Ժϵ��Ϣ��\n");
             printf("          Ժϵ����|  ��ʦ����|  ��������|    ����������|    �о�������|\n");
             scanf("%s %d %d %d %d",p1->name,&p1->teacher,&p1->professor,&p1->aprofessor,&p1->grastu);
        }
        else{
            printf("�����Ժϵ��Ϣ�����޸ģ�\n");
        }
        printf("\n�밴��������أ�");
        getch();
    }
    else {char*pCh[]={"����ʧ�ܣ�","����"};PopPrompt(pCh);}
}

void depinfo3(SCH_NODE *hd)/*ɾ��Ժϵ��Ϣ*/
{
    printf("Process 1: ������Ժϵ����ѧУ�ı�ţ�");
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
        printf("\nԭ��Ժϵ��Ϣ���£�\n\n");
        printf("ѧУ���|          Ժϵ����|  ��ʦ����|  ��������|    ����������|    �о�������|");
        pt=p->dnext;
        while(pt!=NULL){
            printf("%8s|%18s|%10d|%10d|%14d|%14d|",pt->sch_id,pt->name,pt->teacher,pt->professor,pt->aprofessor,pt->grastu);
            pt=pt->next;
        }
        printf("\nProcess 2: ��������Ҫɾ����Ժϵ���ƣ�");
        scanf("%s",name);
        getchar();

    /*����ɾ���ڵ�*/
        pdcurrent=p->dnext;
        pdprior = NULL;
        while(pdcurrent!=NULL&&strcmp(pdcurrent->name,name)){
            pdprior=pdcurrent;
            pdcurrent=pdcurrent->next;
        }

        if(pdcurrent!=NULL){
             printf("ȷ��ɾ������Yȷ��\n");
             char c;
             scanf("%c",&c);
             if(c=='Y'){/*��ʼִ��ɾ������*/
                if(pdprior==NULL){/*��ɾ��������ͷ*/
                    p->dnext=pdcurrent->next;
                }
                else pdprior->next=pdcurrent->next;

                ptcurrent=pdcurrent->tnext;
                free(pdcurrent);/*�ͷ�Ժϵ��Ϣ��*/

                while(ptcurrent!=NULL){/*ѭ���ͷŸ�Ժϵ���ϵ�������Ϣ��*/
                    ptprior=ptcurrent;
                    ptcurrent=ptcurrent->next;
                    free(ptprior);
                }
                printf("---------OK!");
             }
             else printf("---------False");
        }
        else{
            printf("�����Ժϵ��Ϣ����ɾ����\n");
        }
        printf("\n�밴��������أ�");
        getch();
    }
    else {char*pCh[]={"����ʧ�ܣ�","����"};PopPrompt(pCh);}
}

DEP_NODE *SeekdepNode1(SCH_NODE *hd,char *name)/*����ָ����У�ڵ�������Ϊname��Ժϵ��Ϣ*/
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

int SeekdepNode2(SCH_NODE *hd,int num)/*�о����и�У�ڵ��н�ʦ��������Ϊnum��Ժϵ��Ϣ��������Ѱ����Ŀ*/
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
    printf("�����빦�����:\n");
    printf("1.������ķ�����Ϣ\n");
    printf("2.�޸����ķ�����Ϣ\n");
    printf("3.ɾ�����ķ�����Ϣ\n");
    printf("4.����\n\n");
    int num;
    scanf("%d",&num);
    switch(num){
        case 1 : theinfo1(gp_head);break;
        case 2 : theinfo2(gp_head);break;
        case 3 : theinfo3(gp_head);break;
        case 4 : break;
        default : {char*pCh[]={"��������","����"};PopPrompt(pCh);}
    }
    InitInterface();

    return bRet;
}

void theinfo1(SCH_NODE *hd)/*������ķ�����Ϣ*/
{
    printf("Process 1: ������Ժϵ����ѧУ�ı�ţ�");
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
        printf("Process 2: ������Ժϵ���ƣ�");
        scanf("%s",name);
        p1=SeekdepNode1(p,name);
        if(p1!=NULL){
             printf("Process 3: ������������ķ�����Ϣ��Ŀ��");
             int n;
             scanf("%d",&n);
             printf("Process Final: ���������ķ�����Ϣ��\n");
             printf("         ������Ŀ          |   ��һ����   |        ����        |   �ڴ�   |����|");
             while(n--){
                pt=(THE_NODE *)malloc(sizeof(THE_NODE));
                scanf("%s %s %s %s %c",pt->title,pt->writer,pt->ISSN,pt->date,&pt->Rank);
                strcpy(pt->sch_id,s);
                strcpy(pt->depart,name);
                p2=SeektheNode1(p1,pt->title); /*����Ƿ����������ķ�����Ϣ*/
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
            printf("�����Ժϵ��Ϣ��\n");
        }
        printf("\n�밴��������أ�");
        getch();
    }
    else {char*pCh[]={"����ʧ�ܣ�","����"};PopPrompt(pCh);}

}

void theinfo2(SCH_NODE *hd) /*�޸�ָ�����ķ�����Ϣ*/
{
    printf("Process 1: ������Ժϵ����ѧУ�ı�ţ�");
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
        printf("Process 2: ������Ժϵ���ƣ�");
        scanf("%s",name);
        p1=SeekdepNode1(p,name);
        if(p1!=NULL){
             printf("Process 3: ��������Ҫ�޸ĵ����ķ�����Ϣ��Ŀ��");
             char title[50];
             scanf("%s",title);
             p2=SeektheNode1(p1,title);/*����������Ϣ*/
             if(p2!=NULL){
                printf("ԭ��Ϣ���£�\n");
                printf("         ������Ŀ          |   ��һ����   |        ����        |   �ڴ�   |����|");
                printf("%27s|%14s|%20s|%10s|   %c|",p2->title,p2->writer,p2->ISSN,p2->date,p2->Rank);
                printf("Process Final: �������µ����ķ�����Ϣ��\n");
                printf("         ������Ŀ          |   ��һ����   |        ����        |   �ڴ�   |����|");
                scanf("%s %s %s %s %c",p2->title,p2->writer,p2->ISSN,p2->date,&p2->Rank);
                strcpy(p2->sch_id,s);
                strcpy(p2->depart,name);
                printf("------OK!\n");
             }
             else printf("����������Ϣʧ�ܣ�\n");
          }
        else{
            printf("�����Ժϵ��Ϣ��\n");
        }
        printf("\n�밴��������أ�");
        getch();
    }
    else {char*pCh[]={"����ʧ�ܣ�","����"};PopPrompt(pCh);}
}

void theinfo3(SCH_NODE *hd)/*ɾ��ָ��������Ϣ��*/
{
    printf("Process 1: ������Ժϵ����ѧУ�ı�ţ�");
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
        printf("Process 2: ������Ժϵ���ƣ�");
        scanf("%s",name);
        p1=SeekdepNode1(p,name);
        if(p1!=NULL){
             printf("Process Final: ��������Ҫɾ�������ķ�����Ϣ��Ŀ��");
             char title[50];
             scanf("%s",title);
             ptcurrent=p1->tnext;
             while(ptcurrent!=NULL&&strcmp(ptcurrent->title,title)!=0){
                ptprior=ptcurrent;
                ptcurrent=ptcurrent->next;
             }
             if(ptcurrent!=NULL){
                if(ptprior==NULL){/*�������ͷ*/
                    p1->tnext=ptcurrent->next;
                }
                else{
                    ptprior->next=ptcurrent->next;
                }
                free(ptcurrent);
                printf("------OK!\n");
             }
             else printf("����������Ϣʧ�ܣ�\n");
          }
        else{
            printf("�����Ժϵ��Ϣ��\n");
        }
        printf("\n�밴��������أ�");
        getch();
    }
    else {char*pCh[]={"����ʧ�ܣ�","����"};PopPrompt(pCh);}
}

THE_NODE *SeektheNode1(DEP_NODE *pd,char *title)/*����ָ��Ժϵ�ڵ���������ĿΪtitle��������Ϣ*/
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
    printf("�������ѯ��ʽ:\n");
    printf("1.����У��Ų�ѯ\n");
    printf("2.����У���Ʋ�ѯ\n");
    printf("3.��ʾ���и�У��Ϣ\n");
    printf("4.����\n");
    int num;
    scanf("%d",&num);
    switch(num){
        case 1 : schquery1(gp_head);break;
        case 2 : schquery2(gp_head);break;
        case 3 : schquery3(gp_head);break;
        case 4 : break;
        default : {char*pCh[]={"��������","����"};PopPrompt(pCh);}
    }
    InitInterface();
    return TRUE;
}

void schquery1(SCH_NODE *hd)/*����У���*/
{
    printf("��������Ҫ��ѯ�ĸ�У��ţ�");
    SCH_NODE *p=hd;
    char s[8];
    scanf("%s",s);

    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
        printf("��ѯ�������:\n\n");
        printf("ѧУ���|                     ѧУ����|        ������|      ��ϵ�绰|\n");
        printf("%8s|%29s|%14s|%14s|\n",p->sch_id,p->name,p->manager,p->phone);
        printf("\n�밴��������أ�");
        getch();
    }
    else {char*pCh[]={"����ʧ�ܣ�","����"};PopPrompt(pCh);}

}

void schquery2(SCH_NODE *hd)  /*ģ����ѯ����*/
{
    printf("��������Ҫ��ѯ�ĸ�У���ƹؼ��֣�");
    SCH_NODE *p=hd;
    char Keywords[30];
    scanf("%s",Keywords);
    int flag = 0;
    printf("��ѯ�������:\n\n");
    printf("ѧУ���|                     ѧУ����|        ������|      ��ϵ�绰|\n");
    while(p!=NULL){
        if(strstr(p->name,Keywords)){
            printf("%8s|%29s|%14s|%14s|\n",p->sch_id,p->name,p->manager,p->phone);
            flag = 1;
        }
        p=p->next;
    }
    if(flag){
        printf("\n�밴��������أ�");
        getch();
    }
    else {char*pCh[]={"����ʧ�ܣ�","����"};PopPrompt(pCh);}


}

void schquery3(SCH_NODE *hd) /*ȫ����ʾ*/
{
    printf("���н������:\n\n");
    printf("ѧУ���|                     ѧУ����|        ������|      ��ϵ�绰|\n");
    SCH_NODE *p=hd;
    if(p==NULL){printf("����Ϣ��");printf("\n�밴��������أ�");
        getch();}
    else{
        while(p!=NULL){
            printf("%8s|%29s|%14s|%14s|\n",p->sch_id,p->name,p->manager,p->phone);
            p=p->next;
        }
        printf("\n�밴��������أ�");
        getch();
    }
}


BOOL QueryDepInfo(void)
{
    printf("�������ѯ��ʽ:\n");
    printf("1.����У����Լ�Ժϵ���Ʋ�ѯ\n");
    printf("2.����ʦ������ѯ\n");
    printf("3.��ʾ����Ժϵ��Ϣ\n");
    printf("4.����\n");
    int num;
    scanf("%d",&num);
    switch(num){
        case 1 : depquery1(gp_head);break;
        case 2 : depquery2(gp_head);break;
        case 3 : depquery3(gp_head);break;
        case 4 : break;
        default : {char*pCh[]={"��������","����"};PopPrompt(pCh);}
    }
    InitInterface();
    return TRUE;
}

void depquery1(SCH_NODE *hd)/*����У��ż�Ժϵ���Ʋ�ѯ*/
{
    printf("Process 1: ������Ժϵ����ѧУ�ı�ţ�");
    char s[8];
    char name[20];
    scanf("%s",s);
    SCH_NODE *p=hd;
    DEP_NODE *p1;
    while(p!=NULL&&strcmp(p->sch_id,s)!=0){
        p=p->next;
    }
    if(p!=NULL){
        printf("Process 2: ��������Ҫ��ѯ��Ժϵ���ƣ�");
        scanf("%s",name);
        p1=SeekdepNode1(p,name);
        printf("Ժϵ��Ϣ���£�\n\n");
        printf("ѧУ���|          Ժϵ����|  ��ʦ����|  ��������|    ����������|    �о�������|");
        if(p1!=NULL){
             printf("%8s|%18s|%10d|%10d|%14d|%14d|\n",p1->sch_id,p1->name,p1->teacher,p1->professor,p1->aprofessor,p1->grastu);
        }
        else{
            printf("�����Ժϵ��Ϣ��\n");
        }
        printf("\n�밴��������أ�");
        getch();
    }
    else {char*pCh[]={"����ʧ�ܣ�","����"};PopPrompt(pCh);}
}

void depquery2(SCH_NODE *hd)/*����ʦ������ѯ*/
{
    printf("�˲�������ʾ���г���ָ����ʦ������Ժϵ��\n");
    printf("�������ʦ������");
    int teacher,find;
    scanf("%d",&teacher);
    printf("Ժϵ��Ϣ���£�\n\n");
    printf("ѧУ���|          Ժϵ����|  ��ʦ����|  ��������|    ����������|    �о�������|");
    find=SeekdepNode2(gp_head,teacher);
    printf("\n----------��%d�������",find);
    printf("\n�밴��������أ�");
    getch();
}

void depquery3(SCH_NODE *hd) /*��ʾ����Ժϵ��Ϣ*/
{
    SCH_NODE *psch_node;
    DEP_NODE *pdep_node;
    printf("����Ժϵ��Ϣ���£�\n\n");
    printf("ѧУ���|          Ժϵ����|  ��ʦ����|  ��������|    ����������|    �о�������|"); /*����80��ȣ��Զ����У����ӻ��з�*/
    for(psch_node=hd;psch_node!=NULL;psch_node=psch_node->next){
        pdep_node=psch_node->dnext;
        while(pdep_node!=NULL){
                printf("%8s|%18s|%10d|%10d|%14d|%14d|",pdep_node->sch_id,pdep_node->name,pdep_node->teacher,pdep_node->professor,pdep_node->aprofessor,pdep_node->grastu);
                pdep_node=pdep_node->next;
            }
        }
    printf("\n�밴��������أ�");
    getch();
}

BOOL QueryTheInfo(void)
{
    BOOL bRet = TRUE;
    printf("�������ѯ��ʽ:\n");
    printf("1.��������Ŀ��ѯ\n");
    printf("2.����һ��������ѯ\n");
    printf("3.����\n");
    int num;
    scanf("%d",&num);
    switch(num){
        case 1 : thequery1(gp_head);break;
        case 2 : thequery2(gp_head);break;
        case 3 : break;
        default : {char*pCh[]={"��������","����"};PopPrompt(pCh);}
    }
    InitInterface();
    return bRet;
}

void thequery1(SCH_NODE *hd)/*��������*/
{
    printf("�������������ƣ�");
    char title[50];
    scanf("%s",title);
    SCH_NODE *p;
    DEP_NODE *pd;
    THE_NODE *pt;
    int find = 0;

    for(p=hd;p!=NULL;p=p->next){/*��������*/
        for(pd=p->dnext;pd!=NULL;pd=pd->next){/*����Ժϵ��*/
            pt=pd->tnext;
            while(pt!=NULL){/*����������*/
            if(strcmp(pt->title,title)==0){
            printf("��У��ţ�  %s   ��У���ƣ�  %s   ",p->sch_id,p->name);
            printf("Ժϵ���ƣ�  %s\n",pd->name);
            printf("         ������Ŀ          |   ��һ����   |        ����        |   �ڴ�   |����|");
            printf("%27s|%14s|%20s|%10s|   %c|",pt->title,pt->writer,pt->ISSN,pt->date,pt->Rank);
            find ++;
            }
            pt=pt->next;
           }
        }
    }
    printf("------��%d�������\n",find);
    printf("\n�밴��������أ�");
    getch();
}

void thequery2(SCH_NODE *hd)/*����һ������*/
{
    printf("�������һ�������ƣ�");
    char writer[15];
    scanf("%s",writer);
    SCH_NODE *p;
    DEP_NODE *pd;
    THE_NODE *pt;
    int find = 0;

    for(p=hd;p!=NULL;p=p->next){/*��������*/
        for(pd=p->dnext;pd!=NULL;pd=pd->next){/*����Ժϵ��*/
            pt=pd->tnext;
            while(pt!=NULL){/*����������*/
            if(strcmp(pt->writer,writer)==0){
            printf("��У��ţ�  %s   ��У���ƣ�  %s   ",p->sch_id,p->name);
            printf("Ժϵ���ƣ�  %s\n",pd->name);
            printf("         ������Ŀ          |   ��һ����   |        ����        |   �ڴ�   |����|");
            printf("%27s|%14s|%20s|%10s|   %c|",pt->title,pt->writer,pt->ISSN,pt->date,pt->Rank);
            find ++;
            }
            pt=pt->next;
           }
        }
    }
    printf("------��%d�������\n",find);
    printf("\n�밴��������أ�");
    getch();
}

BOOL Stat1(void)/*ͳ�Ƹ���У��ʦ������������������ͽ�ʦ�˾������������������1λС����������ʦ�˾������������������������ͳ�ƽ����*/
{
    BOOL bRet = TRUE;
    State1(gp_head);
    printf("\n�밴��������أ�");
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
            num1 = pd->teacher;/*��ʦ����*/
            for(pt=pd->tnext;pt!=NULL;pt=pt->next){
                num2++;/*������*/
            }
            num3=(float)num2/(float)num1; /*ǿ��ת�ͣ�����С��*/
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

    printf("ѧУ���|                      ѧУ����|  ��ʦ����|    ������|  �˾�������|\n");

    /*ѡ������*/
    Stat1_NODE *p1cur,*p1prior,*p1after,*p1temp;
    p1prior=p1;
    if(p1prior==NULL){
        printf("------����Ϣ��");
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

            if(p1cur!=p1prior){/*�����ڲ���Ϣ��������ָ����*/
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
    printf("\n�밴��������أ�");
    getch();
    InitInterface();
    return bRet;
}

void State2(SCH_NODE *hd)/*ͳ��ĳ��Уĳ��ȷ���ġ�a������b������c������d��������������*/
{
    printf("Process 1: �������У��ţ�");
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
    printf("Process 2: ������������ݣ�");
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
        printf("\nͳ����ݣ�   %s   ��У���ƣ�   %s\n",date,p->name);
        printf("����ͳ�ƣ�   a�ࣺ %dƪ\n             b�ࣺ %dƪ\n             c�ࣺ %dƪ\n             d�ࣺ %dƪ\n",a,b,c,d);
    }
    else printf("�����У�������\n");
}

BOOL Stat3(void)
{
    BOOL bRet = TRUE;
    State3(gp_head);
    printf("\n�밴��������أ�");
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
                if(pt->Rank=='a') num++;/*ͳ��a��������*/
            }
        }

        ptemp=(Stat3_NODE *)malloc(sizeof(Stat3_NODE));

        ptemp->next=p1;

        strcpy(ptemp->name,p->name);
        strcpy(ptemp->sch_id,p->sch_id);
        ptemp->num=num;
        p1=ptemp;
    }

    /*ѡ������*/
    Stat3_NODE *p1cur,*p1prior,*p1after,*p1temp;
    p1prior=p1;
    printf("ͳ�ƽ�����£�\n");
    printf("ѧУ���|                      ѧУ����|                       a������Ŀ|\n");

    if(p1prior==NULL){
        printf("------�޸�У��Ϣ��");
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

            if(p1cur!=p1prior){/*�����ڲ���Ϣ��������ָ����*/
                *p1temp=*p1prior;
                *p1prior=*p1cur;
                p1prior->next=p1temp->next;
                p1temp->next=p1cur->next;
                *p1cur=*p1temp;
            }
            p1prior=p1prior->next;
        }
        free(p1temp);

        num = 5;/*ֻͳ��ǰ5��*/

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
    printf("\n�밴��������أ�");
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
    printf("��������ݣ�");
    scanf("%s",date);
    int num1; /*�о�����*/
    int num2; /*������*/
    float num3;
    for(p=hd;p!=NULL;p=p->next){
            num1 = 0;
            num2 = 0;
            num3 = 0;
        for(pd=p->dnext;pd!=NULL;pd=pd->next){
            num1 = pd->grastu;/*�о�������*/
            for(pt=pd->tnext;pt!=NULL;pt=pt->next){
                if(strstr(pt->date,date)) num2++;/*������*/
            }
            num3=(float)num2/(float)num1; /*ǿ��ת�ͣ�����С��*/
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

    printf("ѧУ���|            Ժϵ����|  �о�������|    ������|  �˾�������|\n");

    /*ѡ������*/
    Stat4_NODE *p1cur,*p1prior,*p1after,*p1temp;
    p1prior=p1;
    if(p1prior==NULL){
        printf("------����Ϣ��");
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

            if(p1cur!=p1prior){/*�����ڲ���Ϣ��������ָ����*/
                *p1temp=*p1prior;
                *p1prior=*p1cur;
                p1prior->next=p1temp->next;
                p1temp->next=p1cur->next;
                *p1cur=*p1temp;
            }
            p1prior=p1prior->next;
        }
        free(p1temp);

        int n =10;/*ֻͳ��ǰ10��*/

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
    printf("\n�밴��������أ�");
    getch();
    InitInterface();
    return bRet;
}

void State5(SCH_NODE *hd)
{
    printf("�����������Ϣ��");
    char date[10];
    scanf("%s",date);
    SCH_NODE *p;
    DEP_NODE *pd;
    THE_NODE *pt;
    int find = 0;
    printf("                   ������Ŀ|      ��һ����|                ����|      �ڴ�|����|");
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
    printf("\n------����%d����¼",find);
}

BOOL HelpTopic(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"�����κ�������Ҫ����,����ϵ",
                           "   ����:382825415@qq.com",
                           "ȷ��"
                          };

    ShowModule(plabel_name, 3);

    return bRet;
}

BOOL About(void)
{
    BOOL bRet = TRUE;
    char *plabel_name[] = {"���пƼ���ѧ�����ϵ",
                           " CS1401������������",
                           "ȷ��"
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

    att = BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED;  /*�׵׺���*/
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
