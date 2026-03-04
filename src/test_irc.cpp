#include "Message.hpp"
#include "Reply.hpp"
#include <iostream>

// Función auxiliar para imprimir el resultado del test
void printTest(const std::string& testName, const Message& msg) 
{
    std::cout << "--- TEST: " << testName << " ---" << std::endl;
    std::cout << "Raw string: " << msg.stringify() << std::endl;
    std::cout << "Args Count: " << msg.argsCount() << std::endl;
    if (msg.hasSuffix())
        std::cout << "Suffix:     " << msg.suffix() << std::endl;
    std::cout << "----------------------------\n" << std::endl;
}

int main() 
{
    // --- PRUEBA 1: PARSING DE UN COMANDO REAL ---
    {
        Message msg;
        std::string input = ":pablgarc!user@host PRIVMSG #42madrid :Hola chicos, esto funciona?";
        if (msg.parseRequest(input))
        {
            printTest("Parsing PRIVMSG", msg);
        }
    }

    // PRUEBA 2: PARSING CON MUCHOS ESPACIOS
    {
        Message msg;
        std::string input = "NICK      pablgarc   ";
        msg.parseRequest(input);
        printTest("Parsing NICK con espacios", msg);
    }

    // PRUEBA 3: GENERACIÓN DE RESPUESTA 
    {
        // Simulamos namespace Reply
        Message error = Reply::errNicknameInUse("pablo", "pablgarc");
        printTest("Generación Error 433", error);
        
        // Verifica el formato manual para ver si funciona Reply.cpp:
        Message manual;
        manual.setPrefix("irc.42madrid.com").setReplyCode(433).pushArg("pablo").pushArg("pablgarc").pushSuffix("Nickname is already in use");
        printTest("Generación Manual Error 433", manual);
    }

        // PRUEBA 4: MÁS DE 15 ARGUMENTOS (debe fallar)
    {
        Message msg;

        // Construimos un mensaje con 16 argumentos
        std::string input = "CMD";
        for (int i = 0; i < 16; i++)
            input += " arg" + std::to_string(i);

        bool ok = msg.parseRequest(input);

        std::cout << "--- TEST: Más de 15 argumentos ---" << std::endl;
        std::cout << "Resultado parseRequest(): " << (ok ? "true (ERROR)" : "false (OK)") << std::endl;
        std::cout << "Args Count: " << msg.argsCount() << std::endl;
        std::cout << "----------------------------------\n" << std::endl;
    }

    // PRUEBA 5: OTRO ERROR DE Reply.cpp (por ejemplo ERR_NOSUCHNICK)
    {
        Message error = Reply::errNoSuchNick("usuario", "usuarioInexistente");
        printTest("Generación Error 401 (No Such Nick)", error);
    }

        // PRUEBA 6: Mensaje sin prefijo
    {
        Message msg;
        std::string input = "PING server1";
        msg.parseRequest(input);
        printTest("Parsing sin prefijo", msg);
    }

    // PRUEBA 7: Mensaje sin argumentos
    {
        Message msg;
        std::string input = "QUIT";
        msg.parseRequest(input);
        printTest("Parsing sin argumentos", msg);
    }

    // PRUEBA 8: Mensaje con sufijo vacío
    {
        Message msg;
        std::string input = "PRIVMSG #canal :";
        msg.parseRequest(input);
        printTest("Parsing sufijo vacío", msg);
    }

    // PRUEBA 9: Mensaje con solo prefijo (debería tener comando)
    {
        Message msg;
        std::string input = ":servidor";
        bool ok = msg.parseRequest(input);

        std::cout << "--- TEST: Solo prefijo ---" << std::endl;
        std::cout << "Resultado parseRequest(): " << (ok ? "true" : "false") << std::endl;
        std::cout << "Command: '" << msg.command() << "'" << std::endl;
        std::cout << "----------------------------\n" << std::endl;
    }

    // PRUEBA 10: Mensaje vacío
    {
        Message msg;
        bool ok = msg.parseRequest("");

        std::cout << "--- TEST: Mensaje vacío ---" << std::endl;
        std::cout << "Resultado parseRequest(): " << (ok ? "true (ERROR)" : "false (OK)") << std::endl;
        std::cout << "----------------------------\n" << std::endl;
    }

    // PRUEBA 11: Más de 15 argumentos (parser debe fallar)
    {
        Message msg;

        std::string input = "CMD";
        for (int i = 0; i < 16; i++)
            input += " arg" + std::to_string(i);

        bool ok = msg.parseRequest(input);

        std::cout << "--- TEST: Más de 15 argumentos ---" << std::endl;
        std::cout << "Resultado parseRequest(): " << (ok ? "true (ERROR)" : "false (OK)") << std::endl;
        std::cout << "Args Count: " << msg.argsCount() << std::endl;
        std::cout << "----------------------------------\n" << std::endl;
    }

    // PRUEBA 12: Otro error de Reply.cpp (ERR_NEEDMOREPARAMS)
    {
        Message error = Reply::errNeedMoreParams("pablgarc", "PRIVMSG");
        printTest("Generación Error 461 (Need More Params)", error);
    }

    // PRUEBA 13: Otro error de Reply.cpp (ERR_NOSUCHCHANNEL)
    {
        Message error = Reply::errNoSuchChannel("pablgarc", "#canalInexistente");
        printTest("Generación Error 403 (No Such Channel)", error);
    }

    // PRUEBA 14: Comprobación exacta de stringify()
    {
        Message msg;
        msg.setPrefix("irc.test.net")
           .setCommand("PRIVMSG")
           .pushArg("#canal")
           .pushSuffix("Hola mundo");

        std::string expected = ":irc.test.net PRIVMSG #canal :Hola mundo";
        std::string result = msg.stringify();

        std::cout << "--- TEST: stringify exacto ---" << std::endl;
        std::cout << "Esperado: " << expected << std::endl;
        std::cout << "Obtenido: " << result << std::endl;
        std::cout << "Resultado: " << (expected == result ? "OK" : "ERROR") << std::endl;
        std::cout << "--------------------------------\n" << std::endl;
    }



    return 0;
}