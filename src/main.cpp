/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdiaz-he <jdiaz-he@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 20:14:50 by jdiaz-he          #+#    #+#             */
/*   Updated: 2026/03/06 13:32:43 by jdiaz-he         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdlib> // Para atoi
#include <cerrno>
#include <iostream>

int main(int argc, char** argv) {
	if (argc != 3) {
		std::cerr << "Uso: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	try {
		int port = std::atoi(argv[1]);
		std::string password = argv[2];

		Server server(port, password);
		server.init();
		server.run();
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}