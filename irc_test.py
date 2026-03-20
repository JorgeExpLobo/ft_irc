#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ircserv 6667 123
import socket
import threading
import time
import sys


HOST = '127.0.0.1'
PORT = 6667
PASS = "123" 

def log(msg, nick="SYSTEM"):
    print(f"[{nick}] {msg}")

# 
# TEST 1:  (Comandos Parciales)

def test_partial_commands():
    log("Iniciando prueba de comandos fragmentados...", "PARTIAL")
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    
    # Enviamos el PASS letra a letra con pausas
    for char in f"PASS {PASS}\r":
        s.send(char.encode())
        time.sleep(0.01)
    s.send("\n".encode())
    
    # NICK enviado en dos trozos sin saltos de línea intermedios
    s.send("NICK fragmen".encode())
    time.sleep(0.5)
    s.send("tado\r\n".encode())
    
    s.send("USER f 0 * :f\r\n".encode())
    time.sleep(0.5)
    
    # Si el server no tiene buffer por cliente, esto fallará
    s.settimeout(1)
    try:
        resp = s.recv(1024).decode()
        if "001" in resp: log("Login exitoso con comandos fragmentados.", "OK")
    except: log("Error: El servidor no procesó el comando fragmentado.", "FAIL")
    finally:
        s.close()

# TEST 2: Desconexión Abrupta (No QUIT)

def test_abrupt_exit(n):
    nick = f"ghost_{n}"
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((HOST, PORT))
        s.send(f"PASS {PASS}\r\nNICK {nick}\r\nUSER {nick} 0 * :{nick}\r\n".encode())
        time.sleep(0.2)
        # Cerramos el socket de golpe sin mandar QUIT. 
        # El servidor DEBE detectar esto en el poll() y limpiar.
        s.close() 
        log(f"Cliente {nick} desconectado abruptamente.", "ABRUPT")
    except Exception as e:
        log(f"Error en abrupt_exit: {e}", "FAIL")


# TEST 3: (Concurrencia)

def client_thread(n):
    nick = f"user{n}"
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((HOST, PORT))
        s.sendall(f"PASS {PASS}\r\nNICK {nick}\r\nUSER {nick} 0 * :{nick}\r\n".encode())
        time.sleep(0.1)

        s.sendall(f"JOIN #test\r\n".encode())
        for i in range(2):
            s.sendall(f"PRIVMSG #test :Mensaje {i} de {nick}\r\n".encode())
            time.sleep(0.05)
        
        # QUIT ordenado
        s.sendall(f"QUIT :Finalizando test\r\n".encode())
        s.close()
    except Exception as e:
        log(f"Error en hilo {n}: {e}", "FAIL")

def test_privmsg_errors():
    log("Iniciando prueba de PRIVMSG y errores...", "PRIVMSG")
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    s.sendall(f"PASS {PASS}\r\nNICK tester\r\nUSER tester 0 * :tester\r\n".encode())
    time.sleep(0.2)

    # Sin destinatario
    s.sendall("PRIVMSG :Hola sin nick\r\n".encode())
    try:
        resp = s.recv(1024).decode()
        if "411" in resp: log("Error 411 recibido correctamente.", "OK")
    except: log("No se recibió error 411.", "FAIL")

    # Destinatario inexistente
    s.sendall("PRIVMSG nobody :Hola\r\n".encode())
    try:
        resp = s.recv(1024).decode()
        if "401" in resp: log("Error 401 recibido correctamente.", "OK")
    except: log("No se recibió error 401.", "FAIL")

    # Mensaje vacío
    s.sendall("PRIVMSG tester :\r\n".encode())
    try:
        resp = s.recv(1024).decode()
        if "412" in resp or resp == "": log("Error 412 o mensaje vacío manejado.", "OK")
    except: log("No se manejó correctamente mensaje vacío.", "FAIL")

    s.close()


def test_join_part():
    log("Iniciando prueba de JOIN/PART...", "CHANNEL")
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((HOST, PORT))
    s.sendall(f"PASS {PASS}\r\nNICK chanuser\r\nUSER chanuser 0 * :chanuser\r\n".encode())
    time.sleep(0.2)

    # JOIN canal
    s.sendall("JOIN #test\r\n".encode())
    try:
        resp = s.recv(1024).decode()
        if "#test" in resp: log("JOIN correcto.", "OK")
    except: log("JOIN falló.", "FAIL")

    # JOIN canal inexistente (dependiendo de tu server, podría fallar)
    s.sendall("JOIN #noexiste\r\n".encode())
    try:
        resp = s.recv(1024).decode()
        log("JOIN a canal inexistente recibido: " + resp.strip(), "INFO")
    except: pass

    # PART canal
    s.sendall("PART #test :Adiós\r\n".encode())
    try:
        resp = s.recv(1024).decode()
        if "#test" in resp: log("PART correcto.", "OK")
    except: log("PART falló.", "FAIL")

    s.close()


# Ejecución del Plan

if __name__ == "__main__":
    # 1. Probar comandos parciales primero (esto suele tirar servidores mal hechos)
    test_partial_commands()
    
    time.sleep(1)

    # TEST 4
    test_privmsg_errors()
    time.sleep(0.5)

    # TEST 5
    test_join_part()
    time.sleep(0.5)



    # 2. Lanzar mezcla de clientes normales y desconexiones sucias
    threads = []
    log("Lanzando ráfaga de clientes concurrentes y cierres sucios...")
    
    for i in range(10):
        if i % 2 == 0:
            t = threading.Thread(target=client_thread, args=(i,))
        else:
            t = threading.Thread(target=test_abrupt_exit, args=(i,))
        threads.append(t)
        t.start()

    for t in threads:
        t.join()

    log("TEST COMPLETO. Revisa Valgrind para asegurar que no hay leaks.", "DONE")