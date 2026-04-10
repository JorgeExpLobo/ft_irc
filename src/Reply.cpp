/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reply.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablo <pablo@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:08:29 by pablo             #+#    #+#             */
/*   Updated: 2026/03/08 22:29:53 by pablo            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Reply.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include <set>

#define SERVER_NAME "irc.42madrid.com"


Message Reply::welcome(const std::string &nick, const std::string &user, const std::string &host) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(1)
                    .pushArg(nick)
                    .pushSuffix("Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host);
}

Message Reply::error(const std::string &msg) 
{
    Message m;
    m.setPrefix(SERVER_NAME).setCommand("ERROR");
    if (!msg.empty())
        m.pushSuffix(msg);
    return m;
}

Message Reply::kill(const std::string &nick, const std::string &msg) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setCommand("KILL")
                    .pushArg(nick)
                    .pushSuffix(msg);
}


// Topic
Message Reply::noTopic(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(331)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("No topic is set");
}

Message Reply::topic(const std::string &nick, const std::string &channel, const std::string &topic) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(332)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix(topic);
}

// Invite
Message Reply::inviting(const std::string &nick, const std::string &targetNick, const std::string &channel)
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(341)
                    .pushArg(nick)
                    .pushArg(targetNick)
                    .pushArg(channel);
}
Message Reply::invite(const std::string &inviterNick, const std::string &channel)
{
    Message m;
    m.setPrefix(inviterNick)
     .setCommand("INVITE")
     .pushArg(channel);
    return m;
}

// Away
Message Reply::away(const std::string &nick, const std::string& target, const std::string &awayMessage) 
{
    Message msg;
    msg.setPrefix(SERVER_NAME);
    msg.setCommand("301");
    msg.pushArg(nick);
    msg.pushArg(target);
    msg.pushSuffix(awayMessage);
    return msg;
}

Message Reply::unaway(const std::string& nick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(305)
                    .pushArg(nick)
                    .pushSuffix("You are no longer marked as being away");
                
}

Message Reply::nowAway(const std::string& nick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(306)
                    .pushArg(nick)
                    .pushSuffix("You have been marked as being away");
}

// Names
Message Reply::nameReply(const std::string &nick, const Channel &chan) 
{
    std::string names;
	const std::set<Client*>& users = chan.getClients();

    for (std::set<Client*>::const_iterator it = users.begin(); it != users.end(); ++it)
    {
        if (it != users.begin())
            names.append(" ");
        if (chan.isOperator(*it) )
            names.append("@");

        names.append((*it)->getNickname());
    }

    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(353)
                    .pushArg(nick)
                    .pushArg("=")
                    .pushArg(chan.getName())
                    .pushSuffix(names);
}

Message Reply::endOfNames(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(366)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("End of NAMES list");
}

// List
Message Reply::list(const std::string &nick, const std::string &channel, const std::string &nUsers, const std::string &topic) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(322)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushArg(nUsers)
                    .pushSuffix(topic.empty() ? "No topic set" : topic);
}

Message Reply::listEnd(const std::string &nick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(323)
                    .pushArg(nick)
                    .pushSuffix("End of LIST");
}

// Mode
Message Reply::channelModeIs(const std::string &nick, const std::string &channel, const std::string &modes) 
{
    Message m;
    m.setPrefix(SERVER_NAME).setReplyCode(324).pushArg(nick).pushArg(channel);
    if (!modes.empty())
        m.pushArg(modes);
    return m;
}

// Errors
Message Reply::errUnknownMode(const std::string &nick, const std::string &channel, const std::string &mode) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(472)
                    .pushArg(nick)
                    .pushArg(mode)
                    .pushSuffix("is unknown mode char for " + channel);
}

Message Reply::errUnknownCommand(const std::string &nick, const std::string &command)
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(421)
                    .pushArg(nick)
                    .pushArg(command)
                    .pushSuffix("Unknown command");
}

Message Reply::errNoNicknameGiven(const std::string &nick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(431)
                    .pushArg(nick)
                    .pushSuffix("No nickname given");
}

Message Reply::errErroneousNickname(const std::string &nick, const std::string &badnick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(432)
                    .pushArg(nick)
                    .pushArg(badnick)
                    .pushSuffix("Erroneous nickname");
}

Message Reply::errNicknameInUse(const std::string &nick, const std::string &badnick)
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(433)
                    .pushArg(nick)
                    .pushArg(badnick)
                    .pushSuffix("Nickname is already in use");
}

Message Reply::errNoSuchNick(const std::string &nick, const std::string &badnick)
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(401)
                    .pushArg(nick)
                    .pushArg(badnick)
                    .pushSuffix("No such nick/channel");
}

Message Reply::errNeedMoreParams(const std::string &nick, const std::string &command)
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(461)
                    .pushArg(nick)
                    .pushArg(command)
                    .pushSuffix("Not enough parameters");
}

Message Reply::errAlreadyRegistered(const std::string &nick)
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(462)
                    .pushArg(nick)
                    .pushSuffix("Unauthorized command (already registered)");
}

Message Reply::errNoSuchChannel(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(403)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("No such channel");
}

Message Reply::errPassWdMissMatch(const std::string &nick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(464)
                    .pushArg(nick)
                    .pushSuffix("Password incorrect");
}

Message Reply::errInviteOnlyChan(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(473)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("Cannot join channel (+i)");
}

Message Reply::errBadChannelKey(const std::string &nick,  const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(475)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("Cannot join channel (+k)");
}


Message Reply::errChannelIsFull(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(471)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("Cannot join channel (+l)");
}

Message Reply::errNotOnChannel(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(442)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("You're not on that channel");
}

Message Reply::errNoRecipient(const std::string &nick, const std::string &command) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(411)
                    .pushArg(nick)
                    .pushSuffix("No recipient given (" + command + ")");
}

Message Reply::errNoTextToSend(const std::string &nick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(412)
                    .pushArg(nick)
                    .pushSuffix("No text to send");
}

Message Reply::errCannotSendToChan(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(404)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("Cannot send to channel");
}

Message Reply::errChanOpIsNeeded(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(482)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("You're not a channel operator");
}

Message Reply::errUserNotInChannel(const std::string &nick, const std::string &channel, const std::string &newnick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(441)
                    .pushArg(newnick)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("They aren't on that channel");
}

Message Reply::pong(const std::string &serverName, const std::string &token)
{
    Message msg;
    msg.setPrefix(SERVER_NAME)
       .setCommand("PONG")
       .pushArg(serverName)
       .pushSuffix(token);
    return msg;
}
Message Reply::errNotRegistered(const std::string &nick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(451)
                    .pushArg(nick)
                    .pushSuffix("You are not Registered");
}

Message Reply::errUserOnChannel(const std::string &nick, const std::string &target, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(443)
                    .pushArg(nick)
                    .pushArg(target)
                    .pushArg(channel)
                    .pushSuffix("is already on channel");
}

Message Reply::errChanOPrivsNeeded(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(482)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("You're not channel operator");
}

Message Reply::rplInviting(const std::string &nick, const std::string &target, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(341)
                    .pushArg(nick)
                    .pushArg(target)
                    .pushArg(channel);
}