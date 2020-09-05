#include "server_dict.h"

int do_client(int acceptfd, sqlite3 *db)
{
    MSG msg;
    while (recv(acceptfd, &msg, sizeof(msg), 0) > 0)
    {
        switch (msg.type)
        {
        case R:
            ser_register(acceptfd, &msg, db);
            break;
        case L:
            ser_login(acceptfd, &msg, db);
            break;
        case Q:
            ser_findword(acceptfd, &msg, db);
            break;
        case H:
            call_history(acceptfd, &msg, db);
            break;
        default:
            printf("无效输入");
        }
    }
    printf("用户退出\n");
    close(acceptfd);
    exit(0);
    // return 0;
}

int ser_register(int acceptfd, MSG *msg, sqlite3 *db)
{
    char *errmsg;
    char sql[128];

    snprintf(sql, 512, "insert into user values('%s','%s');", msg->name, msg->passwd);
    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        // printf("%s\n", errmsg);
        strcpy(msg->data, "exist");
        printf("用户：%s 已存在\n", msg->name);
    }
    else
    {
        strcpy(msg->data, "OK");
        printf("用户：%s 注册\n", msg->name);
    }
    if (send(acceptfd, msg, sizeof(MSG), 0) < 0)
    {
        perror("failed to send");
        return -1;
    }
    return 0;
}
int ser_login(int acceptfd, MSG *msg, sqlite3 *db)
{
    char *errmsg;
    int nrow;
    int ncloumn;
    char **resultp;
    char sql[128];
    int status;

    snprintf(sql, 256, "select * from user where name = '%s' and passwd = '%s';", msg->name, msg->passwd);
    if (sqlite3_get_table(db, sql, &resultp, &nrow, &ncloumn, &errmsg) != SQLITE_OK)
    {
        printf("%s\n", errmsg);
        return -1;
    }
    if (1 == nrow)
    {
        printf("用户:%s 登录\n", msg->name);
        strcpy(msg->data, "OK");
        status = 1;
    }
    else
    {
        printf("用户 %s 登录失败\n", msg->name);
        strcpy(msg->data, "failed");
        status = -1;
    }
    send(acceptfd, msg, sizeof(MSG), 0);
    return status;
}
// int ser_query(int acceptfd, MSG *msg, sqlite3 *db)
// {
//     strcpy(msg->data, "This is a server search message");
//     if (send(acceptfd, msg, sizeof(MSG), 0) < 0)
//     {
//         perror("fail to send client");
//         return -1;
//     }
//     return 0;
// }

// int get_history(int acceptfd, MSG *msg, sqlite3 *db)
// {
//     //是否是root用户
//     if (1 == msg->flag)
//     {
//         strcpy(msg->data, "This is a server history of root message");
//     }
//     else
//     {
//         strcpy(msg->data, "This is a server history message");
//     }
//     if (send(acceptfd, msg, sizeof(MSG), 0) < 0)
//     {
//         perror("fail to send client");
//         return -1;
//     }
//     return 0;
// }

int server_searchword(int acceptfd, MSG *msg, const char word[])
{

    FILE *fp;
    int len = 0;
    char temp[512] = {};
    int result;
    char *p;

    if ((fp = fopen("dict.txt", "r")) == NULL)
    {
        perror("failed to fopen.\n");
        strcpy(msg->data, "Failed to open dict.txt");
        send(acceptfd, msg, sizeof(MSG), 0);
        return -1;
    }

    while ((fp = fopen("dict.txt", "r")) == NULL)
    {
        perror("failed to open file.\n");
        strcpy(msg->data, "Failed to open dict.txt");
        send(acceptfd, msg, sizeof(MSG), 0);
        return -1;
    }

    /**************************************************
 * 由于文件中的单词按字母升序排列
 * 可以用strncmp顺序比较查找,单词末有空格不方便使用 strcmp
 *
 * 结果小于0,则继续找，例如：目标单词：dog大于cat
 * 结果大于0,则停止查找，例如：目标单词：good大于god
 * 等于0,且末尾为空格，则为找到
 *
 **************************************************/
    printf("要查询的单词:%s\n", word);
    len = strlen(word);
    while (fgets(temp, 512, fp) != NULL)
    {
        result = strncmp(temp, word, len);

        if (result < 0)
        {
            continue;
        }
        if (result > 0 || ((result == 0) && (temp[len] != ' ')))
        {
            break;
        }

        p = temp + len;

        while (' ' == *p)
        {
            p++;
        }

        strcpy(msg->data, p);
        printf("找到释义\n:%s", msg->data);

        fclose(fp);
        return 1;
    }
    fclose(fp);
    return 0;
}

int get_time(char *date)
{
    time_t t;
    struct tm *tp;

    time(&t);

    tp = localtime(&t);

    snprintf(date, 255, "%d-%d-%d %d:%d:%d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
    printf("time:%s \n", date);

    return 0;
}

int ser_findword(int acceptfd, MSG *msg, sqlite3 *db)
{
    char word[64];
    int found = 0;
    char date[128] = {};
    char sql[512] = {};
    char *errmsg;

    strcpy(word, msg->data);

    found = server_searchword(acceptfd, msg, word);

    if (found == 1)
    {
        get_time(date);

        sprintf(sql, "insert into record values('%s','%s','%s');", msg->name, date, word);

        if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
        {
            printf("%s\n", errmsg);
            return -1;
        }
        else
        {
            printf("记录成功\n");
        }
    }
    else
    {
        strcpy(msg->data, "Not found!");
    }
    send(acceptfd, msg, sizeof(MSG), 0);

    return 0;
}

//回调函数
int history_show(void *arg, int f_num, char **f_value, char **f_name)
{
    int acceptfd;
    MSG msg;

    acceptfd = *((int *)arg);

    sprintf(msg.data, "%s,%s", f_value[1], f_value[2]);
    send(acceptfd, &msg, sizeof(MSG), 0);

    return 0;
}

int root_historyshow(void *arg, int f_num, char **f_value, char **f_name)
{
    int acceptfd;
    MSG msg;

    acceptfd = *((int *)arg);

    sprintf(msg.data, "%s,%s,%s", f_value[0], f_value[1], f_value[2]);
    send(acceptfd, &msg, sizeof(MSG), 0);

    return 0;
}

int call_history(int acceptfd, MSG *msg, sqlite3 *db)
{
    char sql[512];
    char *errmsg;

    //root 用户查询所有用户信息
    if (2 == msg->flag)
    {

        snprintf(sql, 128, "select * from record order by name desc;");

        // snprintf(sql, 128, "select * from record;");
        if (sqlite3_exec(db, sql, root_historyshow, (void *)&acceptfd, &errmsg) != SQLITE_OK)
        {
            printf("%s\n", errmsg);
        }
        else
        {
            printf("root 历史记录查询完毕！\n");
        }
    }
    else
    {

        snprintf(sql, 128, "select * from record where name = '%s';", msg->name);

        if (sqlite3_exec(db, sql, history_show, (void *)&acceptfd, &errmsg) != SQLITE_OK)
        {
            printf("%s\n", errmsg);
        }
        else
        {
            printf("历史记录查询完毕!\n");
        }
    }
    msg->data[0] = '\0';

    send(acceptfd, msg, sizeof(MSG), 0);

    return 0;
}
