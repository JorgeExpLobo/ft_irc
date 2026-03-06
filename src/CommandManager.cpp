#include "CommandManager.hpp"
#include "Reply.hpp"

CommandManager::CommandManager() {}

CommandManager::~CommandManager()
{
	for (std::map<std::string, ACommand*>::iterator it = _commands.begin();
		 it != _commands.end(); ++it)
		delete it->second;
}

void CommandManager::registerCommand(const std::string& name, ACommand* cmd)
{
	_commands[name] = cmd;
}

void CommandManager::execute(Server* server, Client* client, const Message& msg)
{
	std::string cmd = msg.command();

	std::map<std::string, ACommand*>::iterator it = _commands.find(cmd);

	if (it == _commands.end())
	{
		Message err = Reply::errUnknownCommand(client->getNickname(), cmd);
		std::string out = err.stringify() + "\r\n";
		send(client->getFd(), out.c_str(), out.size(), 0);
		return;
	}

	it->second->execute(server, client, msg);
}