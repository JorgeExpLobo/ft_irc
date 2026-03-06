#pragma once

#include "../ACommand.hpp"

class PassCommand : public ACommand
{
public:
	PassCommand();
	~PassCommand();
	
    void execute(Server* server, Client* client, const Message& msg);
};