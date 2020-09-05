#include "client_dict.h"

int client_register(int sockfd, MSG *msg)
{
    msg->type = R;

    printf("请输入名字:");
    scanf("%s", msg->name);
    getchar();

    printf("请输入密码:");
    scanf("%s", msg->passwd);
    getchar();

    if (send(sockfd, msg, sizeof(MSG), 0) < 0)
    {
        perror("请求失败");
        return -1;
    }
    if (recv(sockfd, msg, sizeof(MSG), 0) < 0)
    {
        perror("接受请求失败");
        return -1;
    }
    if (strncmp(msg->data, "OK", 3) == 0)
    {
        printf("注册成功!\n");
    }
    else if (strncmp(msg->data, "exist", 6) == 0)
    {
        printf("用户已存在\n");
    }
    return 0;
}

int client_login(int sockfd, MSG *msg)
{
    msg->type = L;
    msg->flag = 0;

    printf("请输入登录用户名:");
    scanf("%s", msg->name);
    getchar();

    printf("请输入登录密码:");
    scanf("%s", msg->passwd);
    getchar();

    //判断是否是root用户登录
    if (strcmp(msg->name, "root") == 0)
    {
        msg->flag = 1;
    }
    if (send(sockfd, msg, sizeof(MSG), 0) < 0)
    {
        perror("请求失败\n");
        return -1;
    }
    if (recv(sockfd, msg, sizeof(MSG), 0) < 0)
    {
        perror("接受请求失败");
        return -1;
    }

    //检查用户名和密码是否正确
    if (strncmp(msg->data, "OK", 3) == 0)
    {
        printf("登录成功!\n");
        return 1;
    }
    else
    {
        printf("登录失败\n");
    }
    return 0;
}

int client_query(int sockfd, MSG *msg)
{
    msg->type = Q;

    printf("请输要查询的单词：");
    scanf("%s", msg->data);
    getchar();

    if (strncmp(msg->data, "#", 1) == 0)
    {
        return -1;
    }

    printf("\033[1;33m%s \033[0m", msg->data);
    if (send(sockfd, msg, sizeof(MSG), 0) < 0)
    {
        perror("请求失败");
        return -1;
    }

    if (recv(sockfd, msg, sizeof(MSG), 0) < 0)
    {
        perror("接受请求失败");
        return -1;
    }

    int word_len = strlen(msg->data);
    char explain[128];
    strncpy(explain, msg->data, word_len - 1);
    printf("释意:\n");
    printf("\033[1;34m--------------------------------------------------\033[0m\n");
    printf("\t%s\n", explain);
    printf("\033[1;34m--------------------------------------------------\033[0m\n");
    return 0;
}

int client_history(int sockfd, MSG *msg)
{
    msg->type = H;
    msg->flag = 0; //无查询所有用户权限

    if (send(sockfd, msg, sizeof(MSG), 0) < 0)
    {
        perror("请求失败");
        return -1;
    }
    for (;;)
    {
        if (recv(sockfd, msg, sizeof(MSG), 0) < 0)
        {
            perror("接受请求失败");
            return -1;
        }
        if (msg->data[0] == '\0')
        {
            break;
        }
        printf("%s\n", msg->data);
    }
    return 0;
}
int root_history(int sockfd, MSG *msg)
{
    msg->type = H;
    msg->flag = 2; //查询权限

    if (send(sockfd, msg, sizeof(MSG), 0) < 0)
    {
        perror("请求失败");
        return -1;
    }
    for (;;)
    {
        if (recv(sockfd, msg, sizeof(MSG), 0) < 0)
        {
            perror("接受请求失败");
            return -1;
        }
        if (msg->data[0] == '\0')
        {
            return -1;
        }
        printf("用户----时间----单词\n");
        printf("%s\n", msg->data);
        // printf("fix function!\n");
    }
    return 0;
}

int client_check(int sockfd, MSG *msg)
{
    for (;;)
    {
        printf("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        printf("+1.查词             2.查历史               3.退出+\n");
        printf("++++++++++++++++++++++++++++++++++++++++++++++++++\n");

        int n = 0;
        int is_num = 0;
        int go_home = 0;

        printf("请选择( 非数字键 返回上一级菜单):");
        is_num = scanf("%d", &n);
        getchar();

        if (0 == is_num)
        {
            return -1;
        }

        switch (n)
        {
        case 1:
            go_home = client_query(sockfd, msg);
            if (go_home == -1)
            {
                return -1;
            }
            break;
        case 2:
            go_home = client_history(sockfd, msg);
            if (-1 == go_home)
            {
                return -1;
            }
            break;
        case 3:
            close(sockfd);
            exit(0);
        default:
            while (getchar() != '\n')
                ;
            printf("无效输入\n");
            break;
        }
    }
    return 0;
}
int root_check(int sockfd, MSG *msg)
{
    for (;;)
    {
        printf("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        printf("+1.查词     2.查历史    3.查询用户记录     4.退出+\n");
        printf("++++++++++++++++++++++++++++++++++++++++++++++++++\n");

        int n = 0;
        int is_num = 0;
        int go_home = 0;

        printf("请选择( 非数字键 返回上一级菜单):");
        is_num = scanf("%d", &n);
        getchar();

        if (0 == is_num)
        {
            return -1;
        }

        switch (n)
        {
        case 1:
            go_home = client_query(sockfd, msg);
            if (go_home == -1)
            {
                return -1;
            }
            break;
        case 2:
            go_home = client_history(sockfd, msg);
            if (-1 == go_home)
            {
                return -1;
            }
            break;
        case 3:
            root_history(sockfd, msg);
            break;
        case 4:
            close(sockfd);
            exit(0);
        default:
            while (getchar() != '\n')
                ;
            printf("无效输入");
        }
    }
    return 0;
}
