#pragma once

#include "ACommand.hpp"

class InviteCommand : public ACommand
{
public:
    void execute(Server* server, Client* client, const Message& msg);
};