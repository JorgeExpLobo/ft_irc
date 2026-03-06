#pragma once

#include "../ACommand.hpp"

class NickCommand : public ACommand
{
public:
	NickCommand();
	~NickCommand();
	
    void execute(Server* server, Client* client, const Message& msg);
};