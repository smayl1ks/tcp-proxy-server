#include "include/proxy_server.h"
#include "include/database.h"

#include <iostream>

int main()
{
    int port = 8888; 
    std::string ip = "127.0.0.1";
    std::string db_conninfo = "dbname=test user=tester password=12345 host=localhost port=5432";

    // config(port, ip, db_conninfo);

    ProxyServer server;
    PGdatabase database;

    try
    {
        server.Start(port, ip);
        database.Connect(db_conninfo);

        while (true)
        {
            if(server.Poll() == NO_ACTIVE_CLIENTS) {
                continue;
            }

            for (const auto& [sockfd, client]: server.clients)
            {
                if (client.active) {

                    std::string query = server.Recv(sockfd);

                    if (query.empty()) {
                        continue;
                    }

                    std::cout << "QUERY: " << query << "\n";
                    //saveLog(query, client.ip);
                    std::string db_answer = database.Exec(query);
                    server.Send(sockfd, db_answer);
                }
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