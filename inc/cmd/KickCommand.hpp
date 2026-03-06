#pragma once

#include "ACommand.hpp"

class KickCommand : public ACommand
{
public:
    void execute(Server* server, Client* client, const Message& msg);
};