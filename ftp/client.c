#include "client_fun.h"

#define SER_IP "127.0.0.1"
// #define SER_PORT 8866

int main()
{
  int sockfd;
  struct sockaddr_in server_addr;
  MSG msg;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("failed socket");
    return 0;
  }

  struct timeval timeout = {10, 0};
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));

  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  inet_pton(AF_INET, SER_IP, &server_addr.sin_addr);
  // server_addr.sin_addr.s_addr = inet_addr(SER_IP);
  server_addr.sin_port = htons(SER_PORT);

  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0)
  {
    perror("failed connect");
    return -1;
  }
  for (;;)
  {
    printf("**************************************************\n");
    printf("*********** 输入 help 查看帮助选项 ***************\n");
    printf("**************************************************\n");
    printf("*** list       get         put            quit ***\n");
    printf("**************************************************\n");
    printf("Input you choice:>>>");

    char cmd[128];
    scanf("%s", cmd);

    int n = 0; //页面选择
    MSG msg;

    if (strncmp(cmd, "help", 4) == 0)
    {
      n = 5;
    }
    if (strncmp(cmd, "list", 4) == 0)
    {
      n = 1;
    }
    if (strncmp(cmd, "get", 3) == 0)
    {
      n = 3;
    }
    if (strncmp(cmd, "put", 3) == 0)
    {
      n = 2;
    }
    if (strncmp(cmd, "quit", 4) == 0)
    {
      n = 4;
    }
    if (0 == n)
    {
      printf("无效输入\n");
      continue;
    }

    switch (n)
    {
    case L: //列出服务器的文件
      client_listfile(sockfd, &msg);
      break;
    case P: //上传文件
      pushfile(sockfd, &msg);
      break;
    case G: //下载文件
      pullfile(sockfd, &msg);
      break;
    case 4:
      close(sockfd);
      exit(0);
    // break;
    case 5:
      help();
      break;
    }
  }
  printf("退出\n");
  close(sockfd);
  return 0;
}
