#pragma once

#include "ACommand.hpp"

class TopicCommand : public ACommand
{
public:
    void execute(Server* server, Client* client, const Message& msg);
};