#pragma once

#include <pqxx/pqxx>


namespace postgres_client {

class SqlSession {
    pqxx::connection m_connection;

public:
    void RunSingle(const char *sql_cmd);
};

}//namespace postgres_client
