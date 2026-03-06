#pragma once

#include "../ACommand.hpp"

class QuitCommand : public ACommand
{
public:
	QuitCommand();
	~QuitCommand();
	
    void execute(Server* server, Client* client, const Message& msg);
};