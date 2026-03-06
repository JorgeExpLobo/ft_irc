/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdiaz-he <jdiaz-he@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 18:29:50 by jdiaz-he          #+#    #+#             */
/*   Updated: 2026/03/05 20:27:44 by jdiaz-he         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdio>
#include <cerrno>
#include <csignal> // Para manejar señales

// Flags globales para control de señales (Estándar en 42)
bool g_server_shutdown = false;

void handleSignal(int signum) {
	if (signum == SIGINT) {
		std::cout << "\n[INFO] Señal SIGINT recibida. Cerrando servidor de forma segura..." << std::endl;
		g_server_shutdown = true;
	}
}

Server::Server(int port, std::string password) 
	: _port(port), _password(password), _server_master_fd(-1), _is_running(false) {}

Server::~Server() {
	this->stopEngine();
}

void Server::init() {
	// Configurar el manejador de señales
	signal(SIGINT, handleSignal);

	_server_master_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_master_fd == -1) throw std::runtime_error("Error: No se pudo crear el socket maestro.");

	int opt_reuse = 1;
	if (setsockopt(_server_master_fd, SOL_SOCKET, SO_REUSEADDR, &opt_reuse, sizeof(opt_reuse)) == -1)
		throw std::runtime_error("Error: No se pudo configurar SO_REUSEADDR.");

	// OBLIGATORIO: fcntl para modo no bloqueante
	if (fcntl(_server_master_fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error("Error: fcntl no pudo establecer O_NONBLOCK.");

	struct sockaddr_in server_address;
	std::memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(_port);

	if (bind(_server_master_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
		throw std::runtime_error("Error: Bind falló (¿puerto ocupado?).");

	if (listen(_server_master_fd, 128) == -1)
		throw std::runtime_error("Error: Listen falló.");

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
	std::cout << "--- Servidor IRC 'The Engine' iniciado ---" << std::endl;
	std::cout << "Puerto: " << _port << " | Password: " << _password << std::endl;
}

void Server::run() {
	while (_is_running && !g_server_shutdown) {
		// Usamos un timeout de 1 segundo para poder checkear g_server_shutdown
		int poll_count = poll(&_poll_fds[0], _poll_fds.size(), 1000);
		
		if (poll_count < 0) {
			if (g_server_shutdown) break;
			std::cerr << "[ERROR] Error en poll: " << std::strerror(errno) << std::endl;
			break;
		}

		for (size_t i = 0; i < _poll_fds.size(); ++i) {
			if (_poll_fds[i].revents & POLLIN) {
				if (_poll_fds[i].fd == _server_master_fd)
					this->establishNewConnection();
				else
					this->processIncomingData(_poll_fds[i].fd);
			}
			
			if (i < _poll_fds.size() && (_poll_fds[i].revents & (POLLERR | POLLHUP))) {
				this->terminateClientConnection(_poll_fds[i].fd);
			}
		}
	}
	this->stopEngine();
}

void Server::establishNewConnection() {
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	int new_client_fd = accept(_server_master_fd, (struct sockaddr *)&client_addr, &addr_len);

	if (new_client_fd != -1) {
		fcntl(new_client_fd, F_SETFL, O_NONBLOCK);
		
		struct pollfd client_pfd;
		client_pfd.fd = new_client_fd;
		client_pfd.events = POLLIN;
		client_pfd.revents = 0;
		_poll_fds.push_back(client_pfd);

		// --- GESTIÓN DE CLIENTE ---
		
		Client* new_client = new Client(new_client_fd, "localhost");
		_clients[new_client_fd] = new_client;
		
		// Futura implementación con clase Client +-:
		// Client* new_client = new Client(new_client_fd);
		// _clients_map[new_client_fd] = new_client;

		std::cout << "[+] Cliente vinculado en FD: " << new_client_fd << std::endl;
	}
}

void Server::processIncomingData(int fd) {
	char read_buffer[512];
	std::memset(read_buffer, 0, sizeof(read_buffer));
	
	int bytes_received = recv(fd, read_buffer, sizeof(read_buffer) - 1, 0);

	if (bytes_received <= 0) {
		this->terminateClientConnection(fd);
	} else {
		read_buffer[bytes_received] = '\0';
		
		// --- GESTIÓN DE BUFFER ---
		_clients[fd]->appendBuffer(read_buffer);


		// --- PROCESAR COMANDOS ---
		std::string& current_buffer = _clients[fd]->getBuffer();

		size_t newline_pos;
		while ((newline_pos = current_buffer.find("\n")) != std::string::npos) {
			std::string raw_command = current_buffer.substr(0, newline_pos);
			
			if (!raw_command.empty() && raw_command[raw_command.size() - 1] == '\r')
				raw_command.erase(raw_command.size() - 1);
			
			// Lógica de parseo y ejecución de comandos
			Message msg;
			if (!msg.parseRequest(raw_command))
				return;
			_commandManager.execute(this, _clients[fd], msg);
			current_buffer.erase(0, newline_pos + 1);
		}
	}
}

void Server::terminateClientConnection(int fd) {
	std::cout << "[-] Desvinculando FD: " << fd << std::endl;
	
	// Limpieza de canales (Llamada al metodo)
	// Aquí va la función para sacar al cliente de todos los canales antes de borrarlo
	this->removeClientFromAllChannels(fd);

	// Cerrar socket
	close(fd);
	
	// Eliminar de la lista de clientes del servidor
	delete _clients[fd];
	_clients.erase(fd);

	// Eliminar del vector de poll para que el loop no lo procese más
	for (std::vector<struct pollfd>::iterator it = _poll_fds.begin(); it != _poll_fds.end(); ++it) {
		if (it->fd == fd) {
			_poll_fds.erase(it);
			break;
		}
	}
}

void Server::stopEngine() {
	if (!_is_running && _poll_fds.empty()) // Si ya se cerró, salimos
        return;
	_is_running = false;
	std::cout << "[SHUTDOWN] Limpiando recursos y cerrando descriptores..." << std::endl;
	for (size_t i = 0; i < _poll_fds.size(); ++i) {
		close(_poll_fds[i].fd);
	}
	_poll_fds.clear();
	_clients.clear();
	_server_master_fd = -1; // Marcamos como cerrado, quiza no haga falta, revisar
}

// Función de limpieza
void Server::removeClientFromAllChannels(int fd) {
	std::cout << "[CLEANUP] Sacando FD " << fd << " de todos los canales." << std::endl;
	
	// Suponiendo que tengo std::vector<Channel*> _channels seria algo asi;
	/*
	for (size_t i = 0; i < _channels.size(); ++i) {
		if (_channels[i]->hasClient(fd)) {
			_channels[i]->removeClient(fd);
			// Si el canal se queda vacío, bórralo
			if (_channels[i]->isEmpty()) {
				delete _channels[i];
				_channels.erase(_channels.begin() + i);
				i--;
			}
		}
	}
	*/
}

// Para enviar mensajes a todo un canal menos al que escribe
void Server::broadcastToChannel(std::string message, std::string channel_name, int exclude_fd) {
	(void)message; (void)channel_name; (void)exclude_fd;
	// Aquí busco el canal por nombre y llamarías a su propio broadcast
	std::cout << "[BROADCAST] Enviando a " << channel_name << " excluyendo " << exclude_fd << std::endl;
}

// Para buscar canales (para el JOIN)
void* Server::findChannel(std::string name) {
	(void)name;
	// Retornará el puntero al canal o NULL
	return NULL; 
}
