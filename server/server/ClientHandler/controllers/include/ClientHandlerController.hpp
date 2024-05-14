#ifndef POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_CONTROLLERS_CLIENTHANDLERCONTROLLER_HPP
#define POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_CONTROLLERS_CLIENTHANDLERCONTROLLER_HPP

#include "IClientHandlerModel.hpp"
#include "IServerStarterModel.hpp"

#include <set>
#include <memory>

namespace server::client_handler::controllers {

class ClientHandlerController {
public:
    ClientHandlerController(std::weak_ptr<serverstarter::models::IServerStarterModel> server_model);
    void start(void);

private:
    void catch_new_connection(void);
    void read_data(std::weak_ptr<models::IClientHandlerModel> client);
    void send_data(std::weak_ptr<models::IClientHandlerModel> client);

    void handle_connect(std::int32_t client_fd);

private:
    std::shared_ptr<const serverstarter::models::IServerStarterModel> m_SERVER_STARTER_MODEL;

    std::set<std::shared_ptr<models::IClientHandlerModel>> m_client_handler_model_container;
};

}   // !server::client_handler::controllers;

#endif  // !POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_CONTROLLERS_CLIENTHANDLERCONTROLLER_HPP;