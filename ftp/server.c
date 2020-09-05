#include "server_ftp.h"

int main()
{
  int sockfd;
  struct sockaddr_in server_addr;
  MSG msg;
  struct timeval timeout = {5, 0};
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("failed to socket");
    return -1;
  }

  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SER_PORT);

  int b_reuse = 1; //服务器地址重用
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &b_reuse, sizeof(b_reuse));

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("failed bind");
    return -1;
  }

  if (listen(sockfd, 5) < 0)
  {
    perror("failed to listen");
    return -1;
  }
  printf("server start!\n");

  int acceptfd;
  pid_t pid;

  signal(SIGCHLD, SIG_IGN);

  for (;;)
  {
    if ((acceptfd = accept(sockfd, NULL, NULL)) < 0)
    {
      perror("failed accept 1");
      return -1;
    }

    if ((pid = fork()) < 0)
    {
      perror("failed fork");
      return -1;
    }
    else if (0 == pid)
    {
      close(sockfd);
      do_client(acceptfd);
    }
    else
    {
      close(acceptfd); //父进程关闭连接套接字
    }
  }
  return 0;
}