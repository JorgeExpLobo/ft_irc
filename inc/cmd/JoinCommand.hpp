#pragma once

#include "ACommand.hpp"

class JoinCommand : public ACommand
{
public:
    void execute(Server* server, Client* client, const Message& msg);
};