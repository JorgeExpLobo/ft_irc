/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdiaz-he <jdiaz-he@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 18:29:50 by jdiaz-he          #+#    #+#             */
/*   Updated: 2026/04/09 20:26:55 by jdiaz-he         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "CommandManager.hpp"
#include "PassCommand.hpp"
#include "NickCommand.hpp"
#include "UserCommand.hpp"
#include "JoinCommand.hpp"
#include "PartCommand.hpp"
#include "PrivmsgCommand.hpp"
#include "QuitCommand.hpp"
#include "InviteCommand.hpp"
#include "KickCommand.hpp"
#include "TopicCommand.hpp"
#include "ModeCommand.hpp"
#include "AwayCommand.hpp"
#include "PingCommand.hpp"
#include "ListCommand.hpp"

#include <cstring>
#include <iostream>
#include <cstdio>
#include <cerrno>
#include <csignal> 

bool g_server_shutdown = false;

void handleSignal(int signum) 
{
	if (signum == SIGINT) 
	{
		std::cout << "\n[INFO] SIGINT signal received. Securely shutting down server..." << std::endl;
		g_server_shutdown = true;
	}
}

Server::Server(int port, std::string password) 
	: _port(port), _password(password), _server_master_fd(-1), _is_running(false) {}

Server::~Server() 
{
	this->stopEngine();
}

void Server::init() 
{
	// Configurar el manejador de señales
	signal(SIGINT, handleSignal);

	_server_master_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_master_fd == -1) throw std::runtime_error("Error: The master socket could not be created.");

	int opt_reuse = 1;
	if (setsockopt(_server_master_fd, SOL_SOCKET, SO_REUSEADDR, &opt_reuse, sizeof(opt_reuse)) == -1)
		throw std::runtime_error("Error: Can not configure SO_REUSEADDR.");

	// OBLIGATORIO: fcntl para modo no bloqueante
	if (fcntl(_server_master_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Error: fcntl could not set O_NONBLOCK.");

	struct sockaddr_in server_address;
	std::memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(_port);

	if (bind(_server_master_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
		throw std::runtime_error("Error: Bind failed (port busy?).");

	if (listen(_server_master_fd, 128) == -1)
		throw std::runtime_error("Error: Listen failed.");

	struct pollfd master_pfd;
	master_pfd.fd = _server_master_fd;
	master_pfd.events = POLLIN;
	master_pfd.revents = 0;
	_poll_fds.push_back(master_pfd);

	_is_running = true;
	_commandManager.registerCommand("PASS", new PassCommand());
	_commandManager.registerCommand("NICK", new NickCommand());
	_commandManager.registerCommand("USER", new UserCommand());
	_commandManager.registerCommand("JOIN", new JoinCommand());
	_commandManager.registerCommand("PART", new PartCommand());
	_commandManager.registerCommand("PRIVMSG", new PrivmsgCommand());
	_commandManager.registerCommand("QUIT", new QuitCommand());
	_commandManager.registerCommand("INVITE", new InviteCommand());
	_commandManager.registerCommand("KICK", new KickCommand());
	_commandManager.registerCommand("TOPIC", new TopicCommand());
	_commandManager.registerCommand("MODE", new ModeCommand());
	_commandManager.registerCommand("AWAY", new AwayCommand());
	_commandManager.registerCommand("PING", new PingCommand());
	_commandManager.registerCommand("LIST", new ListCommand());
	
	std::cout << "--- IRC server 'The Engine' started ---" << std::endl;
	std::cout << "Port: " << _port << " | Password: " << _password << std::endl;
}

void Server::run() 
{
	while (_is_running && !g_server_shutdown) 
	{
		// We use a timeout of 1 second so we can check g_server_shutdown
		int poll_count = poll(&_poll_fds[0], _poll_fds.size(), 1000);
		
		if (poll_count < 0) 
		{
			if (g_server_shutdown) 
				break;
			std::cerr << "[ERROR] Error poll: " << std::strerror(errno) << std::endl;
				break;
		}

		for (size_t i = 0; i < _poll_fds.size(); ++i)
		{
			int current_fd = _poll_fds[i].fd;

			if (_poll_fds[i].revents & POLLIN)
			{
				if (current_fd == _server_master_fd)
					this->establishNewConnection();
				else
					this->processIncomingData(current_fd);
			}
			if (i < _poll_fds.size() && (_poll_fds[i].revents & (POLLERR | POLLHUP)))
			{
				this->terminateClientConnection(current_fd);
				--i;
			}
		}
	}
	this->stopEngine();
}

void Server::establishNewConnection() 
{
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	int new_client_fd = accept(_server_master_fd, (struct sockaddr *)&client_addr, &addr_len);

	if (new_client_fd != -1) 
	{
		fcntl(new_client_fd, F_SETFL, O_NONBLOCK);
		
		struct pollfd client_pfd;
		client_pfd.fd = new_client_fd;
		client_pfd.events = POLLIN;
		client_pfd.revents = 0;
		_poll_fds.push_back(client_pfd);
		
		Client* new_client = new Client(new_client_fd, "localhost");
		_clients[new_client_fd] = new_client;
		std::cout << "[+] Client linked in FD: " << new_client_fd << std::endl;
	}
}

void Server::processIncomingData(int fd)
{
	char read_buffer[512];
	std::memset(read_buffer, 0, sizeof(read_buffer));

	int bytes_received = recv(fd, read_buffer, sizeof(read_buffer) - 1, 0);

	if (bytes_received == 0)
	{
		this->terminateClientConnection(fd);
		return;
	}
	else if (bytes_received < 0)
	{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
				this->terminateClientConnection(fd);
			return;	
	}

	read_buffer[bytes_received] = '\0';
	
	////////////// TEST
	//std::cout << "REDIBIDO recv buffer:\n" << read_buffer << "------" << std::endl;
	///////////////
	
	//  save the client before processing
	std::map<int, Client*>::iterator itClient = _clients.find(fd);
	if (itClient == _clients.end())
		return;
	Client* client = itClient->second;

	// --- BUFFER MANAGEMENT ---
	client->appendBuffer(read_buffer);

	// --- PROCESS COMMANDS ---
	std::string& current_buffer = client->getBuffer();

	size_t newline_pos;
	while ((newline_pos = current_buffer.find("\n")) != std::string::npos)
	{
		std::string raw_command = current_buffer.substr(0, newline_pos);

		if (!raw_command.empty() && raw_command[raw_command.size() - 1] == '\r')
			raw_command.erase(raw_command.size() - 1);

		Message msg;
		if (!msg.parseRequest(raw_command))
			return;

		_commandManager.execute(this, client, msg);

		// If the client was removed (QUIT, error, etc.), exit
		if (_clients.find(fd) == _clients.end())
			return;

		current_buffer.erase(0, newline_pos + 1);
	}
}

void Server::terminateClientConnection(int fd) 
{
	std::map<int, Client*>::iterator it_client = _clients.find(fd);
	if (it_client == _clients.end())
		return;

	Client* client = it_client->second;
	std::string nick = client->getNickname();
	if (!nick.empty() && nick != "*")
		removeNick(nick);

	std::cout << "[DISCONNECT] "
			  << (client->getNickname().empty() ? "Unknown" : client->getNickname())
			  << " (" << client->getUsername() << "@" << client->getHost() << ")"
			  << " en FD: " << fd << std::endl;

	this->removeClientFromAllChannels(fd);

	// Important: erase() invalidates the iterator, that's why the break is vital.
	for (std::vector<struct pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it) 
	{
		if (it->fd == fd) 
		{
			_poll_fds.erase(it);
			break; 
		}
	}

	close(fd);
	
	_clients.erase(it_client);
	delete client;
}

void Server::stopEngine() 
{
	if (!_is_running && _poll_fds.empty())
		return;

	_is_running = false;
	std::cout << "[SHUTDOWN] Cleaning up resources and closing FDs..." << std::endl;

	// Close all client sockets
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		close(it->first); 
		delete it->second;     
	}
	_clients.clear();

   // delete all channels
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		delete *it;
	}
	_channels.clear();

	
	if (_server_master_fd != -1)
		close(_server_master_fd);
	_server_master_fd = -1;

	_poll_fds.clear();
}

Channel* Server::findChannel(const std::string& name)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i]->getName() == name)
			return _channels[i];
	}
	return NULL;
}

Client* Server::findClient(const std::string& nickname)
{
	for (std::map<int, Client*>::iterator it = _clients.begin();  it != _clients.end(); ++it)
	{
		if (it->second->getNickname() == nickname)
			return it->second;
	}
	return NULL;
}

Channel* Server::getChannel(const std::string& name)
{
	for (std::vector<Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if ((*it)->getName() == name)
			return *it;
	}
	return NULL;
}

const std::vector<Channel*>& Server::getChannels() const
{
    return _channels;
}

Channel* Server::createChannel(const std::string& name, Client* creator)
{
	Channel* channel = new Channel(name);

	(void)creator;

	_channels.push_back(channel);
   
	return channel;
}

Channel* Server::getOrCreateChannel(const std::string& name, Client* creator)
{
	for (size_t i = 0; i < _channels.size(); i++)
	{
		if (_channels[i]->getName() == name)
			return _channels[i];
	}
	return createChannel(name, creator);
}

void Server::addClientToChannel(Client* client, const std::string& channel_name)
{
	Channel* channel = findChannel(channel_name);

	if (!channel)
	{
		channel = createChannel(channel_name, client);
		return;
	}

	channel->addClient(client);
	client->joinChannel(channel);

	std::cout << "[JOIN] "
			  << client->getNickname()
			  << " -> "
			  << channel_name
			  << std::endl;
}

void Server::removeClientFromChannel(Client* client, const std::string& channel_name)
{
	Channel* channel = findChannel(channel_name);

	if (!channel)
		return;

	channel->removeClient(client);
	client->leaveChannel(channel);

	std::cout << "[PART] "
			  << client->getNickname()
			  << " <- "
			  << channel_name
			  << std::endl;

	if (channel->isEmpty())
		removeChannel(channel_name);
}

void Server::removeChannel(const std::string& name)
{
	for (std::vector<Channel*>::iterator it = _channels.begin();
		 it != _channels.end(); ++it)
	{
		if ((*it)->getName() == name)
		{
			std::cout << "[CHANNEL REMOVED] " << name << std::endl;

			delete *it;
			_channels.erase(it);
			return;
		}
	}
}

void Server::removeClientFromAllChannels(int fd)
{
	std::map<int, Client*>::iterator itClient = _clients.find(fd);
	if (itClient == _clients.end())
		return;
	Client* client = itClient->second;

	std::set<Channel*> channels = client->getChannels();
	std::vector<std::string> toDelete;

	for (std::set<Channel*>::iterator it = channels.begin();
		 it != channels.end(); ++it)
	{
		Channel* channel = *it;

		channel->removeClient(client);
		client->leaveChannel(channel);
		
		channel->notifyQuit(*client, *this, "Client disconnected...");

		if (channel->isEmpty())
			toDelete.push_back(channel->getName());
	}

	for (size_t i = 0; i < toDelete.size(); i++)
		removeChannel(toDelete[i]);
	std::cout << "[CLEANUP] Removed client from all channels" << std::endl;
}

void Server::broadcastToChannel(Channel* channel, const std::string& message, int exclude_fd)
{
	const std::set<Client*>& clients = channel->getClients();

	std::string out = message + "\r\n";


	for (std::set<Client*>::const_iterator it = clients.begin();
		 it != clients.end(); ++it)
	{
		Client* client = *it;
		if (!client)
			continue;

		if (_clients.find(client->getFd()) == _clients.end())
			continue;

		if (client->getFd() == exclude_fd)
			continue;

		ssize_t sent = send(client->getFd(), out.c_str(), out.size(), 0);
		if (sent < 0)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
				terminateClientConnection(client->getFd());
		}
	}
}

void Server::sendToClient(Client* client, const std::string& message)
{
	std::string out = message + "\r\n";

	//for debug
	//std::cout << "SEND -> " << out << std::endl;

	ssize_t sent = send(client->getFd(), out.c_str(), out.size(), 0);
	if (sent < 0)
	{
		if (errno == EPIPE)
		{
			terminateClientConnection(client->getFd());
			return;
		}
		if (errno != EAGAIN && errno != EWOULDBLOCK)
			terminateClientConnection(client->getFd());
	}
}


bool Server::nickExists(const std::string& nick) const
{
	return _clientsByNick.find(nick) != _clientsByNick.end();
}

void Server::addNick(const std::string& nick, Client* client)
{
	_clientsByNick[nick] = client;
}

void Server::removeNick(const std::string& nick)
{
	_clientsByNick.erase(nick);
}