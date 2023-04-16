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
#include <pthread.h>
 
void *recv_data(void *arg)
{
	int sockfd = *(int *)arg;
	char buf[64] = {0};
	struct sockaddr_in ser;
	socklen_t n = sizeof(ser);
	while(1)
    {
    int m = recvfrom(sockfd, buf, 64, 0, (struct sockaddr *)&ser,&n );
    printf(">>:%s\n", buf);
    memset(buf, 0, 64);
    }
}
 
int main(int argc, char *argv[])
{ 
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        perror("sockfd");
        return -1;
    }
    int on = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
    
    struct sockaddr_in ser, cli;
    ser.sin_family = AF_INET;
    ser.sin_port = htons(7777);
    ser.sin_addr.s_addr = inet_addr(argv[1]);
    
    pthread_t pid;
    pthread_create(&pid, NULL, recv_data, (void *)&sockfd);
    pthread_detach(pid);
	
    int n = sizeof(ser);
    char buf[64] = {0};
    
    while(1)
    {
    fgets(buf, 64, stdin);
    sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&ser, n);
    memset(buf, 0, 64);
    }
 
    close(sockfd);
    
 
    return 0;
} 