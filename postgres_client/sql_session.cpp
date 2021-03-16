#include <postgres_client/sql_session.hpp>

#include <iostream>


namespace postgres_client {

void SqlSession::RunSingle(const char *sql_cmd) {
    try {
        pqxx::work w(m_connection);

        pqxx::row r = w.exec1(sql_cmd);

        w.commit();

        std::cout << "Result: " << r[0].as<int>() << std::endl;
    } catch (std::exception const &e) {
        std::cerr << e.what() << std::endl;
    }
}

}//namespace postgres_client
