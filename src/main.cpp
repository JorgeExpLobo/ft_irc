/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/16 20:14:50 by jdiaz-he          #+#    #+#             */
/*   Updated: 2026/03/18 20:58:18 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdlib> // Para atoi

int main(int argc, char** argv) 
{

	if (argc != 3)
	{
		std::cerr << "Uso: ./ircserv <port> <password>" << std::endl;
		return 1;
	}

	try 
	{
		int port = std::atoi(argv[1]);
		std::string password = argv[2];

		Server server(port, password);
		server.init();
		server.run();
	} 
	catch (const std::exception& e) 
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}