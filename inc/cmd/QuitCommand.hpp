#pragma once

#include "ACommand.hpp"

class QuitCommand : public ACommand
{
public:
    void execute(Server* server, Client* client, const Message& msg);
};