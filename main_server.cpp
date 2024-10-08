#include "include/proxy_server.h"
#include "src/logger.cpp"
#include "include/database.h"
#include <iostream>

int main()
{
    Logger logger("../logs/basic-log.txt", "INFO");

    int port = 8888; 
    std::string ip = "127.0.0.1";

    // Тестотовая бд создавалась вручную
    std::string db_conninfo = "dbname=test user=tester password=12345 host=localhost port=5432";

    // config(port, ip, db_conninfo);

    ProxyServer server;
    PGdatabase database;

    std::vector <Client> clients;
    try
    {
        server.Start(port, ip);
        database.Connect(db_conninfo);

        while (true)
        {
            if (server.Poll() == NO_EVENTS)
                continue;

            server.setActiveClients(clients);

            for (const auto& client: clients)
            {
                std::string query = server.Recv(client.fd);

                if (query.empty()) {
                    continue;
                }

                logger.log(query, client.ip);

                std::string db_answer = database.Exec(query);
                server.Send(client.fd, db_answer);
            }
        }
    } catch (const std::system_error& ex)
    {
        std::cout << "SYSTEM ERROR: " << ex.what() << std::endl;
    } 
    catch (const std::runtime_error& ex)
    {
        std::cout << "RUNTIME ERROR: " << ex.what() << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cout << "OTHER ERROR: " << ex.what() << std::endl;
    }

    server.Close();
    database.Close();

    return 0;
}