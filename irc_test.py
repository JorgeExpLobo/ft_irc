#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket
import threading
import time

HOST = '127.0.0.1'
PORT = 6667

NUM_CLIENTS = 5
CHANNELS = ["#test1", "#test2"]

# -------------------------
# Funciones base
# -------------------------
def connect_client(nick, user):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    s.sendall(f"NICK {nick}\r\n".encode())
    s.sendall(f"USER {user} 0 * :{user}\r\n".encode())
    return s

def join_channel(sock, channel):
    sock.sendall(f"JOIN {channel}\r\n".encode())

def send_message(sock, target, text):
    sock.sendall(f"PRIVMSG {target} :{text}\r\n".encode())

def part_channel(sock, channel):
    sock.sendall(f"PART {channel}\r\n".encode())

def quit_client(sock, reason="bye"):
    sock.sendall(f"QUIT :{reason}\r\n".encode())
    sock.close()

def read_response(sock, timeout=0.1):
    """Opcional: leer mensajes del servidor para debug"""
    sock.settimeout(timeout)
    try:
        while True:
            data = sock.recv(4096)
            if not data:
                break
            print(data.decode(), end="")
    except:
        pass

# -------------------------
# Hilo cliente
# -------------------------
def client_thread(n):
    nick = f"user{n}"
    user = f"user{n}"
    s = connect_client(nick, user)
    time.sleep(0.1)  # esperar registro

    # Unirse a todos los canales
    for ch in CHANNELS:
        join_channel(s, ch)
        time.sleep(0.05)

    # Enviar 3 mensajes por canal
    for ch in CHANNELS:
        for i in range(3):
            send_message(s, ch, f"Mensaje {i} desde {nick}")
            time.sleep(0.05)

    # Part de un canal
    part_channel(s, CHANNELS[0])
    time.sleep(0.05)

    # Quedarse en otro canal, opcional leer mensajes
    read_response(s, 0.05)

    # Quit final
    quit_client(s)

# -------------------------
# Lanzar múltiples clientes
# -------------------------
threads = []
for i in range(NUM_CLIENTS):
    t = threading.Thread(target=client_thread, args=(i,))
    threads.append(t)
    t.start()
    time.sleep(0.05)  # iniciar ligeramente espaciados

for t in threads:
    t.join()

print("[TEST COMPLETO] Todos los clientes se desconectaron.")