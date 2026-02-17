# Especificaciones del Proyecto: ircserv

## 📋 Información General
| Parámetro | Detalle |
| :--- | :--- |
| **Nombre del programa** | `ircserv` |
| **Lenguaje** | C++ 98 |
| **Argumentos** | `<port>` (puerto de escucha), `<password>` (contraseña de red) |
| **Makefile** | `NAME`, `all`, `clean`, `fclean`, `re` |

---

## 📂 Archivos y Extensiones
* **Makefile**: Script para automatizar la compilación.
* **\*.cpp**: Archivos de código fuente (implementación de clases y lógica).
* **\*.h / \*.hpp**: Archivos de cabecera (declaraciones de clases y prototipos).
* **\*.tpp**: Implementación de **Templates** (plantillas). Se separan para mantener las cabeceras limpias.
* **\*.ipp**: Implementación de funciones **Inline**. Se usan para mejorar el rendimiento en funciones pequeñas.

---

## 🛠️ Funciones Externas Permitidas

### Gestión de Red y Sockets
* `socket`: Crea un punto de acceso para la comunicación (endpoint).
* `bind`: Asocia el socket a una dirección IP y puerto específicos.
* `listen`: Prepara el servidor para aceptar conexiones entrantes.
* `accept`: Acepta una nueva conexión y crea un socket específico para ella.
* `connect`: Establece una conexión con un servidor remoto.
* `setsockopt`: Configura opciones del socket (ej: permitir reutilizar el puerto).
* `getsockname`: Devuelve la dirección local vinculada a un socket.
* `send` / `recv`: Envían y reciben datos a través del socket.

### Conversión de Datos y Direcciones
* `htons` / `htonl`: Convierte números (puertos/IP) de formato "host" a "network" (Big Endian).
* `ntohs` / `ntohl`: Convierte números de formato "network" a "host".
* `inet_addr`: Convierte una cadena IP (v4) a formato binario.
* `inet_ntoa` / `inet_ntop`: Convierte direcciones de red binarias a cadenas de texto legibles.
* `getaddrinfo` / `freeaddrinfo`: Resuelve nombres de dominio y gestiona estructuras de red modernas.
* `getprotobyname` / `gethostbyname`: Obtiene información de protocolos o hosts por su nombre.

### Control de E/S y Archivos
* `poll`: Monitoriza múltiples sockets para saber cuáles tienen datos listos para leer/escribir.
* `fcntl`: Configura el descriptor de archivo (ej: poner el socket en modo no bloqueante).
* `lseek` / `fstat`: Manipula la posición del puntero de archivo y obtiene estadísticas del mismo.
* `close`: Cierra un descriptor de archivo o socket.

### Gestión de Señales
* `signal` / `sigaction`: Establece funciones para manejar señales del sistema (como SIGINT).
* `sigemptyset` / `sigfillset`: Inicializa conjuntos de señales (vacíos o llenos).
* `sigaddset` / `sigdelset`: Añade o elimina señales específicas de un conjunto.
* `sigismember`: Comprueba si una señal pertenece a un conjunto determinado.

---

### 🚀 Uso de archivos `.ipp` y funciones `inline` en ircserv

En un servidor IRC, el rendimiento y la organización son clave. El uso de estos archivos tiene tres propósitos principales:

1. **Eficiencia en Getters y Setters**: 
   El servidor maneja múltiples clases (`User`, `Channel`, `Server`). Para obtener el *nickname* de un usuario o el *fd* de un socket miles de veces por segundo, se usa `inline` para que el acceso sea **instantáneo**, eliminando el coste de llamada a función.

2. **Optimización del Bucle Principal**: 
   Las comprobaciones de estado constantes que ocurren dentro de `poll()` o `select()` (como `isAlive()`, `isRegistered()` o `canWrite()`) se definen en el `.ipp` para no ralentizar el flujo de mensajes.

3. **Arquitectura Limpia (Requisito 42)**: 
   Permite separar la **interfaz** (el archivo `.hpp`, que queda limpio y solo con declaraciones) de la **implementación de alto rendimiento** (el archivo `.ipp`). Esto demuestra un dominio avanzado de la estructura de archivos en C++.

**Resumen técnico:** Se usan para que las funciones más pequeñas y repetitivas no generen un "cuello de botella" en el tráfico de datos del servidor.

---

# 🎯 Objetivos y Reglas del Proyecto: ircserv

## 🚀 Requisitos de Ejecución
* **Comando**: `./ircserv <port> <password>`
* **Puerto**: Puerto donde el servidor escuchará conexiones.
* **Password**: Contraseña necesaria para cualquier cliente que intente conectar.

---

## 🛠️ Especificaciones Técnicas (Ubuntu)
* **Estándar**: C++ 98 estricto.
* **Sin Forking**: Prohibido crear procesos hijos.
* **E/S No Bloqueante**: Todas las operaciones de entrada/salida deben ser no bloqueantes.
* **Multiplexación Única**: Solo se permite usar **un único `poll()`** (o equivalente como `select`, `epoll`) para manejar todas las operaciones (lectura, escritura, escucha).
* **Gestión de Recursos**: Intentar hacer `read/recv` o `write/send` en un descriptor de archivo sin pasar por `poll()` resultará en una nota de **0**.
* **Protocolo**: Comunicación vía TCP/IP (v4 o v6).

---

## 💬 Funcionalidades del Servidor
Debes elegir un **cliente IRC de referencia** (ej. Irssi, mIRC, HexChat). Tu servidor debe permitir:
* **Autenticación**: Validación de contraseña.
* **Gestión de Usuario**: Establecer *nickname* y *username*.
* **Canales**: Unirse a canales (`JOIN`) y retransmitir mensajes a todos los usuarios del canal.
* **Mensajería**: Enviar y recibir mensajes privados.
* **Roles**: Diferencia entre usuarios regulares y **Operadores de canal**.

### Comandos de Operador (Canales)
[Lista de comandos IRC (link)](https://en.wikipedia.org/wiki/List_of_IRC_commands)
* `KICK`: Expulsar a un cliente del canal.
* `INVITE`: Invitar a un cliente a un canal.
* `TOPIC`: Ver o cambiar el tema del canal.
* `MODE`: Cambiar modos del canal:
    * `i`: Canal solo por invitación.
    * `t`: Restringir `TOPIC` solo a operadores.
    * `k`: Establecer/quitar contraseña del canal.
    * `o`: Dar/quitar privilegio de operador.
    * `l`: Establecer límite de usuarios.

---

## ⚠️ Pruebas Críticas y Robustez
* **Agregación de Datos**: El servidor debe ser capaz de reconstruir comandos enviados en partes (ej. usando `nc` y enviando fragmentos con `Ctrl+D`).
* **Resiliencia**: Manejar datos parciales, ancho de banda bajo y múltiples clientes simultáneos sin bloquearse.

---

## 📝 Requisitos del README.md
El archivo debe estar en la raíz, escrito en **Inglés** y contener:
1. **Línea Obligatoria**: *This project has been created as part of the 42 curriculum by [tus nombres]...* (en cursiva).
2. **Description**: Meta y visión general.
3. **Instructions**: Compilación y ejecución.
4. **Resources**: Documentación utilizada y **explicación del uso de IA** (qué tareas y qué partes).

---

## 🌟 Bonus (Solo si la parte obligatoria es perfecta)
* **Transferencia de archivos**.
* **Creación de un Bot**.