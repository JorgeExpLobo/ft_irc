/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 20:29:46 by jdiaz-he          #+#    #+#             */
/*   Updated: 2026/03/18 17:08:14 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP


# include <vector>
# include <map>
# include <string>
# include <poll.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <unistd.h>
#include <algorithm>
#include "Client.hpp"
#include "Channel.hpp"
#include "CommandManager.hpp"


class Server 
{
private:
	int							_port;
	std::string					_password;
	int							_server_master_fd;
	bool						_is_running;

	// Connection Management
	std::vector<struct pollfd>	_poll_fds;
	
	std::map<int, Client*>	_clients;
	std::vector<Channel*> _channels;

	// Engine
	void	establishNewConnection();
	void	processIncomingData(int fd);

	// chat IRC
	void	executeIrcCommand(int fd, std::string cmd_line);
	void	removeClientFromAllChannels(int fd);

	CommandManager _commandManager;

public:
	Server(int port, std::string password);
	~Server();

	void	init();
	void	run();
	void	stopEngine();

	// Getters
	int			getPort() const { return _port; }
	std::string	getPassword() const { return _password; }
	const std::map<int, Client*>& getClients() const { return _clients; }

	// CHANNEL MANAGEMENT

	Channel* getChannel(const std::string& name);
	Channel* createChannel(const std::string& name, Client* creator);
	Channel* getOrCreateChannel(const std::string& name, Client* creator);
	void removeChannel(const std::string& name);
	void addClientToChannel(Client* client, const std::string& channel_name);
	void removeClientFromChannel(Client* client, const std::string& channel_name);
	void terminateClientConnection(int fd);
	void 	disconnectClient(int fd);
	// MESSAGE HELPERS

	void sendToClient(Client* client, const std::string& message);
	void broadcastToChannel(Channel* channel, const std::string& message, int exclude_fd);

	//  AUX functions
	bool nickExists(const std::string& nick) const;
	Channel* findChannel(const std::string& name);
	Client* findClient(const std::string& nickname);
};

// signal handler
void handleSignal(int signum);

#endif