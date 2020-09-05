#include "client_dict.h"

int main()
{
    int socket_fd;
    struct sockaddr_in server_addr;
    int n = 0; //页面选项
    MSG msg;

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("failed socket");
        return -1;
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);
    server_addr.sin_port = htons(SERVER_PORT);

    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("fail to connect");
        return -1;
    }
    for (;;)
    {
        printf("++++++++++++++++++++++++++++++++++++++++\n");
        printf("+ 1.注册        2.登录          3.退出 +\n");
        printf("++++++++++++++++++++++++++++++++++++++++\n");
        printf("请选择：");

        scanf("%d", &n);

        switch (n)
        {
        case 1:
            client_register(socket_fd, &msg);
            break;
        case 2:
            if (client_login(socket_fd, &msg) == 1)
            {
                if (1 == msg.flag)
                {
                    root_check(socket_fd, &msg);
                }
                else
                {
                    client_check(socket_fd, &msg);
                }
            }
            break;
        case 3:
            close(socket_fd);
            exit(0);
            break;
        default:
            while (getchar() != '\n')
                ;
            printf("无效输入\n");
        }
        n = 0;
    }
    return 0;
}
