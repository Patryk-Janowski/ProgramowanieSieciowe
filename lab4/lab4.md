# Lab 4

# Zad 1

## Nawiazanie polaczenia:

<img src="zad1/zad1-3/zad1-3.png"  style="width:1000px">

## Z funkcja dt_cli

**Stany gniazd**

<img src="zad1/zad1-4-5/no-conn/zad1-4-no-conn.png"  style="width:1000px">

**W przypadku wylaczonego serwera**

<img src="zad1/zad1-4-5/no-conn/zad1-4-no-conn-1.png"  style="width:1000px">

errno=11 pojawia sie po wywolaniu recfrom() na zamkniętym gnziedzie przy ustawionej opcji SO_RECVTIMEO

<img src="zad1/zad1-4-5/no-conn/zad1-4-no-conn-2.png"  style="width:1000px">

Komunikaty sa spowodowane ustawioan opcją SO_RECVTIMEO w gniezdzie 

## Z funkcja dt_cli_connect

**Stany gniazd**

<img src="zad1/zad1-4-5/conn/zad1-4-conn.png"  style="width:1000px">

**W przypadku wylaczonego serwera**

<img src="zad1/zad1-4-5/conn/zad1-4-conn-2.png"  style="width:1000px">

<img src="zad1/zad1-4-5/conn/zad1-4-conn-1.png"  style="width:1000px">

komunikaty sa wysylane przez protokol ICMPv6

## Dodanie zabezpieczenia do funkcji dt_cli_connect

poczatek funkcji dt_cli_connect

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
```

## Teraz klient ponawia polaczenie

<img src="zad1/zad1-6/zad1-6-2.png"  style="width:1000px">

Na odbieranie asynhcoroncizyncyh bledow błędy ICMP przez gniazdo UDP pozwala zastowowanei funkcji setsockopt() z argumentem IP_RECVERR

# Zad2 