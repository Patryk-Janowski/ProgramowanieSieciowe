#include    <sys/types.h>   /* basic system data types */
#include    <sys/socket.h>  /* basic socket definitions */
#include    <sys/time.h>    /* timeval{} for select() */
#include    <time.h>                /* timespec{} for pselect() */
#include	<netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>   /* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>               /* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<wait.h>
#include	<unistd.h>
#define MAXLINE 1024

#define SA struct sockaddr

#define LISTENQ 2


void
sig_chld(int signo)
{
	pid_t	pid;
	int		stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}

void
sig_pipe(int signo)
{
	printf("Server received SIGPIPE - Default action is exit \n");
	exit(1);
}

ssize_t						/* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}
/* end writen */

void
Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
		perror("writen error");
}

void
str_echo(int sockfd)
{
	ssize_t		n;
	char		buf[MAXLINE];

again:
	while ( (n = read(sockfd, buf, MAXLINE)) > 0){
//		sleep(1); //Sztuczne opóźnienie dla wymuszenia sytuacji awaryjnych
		Writen(sockfd, buf, n);
	}
	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		perror("str_echo: read error");
}
			

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in6	cliaddr, servaddr;
	void				sig_chld(int);
	char 				addr_buf[INET6_ADDRSTRLEN+1];

	if ( (listenfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0){
	       fprintf(stderr,"socket error : %s\n", strerror(errno));
	       return 1;
	}

	int on = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
	      fprintf(stderr,"SO_REUSEADDR setsockopt error : %s\n", strerror(errno));
	}


	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin6_family = AF_INET6;
	servaddr.sin6_addr   = in6addr_any;
	servaddr.sin6_port   = htons(7);	/* echo server */

	if ( bind( listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0){
	        fprintf(stderr,"bind error : %s\n", strerror(errno));
	        return 1;
	}
	
	if ( listen(listenfd, LISTENQ) < 0){
	        fprintf(stderr,"listen error : %s\n", strerror(errno));
	        return 1;
	}

	signal(SIGCHLD, sig_chld);
	signal(SIGPIPE, sig_pipe);

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		if ( (connfd = accept(listenfd, (SA *) &cliaddr, &clilen)) < 0) {
			if (errno == EINTR)
				continue;		/* back to for() */
			else
				perror("accept error");
				exit(1);
		}
		
			bzero(addr_buf, sizeof(addr_buf));
			inet_ntop(AF_INET6, (struct sockaddr  *) &cliaddr.sin6_addr,  addr_buf, sizeof(addr_buf));
			printf("New client: %s, port %d\n",
					addr_buf, ntohs(cliaddr.sin6_port));


		if ( (childpid = fork()) == 0) {	/* child process */
			close(listenfd);	/* close listening socket */
			str_echo(connfd);	/* process the request */
			exit(0);
		}
		close(connfd);			/* parent closes connected socket */
	}
}