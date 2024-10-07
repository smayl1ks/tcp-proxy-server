#include "../include/database.h"

void PGdatabase::Connect(std::string db_conninfo)
{
    conn = PQconnectdb(db_conninfo.data());

    if (PQstatus(conn) != CONNECTION_OK) {
        throw std::runtime_error(PQerrorMessage(conn));
    }

    std::cout << "Connection Established\n";
    std::cout << "Port: " << PQport(conn) << '\n';
    std::cout << "Host: " << PQhost(conn) << '\n';
}

std::string PGdatabase::Exec(std::string query)
{
    result = PQexec(conn, query.data());

    if (PQresultStatus(result) != PGRES_COMMAND_OK &&
        PQresultStatus(result) != PGRES_TUPLES_OK) {
        PQclear(result);
        return PQerrorMessage(conn);
    }

    return "CORRECT";
}

void PGdatabase::Close()
{
    PQfinish(conn);
}