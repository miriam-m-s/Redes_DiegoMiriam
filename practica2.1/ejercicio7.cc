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
#include <pthread.h>
#include <thread>

class hijo{
     
private:    
    int cliente_socket;

public:
    
    hijo(int socket_){
        cliente_socket = socket_;
    }

    void recibeMensaje(){
        int continua=1;
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
         printf("Conexión terminada\n");
        
    }

};

struct initInfo{
    const char *node;
    const char *service;
    struct addrinfo hints;
    struct addrinfo *res;
    int *sock;
};

void * conexionTCP(void *arg){
    // const char *node, const char *service, struct addrinfo hints,
    // struct addrinfo *res,int *sock

    struct initInfo* info = (struct initInfo*)arg;

    int s = getaddrinfo(info->node, info->service, &(info->hints), &(info->res));
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
    }

    (*(info->sock))= socket(info->res->ai_family, info->res->ai_socktype, 0);
    if((*(info->sock))==-1){     
        fprintf(stderr, "socket: %s\n", gai_strerror((*(info->sock))));  
    }
    s=bind((*info->sock), (struct sockaddr *) info->res->ai_addr, info->res->ai_addrlen);
        if (s != 0) {
        fprintf(stderr, "bind: %s\n", gai_strerror(s));
    }
    
     listen( (*(info->sock)), 5);
}

int main(int argc,char* argv[]){


    if (argc!=3) {
        printf("Argumentos invalidos");
        return -1;
    }

    pthread_t init;
    struct initInfo* info=new initInfo();
    
    info->node=argv[1];
    info->service=argv[2];

    struct addrinfo hints;
    struct addrinfo *result=new addrinfo();

    //flags de hint
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;    
    hints.ai_socktype = SOCK_STREAM; 

    info->hints = hints;
    info->res = result;
    info->sock=new int();
    //pthread_create(&init, NULL, conexionUDP, (void*)info);

    conexionTCP((void*)info);

       
    
    struct sockaddr_in cliente;
    socklen_t cliente_len = sizeof(cliente);
    int continua=1;
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    while (continua){
        int cliente_socket = accept(*(info->sock), (struct sockaddr *) &cliente, &cliente_len);

        if(cliente_socket == -1 ){
            fprintf(stderr, "accept: %s\n", gai_strerror(cliente_socket));
            continua=0;
            return -1;
        }
    
        getnameinfo((struct sockaddr *) &cliente, cliente_len, host, 
            NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST|NI_NUMERICSERV);
        printf("Conexión desde %s %s\n",host, serv);

        //CREACION HIJO
         hijo *h = new hijo(cliente_socket);
         auto thread_func = [&h]() {
         h->recibeMensaje();
        delete h;
        };
        std::thread t(thread_func);
        t.detach();

    }

    close(*(info->sock));
    return 0;
}