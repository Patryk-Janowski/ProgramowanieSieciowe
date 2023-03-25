# Lab 4

# Zad 1

**Jan Czernecki**

**Patryk Janowski**

## Nawiązanie połączenia:

<img src="zad1/zad1-3/zad1-3.png"  style="width:1000px">

## Z funkcja dt_cli

**Stany gniazd**

<img src="zad1/zad1-4-5/no-conn/zad1-4-no-conn.png"  style="width:1000px">

**W przypadku wyłączonego serwera**

<img src="zad1/zad1-4-5/no-conn/zad1-4-no-conn-1.png"  style="width:1000px">

errno=11 pojawia sie po wywołaniu recfrom() na zamkniętym gnieździe przy ustawionej opcji SO_RECVTIMEO

<img src="zad1/zad1-4-5/no-conn/zad1-4-no-conn-2.png"  style="width:1000px">

Komunikaty sa spowodowane ustawioną opcją SO_RECVTIMEO w gnieździe 

## Z funkcja dt_cli_connect

**Stany gniazd**

<img src="zad1/zad1-4-5/conn/zad1-4-conn.png"  style="width:1000px">

**W przypadku wylaczonego serwera**

<img src="zad1/zad1-4-5/conn/zad1-4-conn-2.png"  style="width:1000px">

<img src="zad1/zad1-4-5/conn/zad1-4-conn-1.png"  style="width:1000px">

Komunikaty są wysyłane przez protokół ICMPv6

## Dodanie zabezpieczenia do funkcji dt_cli_connect

Początek funkcji dt_cli_connect

```c
dt_cli_connect(int sockfd, const SA *pservaddr, socklen_t servlen)
{
	int		n;
	char	sendline[MAXLINE], recvline[MAXLINE + 1];
	char		str[INET6_ADDRSTRLEN+1];
    struct timeval delay;
	socklen_t	len;

	delay.tv_sec = 3;  //op�nienie na gnie�dzie
	delay.tv_usec = 1; 
	len = sizeof(delay);
	if( setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &delay, len) == -1 ){
		fprintf(stderr,"SO_RCVTIMEO setsockopt error : %s\n", strerror(errno));
		exit(1);
	}
    ...
}
```

## Teraz klient ponawia polaczenie

<img src="zad1/zad1-6/zad1-6-2.png"  style="width:1000px">

Na odbieranie asynhcoroncizyncyh bledow błędy ICMP przez gniazdo UDP pozwala zastowowanei funkcji setsockopt() z argumentem IP_RECVERR

# Zad2 

## Dzialanie programu z odkomentowana linia 20

W odróżnieniu od wersji podstawowej m tutaj nie występuje errno 13 przy wywołaniu recfrom() orzy ustawionym timeout, tylko po 3 sekundach wysyłany jest sygnał SIGALARM, który następnie obsługiwany jest przez funkcje sig_alarm() - wyświetla tylko pojawienie sie alarmu, a następnie przez brak ustawionej flagi SA_RESTART w obsłudze sygnału proces czytania z gniazda zostaje przerwany (errno 4).

<img src="zad2/zad2-1.png"  style="width:1000px">

## Zmiany w kodzie

**gniazdo i inne zmienne globalne**

```c
#define MAXLINE 1024
#define SA      struct sockaddr

#define M_ALARM
#ifdef M_ALARM

struct sockaddr	*preply_addr;
struct sockaddr_in6	servaddr;
char sendline[MAXLINE];
int sockfd;
...

```

**Funkcaj sigalarm oraz dodanie lagi SA_RESTART**

```c
void sig_alarm(int signo)
{
    printf("Received SIGALARM = %d\n", signo);
	alarm(3);
	if( sendto(sockfd, sendline, 0, 0, (SA *) &servaddr, sizeof(servaddr)) <0 ){
			perror("sendto error");
			free(preply_addr);
			exit(1);
	}
}

int m_signal(int signum, void handler(int)){
    struct sigaction new_action, old_action;

  /* Set up the structure to specify the new action. */
    new_action.sa_handler = handler;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = SA_RESTART;
    ...
}
```

**Usuniecie pętli for oraz przypisanie funkcji sig_alarm jako handlera sygnału SIGALARM**

```c
    m_signal(SIGALRM, sig_alarm);
    ...

	len = servlen;

	if( sendto(sockfd, sendline, 0, 0, pservaddr, servlen) <0 ){
		perror("sendto error");
		free(preply_addr);
		exit(1);
	}
#ifdef M_ALARM
	alarm(3);
#endif
```

## Działanie programu bez pętli for

<img src="zad2/zad2-2.png"  style="width:1000px">


# Zad 3


## ustawanie TTL w serwerze

```c
TTL = strtol(argv[1], NULL, 10);

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
```

## Odbeiranie pola TTL w kliencie


```c
	// Ustawianie opcji odbierania HOP LIMIT w gnieździe na poziomie warstwy IPv6
	int yes = 1;
	if( setsockopt(sockfd, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, &yes, sizeof(yes)) < 0){
		fprintf(stderr, "IP_RECVTTL setsockopt error : %s\n", strerror(errno));
		return -1;
	}

	// Ustawianie opcji odbierania TTL w gnieździe na poziomie warstwy IP
	if( setsockopt(sockfd, IPPROTO_IP, IP_RECVTTL, &yes, sizeof(yes)) < 0){
	fprintf(stderr, "IP_RECVTTL setsockopt error : %s\n", strerror(errno));
	return -1;
	}
...

// Odbieranie TTL oraz IPV6_UNICAST_HOPS
for (cmptr = CMSG_FIRSTHDR(&msg); cmptr != NULL;
        cmptr = CMSG_NXTHDR(&msg, cmptr))
{
    if (preply_addr->sa_family == AF_INET)
    {
        pdstaddrv4->sin_family = AF_INET;
        if (cmptr->cmsg_level == IPPROTO_IP &&
            cmptr->cmsg_type == IP_PKTINFO)
        {
            memcpy(&pktinfov4, CMSG_DATA(cmptr), sizeof(struct in_pktinfo));
            memcpy(&(pdstaddrv4->sin_addr), &pktinfov4.ipi_addr, sizeof(struct in_addr));
            pdstaddrv4->sin_family = AF_INET;
        }
        else if (cmptr->cmsg_level == IPPROTO_IP && cmptr->cmsg_type == IP_RECVTTL)
        {
            memcpy(&TTL, CMSG_DATA(cmptr), sizeof(TTL));
            printf("TTL set to: %d\n", TTL);
        }
        continue;
    }
    else if (preply_addr->sa_family == AF_INET6)
    {
        pdstaddrv6->sin6_family = AF_INET6;
        if (cmptr->cmsg_level == IPPROTO_IPV6 &&
            cmptr->cmsg_type == IPV6_PKTINFO)
        {
            memcpy(&pktinfov6, CMSG_DATA(cmptr),
                    sizeof(struct in6_pktinfo));
            memcpy(&(pdstaddrv6->sin6_addr), &pktinfov6.ipi6_addr, sizeof(struct in6_addr));
            memcpy(&(pdstaddrv6->sin6_addr), &pktinfov6.ipi6_addr, sizeof(struct in6_addr));
        }
        else if (cmptr->cmsg_level == IPPROTO_IPV6 &&
            cmptr->cmsg_type == IPV6_UNICAST_HOPS)
        {
            memcpy(&TTL, CMSG_DATA(cmptr), sizeof(TTL));
            printf("TTL set to: %d\n", TTL);
        }
        continue;
    }
``` 
Odbieranie wartości TTL czy IPV6_UNICAST_HOPS nie działa do końca poprawnie, jednak spędziliśmy nad tym niepoważnie dużą ilość czasu, wiec zdecydowaliśmy się zostawić to w stanie niedokończonym :) 