#include "ClientHandlerController.hpp"
#include "ClientHandlerModel.hpp"

#include "config.hpp"
#include "EnumStringConvertor.hpp"
#include "HttpHeaders.hpp"
#include "Socket.hpp"
#include "Logger.hpp"

#include <json/json.h>

#include <thread>
#include <unordered_map>

#include <sys/stat.h>
#include <unistd.h>
#include <sys/sendfile.h>

namespace server::client_handler::controllers {

ClientHandlerController::ClientHandlerController(std::weak_ptr<serverstarter::models::IServerStarterModel> server_model,
    std::weak_ptr<view::IClientHandlerInterface> client_handler_interface,
    std::weak_ptr<context_handler::view::IContextHandlerInterface> context_handler_interface
)   : m_SERVER_STARTER_MODEL{server_model}, m_client_handler_interface{client_handler_interface.lock()}
    , m_context_handler_interface{context_handler_interface.lock()}
    , on_page_updated_slot{std::bind(&ClientHandlerController::on_page_updated, this, std::placeholders::_1, std::placeholders::_2 )}
{
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("constructor called on thread #", std::this_thread::get_id());

    m_db_query = std::make_shared<db::DBQuery>();
}

ClientHandlerController::~ClientHandlerController(void) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("destructor called on thread #", std::this_thread::get_id());
    for (auto &client : m_client_handler_model_container) {
        client->socket().close();

        if (!client->socket().is_valid()) {
            BLOG_DEBUG("client = ", client->socket().to_string(), " is closed");
        } else {
            BLOG_WARNING("Impossible to close the client = ", client->socket().to_string(), ". ", client->socket().latest_error());
        }
    }
}

void ClientHandlerController::start(void) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("called");

    init();

    catch_new_connection();
}

void ClientHandlerController::init() {
    common::connect(&m_context_handler_interface->page_updated, &on_page_updated_slot);

}

void ClientHandlerController::on_page_updated(std::string address, common::Socket socket) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("Page address = ", address);

}

void ClientHandlerController::catch_new_connection(void) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("called");

    auto server_socket = m_SERVER_STARTER_MODEL->socket();
    while (server_socket.is_valid()) {
        BLOG_DEBUG("waiting for connection");

        auto client_socket = server_socket.accept();
        if (!client_socket.is_valid()) {
            BLOG_ERROR("impossible to connect to client: ", client_socket.latest_error());
            continue;
        }

        handle_connect(client_socket);
    }
}

void ClientHandlerController::disconnect(std::weak_ptr<models::IClientHandlerModel> weak_client) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = weak_client.lock();

    BLOG_INFO("client: ", client->socket().to_string());

    m_client_handler_interface->on_client_disconnected.emit(client->socket());

    m_client_handler_model_container.erase(client);
    
    if (client->socket().close()) {
        BLOG_ERROR("Cannot to close the client!");
    }
}

void ClientHandlerController::read_data(std::weak_ptr<models::IClientHandlerModel> weak_client) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = weak_client.lock();

    BLOG_INFO("client: ", client->socket().to_string());

    while (client->socket().is_valid()) {
        BLOG_DEBUG("Reading client: ", client->socket().to_string());
        std::string read_buf;
        auto bytes_read = client->socket().read(read_buf, common::config::BUFFER_SIZE);
        handle_read(client, std::move(read_buf), bytes_read);
    }
}

void ClientHandlerController::send_data(std::weak_ptr<models::IClientHandlerModel> weak_client) {

}

void ClientHandlerController::handle_connect(const common::Socket& client_socket) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = std::make_shared<models::ClientHandlerModel>();
    client->set_socket(std::move(client_socket));

    BLOG_INFO("client: ", client->socket().to_string());

    m_client_handler_model_container.insert(client);

    std::thread(&ClientHandlerController::read_data, this, client).detach();
}

void ClientHandlerController::handle_read(std::weak_ptr<models::IClientHandlerModel> weak_client, std::string&& read_data, std::int32_t bytes_read) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = weak_client.lock();

    BLOG_INFO("client: ", client->socket().to_string(), "bytes_read = ", bytes_read, "; data = ", read_data);

    if (bytes_read > 0) {
        common::HttpHeaders http_headers(read_data);
        handle_http_request(client, http_headers);
    } else if (bytes_read == 0) {
        BLOG_INFO("Disconnect client", client->socket().to_string());
        disconnect(client);
        // TODO: create signal-slot for disconnect client;
    } else {
        BLOG_ERROR("Error in reading data: ", client->socket().latest_error());
    }
}

void ClientHandlerController::handle_http_request(std::weak_ptr<models::IClientHandlerModel> weak_client, const common::HttpHeaders &header) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = weak_client.lock();

    BLOG_INFO("client: ", client->socket().to_string(), "; http request type = ", common::EnumStringConvertor::init()->to_string(header.method), "; http body = ", header.body);

    switch (header.method) {
    case common::HttpHeaders::HttpRequestType::GET:
        m_context_handler_interface->request_to_open_uri(header.uri, client->socket());
        break;
    case common::HttpHeaders::HttpRequestType::POST:
        BLOG_WARNING("Tmp logic for the POST request");
        std::thread(&ClientHandlerController::handle_json_post, this, client->socket(), header.uri, header.body).detach();
        break;
    default:
        BLOG_WARNING("HTTP request method \"", common::EnumStringConvertor::init()->to_string(header.method), "\" is not handledsss");
        break;
    }
    
}

void ClientHandlerController::handle_json_post(common::Socket socket, std::string uri, std::string json_pkg) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("Socket = ", socket.to_string(), "; uri = ", uri, " json content = ", json_pkg);

    Json::Value json_data;
    Json::Value json_response;
    Json::CharReaderBuilder json_reader;
    std::istringstream json_stream(json_pkg);
    std::string err;

    common::HttpHeaders http_header;
    std::stringstream request;

    if (!Json::parseFromStream(json_reader, json_stream, &json_data, &err)) {
        BLOG_ERROR("Impossible to read json data: ", err);
        return;
    }
    try {
        if (uri == "/api/login") {
        BLOG_DEBUG("email = ", json_data["email"].asString(), "; password = ", json_data["password"].asString());

        auto login_res = m_db_query->check_signin_is_valid(json_data["email"].asString(), json_data["password"].asString());
        json_response["is_login_success"] = login_res.first;
        json_response["token"] = login_res.second.id;
        Json::Value phone_numbers(Json::arrayValue);
        for (const auto& number : login_res.second.phone_numbers) {
            phone_numbers.append(number);
        }
        json_response["phone_numbers"] = phone_numbers;
        request << "HTTP/1.1 200 OK\r\n";
    } else if (uri == "/api/register") {
        db::UserInfo registered_user;
        registered_user.email = json_data["email"].asString();
        registered_user.name = json_data["name"].asString();
        registered_user.surname = json_data["surname"].asString();
        registered_user.middle_name = json_data["middle_name"].asString();
        registered_user.password = json_data["password"].asString();
        registered_user.role_id = json_data["role_id"].asInt();

        auto reg_res = m_db_query->register_new_user(registered_user);
        
        json_response["is_success"] = reg_res.first;
        json_response["error_message"] = reg_res.second;

        if (reg_res.first) {
            registered_user = m_db_query->get_user_info_by_email(registered_user.email);
            json_response["token"] = registered_user.id;
        }

        request << "HTTP/1.1 200 OK\r\n";
    } else if (uri == "/api/user") {
        try {
            std::uint64_t id = json_data["token"].asUInt64();
            auto user_info = m_db_query->get_user_info_by_id(id);
            json_response["token"] = user_info.id;
            json_response["name"] = user_info.name;
            json_response["surname"] = user_info.surname;
            json_response["middle_name"] = user_info.middle_name;
            json_response["email"] = user_info.email;
            json_response["role"] = user_info.role;
            Json::Value phone_numbers(Json::arrayValue);
            for (const auto& number : user_info.phone_numbers) {
                phone_numbers.append(number);
            }
            json_response["phone_numbers"] = phone_numbers;

            auto last_booked_doc_info = m_db_query->get_last_booked_doctor(id);
            json_response["latest_booked_doctor"] = last_booked_doc_info.to_json();

            auto last_analyse_appointment_info = m_db_query->get_last_analyse_appointment(id);
            json_response["last_analyse_appointment_info"] = last_analyse_appointment_info.to_json();

            auto last_appointment_info = m_db_query->get_last_appointment(id);
            json_response["last_appointment_info"] = last_appointment_info.to_json();

            auto last_descriptioned_drug_info = m_db_query->get_last_descriptioned_drug(id);
            json_response["last_descriptioned_drug_info"] = last_descriptioned_drug_info.to_json();

            request << "HTTP/1.1 200 OK\r\n";
        } catch (std::exception& err) {
            BLOG_ERROR("Cannot to get info with token: ", json_data["token"].asString(), ". Error: ", err.what());
        }
    } else if (uri == "/api/booked_doctors") {
        std::uint64_t id = json_data["token"].asUInt64();
        auto db_response = m_db_query->get_booked_doctors(id);

        Json::Value json_db_response(Json::arrayValue);
        json_db_response.resize(db_response.size());
        std::transform(std::begin(db_response), std::end(db_response), std::begin(json_db_response), [](const db::BookedDoctorInfo& it){
            return it.to_json();
        });

        json_response["bookedDoctorsList"] = json_db_response;
        request << "HTTP/1.1 200 OK\r\n";

    } else if (uri == "/api/appointments") {
        std::uint64_t id = json_data["token"].asUInt64();
        auto db_response = m_db_query->get_appointments(id);

        Json::Value json_db_response(Json::arrayValue);
        json_db_response.resize(db_response.size());
        std::transform(std::begin(db_response), std::end(db_response), std::begin(json_db_response), [](const db::DoctorAppointmentInfo& it){
            return it.to_json();
        });

        json_response["appointmentsList"] = json_db_response;
        request << "HTTP/1.1 200 OK\r\n";

    } else if (uri == "/api/analyses") {
        std::uint64_t id = json_data["token"].asUInt64();
        auto db_response = m_db_query->get_analyse_appointments(id);

        Json::Value json_db_response(Json::arrayValue);
        json_db_response.resize(db_response.size());
        std::transform(std::begin(db_response), std::end(db_response), std::begin(json_db_response), [](const db::AnalyseAppointmentInfo& it){
            return it.to_json();
        });

        json_response["analysesList"] = json_db_response;
        request << "HTTP/1.1 200 OK\r\n";

    } else if (uri == "/api/allergens") {
        std::uint64_t id = json_data["token"].asUInt64();
        BLOG_WARNING("NO LOGIC IMPLEMENTED");
        // auto db_response = m_db_query->get_db_response(id);

        // Json::Value json_db_response(Json::arrayValue);
        // json_db_response.resize(db_response.size());
        // std::transform(std::begin(db_response), std::end(db_response), std::begin(json_db_response), [](const db::BookedDoctorInfo& it){
        //     return it.to_json();
        // });

        // json_response["allergensList"] = json_db_response;
        // request << "HTTP/1.1 200 OK\r\n";

    } else if (uri == "/api/prescription_drugs") {
        std::uint64_t id = json_data["token"].asUInt64();
        auto db_response = m_db_query->get_descriptioned_drugs(id);

        Json::Value json_db_response(Json::arrayValue);
        json_db_response.resize(db_response.size());
        std::transform(std::begin(db_response), std::end(db_response), std::begin(json_db_response), [](const db::DescriptionedDrugInfo& it){
            return it.to_json();
        });

        json_response["prescriptionDrugsList"] = json_db_response;
        request << "HTTP/1.1 200 OK\r\n";request << "HTTP/1.1 200 OK\r\n";

    } else {
        BLOG_WARNING("uri = ", uri, "; is not handled!");
    }
    } catch(...) {
        BLOG_ERROR("ПОШЁЛ НАХЕР, ПИДОРАС");
    }
    

    request << http_header.extension_to_content_type("json");
    auto json_response_str = std::move(json_response.toStyledString());

    request << "Content-Length: " << json_response_str.size() << "\r\n";
    request << "Connection: close\r\n\r\n";
    request << json_response_str;

    socket.send(request.str());

    BLOG_DEBUG("Send http response : ", request.str());
}

}   // !server::client_handler::controllers;