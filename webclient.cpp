#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <fstream>
#include <iostream>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define BUFFSIZE 2048
#define SERVER_IP "127.0.0.1" // 指定服务端的IP，记得修改为你的服务端所在的ip
#define SERVER_PORT 16555     // 指定服务端的port
int main(int argc, char *args[])
{
    struct sockaddr_in servaddr;
    char buff[BUFFSIZE];
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
        printf("Create socket error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr);
    servaddr.sin_port = htons(SERVER_PORT);
    if (-1 == connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    {
        printf("Connect error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
    strcpy(buff, args[1]);
    if (std::string(buff) == "-u")
    {
        //give -u operation
        send(sockfd, buff, strlen(buff), 0);
        // buff = args[0];
        strcpy(buff,args[2]);
        printf("input: %s\n", buff);
        auto k = fopen(buff, "rb");
        if (!k)
        {
            printf("error of open %s\n", buff);
            return -1;
        }
        //send file name
        send(sockfd, buff, strlen(buff), 0);
        // rec file name
        recv(sockfd, buff, BUFFSIZE - 1, 0);
        std::cout<<buff<<"\n";
        fseek(k, 0, SEEK_END);
        auto sz = ftell(k);
        fseek(k, 0, SEEK_SET);
        printf("sizeof file is %ld\n", sz);
        auto bf = new char[sz];
        int ncnt = 0, ret = 0;
        while ((ncnt = fread(bf, 1, sz, k)) > 0)
        {
            //send content
            ret = send(sockfd, bf, ncnt, 0);
            if (ret == -1)
            {
                printf("error from send file");
                break;
            }
            else if (ret == 0)
            {
                break;
            }
        }
         delete[] bf;
    }else if(std::string(buff) == "-d"){
        send(sockfd,buff,strlen(buff),0);
        //send file name
        strcpy(buff,args[2]);
        send(sockfd, buff, strlen(buff), 0);
        std::string filename = buff;
        bzero(buff,BUFFSIZE - 1);
        recv(sockfd,buff,BUFFSIZE - 1,0);
        std::cout<<buff<<"\n";
        std::ofstream out;
        out.open("down/" + filename);
        recv(sockfd,buff,BUFFSIZE - 1,0);
        out<<buff;
    }
    bzero(buff, sizeof(buff));
    // two shakes
    send(sockfd,"STOP",BUFFSIZE - 1,0);
    // rec stop
    recv(sockfd, buff, BUFFSIZE - 1, 0);
    printf("Recv2: %s\n", buff);
    close(sockfd);
   
    return 0;
}