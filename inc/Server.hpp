/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdiaz-he <jdiaz-he@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 20:29:46 by jdiaz-he          #+#    #+#             */
/*   Updated: 2026/03/05 20:29:47 by jdiaz-he         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include "Channel.hpp"
# include "CommandManager.hpp"
# include <iostream>
# include <vector>
# include <map>
# include <string>
# include <poll.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <unistd.h>
# include <cstring>
# include <algorithm>

class Server {
private:
	int							_port;
	std::string					_password;
	int							_server_master_fd;
	bool						_is_running;

	// Gestion de conexiones
	std::vector<struct pollfd>	_poll_fds;
	
	std::map<int, Client*>	_clients;
	std::vector<Channel*> _channels;

	// Métodos internos (Engine)
	void	establishNewConnection();
	void	processIncomingData(int fd);

	// Gestion del chat IRC
	void	executeIrcCommand(int fd, std::string cmd_line);
	void	removeClientFromAllChannels(int fd);

	// Manager de comandos
	CommandManager _commandManager;

public:
	Server(int port, std::string password);
	~Server();

	void	init();			// Configuración inicial
	void	run();			// Bucle principal (poll)
	void	stopEngine(); 	// Cierre limpio

	// Getters básicos (útiles para el CommandHandler si finalmente se hace)
	int			getPort() const { return _port; }
	std::string	getPassword() const { return _password; }

	// CHANNEL MANAGEMENT

	Channel* getChannel(const std::string& name);
	Channel* createChannel(const std::string& name, Client* creator);
	Channel* getOrCreateChannel(const std::string& name, Client* creator);
	void removeChannel(const std::string& name);
	void addClientToChannel(Client* client, const std::string& channel_name);
	void removeClientFromChannel(Client* client, const std::string& channel_name);
	void terminateClientConnection(int fd);

	// MESSAGE HELPERS

	void sendToClient(Client* client, const std::string& message);
	void broadcastToChannel(Channel* channel, const std::string& message, int exclude_fd);

	// FUNCIONES AUXILIARES PARA COMANDOS
	bool nickExists(const std::string& nick) const;
	Channel* findChannel(const std::string& name);
	Client* findClient(const std::string& nickname);
};

// Declaración del handler de señales (va fuera de la clase)
void handleSignal(int signum);

#endif