#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <windows.h>
#include <conio.h>  

// ===================== 全局变量 =====================
int check_login_success = 0; // 在欢迎标题显示用户名称时会用到
char user_number[12];        // 在欢迎标题显示用户账号名时会用到

// ===================== 结构体定义 =====================
struct sysuser // 定义sysuser结构体
{
    char usernum[12]; // 学号或手机号 + '\0'
    char password[8];
    struct sysuser *next; // 指向下一个用户
};

struct book // 定义book结构体
{
    int number;          // 编号
    char name[30];       // 书名
    char category[10];   // 类别
    int lent;            // 是否借出（已借为1，未借为0）
    int year;            // 借出时间--年
    int month;           // 借出时间--月
    int day;             // 借出时间--日
    struct book *next;   // 指向下一个book
};

// 借阅记录结构体 
struct borrow_record {
    char usernum[12];
    int book_number;
    char book_name[30];
    int year;
    int month;
    int day;
    int returned; // 0/1
    struct borrow_record *next;
};

// ===================== 全局常量 =====================
static const int   BORROW_LIMIT_DAYS = 30;
static const double FINE_RATE_PER_DAY = 0.1; // 超一天 0.1 元

// ===================== 前置声明 =====================
void print_main_title();

// user.txt 相关
int  check_usernum(struct sysuser *user);
int  create_user();
int  checkUserValid(struct sysuser *user);
int  login();

// library.txt 相关
int         check_void_file();
struct book* make_linklist();
int         input_new_book(struct book *head);
struct book* delete_book(struct book *head, struct book *p);
struct book* modify_book(struct book *head, struct book *t);
void        override_to_file(struct book *head);
struct book* search_by_number(struct book *head, int number);
struct book* search_by_name(struct book *head, char *name);
int         search_by_category(struct book *head, char *category);
struct book* search_by_time(struct book *head, int year, int month, int day);
void        print_booklist_title();
void        print_booknode(struct book *t);
int         print_all_book(struct book *head);

// sysuser linklist（user.txt）
int              check_sysuser_void_file();
struct sysuser*  make_sysuser_linklist();
struct sysuser*  search_by_usernum(struct sysuser *sysuser_head, char *del_sysuser_num);
struct sysuser*  delete_sysuser(struct sysuser *sysuser_head, struct sysuser *p);
void             override_to_sysuser_file(struct sysuser *sysuser_head);
void             print_sysuserlist_title();
void             print_sysusernode(struct sysuser *m);
int              print_all_sysuser(struct sysuser *sysuser_head);

// 借阅记录 borrow_record.txt 相关
int                  check_borrow_void_file();
struct borrow_record* make_borrow_linklist();
void                 append_borrow_record(const char *usernum, struct book *b, int y, int m, int d, int returned);
void                 override_borrow_file(struct borrow_record *head);
void                 print_borrow_title();
void                 print_one_borrow(struct borrow_record *r, int showFine);
int                  days_between_dates(int y1,int m1,int d1,int y2,int m2,int d2);
double               calc_overdue_fine(int by,int bm,int bd,int returned);
void                 list_all_borrow_records();
void                 list_borrow_records_by_user(const char *usernum);
int                  mark_record_returned(const char *usernum, int book_number);
void                 print_borrow_rules();

// 读者查询和增强借书
void reader_search_books(struct book *head);
void enhanced_borrow_book(struct book *head);

// ===================== 函数实现 =====================

// ===================== 欢迎标题 =====================
void print_main_title()
{
    time_t time_login;
    struct tm *p;
    time(&time_login);
    p = localtime(&time_login);

    if(check_login_success == 0)
    {
        printf("--------------------------------------------------------------------------------\n");
        printf("欢迎您，亲爱的用户！\n");
        printf("                                                                                \n");
        printf("                           欢迎使用图书馆管理系统                                 \n");
        printf("                                                                                \n");
        printf("                        现在时间：%d年%d月%d日 %02d:%02d:%02d\n",
               1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
        printf("                                                                                \n");
        printf("                                                                                \n");
        printf("--------------------------------------------------------------------------------\n");
    }
    else
    {
        printf("--------------------------------------------------------------------------------\n");
        printf("欢迎您，%s！\n",user_number);
        printf("                                                                                \n");
        printf("                           欢迎使用图书馆管理系统                             \n");
        printf("                                                                                \n");
        printf("                        现在时间：%d年%d月%d日 %02d:%02d:%02d\n",
               1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
        printf("                                                                                \n");
        printf("                                                                                \n");
        printf("--------------------------------------------------------------------------------\n");
    }
}

// ===================== user.txt 相关函数 =====================
int check_usernum(struct sysuser *user)
{
    int check=0;
    char usrno[12];
    strcpy(usrno, user->usernum);

    char usrno_f[12];
    FILE *fp = fopen("user.txt", "r+");
    if (fp == NULL) fp = fopen("user.txt", "w");
    if (fp == NULL) return 0;

    while(fscanf(fp, "%11s%*s", usrno_f) != EOF)
    {
        if(strcmp(usrno, usrno_f) == 0) { check = 1; break; }
    }
    fclose(fp);
    return check; // 被注册过返回1，没被注册过返回0
}

int create_user()
{
    struct sysuser su;
    FILE *fp = fopen("user.txt", "a");
    if (fp == NULL) fp = fopen("user.txt", "w");
create_loop:
    system("CLS");
    printf("\n");
    printf("===================================注册新用户===================================\n\n");
    printf("\n\n");
    printf("  【用户账号】为11位手机号，请确保您的将要用于注册的手机号未曾用于注册本系统\n\n");
    printf("           在校学生不用注册，可直接使用学号+初始密码123直接登录        \n\n");
    printf("    【密码】的长度应小于等于8位，仅允许使用英文字符或数字，不允许空格\n\n");
    printf("                p.s.为保护您的隐私，输入的密码不会显示在屏幕上\n\n");
    printf("================================================================================");
    printf("\n\n");

    printf("                请输入新的手机号，按回车结束：");
    scanf("%11s", su.usernum);
    printf("\n");

    if (check_usernum(&su)==0)
    {
        printf("                请输入新的密码，按回车结束：");
        int i=0;
        while(i<8 && (su.password[i]=getch())!='\r')
        {
            if(su.password[i]=='\b') { if(i>0) i--; }
            else { i++; if (i>8){ printf("\n           注意：已达8位上限！\n"); i--; } }
        }
        su.password[i]='\0';

        char check_password[8];
        printf("\n\n                请再次输入密码，按回车结束：");
        for(i=0;i<8;i++) check_password[i]='\0';
        i=0;  
        while(i<8 && (check_password[i]=getch())!='\r')
        {
            if(check_password[i]=='\b') { if(i>0) i--; }
            else { i++; if (i>8){ printf("\n           注意：已达8位上限！\n"); i--; } }
        }
        check_password[i]='\0';

        if (strcmp(check_password, su.password) != 0)
        {
            printf("\n\n                  两次密码输入不一致！请重新注册...\n");
            Sleep(2000);
            goto create_loop;
        }

        fprintf(fp, "%s %s\n", su.usernum, su.password);
        fclose(fp);
        printf("\n\n\n\n注册成功!请记住您的用户名：%s，并牢记您的密码！\n\n", su.usernum);
        printf("\n\n\n\n按任意键返回主菜单，选登录即可");
        getch();
        system("cls");
        return 1;
    }
    else 
    {
        printf("                这个手机号被注册过啦，请换个手机号来注册哦\n\n");
        printf("                按任意键返回");
        getch();
        goto create_loop;
    }
}

// 检测用户合法性（登录时调用）
int checkUserValid(struct sysuser *user)
{
    FILE *user_file_p = fopen("user.txt","r");
    if (user_file_p == NULL) user_file_p = fopen("user.txt","w");
    if (user_file_p == NULL) return 0;

    char usr[30], pwd[10];
    char usr_f[30];

    strcpy(usr, user->usernum);
    strcat(usr, " ");
    strcpy(pwd, user->password);
    strcat(usr, pwd);
    strcat(usr, "\n");

    while(feof(user_file_p) == 0)
    {
        if (fgets(usr_f, 30, user_file_p) == NULL) break;
        if(strcmp(usr, usr_f) == 0) { fclose(user_file_p); return 1; }
    }
    fclose(user_file_p);
    return 0;
}

// 登录功能
int login()
{
    struct sysuser su; char ch; int ok = 0;
    system("CLS");
    print_main_title();
    printf("\n\n");
    printf("                  请输入学号或手机号，按回车键结束：");
    scanf("%11s", su.usernum);
    printf("\n\n");
    printf("                       请输入小于等于8位的密码\n\n");   
    printf("         该密码输入时不会显示在屏幕上，输完时请按回车键结束哦\n\n");
    printf("                               请输入吧：");
    int i=0;
    while(i<8 && (su.password[i]=getch())!='\r')
    {
        if(su.password[i]=='\b') { if(i>0) i--; }
        else { i++; if (i>8){ printf("\n             注意：已达8位上限！\n\n"); i--; } }
    }
    su.password[i]='\0';

    //检测用户合法性 
    if(checkUserValid(&su) == 1)
    {
        strcpy(user_number, su.usernum);
        ok = 1;
        printf("\n\n\n                              登录成功！\n\n按任意键进入主菜单...");
        ch = getch(); (void)ch;
        system("cls");
    }
    else
    {
        printf("\n\n\n该用户不存在或密码错误，请核对后重新登录，或注册个新账号！\n\n如需帮助，请与管理员联系!\n\n按任意键返回主菜单...");
        ch = getch(); (void)ch;
        system("cls");
    }
    return ok;
}

// ===================== library.txt 相关函数 =====================
// 检查library.txt是否为空
int check_void_file()
{
    FILE *fp = fopen("library.txt", "r+");
    if (!fp) return 0;
    int ch = fgetc(fp);
    fclose(fp);
    return (ch==EOF)?0:1;
}

// 创建书籍链表
struct book *make_linklist()
{
    struct book *head = (struct book *)malloc(sizeof(struct book));
    head->next = NULL;
    FILE *fp = fopen("library.txt", "r+");
    if (fp == NULL) fp = fopen("library.txt", "w+");
    if (fp == NULL) return head;

    if (check_void_file() == 0)
    {
        // 空库
        fclose(fp);
        return head;
    }
    struct book *p = head; char ch;
    while(!feof(fp))
    {
        struct book *nb = (struct book *)malloc(sizeof(struct book));
        if (fscanf(fp, "%d", &nb->number)!=1) { free(nb); break; }
        ch = fgetc(fp);
        fscanf(fp, "%[^\n]%*c", nb->name);
        fscanf(fp, "%[^\n]%*c", nb->category);
        fscanf(fp, "%d", &nb->lent);
        ch = fgetc(fp);
        fscanf(fp, "%d", &nb->year); ch = fgetc(fp);
        fscanf(fp, "%d", &nb->month); ch = fgetc(fp);
        fscanf(fp, "%d", &nb->day); ch = fgetc(fp);
        nb->next = NULL;
        p->next = nb; p = p->next;
    }
    fclose(fp);
    return head;
}

// 增加书本，单链表的插入结点 
int input_new_book(struct book *head)
{
    FILE *fp; struct book newbook; struct book *p=head;
    if (p->next==NULL) { /* 空链表提示 */ }
input_loop:
    fp = fopen("library.txt", "a+");
    system("CLS");
    printf("\n===================================录入新图书===================================\n\n");
    printf("\n\n   请注意：编号要求为30位以内的纯数字\n\n");
    printf("            名字要求为30个字符以内\n\n");
    printf("            分类要求为10个字符以内\n\n");
    printf("\n");
    printf("================================================================================\n\n");
input_number_loop:
    printf("                        请输入新书的编号：");
    scanf("%d", &newbook.number);
    p = head; while (p->next != NULL) { p=p->next; if (newbook.number == p->number) { printf("\n该书已存在！请核对后重新输入\n"); system("PAUSE"); printf("\n"); p=head; goto input_number_loop; } }
    p=head;
    printf("\n");
input_name_loop:
    printf("                        请输入新书的名字：");
    fflush(stdin);
    scanf("%[^'\n']", newbook.name);
    p=head; while (p->next != NULL) { p=p->next; if (strcmp(p->name,newbook.name)==0){ printf("\n该书已存在！请核对后重新输入\n"); system("PAUSE"); printf("\n"); p=head; goto input_name_loop; } }
    p=head;
    fflush(stdin);
    printf("\n");
    printf("                        请输入新书的分类：");
    scanf("%[^'\n']", newbook.category);
    fflush(stdin);

    newbook.lent=0; newbook.year=0; newbook.month=0; newbook.day=0; newbook.next=NULL;
    fprintf(fp, "%d %s\n%s\n%d %d %d %d\n", newbook.number, newbook.name, newbook.category,
            newbook.lent,newbook.year,newbook.month,newbook.day);
    fclose(fp);
    printf("\n录入成功！\n\n编号：%d\n书名：%s\n分类：%s\n\n是否继续录入？ y：是  n：否 ", newbook.number, newbook.name, newbook.category);
    fflush(stdin); char ch; scanf(" %c", &ch);
    if (ch=='y') goto input_loop; else { printf("\n\n即将在3秒后返回主菜单...\n"); Sleep(3000); return 1; }
}

// 删除书本
struct book *delete_book(struct book *head, struct book *p)
{
    system("CLS"); print_booklist_title(); print_booknode(p);
    printf("                           警告！该操作不可恢复！\n\n");
    printf("                        WARNING!THIS CAN'T BE UNDONE!!\n\n");
    printf("                            是否继续？\n");
    printf("                            y：是  任意键：否\n");
    printf("                            请输入对应选项，按回车继续：");
    char ch; scanf(" %c", &ch);
    if (ch=='y'){
        struct book *q=head; while(q && q->next!=p) q=q->next; if(q){ q->next=p->next; free(p);} 
        printf("删除成功！\n"); system("pause");
    }
    return head;
}

// 修改书本 
struct book *modify_book(struct book *head, struct book *t)
{
    int i; char name_temp[30]={0}; char cate_temp[10]={0}; int num_temp=0;
    fflush(stdin); system("CLS");
    printf("\n==================================修改图书======================================");
    printf("\n\n                           警告！该操作不可恢复！\n\n");
    printf("\n============================这是你要修改的图书==================================");
    print_booklist_title(); print_booknode(t); 
	printf("-------------------------------------------------------------------------------\n\n");
    printf("\n-----------------------------请选择你要进行的操作-----------------------------"); 
    printf("\n\n"); 
    printf("                      1、修改名称\n\n");
    printf("                      2、修改编号\n\n");
    printf("                      3、修改分类\n\n");
    printf("                      4、返回\n\n");
    printf("请输入选项前的编号，按回车进入：");
    scanf("%d", &i);
    switch(i){
        case 1:
            printf("名称需在30个字符以内\n请输入新的名称：");
            fflush(stdin); gets(name_temp); strcpy(t->name, name_temp); printf("修改成功！"); system("PAUSE"); return head;
        case 2:
            printf("编号需全为数字，且在20位以内\n请输入新的编号：");
            fflush(stdin); scanf("%d", &t->number); printf("修改成功！"); system("PAUSE"); return head;
        case 3:
            printf("分类需在10个字符以内\n请输入新的分类：");
            fflush(stdin); gets(cate_temp); strcpy(t->category, cate_temp); printf("修改成功！"); system("PAUSE"); return head;
        default: return head;
    }
}

// 覆盖链表到library.txt，将清空原library.txt的数据
void override_to_file(struct book *head)
{
    FILE *fp = fopen("library.txt", "w"); if(!fp) return; 
    struct book *p = head->next;
    while(p){
        fprintf(fp, "%d %s\n%s\n%d %d %d %d\n", p->number, p->name, p->category,
                p->lent, p->year, p->month, p->day);
        p = p->next;
    }
    fclose(fp);
}

// 查找书本 - 按编号
struct book *search_by_number(struct book *head, int number)
{
    struct book *p = head->next; if(!p) return NULL;
    while(p){ if(p->number==number) return p; p=p->next; }
    return NULL;
}

// 查找书本 - 按书名
struct book *search_by_name(struct book *head, char *name)
{
    struct book *p = head->next; if(!p) return NULL;
    while(p){ if(strcmp(name,p->name)==0) return p; p=p->next; }
    return NULL;
}

// 查找书本 - 按分类
int search_by_category(struct book *head, char *category)
{
    struct book *p=head->next; int flag=0; while(p){ if(strcmp(category,p->category)==0){ print_booknode(p); flag=1; } p=p->next; }
    if(!flag){ printf("没有找到该分类的书！\n按任意键返回主菜单...\n"); getch(); return 0; }
    printf("按任意键返回主菜单...\n"); getch(); return 0;
}

// 查找书本 - 按时间
struct book *search_by_time(struct book *head, int year, int month, int day)
{
    int flag=0; struct book *p=head->next; while(p){ if(p->year==year && p->month==month && p->day==day){ print_booknode(p); flag=1; } p=p->next; }
    if(!flag) printf("没有找到书本！\n"); system("PAUSE"); return NULL;
}

// 输出书籍列表标题
void print_booklist_title()
{
    printf("您查找的书的资料是：\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("编号                书名                          分类               借出时间       \n");
    printf("--------------------------------------------------------------------------------\n");
}

// 输出单个书籍信息
void print_booknode(struct book *t)
{
    if(t){
        printf("%-20d%-30s%-10s", t->number, t->name, t->category);
        if(t->lent==1) printf("  %d年%d月%d日\n", t->year, t->month, t->day);
        else printf("  在库\n");
    }else{
        printf("没有找到该书！\n按任意键返回主菜单...\n"); getch();
    }
}

// 输出所有书籍
int print_all_book(struct book *head)
{
    system("CLS");
    printf("\n================================显示所有书本====================================\n\n");
    char ch; (void)ch; struct book *p=head->next; if(!p){ printf("\n数据库中没有书本！\n\n按任意键返回..."); ch=getch(); return 0; }
    print_booklist_title(); while(p){ print_booknode(p); p=p->next; }
    printf("\n按任意键返回上级菜单哦..."); ch=getch(); return 1;
}

// ===================== user.txt 操作函数 =====================
// 检查user.txt是否为空
int check_sysuser_void_file()
{
    FILE *fp = fopen("user.txt", "r+"); if(!fp) return 0; int ch=fgetc(fp); fclose(fp); return (ch==EOF)?0:1; 
}

// 创建用户链表
struct sysuser *make_sysuser_linklist()
{
    struct sysuser *head=(struct sysuser*)malloc(sizeof(struct sysuser)); head->next=NULL;
    FILE *fp=fopen("user.txt","r+"); if(!fp) fp=fopen("user.txt","w+"); if(!fp) return head;
    if(check_sysuser_void_file()==0){ fclose(fp); return head; }
    struct sysuser *p=head; char user[16], pwd[16];
    while(fscanf(fp, "%11s %7s", user, pwd)==2){ 
        struct sysuser *n=(struct sysuser*)malloc(sizeof(struct sysuser));
        strcpy(n->usernum,user); strcpy(n->password,pwd); n->next=NULL; p->next=n; p=p->next; 
    }
    fclose(fp); return head;
}

// 查找用户 - 按用户号
struct sysuser *search_by_usernum(struct sysuser *head, char *del_sysuser_num)
{
    struct sysuser *p=head->next; while(p){ if(strcmp(del_sysuser_num,p->usernum)==0) return p; p=p->next; } return NULL;
}

// 删除用户
struct sysuser *delete_sysuser(struct sysuser *head, struct sysuser *p)
{
    struct sysuser *q=head; while(q && q->next!=p) q=q->next; 
    if(q){ q->next=p->next; free(p);} 
    printf("删除成功！\n"); system("pause"); 
    return head;
}

// 覆盖用户链表到user.txt
void override_to_sysuser_file(struct sysuser *head)
{
    FILE *fp=fopen("user.txt","w"); if(!fp) return; 
    struct sysuser *p=head->next; 
    while(p){ 
        fprintf(fp, "%s %s\n", p->usernum, p->password); 
        p=p->next; 
    } 
    fclose(fp);
}

// 输出用户列表标题
void print_sysuserlist_title(){
    printf("您查找的用户的资料是：\n");
    printf("--------------------------------------------------------------------------------\n");
    printf("   用户账号                                                      密码           \n");
    printf("--------------------------------------------------------------------------------\n");
}

// 输出单个用户信息
void print_sysusernode(struct sysuser *m){ 
    if(m) printf("%-64s%-10s\n", m->usernum, m->password); 
    else { printf("没有找到该用户！\n按任意键返回主菜单...\n"); getch(); } 
}

// 输出所有用户
int print_all_sysuser(struct sysuser *head)
{
    system("CLS");
    printf("\n================================显示所有用户====================================\n\n");
    char ch; (void)ch; struct sysuser *p=head->next; if(!p){ printf("\n数据库中没有任何用户信息！\n\n按任意键返回..."); ch=getch(); return 0; }
    print_sysuserlist_title(); while(p){ print_sysusernode(p); p=p->next; }
    printf("\n按任意键返回上级菜单哦..."); ch=getch(); return 1;
}

// ===================== 借阅记录 borrow_record.txt 相关函数 =====================
// 检查借阅记录文件是否为空
int check_borrow_void_file(){ 
    FILE *fp=fopen("borrow_record.txt","r+"); if(!fp) return 0; 
    int ch=fgetc(fp); fclose(fp); return (ch==EOF)?0:1; 
}

// 创建借阅记录链表
struct borrow_record* make_borrow_linklist(){
    struct borrow_record *head=(struct borrow_record*)malloc(sizeof(struct borrow_record)); head->next=NULL;
    FILE *fp=fopen("borrow_record.txt","r+"); if(!fp) fp=fopen("borrow_record.txt","w+"); if(!fp) return head;
    if(check_borrow_void_file()==0){ fclose(fp); return head; }
    char line[256]; struct borrow_record *p=head;
    while(fgets(line, sizeof(line), fp)){
        struct borrow_record *r=(struct borrow_record*)malloc(sizeof(struct borrow_record));
        memset(r,0,sizeof(*r));
        if(sscanf(line, "%11s %d %29s %d %d %d %d", r->usernum, &r->book_number, r->book_name, &r->year, &r->month, &r->day, &r->returned)==7){
            r->next=NULL; p->next=r; p=p->next; 
        } else { free(r); }
    }
    fclose(fp); return head;
}

// 添加借阅记录
void append_borrow_record(const char *usernum, struct book *b, int y, int m, int d, int returned)
{
    FILE *fp=fopen("borrow_record.txt","a+"); if(!fp) fp=fopen("borrow_record.txt","w+"); if(!fp) return;
    fprintf(fp, "%s %d %s %d %d %d %d\n", usernum, b->number, b->name, y, m, d, returned);
    fclose(fp);
}

// 覆盖借阅记录文件
void override_borrow_file(struct borrow_record *head)
{
    FILE *fp=fopen("borrow_record.txt","w"); if(!fp) return; 
    struct borrow_record *p=head->next; 
    while(p){
        fprintf(fp, "%s %d %s %d %d %d %d\n", p->usernum, p->book_number, p->book_name, p->year, p->month, p->day, p->returned);
        p=p->next;
    } 
    fclose(fp);
}

// 输出借阅记录标题
void print_borrow_title(){
    printf("---------------------------------------------------------------------------------------\n");
    printf("用户账号       书号     书名               借出日期      状态  逾期天数      预计罚款\n");
    printf("---------------------------------------------------------------------------------------\n");
}

// 计算两个日期之间的天数
int days_between_dates(int y1,int m1,int d1,int y2,int m2,int d2)
{
    struct tm a={0}, b={0};
    a.tm_year=y1-1900; a.tm_mon=m1-1; a.tm_mday=d1;
    b.tm_year=y2-1900; b.tm_mon=m2-1; b.tm_mday=d2;
    time_t ta=mktime(&a), tb=mktime(&b); if(ta==(time_t)-1 || tb==(time_t)-1) return 0;
    double diff = difftime(tb, ta)/(60*60*24);
    if (diff<0) return (int)(diff-0.5); else return (int)(diff+0.5);
}

// 计算逾期罚款
double calc_overdue_fine(int by,int bm,int bd,int returned)
{
    if (returned) return 0.0; // 已归还不计罚款
    time_t now=time(NULL); struct tm *p=localtime(&now);
    int days = days_between_dates(by,bm,bd, 1900+p->tm_year, 1+p->tm_mon, p->tm_mday);
    int overdue = days - BORROW_LIMIT_DAYS;
    if (overdue<=0) return 0.0; return overdue * FINE_RATE_PER_DAY;
}

// 输出单个借阅记录
void print_one_borrow(struct borrow_record *r, int showFine)
{
    char status[10]; strcpy(status, r->returned?"已还":"未还");
    int overdue_days=0; double fine=0.0;
    if (showFine && !r->returned){
        time_t now=time(NULL); struct tm *p=localtime(&now);
        int days = days_between_dates(r->year,r->month,r->day, 1900+p->tm_year,1+p->tm_mon,p->tm_mday);
        overdue_days = (days> BORROW_LIMIT_DAYS)? (days - BORROW_LIMIT_DAYS):0;
        fine = overdue_days * FINE_RATE_PER_DAY;
    }
    printf("%-14s %-8d %-18s  %04d-%02d-%02d   %-6s    %-6d        %.2f\n",
           r->usernum, r->book_number, r->book_name, r->year, r->month, r->day, status, overdue_days, fine);
}

// 列出所有借阅记录
void list_all_borrow_records()
{
    system("CLS"); print_main_title(); printf("\n\n============= 全部借阅记录 =============\n\n");
    struct borrow_record *head=make_borrow_linklist(); struct borrow_record *p=head->next; 
    if(!p){ printf("暂无借阅记录。\n\n按任意键返回...\n"); getch(); free(head); return; }
    print_borrow_title(); while(p){ print_one_borrow(p,1); p=p->next; } printf("\n按任意键返回...\n"); getch();
    // 释放内存
    p=head; while(p){ struct borrow_record *n=p->next; free(p); p=n; }
}

// 按用户列出借阅记录
void list_borrow_records_by_user(const char *usernum)
{
    system("CLS"); print_main_title(); printf("\n\n============= %s 的借阅记录 =============\n\n", usernum);
    struct borrow_record *head=make_borrow_linklist(); struct borrow_record *p=head->next; int found=0;
    print_borrow_title();
    while(p){ if(strcmp(p->usernum,usernum)==0){ print_one_borrow(p,1); found=1; } p=p->next; }
    if(!found) printf("(无记录)\n");
    printf("\n按任意键返回...\n"); getch();
    // 释放内存
    p=head; while(p){ struct borrow_record *n=p->next; free(p); p=n; }
}

// 标记记录为已归还
int mark_record_returned(const char *usernum, int book_number)
{
    int ok=0; struct borrow_record *head=make_borrow_linklist(); struct borrow_record *p=head->next, *target=NULL;
    while(p){ if(!p->returned && p->book_number==book_number && strcmp(p->usernum,usernum)==0){ target=p; } p=p->next; }
    if(target){ target->returned=1; override_borrow_file(head); ok=1; }
    // 释放内存
    p=head; while(p){ struct borrow_record *n=p->next; free(p); p=n; }
    return ok;
}

// 输出借阅规则
void print_borrow_rules()
{
    system("CLS"); print_main_title(); printf("\n\n========= 借阅规则 =========\n\n");
    printf("1. 借阅期限：每本书 30 天。\n");
    printf("2. 逾期罚款：超过期限后，每天罚款 0.1 元。\n");
    printf("3. 归还方式：请在图书管理员处办理还书，系统自动更新借阅记录。\n");
    printf("\n按任意键返回...\n"); getch();
}

// ===================== 读者查询书籍 =====================
void reader_search_books(struct book *head)
{
    int search_choice;
    char search_str[30];
    int search_num;
    struct book *result;
    
search_menu:
    system("CLS");
    print_main_title();
    printf("\n\n============= 查询书籍 =============\n\n");
    printf("                        1、按编号查询\n\n");
    printf("                        2、按书名查询\n\n");
    printf("                        3、按分类查询\n\n");
    printf("                        4、返回上一级\n\n");
    printf("                        请输入编号：");
    scanf("%d", &search_choice);
    
    switch(search_choice)
    {
        case 1: // 按编号查询
            printf("\n请输入书籍编号：");
            scanf("%d", &search_num);
            result = search_by_number(head, search_num);
            if(result)
            {
                printf("\n查询结果：\n");
                print_booklist_title();
                print_booknode(result);
            }
            else
            {
                printf("\n未找到编号为 %d 的书籍！\n", search_num);
            }
            printf("\n按任意键继续...");
            getch();
            goto search_menu;
            
        case 2: // 按书名查询
            printf("\n请输入书籍名称：");
            scanf("%s", search_str);
            result = search_by_name(head, search_str);
            if(result)
            {
                printf("\n查询结果：\n");
                print_booklist_title();
                print_booknode(result);
            }
            else
            {
                printf("\n未找到书名为 \"%s\" 的书籍！\n", search_str);
            }
            printf("\n按任意键继续...");
            getch();
            goto search_menu;
            
        case 3: // 按分类查询
            printf("\n请输入分类名称：");
            scanf("%s", search_str);
            printf("\n查询结果：\n");
            search_by_category(head, search_str);
            printf("\n按任意键继续...");
            getch();
            goto search_menu;
            
        case 4: // 返回
            return;
            
        default:
            printf("\n输入错误，请重新选择！\n");
            Sleep(1000);
            goto search_menu;
    }
}

// ===================== 借书功能 =====================
void enhanced_borrow_book(struct book *head)
{
    int borrow_search_choice;
    char search_str[30];
    int search_num;
    struct book *target = NULL;
    
borrow_search_menu:
    system("CLS");
    printf("\n======================================借书======================================\n\n");
    printf("                        请选择查找方式：\n\n");
    printf("                        1、按编号查找\n\n");
    printf("                        2、按书名查找\n\n");
    printf("                        3、按分类查找\n\n");
    printf("                        4、返回上一级\n\n");
    printf("                        请输入编号：");
    scanf("%d", &borrow_search_choice);
    
    switch(borrow_search_choice)
    {
        case 1: // 按编号查找
            printf("\n请输入书籍编号：");
            scanf("%d", &search_num);
            target = search_by_number(head, search_num);
            break;
            
        case 2: // 按书名查找
            printf("\n请输入书籍名称：");
            scanf("%s", search_str);
            target = search_by_name(head, search_str);
            break;
            
        case 3: // 按分类查找
            printf("\n请输入分类名称：");
            scanf("%s", search_str);
            // 按分类查找可能找到多本书，这里只借第一本
            target = head->next;
            while(target)
            {
                if(strcmp(target->category, search_str) == 0)
                    break;
                target = target->next;
            }
            if(target)
            {
                printf("\n找到分类为 \"%s\" 的书籍：\n", search_str);
                print_booklist_title();
                print_booknode(target);
                printf("\n是否借阅这本书？(1:是 2:重新选择查找方式 3:返回)：");
                int confirm;
                scanf("%d", &confirm);
                if(confirm == 2)
                    goto borrow_search_menu;
                else if(confirm == 3)
                    return;
                // 如果选择1，继续借阅流程
            }
            break;
            
        case 4: // 返回
            return;
            
        default:
            printf("\n输入错误，请重新选择！\n");
            Sleep(1000);
            goto borrow_search_menu;
    }
    
    if(target)
    {
        system("CLS");
        printf("\n======================================借书======================================\n\n");
        print_booklist_title(); 
        print_booknode(target);
        
        if(target->lent == 0)
        {
            printf("\n\n请输入借阅人用户账号（11位手机号/学号）："); 
            char who[12]; 
            scanf("%11s", who);
            printf("\n您确定要借这本书吗？1：是 2：否  "); 
            int yes=2; 
            scanf("%d", &yes);
            if(yes == 1)
            {
                time_t timep;
                struct tm *time_p;
                time(&timep);
                time_p = localtime(&timep);
                
                target->lent = 1; 
                target->year = 1900 + time_p->tm_year; 
                target->month = 1 + time_p->tm_mon; 
                target->day = time_p->tm_mday;
                append_borrow_record(who, target, target->year, target->month, target->day, 0);
                printf("\n借出成功！\n"); 
                override_to_file(head); 
                printf("\n按任意键返回...");
                getch();
            }
        }
        else
        {
            printf("\n\n错误：该书已经借出！\n如需帮助，请联系工作人员\n"); 
            printf("\n按任意键返回...");
            getch();
        }
    }
    else
    {
        printf("\n\n错误：没有找到该书！\n"); 
        printf("\n按任意键返回...");
        getch();
        goto borrow_search_menu;
    }
}

// ===================== 主函数 =====================
int main()
{
    char login_temp;    
    
loop:
    // 打印欢迎标题+菜单 
    print_main_title();
    printf("\n\n");
    printf("                               1、注册\n\n");
    printf("                               2、登录\n\n");
    printf("                               3、退出\n\n");
    printf("\n\n");
    printf("                      请输入选项前的编号，按回车键结束：");
    scanf(" %c", &login_temp);

    int login_check_temp=1; 
    char login_exit_temp;  
    
    switch(login_temp)
    {
        case '1':
            create_user();
            goto loop;
        case '2':
        {
            login_check_temp = login();
            if(login_check_temp == 0) goto loop;   
            check_login_success = 1;

            char menu_select; 
            int user_admin_select, del_sysuser_select;    
            int book_admin_select, borrow_select, back_select, modify_select, admin_delete_select; 
            struct sysuser *sysuser_head; 
            struct book *head; 
            char ch; (void)ch; 
            char str_tmp[100]; (void)str_tmp;
            
            while(1)
            {   
                head = make_linklist();   
main_loop:
                system("CLS");
                print_main_title();
                printf("\n");
                printf("                                1、我是【读者】\n\n");
                printf("                                2、我是【系统管理员】\n\n");
                printf("                                3、我是【图书管理员】\n\n");
                printf("                                4、退出\n\n");
                printf("                       请输入要使用功能的编号，按回车进入：");
                scanf(" %c", &menu_select);

                struct sysuser *m; (void)m; struct book *t; int temp=0; int i; (void)i;
                char name_temp[30]={0}; char category_temp[10]={0};
                time_t timep; struct tm *time_p; time(&timep); time_p = localtime(&timep); 
                int year=0, month=0, day=0;

                switch(menu_select)
                {
                    // ===================== 读者界面 =====================
                    case '1': 
                    {
                        int reader_select;
                    reader_loop:
                        system("CLS");
                        print_main_title();
                        printf("\n\n");
                        printf("                        1、查询书籍\n\n");
                        printf("                        2、借书\n\n");
                        printf("                        3、还书\n\n");
                        printf("                        4、查看借阅规则\n\n");
                        printf("                        5、查看借阅记录（包括逾期记录）\n\n");
                        printf("                        6、返回上一级\n"); 
                        printf("\n\n");
                        printf("                        请输入对应功能的编号，按回车进入：");
                        scanf("%d", &reader_select);
                        
                        switch(reader_select)
                        {
                            case 1: // 查询书籍
                                reader_search_books(head);
                                goto reader_loop;
                                
                            case 2: // 借书
                                enhanced_borrow_book(head);
                                goto reader_loop;

                            case 3: // 还书
                            {
                            back_loop:
                                system("CLS");
                                printf("\n======================================还书======================================\n\n");
                                printf("                        1、输入书的编号\n\n");
                                printf("                        2、返回上一级\n\n");
                                printf("                        请输入编号："); 
                                scanf("%d", &back_select);
                                if (back_select == 1)
                                {
                                    printf("\n请输入编号："); 
                                    scanf("%d", &temp);
                                    t = search_by_number(head, temp);
                                    if(t)
                                    {
                                        system("CLS"); 
                                        printf("\n======================================还书======================================\n\n");
                                        print_booklist_title(); 
                                        print_booknode(t);
                                        if(t->lent == 1)
                                        {
                                            printf("\n请输入归还人用户账号（借书时登记的账号）："); 
                                            char who[12]; 
                                            scanf("%11s", who);
                                            printf("\n您确定要还这本书吗？1：是 2：否  "); 
                                            int yes = 2; 
                                            scanf("%d", &yes);
                                            if(yes == 1)
                                            {
                                                t->lent = 0; 
                                                t->year = 0; 
                                                t->month = 0; 
                                                t->day = 0;
                                                if (mark_record_returned(who, temp)) {
                                                    printf("\n还书成功！记录已更新。\n");
                                                } else {
                                                    printf("\n还书成功！(未找到对应借阅记录，未更新记录。)\n");
                                                }
                                                override_to_file(head); 
                                                printf("\n按任意键继续...");
                                                getch();
                                                goto back_loop; // 继续还书操作
                                            }
                                        } 
                                        else { 
                                            printf("\n\n错误：该书已经在库！\n"); 
                                            printf("\n按任意键继续...");
                                            getch();
                                            goto back_loop; // 继续还书操作
                                        }
                                    } 
                                    else { 
                                        printf("\n\n错误：没有找到该书！\n"); 
                                        printf("\n按任意键继续...");
                                        getch();
                                        goto back_loop; // 继续还书操作
                                    }
                                } 
                                else 
                                    goto reader_loop; // 返回读者菜单
                                break;
                            }

                            case 4: // 查看借阅规则
                                print_borrow_rules();
                                goto reader_loop;
                                
                            case 5: // 查看借阅记录
                                list_borrow_records_by_user(user_number);
                                goto reader_loop;
                                
                            case 6: // 返回上一级
                                goto main_loop;
                                
                            default:
                                printf("\n输入错误，请重新选择！\n");
                                Sleep(1000);
                                goto reader_loop;
                        }
                        break;
                    }

                    // ===================== 系统管理员界面 =====================
                    case '2':
                    {
                    user_admin_loop:
                        system("CLS"); 
                        print_main_title(); 
                        printf("\n\n"); 
                        printf("                        1、删除用户\n\n");
                        printf("                        2、列出所有用户\n\n");
                        printf("                        3、查看全部借阅记录\n\n");
                        printf("                        4、按用户号查询借阅记录\n\n");
                        printf("                        5、返回上一级\n\n");
                        printf("                        请输入编号："); 
                        scanf("%d", &user_admin_select);
                        struct sysuser *sysuser_head = make_sysuser_linklist();
                        switch(user_admin_select)
                        {
                            case 1: // 删除用户
                                system("CLS"); 
                                printf("\n\n=====================================删除用户===================================\n\n"); 
                                printf("                                     请谨慎操作！                               \n\n"); 
                                printf("================================================================================\n\n"); 
                                printf("请输入你要删除的用户的账号（11位）："); 
                                char tempno[12]; 
                                scanf("%11s", tempno); 
                                printf("\n\n"); 
                                struct sysuser *who = search_by_usernum(sysuser_head, tempno);
                                if(!who){ 
                                    printf("没有找到该用户！按任意键返回...\n"); 
                                    getch(); 
                                }
                                else { 
                                    sysuser_head = delete_sysuser(sysuser_head, who); 
                                    override_to_sysuser_file(sysuser_head); 
                                }
                                goto user_admin_loop;
                                
                            case 2: // 列出所有用户
                                print_all_sysuser(sysuser_head); 
                                goto user_admin_loop;
                                
                            case 3: // 查看全部借阅记录
                                list_all_borrow_records(); 
                                goto user_admin_loop;
                                
                            case 4: // 按用户号查询借阅记录
                            {
                                printf("请输入要查询的用户账号："); 
                                char qno[12]; 
                                scanf("%11s", qno); 
                                list_borrow_records_by_user(qno); 
                                goto user_admin_loop; 
                            }
                            case 5: // 返回上一级
                                goto main_loop;
                                
                            default: 
                                goto user_admin_loop;
                        }
                    }

                    // ===================== 图书管理员界面 =====================
                    case '3': 
                    {
                    book_admin_loop:
                        system("CLS"); 
                        print_main_title(); 
                        printf("\n\n");
                        printf("                        1、录入新图书到数据库\n\n");
                        printf("                        2、修改已有图书的数据\n\n");
                        printf("                        3、从数据库中删除图书\n\n");
                        printf("                        4、返回上一级\n\n");
                        printf("                        请输入编号："); 
                        scanf("%d", &book_admin_select);
                        switch(book_admin_select)
                        {
                            case 1: // 录入新图书
                                input_new_book(head); 
                                goto book_admin_loop;

                            case 2: // 修改图书数据
                            {
                            modify_loop:
                                system("CLS"); 
                                printf("\n====================================修改图书====================================\n\n");
                                printf("           在这里您可以修改数据库中书的编号、名称、分类和借出时间\n\n");
                                printf("                        1、输入书的编号\n\n");
                                printf("                        2、返回\n\n");
                                printf("                请输入编号："); 
                                scanf("%d", &modify_select);
                                if(modify_select == 1)
                                {
                                    fflush(stdin); 
                                    printf("请输入书的编号："); 
                                    scanf("%d", &temp); 
                                    printf("\n\n"); 
                                    t = search_by_number(head, temp); 
                                    if(!t){ 
                                        printf("没有找到该书！按任意键返回...\n"); 
                                        getch(); 
                                    }
                                    else { 
                                        head = modify_book(head, t); 
                                        override_to_file(head); 
                                    }
                                    goto modify_loop;
                                } 
                                else 
                                    goto book_admin_loop;
                            }

                            case 3: // 删除图书
                            {
                                system("CLS"); 
                                printf("\n====================================删除图书====================================\n\n");
                                printf("                        1、根据编号查找并删除\n\n");
                                printf("                        2、返回\n\n");
                                printf("                        请输入编号："); 
                                scanf("%d", &admin_delete_select);
                                if(admin_delete_select == 1)
                                {
                                    printf("请输入要删除的书的编号："); 
                                    scanf("%d", &temp); 
                                    t = search_by_number(head, temp);
                                    if(t){ 
                                        head = delete_book(head, t); 
                                        override_to_file(head); 
                                    }
                                    else { 
                                        printf("没有找到此书！\n"); 
                                        printf("\n按任意键继续...");
                                        getch();
                                    }
                                }
                                goto book_admin_loop;
                            }

                            case 4: // 返回上一级
                                goto main_loop;
                                
                            default: 
                                goto book_admin_loop;
                        }
                    }

                    case '4': // 退出系统
                    {
                        printf("\n您确定要退出吗？\n按任意键退出，按n返回\n");
                        login_exit_temp = getch();
                        if(login_exit_temp == 'n'){ 
                            system("CLS"); 
                            goto loop; 
                        }
                        else return 1;
                    }

                    default: 
                        goto main_loop;
                }
            }
        }
        case '3': // 退出系统
        {
            printf("\n\n您确定要退出吗？\n\n按任意键退出，按n返回\n\n");
            login_exit_temp = getch();
            if(login_exit_temp == 'n'){ 
                system("CLS"); 
                goto loop; 
            }
            else exit(0);
        }
        default:
            system("CLS"); 
            goto loop;
    }
}
