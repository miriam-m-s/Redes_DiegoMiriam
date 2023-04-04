#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>

int tiposSocket[3] = { SOCK_STREAM, SOCK_DGRAM, SOCK_RAW };
int tipoAddress[2] = {AF_INET, AF_INET6};

void resultado(const char *node, const char *service, struct addrinfo hints,
    struct addrinfo *res){

    for(int i = 0; i < 3; i++){

        hints.ai_socktype = tiposSocket[i];
        int s = getaddrinfo(node, NULL, &hints, &res);
        if (s != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
            exit(EXIT_FAILURE);
        }

        char host[1000], serv[1000];

        s = getnameinfo(res->ai_addr, res->ai_addrlen, host, sizeof(host),
            serv, sizeof(serv), NI_NUMERICHOST);

        if (s != 0) {
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
            exit(EXIT_FAILURE);
        }

        printf("%s  %d  %d \n", host, res->ai_family, res->ai_socktype);
    }
}

int main(int argc,char* argv[]){


    if (argc < 1) {
        printf("Argumentos invalidos");
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo *result;

    memset(&hints, 0, sizeof(struct addrinfo));

    for(int i = 0; i < 2; i++){
        hints.ai_family = tipoAddress[i];
        resultado(argv[1], argv[2], hints, result);
    }

    return 0;
}