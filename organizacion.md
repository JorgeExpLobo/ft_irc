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


###  Reply.cpp
 *  Generación estandarizada de respuestas del servidor IRC.
 *
 * El namespace Reply agrupa un conjunto de funciones puras encargadas de
 * construir mensajes de respuesta conforme a la especificación del protocolo IRC
 * (RFC 2812). Estas funciones no dependen del estado interno del servidor ni de
 * ninguna instancia concreta, y su única responsabilidad es producir objetos
 * `Message` correctamente formateados para ser enviados a los clientes.
 *
 * Justificación del uso de un namespace:
 * --------------------------------------
 * - Organización lógica: todas las respuestas numéricas y mensajes generados
 *   por el servidor forman un módulo conceptual independiente del parser, del
 *   manejo de clientes o de la lógica de canales.
 *
 * - Evita colisiones de nombres: funciones como `error()`, `list()`, `topic()`
 *   o `kill()` podrían entrar en conflicto con funciones del servidor o de
 *   otros módulos si estuvieran en el espacio global.
 *
 * - API más expresiva: usar `Reply::errNoSuchNick(nick)` o
 *   `Reply::welcome(nick, user, host)` deja claro que se trata de respuestas
 *   oficiales del servidor, no de mensajes arbitrarios.
 *
 * - Separación de responsabilidades: el servidor decide *cuándo* enviar una
 *   respuesta, pero el namespace Reply define *cómo* debe formatearse según la
 *   RFC. Esto mantiene el código más limpio y facilita el mantenimiento.
 *
 * - Testeo sencillo: al no depender de estado, estas funciones pueden probarse
 *   de forma aislada, generando mensajes completos sin necesidad de instanciar
 *   clases adicionales.
 *
 * Todas las funciones de este namespace añaden automáticamente el prefijo del
 * servidor (`SERVER_NAME`), tal y como exige la RFC para cualquier mensaje
 * originado por el servidor.
 

### Clase Message
 * Representa un mensaje IRC parseado o construido por el servidor.
 *
 * La clase Message encapsula toda la información necesaria para manejar un
 * mensaje IRC según la especificación de la RFC 2812. Un objeto Message puede
 * representar tanto un mensaje recibido desde un cliente (tras el proceso de
 * parseo) como un mensaje generado por el servidor para ser enviado a un
 * cliente.
 *
 * Responsabilidades principales:
 * ------------------------------
 * - Parsear una línea de texto recibida desde un cliente y descomponerla en:
 *      * prefijo (opcional)
 *      * comando
 *      * argumentos posicionales
 *      * sufijo (trailing argument)
 *
 * - Construir mensajes válidos para enviar al cliente, permitiendo establecer:
 *      * prefijo (normalmente el nombre del servidor)
 *      * comando o código numérico de respuesta
 *      * argumentos
 *      * sufijo
 *
 * - Garantizar que el mensaje resultante cumple el formato estándar:
 *      :<prefix> <command> <args...> :<suffix>
 *
 * Justificación del diseño:
 * -------------------------
 * - Separación clara entre parseo y generación: la clase no asume si el mensaje
 *   proviene de un cliente o del servidor; simplemente almacena su estructura.
 *
 * - Estado autocontenido: cada instancia de Message representa un único mensaje
 *   completo, evitando dependencias externas y facilitando su uso en colas,
 *   buffers o sistemas de envío.
 *
 * - Robustez en el parseo: el método parseRequest() valida el formato, respeta
 *   el límite de argumentos impuesto por la RFC (máximo 15) y distingue
 *   correctamente entre argumentos normales y sufijos.
 *
 * - API fluida para construir respuestas: métodos como setPrefix(), setCommand(),
 *   setReplyCode(), pushArg() y pushSuffix() permiten crear mensajes de forma
 *   clara y encadenada.
 *
 * - stringify(): genera la representación textual final del mensaje, lista para
 *   ser enviada por el socket al cliente.
 *
 * Uso típico:
 * -----------
 * - Mensajes recibidos:
 *      Message msg;
 *      if (msg.parseRequest(lineaRecibida)) { ... }
 *
 * - Mensajes enviados:
 *      Message out;
 *      out.setPrefix(SERVER_NAME)
 *         .setReplyCode(433)
 *         .pushArg("nick")
 *         .pushSuffix("Nickname is already in use");
 *      enviar(out.stringify());
 *
 * Esta clase constituye el núcleo del sistema de comunicación IRC del servidor,
 * proporcionando una representación fiable, consistente y conforme a la RFC de
 * todos los mensajes que entran y salen.
 
### Parseo
 *  Parsea una línea de texto recibida desde un cliente IRC.
 *
 * El método parseRequest() interpreta una línea completa enviada por un cliente
 * y la descompone en los elementos definidos por la RFC 2812:
 *
 *      [ ":" <prefix> <SPACE> ] <command> [ <SPACE> <params> ] <crlf>
 *
 * Donde los parámetros pueden incluir hasta 14 argumentos posicionales y un
 * argumento final (trailing) que comienza por ':' y puede contener espacios.
 *
 * Comportamiento del parseo:
 * --------------------------
 * - Elimina espacios iniciales redundantes.
 * - Detecta un prefijo únicamente si aparece al inicio del mensaje y antes del
 *   comando.
 * - Identifica el comando como el primer token no prefijo.
 * - Separa correctamente los argumentos posicionales (máximo 14).
 * - Detecta el sufijo (trailing argument) cuando un argumento comienza por ':'
 *   tras el comando; este argumento puede contener espacios y se toma completo.
 * - Respeta el límite total de 15 parámetros impuesto por la RFC.
 *
 * Validaciones:
 * -------------
 * - Devuelve false si la línea está vacía.
 * - Devuelve false si se supera el límite de argumentos.
 * - Devuelve false si no se encuentra un comando válido.
 * - Devuelve false si el formato general del mensaje no es coherente.
 *
 * Efectos sobre el estado interno:
 * --------------------------------
 * - Limpia el estado previo del objeto Message (prefijo, comando, argumentos,
 *   sufijo) para garantizar que cada parseo es independiente.
 * - Almacena:
 *      * _prefix      → prefijo del mensaje (si existe)
 *      * _command     → comando textual o numérico
 *      * _args[]      → argumentos posicionales
 *      * _hasSuffix   → indica si existe un argumento trailing
 *      * _argsCount   → número total de argumentos
 *
 * Ejemplos de parseo:
 * -------------------
 * Entrada:
 *      ":nick!user@host PRIVMSG #canal :Hola a todos"
 *
 * Resultado:
 *      prefix  = "nick!user@host"
 *      command = "PRIVMSG"
 *      args[0] = "#canal"
 *      args[1] = "Hola a todos"
 *      hasSuffix = true
 *
 * Entrada:
 *      "PING server1"
 *
 * Resultado:
 *      prefix  = ""
 *      command = "PING"
 *      args[0] = "server1"
 *
 * Importancia en el servidor:
 * ---------------------------
 * Este método constituye la base del procesamiento de comandos del servidor IRC.
 * Permite transformar una línea textual recibida por socket en una estructura
 * interna coherente y segura, sobre la que el servidor puede aplicar lógica de
 * autenticación, gestión de canales, envío de mensajes o generación de errores.
 *
 * El diseño garantiza que:
 * - El servidor nunca procesa datos ambiguos o mal formados.
 * - La estructura del mensaje cumple estrictamente la RFC.
 * - El resto del código puede trabajar con un objeto Message ya validado.
 

### stringify
 *  Genera la representación textual completa de un mensaje IRC.
 *
 * El método stringify() reconstruye un mensaje IRC a partir de los campos
 * internos almacenados en el objeto Message. El resultado es una cadena de
 * texto lista para ser enviada por el socket al cliente, cumpliendo el formato
 * definido por la RFC 2812.
 *
 * Formato generado:
 * -----------------
 *      [ ":" <prefix> <SPACE> ] <command> [ <SPACE> <params> ] [ " :" <suffix> ]
 *
 * Donde:
 * - El prefijo es opcional, pero obligatorio en mensajes originados por el servidor.
 * - El comando puede ser textual (PRIVMSG, JOIN, MODE…) o numérico (001, 433…).
 * - Los parámetros posicionales se imprimen en orden, separados por espacios.
 * - El sufijo (trailing argument) se imprime precedido por ':' y puede contener espacios.
 *
 * Comportamiento:
 * ---------------
 * - Si existe prefijo, se antepone con ':' y un espacio.
 * - El comando siempre se imprime.
 * - Cada argumento posicional se añade separado por un espacio.
 * - Si existe sufijo, se añade como último argumento con el formato " :<texto>".
 * - No se añaden espacios innecesarios ni separadores redundantes.
 *
 * Ejemplos:
 * ---------
 * Prefijo + comando + argumentos + sufijo:
 *      :irc.42madrid.com 433 pablgarc :Nickname is already in use
 *
 * Comando sin prefijo:
 *      PING server1
 *
 * Comando con sufijo vacío:
 *      PRIVMSG #canal :
 *
 * Importancia en el servidor:
 * ---------------------------
 * stringify() es el último paso antes de enviar un mensaje IRC al cliente.
 * Garantiza que:
 * - El formato del mensaje es válido y conforme a la RFC.
 * - Los clientes IRC pueden interpretar correctamente la respuesta.
 * - El servidor mantiene consistencia en todos los mensajes que emite.
 *
 * Este método permite separar completamente la lógica interna del servidor
 * (gestión de usuarios, canales, permisos…) del formato textual final que se
 * envía por red, facilitando el mantenimiento y la extensibilidad del sistema.
 */


---

## ⚙️ Flujo de Datos (Del Socket al Comando)

1. **Lectura**: `Server` detecta actividad en un `fd` vía `poll()`. Hace `recv()` y lo añade al `Client.buffer`.
2. **Delimitación**: El `Server` busca `\r\n` en ese buffer. Si no está, sigue esperando (soporte para comandos fragmentados).
3. **Parsing**: Si hay un mensaje completo, el `Parser` lo trocea:
   - Entrada: `KICK #canal user :razon\r\n`
   - Salida: Comando: `KICK`, Args: `["#canal", "user", "razon"]`.
4. **Ejecución**: El `CommandManager` busca `"KICK"`, recupera el objeto `KickCommand` y lanza `execute()`.
5. **Respuesta**: El comando realiza los cambios y usa `send()` para responder al cliente o difundir al canal.