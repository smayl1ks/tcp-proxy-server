#pragma once

#include <libpq-fe.h>
#include <iostream>
#include <vector>
#include <system_error>
#include <string>

class PGdatabase
{
private:
    PGconn *conn;
    PGresult *result;
    ExecStatusType resStatus;

    std::string db_conninfo;
    // std::string dbhost = "localhost";
    // std::string dbname = "test";
    // std::string dbuser = "tester";
    // std::string dbpass = "12345";
    // int dbport = 5432;

public:

    std::string Exec(std::string query);
    void Connect(std::string conninfo);
    void Close();
};