#include "Chat.h"
#include"string"
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);
    //Serializar los campos type, nick y message en el buffer _data
    char *aux = _data;
    //metemos type
    memcpy(aux, &type, sizeof(uint8_t));
    aux += sizeof(uint8_t);

    int16_t sizenick=nick.lenght();
    memcpy(aux, &sizenick, sizeof(int16_t));
    aux += sizeof(int16_t);
    //metemos nick 
    memcpy(aux, nick.c_str(), nick.lenght()*sizeof(char));
    aux += nick.lenght();

    int16_t sizemessage=message.lenght();
    memcpy(aux, &sizemessage, sizeof(int16_t));
    aux += sizeof(int16_t);

    memcpy(aux, message.c_str(), message.lenght());

    //Serializar los campos type, nick y message en el buffer _data
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char *aux = _data;

    memcpy(&type, aux, sizeof(uint8_t));
    aux += sizeof(uint8_t);

    int sizeofnick;
    memcpy(&sizeofnick, aux, sizeof(int16_t));
    aux += sizeof(int16_t);
    //CON UN STRING SE PUEDE HACER ESTO
    char* auxString=malloc(sizeofnick);
    memcpy(&auxString, aux, sizeofnick);
    nick = std::string(char_array);
    aux += sizeofnick;
    free(auxString);

    int sizeofmessage;

    memcpy(&sizeofmessage, aux, sizeof(int16_t));
    aux+=sizeofmessage;
    message = aux;
    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {

        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */
        ChatMessage* mensaje=new ChatMessage();
        
        //Recibir Mensajes en y en función del tipo de mensaje
        Socket* socket_cliente;
        socket.recv(mensaje, socket_cliente);
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        switch(mensaje.type){
            case LOGIN:
            std::unique_ptr<Socket>socket_(socket_cliente);
            clientes.push_back(std::move(socket_));
            std::cout<<"LOGIN DE: "<<mensaje.nick<<"\n";
            break;
            case LOGOUT:
            std::unique_ptr<Socket>socket_(socket_cliente);
            for(auto it=clientes.begin();it!=clientes.end();){
                if(*(*it)==*socket_cliente){
                    clientes.erase(it);
                }
                else ++it;
            }
             std::cout<<"LOGOUT DE: "<<mensaje.nick<<"\n";
            break;
            case MESSAGE:
             std::unique_ptr<Socket>socket_(socket_cliente);
            for(auto it=clientes.begin();it!=clientes.end();){
                if(*(*it)!=*socket_cliente){
                   *(*it).send(mensaje, *(*it));
                }
               ++it;
            }
            break;
        }
      
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        std::string msg;
        std::getline(std::cin, msg);

        if(msg == "logout") break;
        ChatMessage em(nick, msg);

        em.type = ChatMessage::MESSAGE;
        
        // Enviar al servidor usando socket
        socket.send(em, socket);
    }
    logout();
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

