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
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>

int conexionTCP(const char *node, const char *service, struct addrinfo hints,
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
        // s=bind((*sock), (struct sockaddr *) res->ai_addr, res->ai_addrlen);
        //  if (s != 0) {
        //     fprintf(stderr, "bind: %s\n", gai_strerror(s));
        //     return -1;
        // }
        //listen(*sock, 5);
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
    
     struct sockaddr_in servidor;
    socklen_t servidor_len = sizeof(servidor);

    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(atoi(argv[2]));
    servidor.sin_addr.s_addr = inet_addr(argv[1]);

    int continua=1;

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    int servidor_socket = connect(socket, (struct sockaddr *) &servidor, servidor_len);

    if (servidor_socket == -1 ){
        fprintf(stderr, "accept: %s\n", gai_strerror(servidor_socket));
        return -1;
    }
  
   

    char mensaje[800];
    while (continua) {
        std::cin>>mensaje;
        char buf[800];
        if(mensaje[0]=='Q') continua = 0;
        else{
            int s = send (socket, mensaje, 800, 0);
            if(s==-1){
                fprintf(stderr, "send: %s\n", gai_strerror(s));
                exit(EXIT_FAILURE);
            }
            int bytes = recv (socket, buf, 800, 0);
            if(bytes==-1){
                fprintf(stderr, "recv: %s\n", gai_strerror(bytes));
                exit(EXIT_FAILURE);
            }
            printf("%s \n",buf);
        }
        
    }

    close(socket);
    return 0;
}