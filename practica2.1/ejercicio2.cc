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



int conexionUDP(const char *node, const char *service, struct addrinfo hints,
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
        return 0;

}

int main(int argc,char* argv[]){


    if (argc!=3) {
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
    hints.ai_flags = AI_PASSIVE; 

   if(conexionUDP(argv[1], argv[2], hints, result,&socket)==-1){
        fprintf(stderr, "No se ha establecido conexionUDP: %s\n");
        return -1;
   }
     struct sockaddr cliente;
    socklen_t cliente_len = sizeof(cliente);
    int continua=1;

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    time_t t;
    struct tm *tmp;
    time(&t);

    while (continua) {
        char buf[800];
        int bytes = recvfrom(socket, buf, 800, 0,&cliente, &cliente_len);
        if(bytes==-1){
            fprintf(stderr, "recvfrom: %s\n", gai_strerror(bytes));
            exit(EXIT_FAILURE);
        }

        buf[bytes]='\0'; 
            
        int error=getnameinfo((struct sockaddr *) &cliente, cliente_len, host, NI_MAXHOST,
            serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);

        if(error==-1){
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(error));
            exit(EXIT_FAILURE);
        }

        printf("%d bytes de %s:%s\n",bytes,host,serv);
        bytes=0;
        if(buf[0]=='t'){
            tmp = localtime(&t);
            bytes = strftime(buf, 800, "%H:%M:%S\n", tmp);
        }
        else if(buf[0]=='d'){
             tmp = localtime(&t);
             bytes = strftime(buf, 800, "%F\n", tmp);

        }
         else if(buf[0]=='q'){
             continua =0;
             printf("Saliendo...\n");
             bytes=2;
             buf[0]='^';
             buf[1]='C';

        }
        else{
            printf("Comando no soportado %c \n",buf[0]);
        }
        
      
       
        if(bytes!=0)
        sendto(socket, buf, bytes, 0, (struct sockaddr *) &cliente, cliente_len);
    }
    close(socket);
    return 0;
}