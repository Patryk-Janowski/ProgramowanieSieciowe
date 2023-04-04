# LAB 6 Programowanie Sieciowe
**Jan Czernecki**

**Patryk Janowski**

## Zad 1

### Modyfikacja Programu

```c
if ( (listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP)) < 0){
            fprintf(stderr,"socket error : %s\n", strerror(errno));
            return 1;
    }
```

```c
sleep(10);
close(connfd);
```
