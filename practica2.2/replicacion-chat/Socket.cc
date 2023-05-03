#include <string.h>

#include "Serializable.h"
#include "Socket.h"

Socket::Socket(const char * address, const char * port):sd(-1)
{
    //Construir un socket de tipo AF_INET y SOCK_DGRAM usando getaddrinfo.
    // translate name to socket addresses
   
    addrinfo hints; 
    addrinfo *result;

    memset(&hints, 0, sizeof(addrinfo));
    
    hints.ai_family   = AF_INET;    
    hints.ai_socktype = SOCK_DGRAM; 
    hints.ai_flags    = AI_PASSIVE; 
   
    int err = getaddrinfo(address, port, &hints, &result);
    // open socket with result content. Always 0 to TCP and UDP
    sd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    //Con el resultado inicializar los miembros sd, sa y sa_len de la clase
    // associate address. Where is going to listen
    sa = (struct sockaddr)*(result->ai_addr);
    sa_len = (result->ai_addrlen);

}

int Socket::recv(Serializable &obj, Socket * &sock)
{
    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[MAX_MESSAGE_SIZE];

    ssize_t bytes = ::recvfrom(sd, buffer, MAX_MESSAGE_SIZE, 0, &sa, &sa_len);

    if ( bytes <= 0 )
    {
        return -1;
    }

    if ( sock != 0 )
    {
        sock = new Socket(&sa, sa_len);
    }

    obj.from_bin(buffer);

    return 0;
}

int Socket::send(Serializable& obj, const Socket& sock)
{
    //Serializar el objeto
    obj.to_bin();
    
    //Enviar el objeto binario a sock usando el socket sd
    return sendto(sd, obj.data(), obj.size(), 0, (struct sockaddr *) &sock.sa, sock.sa_len);
}

bool operator== (const Socket &s1, const Socket &s2)
{
     sockaddr_in* socket1 = (sockaddr_in*) &(s1.sa);
    sockaddr_in* socket2 = (sockaddr_in*) &(s2.sa);
    return socket1->sin_family == socket2->sin_family && 
        socket1->sin_addr.s_addr == socket2->sin_addr.s_addr &&
        socket1->sin_port == socket2->sin_port;

    
};

std::ostream& operator<<(std::ostream& os, const Socket& s)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo((struct sockaddr *) &(s.sa), s.sa_len, host, NI_MAXHOST, serv,
                NI_MAXSERV, NI_NUMERICHOST);

    os << host << ":" << serv;

    return os;
};

