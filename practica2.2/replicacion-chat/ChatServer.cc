#include "Chat.h"
class ChatServer
{
public:
    ChatServer(const char * s, const char * p): socket(s, p)
    {
        socket.bind();
    };

    void do_messages(){
        
    };

private:
    std::vector<Socket *> clients;

    Socket socket;
};

int main(int argc, char **argv)
{
    ChatServer es(argv[1], argv[2]);

    es.do_messages();

    return 0;
}
