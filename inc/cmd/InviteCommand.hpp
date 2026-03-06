#pragma once

#include "../ACommand.hpp"

class InviteCommand : public ACommand
{
public:
	InviteCommand();
	~InviteCommand();
	
    void execute(Server* server, Client* client, const Message& msg);
};