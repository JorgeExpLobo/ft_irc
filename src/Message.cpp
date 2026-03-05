/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:16:10 by pablo             #+#    #+#             */
/*   Updated: 2026/03/05 22:22:12 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Message.hpp"
#include <sstream>
#include <iomanip>




Message::Message() :
    _prefix(),
    _command(),
    _args(),
    _argsCount(0),
    _isRequest(false),
    _hasSuffix(false),
    _replyCode(0)
{}

Message::Message(const Message &other)
{
    *this = other;
}

Message &Message::operator=(const Message &other)
{
    if (this != &other)
    {
        _prefix     = other._prefix;
        _command    = other._command;
        _argsCount  = other._argsCount;
        _isRequest  = other._isRequest;
        _hasSuffix  = other._hasSuffix;
        _replyCode  = other._replyCode;

        for (size_t i = 0; i < other._argsCount; i++)
            _args[i] = other._args[i];
    }
    return *this;
}

Message::~Message() {}


/*
 formatShort()

  Convierte un número (1, 42, 433) en un string de 3 dígitos:
    1   → "001"
   42  → "042"
   433 → "433"
 Esto es obligatorio en IRC: todos los códigos numéricos son de 3 dígitos.
 */
static std::string formatShort(unsigned short n)
{
    std::stringstream ss; 
    ss << std::setw(3) << std::setfill('0') << n; 
    return ss.str();
}


/*
  parseRequest()
  Parsea un mensaje recibido del cliente.
 Ejemplo:
  ":nick!user@host PRIVMSG #canal :hola a todos"
 Extrae:
  prefix  → "nick!user@host"
  command → "PRIVMSG"
  args    → ["#canal", "hola a todos"]
 Devuelve true si el formato es válido.
 */
bool Message::parseRequest(const std::string &str)
{
    _isRequest = true;
    _hasSuffix = false;
    _argsCount = 0;
    _prefix.clear();
    _command.clear();

    if (str.empty())
        return false;

    std::string output = str;
    bool commandSet = false;

    while (!output.empty())
    {
        size_t pos = output.find(" ");

        // Saltar espacios iniciales
        if (pos == 0)
        {
            output.erase(0, 1);
            continue;
        }

        // Límite de argumentos según RFC (máximo 15)
        if (_argsCount >= 15)
            return false;

        if (pos == std::string::npos)
            pos = output.size();

        // 1. Prefijo
        if (output.at(0) == ':' && _prefix.empty() && !commandSet)
        {
            _prefix.assign(output, 1, pos - 1);
        }

        // 2. Comando
        else if (!commandSet)
        {
            _command.assign(output, 0, pos);
            commandSet = true;
        }

        // 3. Sufijo (trailing)
        else if (output.at(0) == ':')
        {
            _args[_argsCount] = output.substr(1);
            _argsCount += 1;
            _hasSuffix = true;
            break;
        }

        // 4. Argumento normal
        else
        {
            _args[_argsCount].assign(output, 0, pos);
            _argsCount += 1;
        }

        output.erase(0, pos + 1);
    }

    if (_command.empty())
    return false;

    return true;
}



/*
 stringify()
 Convierte el mensaje en un string IRC válido.
 NO añade "\r\n".
 */
std::string Message::stringify(void) const
{
    std::string output;

    // Prefijo opcional
    if (!this->_prefix.empty())
        output += ":" + this->_prefix + " ";

    // Comando textual o código numerico
    if (this->_isRequest)
        output += this->_command;
    else
        output += formatShort(this->_replyCode);

    // Argumentos
    for (size_t i = 0; _argsCount > 0 && i < this->_argsCount; i++)
    {
        // El último argumento puede ser un sufijo
        if (i == _argsCount - 1 && this->_hasSuffix)
            output += " :" + this->_args[i];
        else
            output += " " + this->_args[i];
    }

    return output;
}


/*
 setPrefix()
 Establece el prefijo del mensaje.
 - El prefijo debe tener al menos 3 caracteres
  O empezar por ':'
 evita prefijos inválidos como "" o "a".
 */
Message &Message::setPrefix(const std::string &prefix)
{
    this->_prefix = prefix;
    return *this;
}


/*
 setReplyCode()
 Convierte el mensaje en una respuesta numérica IRC.
 */

Message &Message::setReplyCode(uint16_t replyCode)
{
    this->_isRequest = false;
    this->_replyCode = replyCode;
    return *this;
}


/*
 pushArg()
 Añade un argumento normal.
 No se puede añadir un argumento después de un sufijo.
 PRIVMSG #canal :hola a todos argumento_extra
 No se pueden tener más de 15 argumentos (RFC).
 */
Message &Message::pushArg(const std::string &arg)
{
    if (arg.empty())
        this->_args[this->_argsCount] = "empty";
    else
        this->_args[this->_argsCount] = arg;

    this->_argsCount++;
    return *this;
}


/*
 pushSuffix()
  Añade un argumento trailing.
 Solo puede haber un sufijo.
 Ejemplo inválido:
  PRIVMSG #canal :hola :adios
 */
Message &Message::pushSuffix(const std::string &arg)
{
    pushArg(arg);
    _hasSuffix = true;

    return *this;
}


/*
  setCommand()
  Establece el comando textual del mensaje.
  El comando no puede estar vacío.
 
 */

Message &Message::setCommand(const std::string &command)
{
    this->_isRequest = true;
    this->_command = command;
    return *this;
}


const std::string &Message::prefix() const { return _prefix; }
const std::string &Message::command() const { return _command; }
const std::string &Message::arg(size_t index) const {return _args[index];}
size_t Message::argsCount() const { return _argsCount; }
const std::string &Message::suffix() const {return _args[_argsCount - 1]; }
bool Message::isRequest() const { return _isRequest; }
bool Message::hasSuffix() const { return _hasSuffix; }
uint16_t Message::replyCode() const { return _replyCode; }
