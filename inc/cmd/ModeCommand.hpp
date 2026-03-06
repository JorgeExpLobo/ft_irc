#pragma once

#include "ACommand.hpp"

class ModeCommand : public ACommand
{
public:
    void execute(Server* server, Client* client, const Message& msg);
};