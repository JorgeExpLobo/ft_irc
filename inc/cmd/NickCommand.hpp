#pragma once

#include "ACommand.hpp"

class NickCommand : public ACommand
{
public:
    void execute(Server* server, Client* client, const Message& msg);
};