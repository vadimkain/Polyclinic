#include "DBQuery.hpp"
#include "config.hpp"

#include "Logger.hpp"

#include <pqxx/result>

#include <sstream>
#include <thread>

namespace config = server::common::config;

namespace server::db 
{

template<typename T>
T convert_to_datatype(const pqxx::row& row, std::string column_name){
    return row[column_name].is_null() ? T{} : row[column_name].as<T>();
}

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

std::pair<bool, UserInfo> DBQuery::check_signin_is_valid(std::string email, std::string password) {
    BDECLARE_TAG_SCOPE("DBQuery", __FUNCTION__);

    std::stringstream db_request;
    auto db_transaction = std::make_unique<pqxx::work>(*m_db_connection);

    db_request << "SELECT * FROM check_user_login('"
        << email << "', '" << password << "')";

    BLOG_INFO("Current request: ", db_request.str());

    pqxx::result res = db_transaction->exec(db_request.str());
    db_transaction->commit();

    bool first = res.size() > 0 && res[0][0].as<bool>();

    if (first) {
        return {first, get_user_info_by_email(email)};
    }
    
    return {first, {}};
}

UserInfo DBQuery::get_user_info_by_email(const std::string& email) {
    BDECLARE_TAG_SCOPE("DBQuery", __FUNCTION__);

    std::stringstream db_request;
    UserInfo ret;
    auto db_transaction = std::make_unique<pqxx::work>(*m_db_connection);

    db_request << "SELECT * FROM user_details_view WHERE email = '"
        << email << "'";

    BLOG_INFO("Current request: ", db_request.str());
    pqxx::result res = db_transaction->exec(db_request.str());
    db_transaction->commit();

    for (const auto& row : res) {
        ret.id = convert_to_datatype<uint64_t>(row, "id");
        ret.name = convert_to_datatype<std::string>(row, "name");
        ret.surname = convert_to_datatype<std::string>(row, "surname");
        ret.middle_name = convert_to_datatype<std::string>(row, "middle_name");
        ret.email = convert_to_datatype<std::string>(row, "email");
        ret.role = convert_to_datatype<std::string>(row, "role");

        // ret.phone_numbers = row["name"].as<std::string>();
        std::stringstream phone_numbers_ss(convert_to_datatype<std::string>(row, "phone_numbers"));

        std::string phone_number;
        while(std::getline(phone_numbers_ss, phone_number, ',')) {
            ret.phone_numbers.push_back(phone_number);
        };
    }

    return ret;
}

UserInfo DBQuery::get_user_info_by_id(std::uint64_t id) {
    BDECLARE_TAG_SCOPE("DBQuery", __FUNCTION__);

    std::stringstream db_request;
    UserInfo ret;
    auto db_transaction = std::make_unique<pqxx::work>(*m_db_connection);

    db_request << "SELECT * FROM user_details_view WHERE id = '"
        << id << "'";

    BLOG_INFO("Current request: ", db_request.str());
    pqxx::result res = db_transaction->exec(db_request.str());
    db_transaction->commit();

    for (const auto& row : res) {
        ret.id = convert_to_datatype<uint64_t>(row, "id");
        ret.name = convert_to_datatype<std::string>(row, "name");
        ret.surname = convert_to_datatype<std::string>(row, "surname");
        ret.middle_name = convert_to_datatype<std::string>(row, "middle_name");
        ret.email = convert_to_datatype<std::string>(row, "email");
        ret.role = convert_to_datatype<std::string>(row, "role");

        // ret.phone_numbers = row["name"].as<std::string>();
        std::stringstream phone_numbers_ss(convert_to_datatype<std::string>(row, "phone_numbers"));

        std::string phone_number;
        while(std::getline(phone_numbers_ss, phone_number, ',')) {
            ret.phone_numbers.push_back(phone_number);
        };
    }

    return ret;
}

BookedDoctorInfo DBQuery::get_last_booked_doctor(std::uint64_t user_id) {
    BDECLARE_TAG_SCOPE("DBQuery", __FUNCTION__);

    std::stringstream db_request;
    BookedDoctorInfo ret;
    auto db_transaction = std::make_unique<pqxx::work>(*m_db_connection);

    db_request << "SELECT * FROM booked_doctor_view"
        << " WHERE user_id = " << user_id
        << " ORDER BY booked_time DESC"
        << " LIMIT 1;";

    BLOG_INFO("Current request: ", db_request.str());
    pqxx::result res = db_transaction->exec(db_request.str());
    db_transaction->commit();

    if (res.size() > 0) {
        ret.id = convert_to_datatype<std::uint64_t>(res[0], "booked_id");
        ret.user_id = convert_to_datatype<std::uint64_t>(res[0], "user_id");
        ret.doc_name = convert_to_datatype<std::string>(res[0], "doctor_name");
        ret.doc_surname = convert_to_datatype<std::string>(res[0], "doctor_surname");
        ret.doc_middle_name = convert_to_datatype<std::string>(res[0], "doctor_middle_name");
        ret.is_compited = convert_to_datatype<bool>(res[0], "appointment_status");

        string_to_time(convert_to_datatype<std::string>(res[0], "booked_time"), ret.booked_time);
    }

    return ret;
}

DoctorAppointmentInfo DBQuery::get_last_appointment(std::uint64_t user_id) {
    BDECLARE_TAG_SCOPE("DBQuery", __FUNCTION__);

    std::stringstream db_request;
    DoctorAppointmentInfo ret;
    auto db_transaction = std::make_unique<pqxx::work>(*m_db_connection);

    db_request << "SELECT * FROM doctor_appointment_view"
        << " WHERE user_id = " << user_id
        << " ORDER BY appointment_time DESC"
        << " LIMIT 1;";

    BLOG_INFO("Current request: ", db_request.str());
    pqxx::result res = db_transaction->exec(db_request.str());
    db_transaction->commit();

    if (res.size() > 0) {
        ret.id = convert_to_datatype<std::uint64_t>(res[0], "appointment_id");
        ret.user_id = convert_to_datatype<std::uint64_t>(res[0], "user_id");
        ret.booked_id = convert_to_datatype<std::uint64_t>(res[0], "booked_id");
        ret.doc_name = convert_to_datatype<std::string>(res[0], "doctor_name");
        ret.doc_surname = convert_to_datatype<std::string>(res[0], "doctor_surname");
        ret.doc_middle_name = convert_to_datatype<std::string>(res[0], "doctor_middle_name");
        ret.complaint = convert_to_datatype<std::string>(res[0], "complaint");

        string_to_time(convert_to_datatype<std::string>(res[0], "appointment_time"), ret.appointment_time);
    }

    return ret;
}

DescriptionedDrugInfo DBQuery::get_last_descriptioned_drug(std::uint64_t user_id) {
    BDECLARE_TAG_SCOPE("DBQuery", __FUNCTION__);

    std::stringstream db_request;
    DescriptionedDrugInfo ret;
    auto db_transaction = std::make_unique<pqxx::work>(*m_db_connection);

    db_request << "SELECT * FROM descriptioned_drugs_view"
        << " WHERE user_id = " << user_id
        << " ORDER BY appointment_id DESC"
        << " LIMIT 1;";

    BLOG_INFO("Current request: ", db_request.str());
    pqxx::result res = db_transaction->exec(db_request.str());
    db_transaction->commit();

    if (res.size() > 0) {
        ret.appointment_id = convert_to_datatype<std::uint64_t>(res[0], "appointment_id");
        ret.user_id = convert_to_datatype<std::uint64_t>(res[0], "user_id");
        ret.doc_name = convert_to_datatype<std::string>(res[0], "doctor_name");
        ret.doc_surname = convert_to_datatype<std::string>(res[0], "doctor_surname");
        ret.doc_middle_name = convert_to_datatype<std::string>(res[0], "doctor_middle_name");
        ret.drug_name = convert_to_datatype<std::string>(res[0], "drug_name");
    }

    return ret;
}

AnalyseAppointmentInfo DBQuery::get_last_analyse_appointment(std::uint64_t user_id) {
    BDECLARE_TAG_SCOPE("DBQuery", __FUNCTION__);

    std::stringstream db_request;
    AnalyseAppointmentInfo ret;
    auto db_transaction = std::make_unique<pqxx::work>(*m_db_connection);

    db_request << "SELECT * FROM analyse_appointments_view"
        << " WHERE user_id = " << user_id
        << " ORDER BY appointment_id DESC"
        << " LIMIT 1;";

    BLOG_INFO("Current request: ", db_request.str());
    pqxx::result res = db_transaction->exec(db_request.str());
    db_transaction->commit();

    if (res.size() > 0) {
        ret.appointment_id = convert_to_datatype<std::uint64_t>(res[0], "appointment_id");
        ret.user_id = convert_to_datatype<std::uint64_t>(res[0], "user_id");
        ret.doc_name = convert_to_datatype<std::string>(res[0], "doctor_name");
        ret.doc_surname = convert_to_datatype<std::string>(res[0], "doctor_surname");
        ret.doc_middle_name = convert_to_datatype<std::string>(res[0], "doctor_middle_name");
        ret.analyse_name = convert_to_datatype<std::string>(res[0], "analyse_name");
        ret.is_completed = convert_to_datatype<bool>(res[0], "is_completed");
    }

    return ret;
}

std::pair<bool, std::string> DBQuery::register_new_user(const UserInfo& info) {
    BDECLARE_TAG_SCOPE("DBQuery", __FUNCTION__);

    std::stringstream db_request;
    UserInfo ret;
    auto db_transaction = std::make_unique<pqxx::work>(*m_db_connection);

    db_request << "SELECT * FROM add_new_user("
        << "'" << info.name << "',"
        << "'" << info.surname << "',"
        << "'" << info.middle_name << "',"
        << "'" << info.email << "',"
        << "'" << info.password << "',"
        << info.role_id
        << ");";

    BLOG_INFO("Current request: ", db_request.str());
    pqxx::result res = db_transaction->exec(db_request.str());
    db_transaction->commit();

    if (res.size() > 0) {
        return {res[0]["is_success"].as<bool>(), convert_to_datatype<std::string>(res[0], "message")};
    } else {
        return {false, std::string{"Query result is empty"}};
    }
}

void DBQuery::string_to_time(const std::string& time_str, std::time_t& time) {
    std::tm tm = {};
    std::stringstream ss{time_str};
    ss >> std::get_time(&tm, "%Y-%M-%d %H:%M:%S");
    time = std::mktime(&tm);
}

}   // !server::db;