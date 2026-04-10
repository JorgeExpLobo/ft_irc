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
Parses a message received from the client.

Example:
":nick!user@host PRIVMSG #channel :hello everyone"

Extracts:
prefix → "nick!user@host"
command → "PRIVMSG"
args → ["#channel", "hello everyone"]
Returns true if the format is valid.

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
        if (pos == 0)
        {
            output.erase(0, 1);
            continue;
        }
        if (_argsCount >= 15)
            return false;

        if (pos == std::string::npos)
            pos = output.size();
        if (output.at(0) == ':' && _prefix.empty() && !commandSet)
        {
            _prefix.assign(output, 1, pos - 1);
        }
        else if (!commandSet)
        {
            _command.assign(output, 0, pos);
            commandSet = true;
        }
        else if (output.at(0) == ':')
        {
            _args[_argsCount] = output.substr(1);
            _argsCount += 1;
            _hasSuffix = true;
            break;
        }
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

std::string Message::stringify(void) const
{
    std::string output;

    if (!this->_prefix.empty())
        output += ":" + this->_prefix + " ";

    if (this->_isRequest)
        output += this->_command;
    else
        output += formatShort(this->_replyCode);

    for (size_t i = 0; _argsCount > 0 && i < this->_argsCount; i++)
    {
        if (i == _argsCount - 1 && this->_hasSuffix)
            output += " :" + this->_args[i];
        else
            output += " " + this->_args[i];
    }

    return output;
}

Message &Message::setPrefix(const std::string &prefix)
{
    this->_prefix = prefix;
    return *this;
}

Message &Message::setReplyCode(uint16_t replyCode)
{
    this->_isRequest = false;
    this->_replyCode = replyCode;
    return *this;
}

Message &Message::pushArg(const std::string &arg)
{
    if (arg.empty())
        this->_args[this->_argsCount] = "empty";
    else
        this->_args[this->_argsCount] = arg;

    this->_argsCount++;
    return *this;
}

Message &Message::pushSuffix(const std::string &arg)
{
    pushArg(arg);
    _hasSuffix = true;

    return *this;
}

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


std::string Message::getCommand() const
{
    return _command;
}

const std::string& Message::getArg(int i) const
{
    return _args[i];
}

int Message::getArgCount() const
{
    return _argsCount;
}

std::string Message::getTrailing() const
{
    if (_hasSuffix)
        return suffix();
    return "";
}

std::string Message::toString() const
{
    return stringify();
}