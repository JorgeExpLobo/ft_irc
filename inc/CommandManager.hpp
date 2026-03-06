#pragma once

#include <map>
#include <string>
#include <sys/socket.h>
#include "ACommand.hpp"

class Server;

class CommandManager
{
private:
	std::map<std::string, ACommand*> _commands;

public:
	CommandManager();
	~CommandManager();

	void registerCommand(const std::string& name, ACommand* cmd);

	void execute(Server* server, Client* client, const Message& msg);
};
