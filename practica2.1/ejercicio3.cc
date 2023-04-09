#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>


int conexionUDP(const char *node, const char *service, struct addrinfo hints,
    struct addrinfo *res,int *sock){
        int s = getaddrinfo(node, service, &hints, &res);
        if (s != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
            return -1;
        }
        
        (*sock)= socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if((*sock)==-1){     
            fprintf(stderr, "socket: %s\n", gai_strerror((*sock)));
            return -1;     
        }

        return 0;

}

int main(int argc,char* argv[]){


    if (argc!= 4) {
        printf("Argumentos invalidos");
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo *result;
    int socket;
    //flags de hint

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;    
    hints.ai_socktype = SOCK_DGRAM; 

   if(conexionUDP(argv[1], argv[2], hints, result,&socket)==-1){
        fprintf(stderr, "No se ha establecido conexionUDP: %s\n");
        return -1;
    }
    struct sockaddr_in servidor;
    socklen_t servidor_len = sizeof(servidor);

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(atoi(argv[2]));
    servidor.sin_addr.s_addr = inet_addr(argv[1]);

    sendto(socket, argv[3], 1, 0, (struct sockaddr *) &servidor, servidor_len);

    char buf[800];

    int bytes = recvfrom(socket, buf, 800, 0, (struct sockaddr *) &servidor, &servidor_len);

    if(bytes==-1){
        fprintf(stderr, "recvfrom: %s\n", gai_strerror(bytes));
        exit(EXIT_FAILURE);
    }

    printf("%s", buf);
        
    close(socket);
    return 0;
}