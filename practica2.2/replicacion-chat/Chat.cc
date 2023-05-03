#include "Chat.h"
#include"string"
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);
    //Serializar los campos type, nick y message en el buffer _data
    char* buffer = _data;
    memcpy(buffer, &type, sizeof(uint8_t));
    buffer += sizeof(uint8_t);
    memcpy(buffer, nick.c_str(), sizeof(char)* 10);
    buffer += sizeof(char) * 10;
    memcpy(buffer, message.c_str(), sizeof(char) * 80);
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE); 
    memcpy(static_cast<void*>(_data), bobj, MESSAGE_SIZE); 
    //Reconstruir la clase usando el buffer _data
     char* buffer = _data; memcpy(&type, buffer, sizeof(uint8_t)); 
     buffer += sizeof(uint8_t); char nick_buffer[10] = {0}; memcpy(nick_buffer, buffer, 10* sizeof(char)); nick = std::string(nick_buffer); buffer += 10* sizeof(char); char message_buffer[81] = {0}; memcpy(message_buffer, buffer, 80 * sizeof(char)); message = std::string(message_buffer); return 0;

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
        ChatMessage mensaje;
        
        //Recibir Mensajes en y en socketfunción del tipo de mensaje
        Socket* socket_cliente=new Socket(socket);
        socket.recv(mensaje, socket_cliente);
        // - LOGIN: Añadir al vectosocketr clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        switch(mensaje.type){
            case ChatMessage::LOGIN:{
                std::unique_ptr<Socket>socket1_(socket_cliente);
                clients.push_back(std::move(socket1_));
                std::cout<<"LOGIN DE: "<<mensaje.nick<<"\n";
                break;
            }  
            case ChatMessage::LOGOUT:{
                std::unique_ptr<Socket>socket_(socket_cliente);
                for(auto it=clients.begin();it!=clients.end();){
                    if(*(*it)==*socket_cliente){
                        clients.erase(it);
                    }
                    else ++it;
                }
                std::cout<<"LOGOUT DE: "<<mensaje.nick<<"\n";
                break;
            }
               
            case ChatMessage::MESSAGE:{
                std::cout<<"mandando mensaje"<<std::endl;
                //std::unique_ptr<Socket>socket_(socket_cliente);
                for(auto it=clients.begin();it!=clients.end();){
                    if(*(*it)==*socket_cliente){
                        std::cout<<"no soy yo"<<std::endl;
                    }
                    else{
                        socket.send(mensaje, *(*it));
                        std::cout<<"enviando"<<std::endl;
                    } 
                ++it;
                }
                break;
            }
             
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
    ChatMessage message_;
    Socket* sock_ = new Socket(socket);
    while(true)
    {
        //Recibir Mensajes de red
        socket.recv(message_, sock_);

        // Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        std::cout << message_.nick << ": " << message_.message << "\n";
     
    }
}

