/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Reply.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pablgarc <pablgarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/05 22:08:29 by pablgarc          #+#    #+#             */
/*   Updated: 2026/03/05 22:08:29 by pablgarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef REPLY_HPP
#define REPLY_HPP

#include <string>
#include "Message.hpp"


class Channel;

class Reply 
{
   private:
    Reply();

    public:

    
    static Message welcome(const std::string &nick, const std::string &user, const std::string &host);
    static Message error(const std::string &msg);
    static Message kill(const std::string &nick, const std::string &msg);
    static Message updateMode(const std::string &nick, const std::string &channel, const std::string update);

    // Topic
    static Message noTopic(const std::string &nick, const std::string &channel);
    static Message topic(const std::string &nick, const std::string &channel, const std::string &topic);

    // Invite
    static Message inviting(const std::string &by, const std::string &nickname, const std::string &channel);

    // Away
    static Message away(const std::string &nick, const std::string& target, const std::string &awayMessage);
    static Message unaway(const std::string& nick);
    static Message nowAway(const std::string& nick);

    // Names
    static Message nameReply(const std::string &nick, const Channel &channel);
    static Message endOfNames(const std::string &nick, const std::string &channel);

    // List
    static Message list(const std::string &nick, const std::string &name, const std::string &nusers, const std::string &topic);
    static Message listEnd(const std::string &nick);

    // Mode
    static Message channelModeIs(const std::string &nick, const std::string &name, const std::string &modes);

    // Errores
    static Message errUnknownMode(const std::string &nick, const std::string &channel, const std::string &mode);
    static Message errUnknownCommand(const std::string &nick, const std::string &command);
    static Message errNoNicknameGiven(const std::string &nick);
    static Message errErroneousNickname(const std::string &nick, const std::string &badnick);
    static Message errNicknameInUse(const std::string &nick, const std::string &badnick);
    static Message errNoSuchNick(const std::string &nick, const std::string &badnick);
    static Message errNickCollision(const std::string &nick, const std::string &user, const std::string &host, const std::string &badnick);
    static Message errUnavailResource(const std::string &nick, const std::string &target);
    static Message errRestricted(const std::string &nick);
    static Message errNeedMoreParams(const std::string &nick, const std::string &command);
    static Message errAlreadyRegistered(const std::string &nick);
    static Message errNoOrigin(const std::string &nick);
    static Message errPassWdMissMatch(const std::string &nick);
   
    static Message errNoSuchChannel(const std::string &nick, const std::string &channel);
    
    static Message errInviteOnlyChan(const std::string &nick, const std::string &channel);
    static Message errBadChannelKey(const std::string &nick,  const std::string &channel);
    static Message errChannelIsFull(const std::string &nick, const std::string &channel);
    static Message errNotOnChannel(const std::string &nick, const std::string &channel);
    static Message errNoRecipient(const std::string &nick, const std::string &command);
    static Message errNoTextToSend(const std::string &nick);
    static Message errCannotSendToChan(const std::string &nick, const std::string &channel);
    static Message errChanOpIsNeeded(const std::string &nick, const std::string &channel);
    static Message errUserNotInChannel(const std::string &nick, const std::string &channel, const std::string &newnick);
    static Message errKeyset(const std::string &nick, const std::string &channel);
    static Message pong(const std::string &serverName, const std::string &token);
    static Message errNotRegistered(const std::string &nick);
};

#endif
