#ifndef REPLY_HPP
#define REPLY_HPP

#include <string>
#include "Message.hpp"


class Channel;

namespace Reply 
{

    // Mensajes generales
    Message welcome(const std::string &nick, const std::string &user, const std::string &host);
    Message error(const std::string &msg);
    Message kill(const std::string &nick, const std::string &msg);
    Message updateMode(const std::string &nick, const std::string &channel, const std::string update);

    // Topic
    Message noTopic(const std::string &nick, const std::string &channel);
    Message topic(const std::string &nick, const std::string &channel, const std::string &topic);

    // Invite
    Message inviting(const std::string &by, const std::string &nickname, const std::string &channel);

    // Away
    Message away(const std::string &nick, const std::string &awayMessage);
    Message unaway();
    Message nowAway();

    // Names
    Message nameReply(const std::string &nick, const Channel &channel);
    Message endOfNames(const std::string &nick, const std::string &channel);

    // List
    Message list(const std::string &nick, const std::string &name, const std::string &nusers, const std::string &topic);
    Message listEnd(const std::string &nick);

    // Mode
    Message channelModeIs(const std::string &nick, const std::string &name, const std::string &modes);

    // Errores
    Message errUnknownMode(const std::string &nick, const std::string &channel, const std::string &mode);
    Message errUnknownCommand(const std::string &nick, const std::string &command);
    Message errNoNicknameGiven();
    Message errErroneousNickname(const std::string &nick, const std::string &badnick);
    Message errNicknameInUse(const std::string &nick, const std::string &badnick);
    Message errNoSuchNick(const std::string &nick, const std::string &badnick);
    Message errNickCollision(const std::string &nick, const std::string &user, const std::string &host, const std::string &badnick);
    Message errUnavailResource(const std::string &nick, const std::string &target);
    Message errRestricted(const std::string &nick);
    Message errNeedMoreParams(const std::string &nick, const std::string &command);
    Message errAlreadyRegistered(const std::string &nick);
    Message errNoOrigin(const std::string &nick);
    Message errPassWdMissMatch(const std::string &nick);
   
    Message errNoSuchChannel(const std::string &nick, const std::string &channel);
    
    Message errInviteOnlyChan(const std::string &nick, const std::string &channel);
    Message errBadChannelKey(const std::string &nick,  const std::string &channel);
    Message errChannelIsFull(const std::string &nick, const std::string &channel);
    Message errNotOnChannel(const std::string &nick, const std::string &channel);
    Message errNoRecipient(const std::string &nick, const std::string &command);
    Message errNoTextToSend(const std::string &nick);
    Message errCannotSendToChan(const std::string &nick, const std::string &channel);
    Message errChanOpIsNeeded(const std::string &nick, const std::string &channel);
    Message errUserNotInChannel(const std::string &nick, const std::string &channel, const std::string &newnick);
    Message errKeyset(const std::string &nick, const std::string &channel);

}

#endif
