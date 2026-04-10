#include "ListCommand.hpp"
#include "Server.hpp"
#include "Reply.hpp"
#include "Channel.hpp"

ListCommand::ListCommand() {}
ListCommand::~ListCommand() {}

void ListCommand::execute(Server* server, Client* client, const Message& msg)
{
	(void)msg;

	if (!client->isRegistered())
	{
		server->sendToClient(
			client,
			Reply::errNotRegistered(client->getNickname()).stringify()
		);
		return;
	}

	const std::vector<Channel*>& channels = server->getChannels();

	for (std::vector<Channel*>::const_iterator it = channels.begin();
		 it != channels.end();
		 ++it)
	{
		Channel* chan = *it;
		if (!chan)
			continue;

		std::stringstream users;
		users << chan->getClients().size();

		server->sendToClient(
			client,
			Reply::list(
				client->getNickname(),
				chan->getName(),
				users.str(),
				chan->getTopic()
			).stringify()
		);
	}

	server->sendToClient(
		client,
		Reply::listEnd(client->getNickname()).stringify()
	);
}