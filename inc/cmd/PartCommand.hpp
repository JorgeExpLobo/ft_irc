#pragma once

#include "../ACommand.hpp"

class PartCommand : public ACommand
{
public:
	PartCommand();
	~PartCommand();
	
    void execute(Server* server, Client* client, const Message& msg);
};