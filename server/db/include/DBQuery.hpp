#ifndef POLYCLINIC_SERVER_DB_DBQUERY_HPP
#define POLYCLINIC_SERVER_DB_DBQUERY_HPP

#include <pqxx/connection>
#include <pqxx/transaction>
#include <string>
#include <vector>

#include <memory>

namespace server::db {

struct UserInfo {
    std::uint64_t id;
    std::string name;
    std::string surname;
    std::string middle_name;
    std::string email;
    std::string role;
    std::vector<std::string> phone_numbers;
};

class DBQuery {
public:
    DBQuery(void);
    DBQuery(const DBQuery &other) = delete;
    ~DBQuery(void);

    void operator=(const DBQuery &other) = delete;

    void output_all_users(void);
    std::pair<bool, UserInfo> check_signin_is_valid(std::string email, std::string password);

    UserInfo get_user_info_by_email(const std::string& email);

private:
    std::unique_ptr<pqxx::connection> m_db_connection;
};

}   // !server::db;

#endif  // !POLYCLINIC_SERVER_DB_DBQUERY_HPP;