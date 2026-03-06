#pragma once

#include "ACommand.hpp"

class UserCommand : public ACommand
{
public:
    void execute(Server* server, Client* client, const Message& msg);
};