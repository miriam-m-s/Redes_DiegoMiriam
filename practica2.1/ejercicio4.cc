#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>



int conexionTCP(const char *node, const char *service, struct addrinfo hints,
    struct addrinfo *res,int *sock){
        int s = getaddrinfo(node, service, &hints, &res);
        if (s != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
            return -1;
        }
        
        (*sock)= socket(res->ai_family, res->ai_socktype, 0);
        if((*sock)==-1){     
            fprintf(stderr, "socket: %s\n", gai_strerror((*sock)));
            return -1;     
        }
        s=bind((*sock), (struct sockaddr *) res->ai_addr, res->ai_addrlen);
         if (s != 0) {
            fprintf(stderr, "bind: %s\n", gai_strerror(s));
            return -1;
        }
        listen(*sock, 5);
        return 0;

}

int main(int argc,char* argv[]){


    if (argc!= 3) {
        printf("Argumentos invalidos");
        return -1;
    }

    struct addrinfo hints;
    struct addrinfo *result;
    int socket;
    //flags de hint

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;    
    hints.ai_socktype = SOCK_STREAM; 

   if(conexionTCP(argv[1], argv[2], hints, result,&socket)==-1){
        fprintf(stderr, "No se ha establecido conexionTCP: %s\n");
        return -1;
   }
    
    struct sockaddr_in cliente;
    socklen_t cliente_len = sizeof(cliente);
    int continua=1;

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    int cliente_socket = accept(socket, (struct sockaddr *) &cliente, &cliente_len);

    if(cliente_socket == -1 ){
        fprintf(stderr, "accept: %s\n", gai_strerror(cliente_socket));
        return -1;
    }
  
    getnameinfo((struct sockaddr *) &cliente, cliente_len, host, 
        NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
    printf("Conexión desde %s %s\n",host, serv);


    while (continua) {
        char buf[800];
        int bytes = recv(cliente_socket, buf, 800, 0);
        if(bytes==-1){
            fprintf(stderr, "recv: %s\n", gai_strerror(bytes));
            exit(EXIT_FAILURE);
        }
      
        if(bytes!=0){
            send(cliente_socket, buf, bytes, 0);
        }
        else continua = 0;
    }

    close(socket);
    printf("Conexión terminada\n");

    return 0;
}