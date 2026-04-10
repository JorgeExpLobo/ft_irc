#pragma once

#include "ACommand.hpp"

class ListCommand : public ACommand
{
public:
	ListCommand();
	~ListCommand();
	
    void execute(Server* server, Client* client, const Message& msg);
};