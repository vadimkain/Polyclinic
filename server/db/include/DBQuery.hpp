#ifndef POLYCLINIC_SERVER_DB_DBQUERY_HPP
#define POLYCLINIC_SERVER_DB_DBQUERY_HPP

#include <pqxx/connection>
#include <pqxx/transaction>

#include <memory>

namespace server::db {

class DBQuery {
public:
    DBQuery(void);
    DBQuery(const DBQuery &other) = delete;
    ~DBQuery(void);

    void operator=(const DBQuery &other) = delete;

    void output_all_users(void);

private:
    std::unique_ptr<pqxx::connection> m_db_connection;
    std::unique_ptr<pqxx::transaction_base> m_db_transaction;
};

}   // !server::db;

#endif  // !POLYCLINIC_SERVER_DB_DBQUERY_HPP;