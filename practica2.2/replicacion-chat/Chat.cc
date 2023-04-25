#include "Chat.h"

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

    int16_t sizenick=sizeof(nick);
    memcpy(aux, &sizenick, sizeof(int16_t));
    aux += sizeof(int16_t);
    //metemos nick 
    memcpy(aux, nick.c_str(), sizeof(nick));
    aux += sizeof(nick);

    int16_t sizemessage=sizeof(message);
    memcpy(aux, &sizemessage, sizeof(int16_t));
    aux += sizeof(int16_t);

    memcpy(aux, message.c_str(), sizeof(message));

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
    nick = aux;
    aux += sizeofnick;
    int sizeofmessage;
    memcpy(&sizeofmessage, aux, sizeof(int16_t));
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

        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
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
    // Completar
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
    }
}

