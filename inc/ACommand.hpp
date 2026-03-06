#pragma once

#include "Client.hpp"
#include "Message.hpp"

class Server;

class ACommand
{
public:
	virtual ~ACommand() {}

	virtual void execute(Server* server, Client* client, const Message& msg) = 0;
};