#include        <sys/types.h>   /* basic system data types */
#include        <sys/socket.h>  /* basic socket definitions */
#include        <sys/time.h>    /* timeval{} for select() */
#include        <time.h>                /* timespec{} for pselect() */
#include        <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include        <arpa/inet.h>   /* inet(3) functions */
#include        <errno.h>
#include        <fcntl.h>               /* for nonblocking */
#include        <netdb.h>
#include        <signal.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>

#define MAXLINE 1024

#define SA struct sockaddr


#undef	MAXLINE
#define	MAXLINE 65507


void
dg_echo(int sockfd, SA *pcliaddr, socklen_t clilen)
{
	int		n;
	socklen_t	len;
	char		mesg[MAXLINE];
	char		str[INET6_ADDRSTRLEN+1];
	time_t		ticks;
	struct sockaddr_in6*	 cliaddr;
	struct sockaddr_in*	 cliaddrv4;

	cliaddr = (struct sockaddr_in6*) pcliaddr;

	fprintf(stderr,"Waiting for clients ... \n");
	for ( ; ; ) {
		len = clilen;
		if( (n = recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len)) < 0 ){
                	fprintf(stderr,"recvfrom error : %s\n", strerror(errno));
 //               	continue;
			exit(1);		    
		}
		
		bzero(str, sizeof(str));
		if( pcliaddr->sa_family == AF_INET6 ){
		      cliaddr = (struct sockaddr_in6*) pcliaddr;
		      inet_ntop(AF_INET6,  (struct sockaddr  *) &cliaddr->sin6_addr,  str, sizeof(str));
		}
		else{
		      cliaddrv4 = (struct sockaddr_in*) pcliaddr;
		      inet_ntop(AF_INET, (struct sockaddr  *) &cliaddrv4->sin_addr,  str, sizeof(str));
		}

		printf("Connection from %s\n", str);

        ticks = time(NULL);
        snprintf(mesg, sizeof(mesg), "PS daytime server: %.24s\r\n", ctime(&ticks));
		
		if( sendto(sockfd, mesg, strlen(mesg), 0, pcliaddr, len) < 0 ) {
            fprintf(stderr,"sendto error : %s\n", strerror(errno));
 //         continue;
			exit(1);		    
		}
	}
}

	

int
main(int argc, char **argv)
{
	int				sockfd, connfd;
	long 				TTL;
	socklen_t			len;
	char				buff[MAXLINE], str[INET6_ADDRSTRLEN+1];
	time_t				ticks;
	struct sockaddr_in6	servaddr, cliaddr;

	if (argc != 2){
		fprintf(stderr,"usage: %s <TTL>\n", argv[0]);
		return 1;
	}

	TTL = strtol(argv[1], NULL, 10);

	if ( (sockfd = socket(AF_INET6, SOCK_DGRAM, 0)) < 0){
			fprintf(stderr,"socket error : %s\n", strerror(errno));
			return 1;
	}

	
	// Ustawianie opcji IPV6_UNICASTHOPS w gnieździe na poziomie warstwy IP
	if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &TTL, sizeof(TTL)) < 0){
		fprintf(stderr, "IP_TTL error\n");
		return -1;
	}

	// Ustawianie opcji IPV6_UNICASTHOPS w gnieździe na poziomie warstwy IP
	if (setsockopt(sockfd, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &TTL, sizeof(TTL)) < 0){
		fprintf(stderr, "IPV6_UNICAST_HOPS error\n");
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin6_family = AF_INET6;
	servaddr.sin6_addr   = in6addr_any;
	servaddr.sin6_port   = htons(13);	/* daytime server */

	if ( bind( sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
			fprintf(stderr,"bind error : %s\n", strerror(errno));
			return 1;
	}

	dg_echo(sockfd, (SA *) &cliaddr, sizeof(cliaddr));

        
}
