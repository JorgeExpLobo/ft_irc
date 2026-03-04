#include "Reply.hpp"
#include "ChannelMock.hpp"

//:<server> <reply-code> <target> <args...> :<mensaje>
//:irc.42madrid.com 401 pablgarc usuario :No such nick/channel
//:irc.42madrid.com 461 pablgarc PRIVMSG :Not enough parameters
//:irc.42madrid.com 433 pablgarc :Nickname is already in use

#define SERVER_NAME "irc.42madrid.com"

namespace Reply 
{

Message welcome(const std::string &nick, const std::string &user, const std::string &host) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(1)
                    .pushArg(nick)
                    .pushSuffix("Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host);
}

Message error(const std::string &msg) 
{
    Message m;
    m.setPrefix(SERVER_NAME).setCommand("ERROR");
    if (!msg.empty())
        m.pushSuffix(msg);
    return m;
}

Message kill(const std::string &nick, const std::string &msg) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setCommand("KILL")
                    .pushArg(nick)
                    .pushSuffix(msg);
}

// hay que revisar los modos, ya que puede tener 0, 1 o mas parametros
Message updateMode(const std::string &nick, const std::string &channel, const std::string update) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setCommand("MODE")
                    .pushArg(channel)
                    .pushArg(update)
                    .pushArg(nick);
}

// Topic
Message noTopic(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(331)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("No topic is set");
}

Message topic(const std::string &nick, const std::string &channel, const std::string &topic) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(332)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix(topic);
}

// Invite
Message inviting(const std::string &nick, const std::string &targetNick, const std::string &channel)
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(341)
                    .pushArg(nick)
                    .pushArg(targetNick)
                    .pushArg(channel);
}

// Away
Message away(const std::string &nick, const std::string &awayMessage) 
{
    Message msg;
    msg.setPrefix(SERVER_NAME).setReplyCode(301).pushArg(nick);
    if (!awayMessage.empty())
        msg.pushSuffix(awayMessage);
    return msg;
}

Message unaway() 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(305)
                    .pushSuffix("You are no longer marked as being away");
}

Message nowAway() 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(306)
                    .pushSuffix("You have been marked as being away");
}

// Names
Message nameReply(const std::string &nick, const Channel &chan) 
{
    std::string spec;
    if (chan.modes.isPrivate)
        spec = "*";
    else if (chan.modes.isSecret)
        spec = "@";
    else
        spec = "=";

    std::string names;
    for (std::list<Channel::UserEntry>::const_iterator it = chan.joinedUsers.begin(); it != chan.joinedUsers.end(); ++it)
    {
        if (it != chan.joinedUsers.begin())
            names.append(" ");

        if (it->flags.isOperator)
            names.append("@");
        else if (it->flags.hasVoicePriviledge)
            names.append("+");

        names.append(it->user->nickname);
    }

    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(353)
                    .pushArg(nick)
                    .pushArg(spec)
                    .pushArg(chan.name)
                    .pushSuffix(names);
}

Message endOfNames(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(366)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("End of NAMES list");
}

// List
Message list(const std::string &nick, const std::string &channel, const std::string &nUsers, const std::string &topic) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(322)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushArg(nUsers)
                    .pushSuffix(topic.empty() ? "No topic set" : topic);
}

Message listEnd(const std::string &nick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(323)
                    .pushArg(nick)
                    .pushSuffix("End of LIST");
}

// Mode
Message channelModeIs(const std::string &nick, const std::string &channel, const std::string &modes) 
{
    Message m;
    m.setPrefix(SERVER_NAME).setReplyCode(324).pushArg(nick).pushArg(channel);
    if (!modes.empty())
        m.pushArg(modes);
    return m;
}

// Errors
Message errUnknownMode(const std::string &nick, const std::string &channel, const std::string &mode) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(472)
                    .pushArg(nick)
                    .pushArg(mode)
                    .pushSuffix("is unknown mode char to me for " + channel);
}

Message errUnknownCommand(const std::string &nick, const std::string &command)
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(421)
                    .pushArg(nick)
                    .pushArg(command)
                    .pushSuffix("Unknown command");
}

Message errNoNicknameGiven() 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(431)
                    .pushSuffix("No nickname given");
}

Message errErroneousNickname(const std::string &nick, const std::string &badnick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(432)
                    .pushArg(nick)
                    .pushArg(badnick)
                    .pushSuffix("Erroneous nickname");
}

Message errNicknameInUse(const std::string &nick, const std::string &badnick)
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(433)
                    .pushArg(nick)
                    .pushArg(badnick)
                    .pushSuffix("Nickname is already in use");
}

Message errNoSuchNick(const std::string &nick, const std::string &badnick)
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(401)
                    .pushArg(nick)
                    .pushArg(badnick)
                    .pushSuffix("No such nick/channel");
}

Message errNickCollision(const std::string &nick, const std::string &user, const std::string &host, const std::string &badnick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(436)
                    .pushArg(nick)
                    .pushArg(badnick)
                    .pushSuffix("Nickname collision KILL from " + user + "@" + host);
}

Message errUnavailResource(const std::string &nick, const std::string &target) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(437)
                    .pushArg(nick)
                    .pushArg(target)
                    .pushSuffix("Nick/channel is temporarily unavailable");
}

Message errRestricted(const std::string &nick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(484)
                    .pushArg(nick)
                    .pushSuffix("Your connection is restricted!");
}

Message errNeedMoreParams(const std::string &nick, const std::string &command)
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(461)
                    .pushArg(nick)
                    .pushArg(command)
                    .pushSuffix("Not enough parameters");
}

Message errAlreadyRegistered(const std::string &nick)
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(462)
                    .pushArg(nick)
                    .pushSuffix("Unauthorized command (already registered)");
}

Message errNoOrigin(const std::string &nick)
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(409)
                    .pushArg(nick)
                    .pushSuffix("No origin specified");
}


Message errNoSuchChannel(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(403)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("No such channel");
}

Message errPassWdMissMatch(const std::string &nick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(464)
                    .pushArg(nick)
                    .pushSuffix("Password incorrect");
}

Message errInviteOnlyChan(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(473)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("Cannot join channel (+i)");
}

Message errBadChannelKey(const std::string &nick,  const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(475)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("Cannot join channel (+k)");
}




Message errChannelIsFull(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(471)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("Cannot join channel (+l)");
}

Message errNotOnChannel(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(442)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("You're not on that channel");
}

Message errNoRecipient(const std::string &nick, const std::string &command) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(411)
                    .pushArg(nick)
                    .pushSuffix("No recipient given (" + command + ")");
}

Message errNoTextToSend(const std::string &nick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(412)
                    .pushArg(nick)
                    .pushSuffix("No text to send");
}

Message errCannotSendToChan(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(404)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("Cannot send to channel");
}

Message errChanOpIsNeeded(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(482)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("You're not a channel operator");
}

Message errUserNotInChannel(const std::string &nick, const std::string &channel, const std::string &newnick) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(441)
                    .pushArg(newnick)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("They aren't on that channel");
}

Message errKeyset(const std::string &nick, const std::string &channel) 
{
    return Message().setPrefix(SERVER_NAME)
                    .setReplyCode(467)
                    .pushArg(nick)
                    .pushArg(channel)
                    .pushSuffix("Channel key already set");
}

}
