*This project has been created as part of the 42 curriculum by pablgarc, jorexpos and jdiaz-he .*


# ft_irc

## 📌 Description

**ft_irc** is a basic implementation of an IRC (Internet Relay Chat) server written in C++. The goal of this project is to explore network programming, handle multiple simultaneous clients, and design an event-driven architecture using low-level system calls such as `poll()`.

The server allows multiple clients to connect, authenticate, and communicate through channels, reproducing the core behavior of a real IRC server.

---

## ⚙️ Instructions

### 🔧 Compilation

```bash
make
```

---

### ▶️ Execution

```bash
./ircserv <port> <password>
```

Example:

```bash
./ircserv 6667 123
```

---

### 🔌 Connect using netcat

```bash
nc localhost 6667
```

Basic workflow:

```bash
PASS 123
NICK user
USER user 0 * :realname
JOIN #channel
PRIVMSG #channel :Hello world
```

---

### 💻 IRC Client (optional)

You can also use a real IRC client such as:

* irssi
* weechat
* hexchat

Example:

```bash
hexchat
/connect localhost 6667 123
/join #channel
```

---

## 🚀 Features

* Non-blocking TCP server
* Multiple client handling using `poll()`
* Modular command system (Command Pattern)
* Supported commands:

  * PASS
  * NICK
  * USER
  * JOIN
  * PRIVMSG
* Per-client buffering for partial commands
* Channel message broadcasting
* Graceful client disconnection handling

---

## 🔐 Channel Modes

The server supports several IRC channel modes to control user permissions and channel behavior:

* **+i (Invite-only)**
  Only invited users can join the channel.

* **+t (Topic restricted)**
  Only channel operators can change the topic.

* **+k (Key protected)**
  Users must provide the correct password to join the channel.

* **+o (Operator privilege)**
  Grants operator status to a user in the channel.

* **+l (User limit)**
  Limits the number of users allowed in the channel.

These modes allow basic moderation and access control within channels.

---

## 🧠 Technical Choices

* Use of `poll()` for I/O multiplexing
* Command-based architecture (`ACommand`)
* Clear separation of responsibilities:

  * `Server` → networking layer
  * `Client` → user state management
  * `Channel` → channel logic
  * `CommandManager` → command dispatching
* Per-client input buffer to handle fragmented messages
* Non-blocking sockets to prevent server freezing

---

## 📚 Resources

### IRC Documentation

  https://modern.ircdocs.horse/
  https://datatracker.ietf.org/doc/html/rfc1459

---

### Additional References

* `poll()` manual pages
* TCP/IP socket programming examples in C/C++

---

### 🤖 AI Usage

Artificial Intelligence (ChatGPT) was used as a support tool for:

* Understanding networking concepts
* Debugging issues (e.g., segmentation faults, socket handling)
* Reviewing architecture and best practices
* Preparing evaluation scenarios and tests

All implementation decisions and final code were fully understood and integrated manually.

---

## 🧪 Testing

The server has been tested using:

* `nc` (netcat)
* multiple simultaneous clients
* partial command handling
* message flooding scenarios
* abrupt client disconnections

---

## 📌 Notes

This project implements a simplified version of the IRC protocol and does not cover all features of a full IRC server.

---
