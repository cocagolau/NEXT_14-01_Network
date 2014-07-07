#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

int main(int argc, char**argv)
{
  int sockfd,n;
  struct sockaddr_in servaddr,cliaddr;
  socklen_t cliaddr_size;
  int a[2] = {0,};
  int b[2] = {0,};
  int status = 0;
  int sum = 0;
  struct timeval tv;

  sockfd=socket(AF_INET,SOCK_DGRAM,0);

  memset(&servaddr,0,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
  servaddr.sin_port=htons(32000);
  bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));


  tv.tv_sec = 0;
  tv.tv_usec = 100000;
  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
    perror("Error");
  }

  for (;;)
    {
      cliaddr_size = sizeof(cliaddr);
      a[0] = 0; a[1] = 0;
      b[0] = 0; b[1] = 0;
      while (1) {
        n = recvfrom(sockfd,&a,sizeof(int)*2,0,(struct sockaddr *)&cliaddr,&cliaddr_size);
        if (a[0] == 1) {
          printf("Received a as %d \n", a[1]);
          break;
        } else {
          printf("A Received %d, %d \n", a[0], a[1]);
          status = 1;
          sendto(sockfd,&status,sizeof(int),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
        }
      }
      while (1) {
        n = recvfrom(sockfd,&b,sizeof(int)*2,0,(struct sockaddr *)&cliaddr,&cliaddr_size);
        if (b[0] == 2) {
          printf("Received b as %d \n", b[1]);
          break;
        } else {
          printf("B Received %d, %d \n", b[0], b[1]);
          status = 2;
          sendto(sockfd,&status,sizeof(int),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
        }
      }

      status = 0;
      sendto(sockfd,&status,sizeof(int),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

      sum = a[1] + b[1];
      sendto(sockfd,&sum,sizeof(int)*2,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
      sendto(sockfd,&sum,sizeof(int)*2,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
      printf("-------------------------------------------------------\n");
      printf("Received %d + %d, and result is %d \n", a[1], b[1], sum);
      printf("-------------------------------------------------------\n");
    }
}