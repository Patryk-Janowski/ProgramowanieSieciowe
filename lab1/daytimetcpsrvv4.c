#include        <sys/types.h>   /* basic system data types */
#include        <sys/socket.h>  /* basic socket definitions */
#include 		<unistd.h>
#include        <time.h>                /* old system? */
#include        <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include        <arpa/inet.h>   /* inet(3) functions */
#include        <errno.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>

#define MAXLINE 100

//#define SA struct sockaddr

#define LISTENQ 5

int
main(int argc, char **argv)
{
    int				listenfd, n, connfd;
    socklen_t			len;
    char				buff[MAXLINE], str[INET_ADDRSTRLEN+1];
    time_t				ticks;
    struct sockaddr_in	servaddr, cliaddr;

    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr,"socket error : %s\n", strerror(errno));
        return 1;
    }

//sleep(3);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(13);	/* daytime server */

    if ( bind( listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
        fprintf(stderr,"bind error : %s\n", strerror(errno));
        return 1;
    }
//sleep(3);
    if ( listen(listenfd, LISTENQ) < 0){
        fprintf(stderr,"listen error : %s\n", strerror(errno));
        return 1;
    }
//sleep(3);

    len = sizeof(cliaddr);
    if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len)) < 0){
        fprintf(stderr,"accept error : %s\n", strerror(errno));
        exit(0);
    }
//sleep(3);

    bzero(str, sizeof(str));
    inet_ntop(AF_INET, (struct sockaddr  *) &cliaddr.sin_addr,  str, sizeof(str));
    printf("Connection from %s\n\n", str);

    for ( ; ; ) {
        bzero(buff, MAXLINE);
        read(connfd, buff, sizeof(buff));
        printf("From client: %sTo client : ", buff);
        
        bzero(buff, MAXLINE);
        n = 0;
        while ((buff[n++] = getchar()) != '\n');
        write(connfd, buff, sizeof(buff));
   
        if (strncmp("exit", buff, 4) == 0) {
            printf("\nClean exit\n");
            break;
        }
    }

    close(listenfd);
}
