#include "include/client.h"

#include <iostream>

int main()
{
    // int port; 
    // std::string ip;

    // config(port, ip);

    Client client;

    // Набор дескрипторов
    struct pollfd pfds[2];

    // Для взаимодествия с сервером
    pfds[0].fd = client.getClientFd();
    pfds[0].events = POLLIN;

    // Для работы с stdin
    pfds[1].fd = STDIN_FILENO;
    pfds[1].events = POLLIN;

    try 
    {
        client.Connect(8888, "127.0.0.1");

        bool connected = true;

        while (connected)
        {
            poll(pfds, 2, INFTIM);

            if (pfds[0].revents & POLLIN)
            {
                std::string message = client.Recv();
                std::cout << "FROM SERVER: " << message << std::endl;
            }

            if (pfds[1].revents & POLLIN) 
            {
                std::string query;

                getline(std::cin, query);

                if (std::cin.eof())
                    connected = false;
                else 
                    client.Send(query);
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

    client.Close();

    return 0;
}
