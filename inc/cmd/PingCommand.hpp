#ifndef PING_HPP
#define PING_HPP

#include "ACommand.hpp"

class PingCommand : public ACommand
{
    public:
        PingCommand();
        ~PingCommand();

        void execute(Server* server, Client* client, const Message& msg);
};

#endif