#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>



class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, 80);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t size=2*sizeof(int16_t) + strlen(name) * sizeof(char);

        alloc_data(size);
        char* aux=_data;

        memcpy(aux, &x, sizeof(int16_t));
        aux+=sizeof(int16_t);
        memcpy(aux, &y, sizeof(int16_t));
        aux+=sizeof(int16_t);
        memcpy(aux, &name, strlen(name));
    }

    int from_bin(char * data)
    {
        char *aux = data;
        memcpy(&x, aux, sizeof(int16_t));
        aux+=sizeof(int16_t);
        memcpy(&y, aux, sizeof(int16_t));
        aux+=sizeof(int16_t);
        memcpy(&name, aux, 80);


        return 0;
    }


public:
    char name[80];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    // 1. Serializar el objeto one_w
    one_w.to_bin();
    // 2. Escribir la serialización en un fichero
    int fd = open("infoJugador.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
 
        return 1;
    }

    int bytes = write(fd, one_w.data(), one_w.size());
    if (bytes == -1) {
    
        return -1;
    }
    // 3. Leer el fichero
    
     fd = open("infoJugador.txt", O_RDONLY);
     if (fd == -1) {
        fprintf(stderr, "getopen: %s\n", gai_strerror(fd));
            exit(EXIT_FAILURE);
        return 1;
    }
    char buffer[2048];
    bytes = read(fd, buffer, 2048);
    if (bytes == -1) {
       fprintf(stderr, "getread: %s\n", gai_strerror(bytes));
            exit(EXIT_FAILURE);
        return 1;
    }

    // 4. "Deserializar" en one_r
    one_r.from_bin(buffer);
    // 5. Mostrar el contenido de one_r

    std::cout << one_r.x << '\n';
    std::cout << one_r.y << '\n';
    std::cout << one_r.name << '\n';

    close(fd);
    return 0;
}

