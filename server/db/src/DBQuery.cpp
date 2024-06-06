#include "DBQuery.hpp"
#include "config.hpp"

#include "Logger.hpp"

#include <pqxx/result>

#include <sstream>
#include <thread>

namespace config = server::common::config;

namespace server::db 
{

DBQuery::DBQuery() {
    BDECLARE_TAG_SCOPE("DBQuery", __FUNCTION__);

    std::stringstream db_connection_command_stream;
    db_connection_command_stream << "host=" << config::DB_HOST << " port=" << config::DB_PORT 
                                 << " user=" << config::DB_USERNAME << " password=" << config::DB_PASSWORD 
                                 << " dbname=" << config::DB_NAME;

    BLOG_INFO("constructor called on thread #", std::this_thread::get_id());
    BLOG_INFO("Make connection to db. ", db_connection_command_stream.str());
    m_db_connection = std::make_unique<pqxx::connection>(db_connection_command_stream.str());

    if (m_db_connection->is_open()) {
        BLOG_DEBUG("Connection success");    
    } else {
        BLOG_FATAL("Connection failed");
        abort();
    }
};

DBQuery::~DBQuery(void) {
    m_db_connection->disconnect();
}

void DBQuery::output_all_users(void) {
    BDECLARE_TAG_SCOPE("DBQuery", __FUNCTION__);
    BLOG_INFO("called");
    auto db_transaction = std::make_unique<pqxx::work>(*m_db_connection);
    pqxx::result res = db_transaction->exec("SELECT * FROM users");
    db_transaction->commit();

    for (const auto &row : res) {
        std::stringstream table_info_output;
        for (const auto &field : row) {
            table_info_output << field.c_str() << '\t';
        }
        BLOG_DEBUG(table_info_output.str());
    }
}

bool DBQuery::check_signin_is_valid(std::string email, std::string password) {
    BDECLARE_TAG_SCOPE("DBQuery", __FUNCTION__);

    std::stringstream db_request;
    auto db_transaction = std::make_unique<pqxx::work>(*m_db_connection);

    db_request << "SELECT * FROM check_user_login('"
        << email << "', '" << password << "')";

    BLOG_INFO("Current request: ", db_request.str());

    pqxx::result res = db_transaction->exec(db_request.str());
    db_transaction->commit();

    return res.size() > 0 && res[0][0].as<bool>();
}

}   // !server::db;