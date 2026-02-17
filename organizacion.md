# Estructura de Proyecto: ft_irc (42 Madrid)

## 📂 Directorios
- `/inc`, `/inc/cmd`: Archivos .hpp (cabeceras).
- `/src`: Archivos .cpp principales.
- `/src/cmd`: Un archivo .cpp por cada comando IRC.
- `/obj`: Objetos de compilación (ya gestionado por Makefile).

---

## 📄 Listado de Archivos

### 1. Núcleo del Servidor (Infraestructura)
| Archivo | Clase | Descripción |
| :--- | :--- | :--- |
| `main.cpp` | - | Punto de entrada. Valida `./ircserv <port> <password>`, instancia el `Server` y gestiona señales (SIGINT para cierre limpio). |
| `Server.hpp/cpp` | `Server` | El motor principal. Contiene el socket maestro, el array de `pollfd` y el bucle `poll()`. Gestiona `accept()` de nuevos clientes. |
| `Client.hpp/cpp` | `Client` | Almacena datos del usuario: `fd`, `hostname`, `nickname`, `username`, estado de registro y un `std::string buffer` para datos parciales. |
| `Channel.hpp/cpp` | `Channel` | Gestiona los canales: lista de clientes presentes, lista de operadores (punteros), el *topic* y los modos (`i, t, k, o, l`). |

### 2. Capa de Abstracción de Comandos (Independencia)
| Archivo | Clase | Descripción |
| :--- | :--- | :--- |
| `ACommand.hpp` | `ACommand` | **CLASE ABSTRACTA (Interfaz)**. Define el método `virtual void execute(Client &sender, std::vector<std::string> &args) = 0;`. |
| `CommandManager.hpp/cpp` | `CommandManager` | Contiene un `std::map<std::string, ACommand*>`, una lista con los comandos. Recibe el comando parseado y llama al objeto correspondiente. |

### 3a. Lógica de Comandos (`/src/cmds`)
*Cada comando hereda de `ACommand`. Se recomienda un archivo .cpp por comando para evitar conflictos en Git.*
| Archivo | Comando | Descripción |
| :--- | :--- | :--- |
| `Pass.cpp` | `PASS` | Verifica la contraseña del servidor al conectar. |
| `Nick.cpp` | `NICK` | Establece o cambia el apodo (comprueba duplicados). |
| `User.cpp` | `USER` | Establece el nombre real y username. |
| `Join.cpp` | `JOIN` | Entra en canales (crea el canal si no existe). |
| `Part.cpp` | `PART` | Sale de un canal. |
| `Privmsg.cpp` | `PRIVMSG` | Envía mensajes a usuarios o canales. |
| `Quit.cpp` | `QUIT` | Desconecta al usuario y limpia sus rastros. |
| `Kick.cpp` | `KICK` | (Op) Expulsa a un usuario. |
| `Invite.cpp` | `INVITE` | (Op) Invita a un usuario a un canal +i. |
| `Topic.cpp` | `TOPIC` | (Op) Cambia o consulta el tema del canal. |
| `Mode.cpp` | `MODE` | (Op) Gestiona los 5 modos obligatorios (i, t, k, o, l). |

### 3b. Comandos de Estabilidad e Información (Adicionales)
*Estos comandos no siempre son obligatorios por el subject, pero son esenciales para que los clientes reales (HexChat, irssi) funcionen sin errores de conexión.*

| Archivo | Comando | Descripción |
| :--- | :--- | :--- |
| `Part.cpp` | `PART` | El usuario sale de un canal específico. Es el comando "espejo" de `JOIN`. |
| `Ping.cpp` | `PING` | El cliente pregunta "¿sigues ahí?". El servidor debe responder inmediatamente. |
| `Pong.cpp` | `PONG` | Respuesta del servidor al `PING` del cliente para evitar desconexiones por timeout. |
| `Notice.cpp` | `NOTICE` | Similar a `PRIVMSG` pero no genera respuestas automáticas (evita bucles infinitos). |
| `Who.cpp` | `WHO` | Devuelve una lista de usuarios. Los clientes lo usan para rellenar la barra lateral de nombres. |
| `List.cpp` | `LIST` | Enumera los canales disponibles y sus temas (topics). |

---

### 🛠️ Detalle Técnico: El ciclo PING/PONG

Es vital implementar `PING` y `PONG` para que la evaluación sea fluida. Si vuestro servidor no responde a los PINGs del cliente de referencia, este cerrará la conexión por "Broken pipe" o "Ping timeout" a los pocos minutos.

**Flujo de ejecución:**
1. El Cliente envía: `PING :<token_aleatorio>`
2. El Parser detecta `cmdName = "PING"`.
3. El `CommandManager` llama a `PingCommand::execute`.
4. La lógica de `execute` simplemente envía de vuelta: `PONG :<token_aleatorio>` al mismo cliente.


### 4. Utilidades y Parsing
| Archivo | Clase/Función | Descripción |
| :--- | :--- | :--- |
| `Parser.hpp/cpp` | `Parser` | Recibe el buffer del `Client`. 1. Busca el `\r\n`. 2. Separa el prefijo (opcional), el comando y los argumentos. 3. Limpia espacios extra. |
| `Replies.hpp` | - | Define macros o funciones con los códigos numéricos de respuesta IRC (ej: `ERR_NOSUCHNICK 401`). |

---

## ⚙️ Flujo de Datos (Del Socket al Comando)

1. **Lectura**: `Server` detecta actividad en un `fd` vía `poll()`. Hace `recv()` y lo añade al `Client.buffer`.
2. **Delimitación**: El `Server` busca `\r\n` en ese buffer. Si no está, sigue esperando (soporte para comandos fragmentados).
3. **Parsing**: Si hay un mensaje completo, el `Parser` lo trocea:
   - Entrada: `KICK #canal user :razon\r\n`
   - Salida: Comando: `KICK`, Args: `["#canal", "user", "razon"]`.
4. **Ejecución**: El `CommandManager` busca `"KICK"`, recupera el objeto `KickCommand` y lanza `execute()`.
5. **Respuesta**: El comando realiza los cambios y usa `send()` para responder al cliente o difundir al canal.