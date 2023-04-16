#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("sockfd");
        return -1;
    }
    struct sockaddr_in ser, cli;
    ser.sin_family = AF_INET;
    ser.sin_port = htons(7777);
    ser.sin_addr.s_addr = inet_addr("0");

    int len = sizeof(ser);

    int ret = bind(sockfd, (struct sockaddr *)&ser, len);
    if (ret < 0)
    {
        perror("bind");
        return -1;
    }

    socklen_t n = sizeof(cli);
    char buf[64] = {0};

    while (1)
    {
        int m = recvfrom(sockfd, buf, 64, 0, (struct sockaddr *)&cli, &n);
        printf("client ip:%s  client port:%d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
        printf("len = %d\n", m);
        printf("%s\n", buf);
        memset(buf, 0, 64);
    }

    close(sockfd);

    return 0;
}