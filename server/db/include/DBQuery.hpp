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
    bool check_signin_is_valid(std::string email, std::string password);

private:
    std::unique_ptr<pqxx::connection> m_db_connection;
};

}   // !server::db;

#endif  // !POLYCLINIC_SERVER_DB_DBQUERY_HPP;