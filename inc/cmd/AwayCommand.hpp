#pragma once

#include "../ACommand.hpp"

class AwayCommand : public ACommand
{
public:
	AwayCommand();
	~AwayCommand();
	
    void execute(Server* server, Client* client, const Message& msg);
};