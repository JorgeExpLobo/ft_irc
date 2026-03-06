/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdiaz-he <jdiaz-he@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 20:29:46 by jdiaz-he          #+#    #+#             */
/*   Updated: 2026/03/06 13:31:39 by jdiaz-he         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <map>
# include <string>
# include <poll.h>
// se incluyen en el cpp, aqui no son necesarios
// # include <iostream>
// # include <sys/socket.h>
// # include <netinet/in.h>
// # include <fcntl.h>
// # include <unistd.h>
// # include <cstring>
// # include <algorithm>

// Estructura temporal para testeo (será sustituida por class Client)
struct Client {
	int		 fd;
	std::string buffer;
	// Aqui podria añadir: std::string nickname;
};

class Server {
private:
	int							_port;
	std::string					_password;
	int							_server_master_fd;
	bool						_is_running;

	// Gestion de conexiones
	std::vector<struct pollfd>	_poll_fds;
	
	// Cambiar por std::map<int, Client*> cuando este la clase Client, es decir, con punteros
	std::map<int, Client>	_clients;
	// std::vector<Channel*>    _channels;

	// Métodos internos (Engine)
	void	establishNewConnection();
	void	processIncomingData(int fd);
	void	terminateClientConnection(int fd);

	// Gestion del chat IRC
	void	executeIrcCommand(int fd, std::string cmd_line);
	void	removeClientFromAllChannels(int fd);
	void	broadcastToChannel(std::string message, std::string channel_name, int exclude_fd);

	// Helper para buscar canales (retorna void* mientras no tenga Channel.hpp)
	void*	findChannel(std::string name);

public:
	Server(int port, std::string password);
	~Server();

	void	init();			// Configuración inicial
	void	run();			// Bucle principal (poll)
	void	stopEngine(); 	// Cierre limpio

	// Getters básicos (útiles para el CommandHandler si finalmente se hace)
	int			getPort() const;
	std::string	getPassword() const;
};

// Declaración del handler de señales (va fuera de la clase)
void handleSignal(int signum);

#endif