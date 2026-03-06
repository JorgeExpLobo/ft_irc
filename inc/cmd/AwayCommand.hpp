#pragma once

#include "ACommand.hpp"

class AwayCommand : public ACommand
{
public:
    void execute(Server* server, Client* client, const Message& msg);
};