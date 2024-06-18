#ifndef POLYCLINIC_SERVER_DB_DBQUERY_HPP
#define POLYCLINIC_SERVER_DB_DBQUERY_HPP

#include <pqxx/connection>
#include <pqxx/transaction>

#include <json/json.h>

#include <string>
#include <vector>
#include <ctime>
#include <memory>

namespace server::db {

struct UserInfo {
    std::uint64_t id;
    std::string role;
    std::uint16_t role_id;
    std::string name;
    std::string surname;
    std::string middle_name;
    std::string email;
    std::vector<std::string> phone_numbers;
    std::string password;
};

struct BookedDoctorInfo {
    std::uint64_t id;
    std::uint64_t user_id;
    std::string doc_name;
    std::string doc_surname;
    std::string doc_middle_name;
    std::time_t booked_time;
    bool is_compited;

    Json::Value to_json() const {
        Json::Value jsonData;
        jsonData["user_id"] = user_id;
        jsonData["book_id"] = id;
        jsonData["doc_name"] = doc_name;
        jsonData["doc_surname"] = doc_surname;
        jsonData["doc_middle_name"] = doc_middle_name;
        jsonData["booked_time"] = booked_time;
        jsonData["is_compited"] = is_compited;

        return jsonData;
    }
};

struct DoctorAppointmentInfo {
    std::uint64_t booked_id;
    std::uint64_t id;
    std::uint64_t user_id;
    std::time_t appointment_time;
    std::string doc_name;
    std::string doc_surname;
    std::string doc_middle_name;
    std::string complaint;

    Json::Value to_json() const {
        Json::Value jsonData;
        jsonData["user_id"] = user_id;
        jsonData["book_id"] = booked_id;
        jsonData["appointment_id"] = id;
        jsonData["doc_name"] = doc_name;
        jsonData["doc_surname"] = doc_surname;
        jsonData["doc_middle_name"] = doc_middle_name;
        jsonData["appointment_time"] = appointment_time;
        jsonData["complaint"] = complaint;

        return jsonData;
    }
};

struct DescriptionedDrugInfo {
    std::uint64_t appointment_id;
    std::uint64_t user_id;
    std::string doc_name;
    std::string doc_surname;
    std::string doc_middle_name;
    std::string drug_name;

    Json::Value to_json() const {
        Json::Value jsonData;
        jsonData["user_id"] = user_id;
        jsonData["appointment_id"] = appointment_id;
        jsonData["doc_name"] = doc_name;
        jsonData["doc_surname"] = doc_surname;
        jsonData["doc_middle_name"] = doc_middle_name;
        jsonData["drug_name"] = drug_name;

        return jsonData;
    }
};

struct AnalyseAppointmentInfo {
    std::uint64_t appointment_id;
    std::uint64_t user_id;
    std::string doc_name;
    std::string doc_surname;
    std::string doc_middle_name;
    std::string analyse_name;
    bool is_completed;

    Json::Value to_json() const {
        Json::Value jsonData;
        jsonData["user_id"] = user_id;
        jsonData["appointment_id"] = appointment_id;
        jsonData["doc_name"] = doc_name;
        jsonData["doc_surname"] = doc_surname;
        jsonData["doc_middle_name"] = doc_middle_name;
        jsonData["is_completed"] = is_completed;
        jsonData["analyse_name"] = analyse_name;

        return jsonData;
    }
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
    UserInfo get_user_info_by_id(std::uint64_t id);

    BookedDoctorInfo get_last_booked_doctor(std::uint64_t user_id);
    DoctorAppointmentInfo get_last_appointment(std::uint64_t user_id);
    DescriptionedDrugInfo get_last_descriptioned_drug(std::uint64_t user_id);
    AnalyseAppointmentInfo get_last_analyse_appointment(std::uint64_t user_id);

    std::pair<bool, std::string> register_new_user(const UserInfo& info);

private:
    void string_to_time(const std::string& time_str, std::time_t& time);

private:
    std::unique_ptr<pqxx::connection> m_db_connection;
};

}   // !server::db;

#endif  // !POLYCLINIC_SERVER_DB_DBQUERY_HPP;