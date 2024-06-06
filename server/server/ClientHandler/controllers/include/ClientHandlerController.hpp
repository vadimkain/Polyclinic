#ifndef POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_CONTROLLERS_CLIENTHANDLERCONTROLLER_HPP
#define POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_CONTROLLERS_CLIENTHANDLERCONTROLLER_HPP

#include "IClientHandlerModel.hpp"
#include "IServerStarterModel.hpp"
#include "IContextHandlerInterface.hpp"
#include "IClientHandlerInterface.hpp"

#include "DBQuery.hpp"

#include "HttpHeaders.hpp"
#include "Signal"

#include <set>
#include <memory>

namespace server::client_handler::controllers {

class ClientHandlerController {
public:
    ClientHandlerController(std::weak_ptr<serverstarter::models::IServerStarterModel> server_model,
        std::weak_ptr<view::IClientHandlerInterface> client_handler_interface,
        std::weak_ptr<context_handler::view::IContextHandlerInterface> context_handler_interface
    );
    ~ClientHandlerController(void);

    void start(void);

/*
 * Slots starts;
 */
public:
    common::Slot<std::string, common::Socket> on_page_updated_slot;
    void on_page_updated(std::string address, common::Socket socket);

/*
 * Slots ends;
 */

private:
    void init();

    void catch_new_connection(void);
    void disconnect(std::weak_ptr<models::IClientHandlerModel> weak_client);
    void read_data(std::weak_ptr<models::IClientHandlerModel> weak_client);
    void send_data(std::weak_ptr<models::IClientHandlerModel> weak_client);

    void handle_connect(const common::Socket& client_socket);
    void handle_read(std::weak_ptr<models::IClientHandlerModel> weak_client, std::string&& read_data, std::int32_t bytes_read);
    void handle_http_request(std::weak_ptr<models::IClientHandlerModel> weak_client, const common::HttpHeaders &header);
    void handle_json_post(common::Socket socket, std::string uri, std::string json_pkg);

private:
    std::shared_ptr<context_handler::view::IContextHandlerInterface> m_context_handler_interface;
    std::shared_ptr<view::IClientHandlerInterface> m_client_handler_interface;

    std::shared_ptr<db::DBQuery> m_db_query;

    std::shared_ptr<const serverstarter::models::IServerStarterModel> m_SERVER_STARTER_MODEL;
    std::set<std::shared_ptr<models::IClientHandlerModel>> m_client_handler_model_container;
};

}   // !server::client_handler::controllers;

#endif  // !POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_CONTROLLERS_CLIENTHANDLERCONTROLLER_HPP;