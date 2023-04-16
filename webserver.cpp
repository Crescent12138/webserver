#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <chrono>
#include <iostream>
#include <signal.h>
#define BUFFSIZE 2048
#define DEFAULT_PORT 16555 // 指定端口为16555
#define MAXLINK 2048
int sockfd, connfd; // 定义服务端套接字和客户端套接字
void stopServerRunning(int p)
{
    close(sockfd);
    printf("Close Server\n");
    exit(0);
}
int main()
{
    struct sockaddr_in servaddr; // 用于存放ip和端口的结构
    char buff[BUFFSIZE];         // 用于收发数据
    // 对应伪代码中的sockfd = socket();
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
        printf("Create socket error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
    // END
    // 对应伪代码中的bind(sockfd, ip::port和一些配置);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(DEFAULT_PORT);
    if (-1 == bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    {
        printf("Bind error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
    // END
    // 对应伪代码中的listen(sockfd);
    if (-1 == listen(sockfd, MAXLINK))
    {
        printf("Listen error(%d): %s\n", errno, strerror(errno));
        return -1;
    }
    // END
    printf("Listening...\n");
    while (true)
    {
        signal(SIGINT, stopServerRunning); // 这句用于在输入Ctrl+C的时候关闭服务器
        // 对应伪代码中的connfd = accept(sockfd);
        connfd = accept(sockfd, NULL, NULL);
        if (-1 == connfd)
        {
            printf("Accept error(%d): %s\n", errno, strerror(errno));
            return -1;
        }
        // END
        bzero(buff, BUFFSIZE);
        //rec operation
        auto k = recv(connfd, buff, BUFFSIZE - 1, 0);
        if (std::string(buff) == "-u")
        {
            //rev file name
            recv(connfd, buff, BUFFSIZE - 1, 0);
            //send file name
            send(connfd, "file name get", strlen("file name get"), 0);
            std::ofstream out;
            out.open("out/" + std::string(buff));
            // 对应伪代码中的recv(connfd, buff);
            while (1)
            {
                bzero(buff, BUFFSIZE);
                //rec content
                auto k = recv(connfd, buff, BUFFSIZE - 1, 0);
                if(std::string(buff) == "STOP"){
                    auto p = send(connfd, "ok", strlen("ok"), 0);
                    break;
                }else if(k == 0){
                    //  auto p = send(connfd, "stop", strlen("stop"), 0);
                }
                std::cout << ">>" << buff << "\n";
                out << buff;
                std::cout << k << " "  << " " << strlen(buff) << "\n";
                if (k <= 0  || strlen(buff) == 0)
                {
                      break;
                }
            }
        }else if(std::string(buff) == "-d"){
            //rev file name
            recv(connfd, buff, BUFFSIZE - 1, 0);
            std::cout<<buff<<std::endl;
            //send file name
            // send(connfd, "file name get", strlen("file name get"), 0);

            //rev file
            std::fstream in;
            in.open("out/" + std::string(buff));
            in.seekg(0,std::ios::end);
            auto len = in.tellg();
            in.seekg(0,std::ios::beg);
            auto buff2 = new char[len];
            in.read(buff2,len);
            std::cout<<buff2<<std::endl;
            in.close();
            send(connfd,buff2,len,0);
            
            delete[] buff2;
        }
        // END
        // 对应伪代码中的send(connfd, buff);
        //send stop 
        // send(connfd, "stop", strlen("stop"), 0);
        // END
        // 对应伪代码中的close(connfd);
        close(connfd);
        // END
    }
    return 0;
}