/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:14:16 by pablo             #+#    #+#             */
/*   Updated: 2026/03/05 22:21:39 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
# define MESSAGE_HPP

#include <string>       // std::string
#include <stdint.h>      // uint16_t


#define SERVER_NAME "irc.42madrid.com"


/*
 Clase Message

  Representa un mensaje IRC completo, tanto de entrada (comando recibido)
 como de salida (respuesta numérica o comando generado por el servidor).
 Un mensaje IRC tiene esta estructura general:

   [:prefix] <command> <arg1> <arg2> ... [:suffix]

 Ejemplos:
   PRIVMSG #canal :hola a todos
   :nick!user@host JOIN #canal
   :servidor 433 nick :Nickname is already in use

 Esta clase permite:
   - Parsear mensajes recibidos del cliente (parseRequest)
   - Construir mensajes para enviar al cliente (setCommand, setReplyCode, pushArg, pushSuffix)
   - Convertir el mensaje a string listo para enviar (stringify)
 */

class Message
{
private:
    /*
     _prefix
     -------
     Parte opcional del mensaje que indica quién envía el mensaje.
     Ejemplo: "nick!user@host"
    
     Solo aparece en:
       - Mensajes enviados por el servidor
       - Mensajes reenviados a otros clientes
     */
    std::string _prefix;

    /*
     _command
     Comando IRC textual (PRIVMSG, JOIN, NICK, ERROR, MODE...)
     Solo se usa cuando el mensaje es una petición o un comando del servidor.
    
     Si el mensaje es una respuesta numérica, se ignora.
     */
    std::string _command;

    /*
     _args
     Lista de argumentos del mensaje.
     IRC permite hasta 15 argumentos antes del sufijo.
    
     Ejemplo:
       PRIVMSG #canal :hola
       args = ["#canal", "hola"]
     */
    std::string _args[15];

    /*
     _argsCount
     Número de argumentos almacenados en _args.
     */
    size_t      _argsCount;

    /*
     _isRequest
     ----------
     true  → mensaje recibido del cliente (comando)
     false → mensaje generado por el servidor (respuesta numérica)
     */
    bool        _isRequest;

    /*
     _hasSuffix
     ----------
     true si el último argumento es un "trailing" (empieza por ':')
     Ejemplo: ":hola a todos"
     */
    bool        _hasSuffix;

    /*
     _replyCode
     ----------
     Código numérico de respuesta IRC (001, 433, 401...)
     Solo se usa cuando _isRequest == false.
     */
    uint16_t    _replyCode;


public:

    Message ();
    Message(const Message &other); 
    Message &operator=(const Message &other);
    ~Message(); 

    
    /*
     stringify()
     -----------
     Convierte el mensaje en un string IRC válido, sin añadir "\r\n".
     El servidor debe añadirlo al enviar.
     */
    std::string stringify(void) const;

    /*
     Métodos de construcción del mensaje
     -----------------------------------
     Se usan para crear mensajes salientes.
     */

    // Establece el prefijo del mensaje (servidor o usuario)
    Message     &setPrefix(const std::string &prefix);

    // Añade un argumento normal (no trailing)
    Message     &pushArg(const std::string &arg);

    // Añade un argumento trailing (empieza por ':')
    Message     &pushSuffix(const std::string &arg);

    // Convierte el mensaje en una respuesta numérica IRC
    Message     &setReplyCode(uint16_t replyCode);

    // Establece el comando textual (PRIVMSG, JOIN, NICK...)
    Message     &setCommand(const std::string &command);

    /*
     Getters
     -------
     Se usan para leer los datos del mensaje parseado.
     */

    const std::string   &prefix () const;
    const std::string   &command () const;
    const std::string   &arg (size_t index) const;
    size_t              argsCount () const;
    const std::string   &suffix () const;
    bool                isRequest () const;
    bool                hasSuffix () const;
    uint16_t            replyCode () const;

    /*
     parseRequest()
     --------------
     Parsea un mensaje recibido del cliente.
     Devuelve true si el formato es válido.
    
     Ejemplo de entrada:
       ":nick!user@host PRIVMSG #canal :hola a todos"
     */
    bool parseRequest(const std::string &str);
};

#endif
