#include "include/proxy_server.h"

#include <iostream>

int main()
{
    // int port; 
    // std::string ip;
    // std::string db_conninfo;

    // config(port, ip, db_conninfo);

    ProxyServer server;
    //PGdatabase database;

    try
    {
        server.Start(8888, "127.0.0.1");
        // database.connect(db_conninfo);

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
                    //std::string db_answer = database.query_exec(query);
                    server.Send(sockfd, query);
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

    return 0;
}