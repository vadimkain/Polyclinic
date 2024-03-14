#include "DBQuery.hpp"
#include "config.hpp"

#include <pqxx/result>

#include <sstream>
#include <iostream>

namespace config = server::common::config;

namespace server::db 
{

DBQuery::DBQuery() {
    std::stringstream db_connection_command_stream;
    db_connection_command_stream << "host=" << config::DB_HOST << " port=" << config::DB_PORT 
                                 << " user=" << config::DB_USERNAME << " password =" << config::DB_PASSWORD 
                                 << " dbname=" << config::DB_NAME;

    m_db_connection = std::make_unique<pqxx::connection>(db_connection_command_stream.str());

    if (m_db_connection->is_open()) {
        std::cout << "Success\n";
    } else {
        std::cout << "Failed\n";
    }

    m_db_transaction = std::make_unique<pqxx::work>(*m_db_connection);
};

DBQuery::~DBQuery(void) {
    m_db_connection->close();
}

void DBQuery::output_all_users(void) {
    pqxx::result res = m_db_transaction->exec("SELECT * FROM users");
    m_db_transaction->commit();
    for (const auto &row : res) {
        for (const auto &field : row) {
            std::cout << field.c_str() << '\t';
        }
        std::cout << std::endl;
    }
}

}   // !server::db;