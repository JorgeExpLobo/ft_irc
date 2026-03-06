#pragma once

#include "../ACommand.hpp"

class UserCommand : public ACommand
{
public:
	UserCommand();
	~UserCommand();
	
    void execute(Server* server, Client* client, const Message& msg);
};