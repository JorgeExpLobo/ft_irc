#pragma once

#include "../ACommand.hpp"

class KickCommand : public ACommand
{
public:
	KickCommand();
	~KickCommand();
	
    void execute(Server* server, Client* client, const Message& msg);
};