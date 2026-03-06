#pragma once

#include "../ACommand.hpp"

class PrivmsgCommand : public ACommand
{
public:
	PrivmsgCommand();
	~PrivmsgCommand();
	
    void execute(Server* server, Client* client, const Message& msg);
};