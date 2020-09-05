// #include "common.h"
#include "server_ftp.h"

int do_client(int acceptfd)
{
  MSG msg;
  while (recv(acceptfd, &msg, sizeof(MSG), 0) > 0)
  {
    // printf("type:%d\n", msg.type);
    switch (msg.type)
    {
    case L:
      showdir(acceptfd, &msg);
      break;
    case G: //服务器发送文件到客户端 对应客户端pullfile()
      printf("server send file\n");
      ser_send(acceptfd, &msg);
      break;
    case P: //服务器接收文件 对应客户端 pushfile()
      printf("client push file\n");
      ser_get(acceptfd, &msg);
      break;
    default:
      break;
    }
  }
  printf("用户退出\n");
  close(acceptfd);
  exit(0);
}

//将文件传送到客户端
int ser_send(int acceptfd, MSG *msg)
{
  char file_name[128];
  FILE *fp;
  long lSize;
  char *buffer;
  size_t result;

  //接收要发送的文件名
  if (recv(acceptfd, msg, sizeof(MSG), 0) < 0)
  {
    perror("failed to recv filename");
    return -1;
  }
  strcpy(file_name, msg->data);

  printf("file_name:%s\n", file_name);
  if ((fp = fopen(file_name, "rb")) == NULL)
  {
    perror("failed open file");
    return -1;
  }

  //获取要发送的文件大小
  fseek(fp, 0, SEEK_END);
  lSize = ftell(fp);
  rewind(fp);

  buffer = (char *)malloc(sizeof(char) * lSize);
  if (buffer == NULL)
  {
    perror("failed malloc");
    exit(1);
  }

  result = fread(buffer, sizeof(char), lSize, fp);
  if (result != lSize)
  {
    perror("failed fread");
    exit(1);
  }

  sprintf(msg->data, "%ld", lSize);
  if (send(acceptfd, msg, sizeof(MSG), 0) < 0)
  {
    perror("failed send to file size");
    return 0;
  }

  //发送文件
  const char *cp;
  cp = buffer;
  while (lSize)
  {
    int n_written = send(acceptfd, cp, lSize, 0);
    if (n_written <= 0)
    {
      error(1, errno, "send failed");
      return -1;
    }
    lSize -= n_written;
    cp += n_written;
  }

  printf("文件传送完成\n");
  free(buffer);
  fclose(fp);
  return 0;
}

//服务器接收文件
int ser_get(int acceptfd, MSG *msg)
{
  char file_name[128];

  // 接收文件名
  if (recv(acceptfd, msg, sizeof(MSG), 0) < 0)
  {
    perror("failed recv filename");
    return -1;
  }
  strcpy(file_name, msg->data);

  FILE *fp;
  long lSize;
  char *buffer;
  size_t result;

  //获取文件大小
  fprintf(stdout, "接收文件大小为：");
  if (recv(acceptfd, msg, sizeof(MSG), MSG_WAITALL) < 0)
  {
    perror("Do not know file size");
    return -1;
  }
  lSize = atol(msg->data);
  fprintf(stdout, "%ld\n", lSize);

  buffer = (char *)malloc(sizeof(char) * lSize);
  if (buffer == NULL)
  {
    perror("failed malloc");
    exit(1);
  }
  fprintf(stdout, "filename:%s\n", file_name);
  if ((fp = fopen(file_name, "wb")) == NULL)
  {
    perror("failed open file");
    return -1;
  }

  char *buffer_pointer = buffer;
  size_t length = lSize;
  for (;;)
  {

    size_t result = recv(acceptfd, buffer_pointer, length, MSG_WAITALL);
    if (length < 0 || result <= 0)
    {
      break;
    }

    length -= result;
    buffer_pointer += result;
  }

  result = fwrite(buffer, sizeof(char), lSize, fp);
  if (result != lSize)
  {
    perror("failed fwrite");
    exit(1);
  }
  fprintf(stdout, "接收文件成功\n");
  free(buffer);
  fclose(fp);

  return 0;
}

//服务器将当前目录下的文件发送到客户端
int showdir(int acceptfd, MSG *msg)
{
  DIR *dir;
  struct dirent *entry;

  if ((dir = opendir("./")) == NULL)
  {
    printf("failed opendir");
    return -1;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    strcpy(msg->data, entry->d_name);
    send(acceptfd, msg, sizeof(MSG), 0);
  }
  msg->data[0] = '\0';
  send(acceptfd, msg, sizeof(MSG), 0);

  printf("目录清单已经成功发送\n");

  closedir(dir);
  return 0;
}
