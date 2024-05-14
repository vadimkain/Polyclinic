#include "ClientHandlerController.hpp"
#include "ClientHandlerModel.hpp"

#include "Logger.hpp"

#include <thread>

namespace server::client_handler::controllers {

ClientHandlerController::ClientHandlerController(std::weak_ptr<serverstarter::models::IServerStarterModel> server_model)
    : m_SERVER_STARTER_MODEL{server_model}
{
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("constructor called on thread #", std::this_thread::get_id());
}

void ClientHandlerController::start(void) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("called");

    catch_new_connection();
}

void ClientHandlerController::catch_new_connection(void) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("called");

    while (m_SERVER_STARTER_MODEL->socket_fd()) {
        BLOG_DEBUG("waiting for connection");
        // handle_connect();
    }
}

void ClientHandlerController::read_data(std::weak_ptr<models::IClientHandlerModel> client) {

}

void ClientHandlerController::send_data(std::weak_ptr<models::IClientHandlerModel> client) {

}

void ClientHandlerController::handle_connect(std::int32_t client_fd) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("called");

    auto client = std::make_shared<models::ClientHandlerModel>();
    client->set_socket_fd(client_fd);
    m_client_handler_model_container.insert(client);

    std::thread(&ClientHandlerController::read_data, this, client).detach();
}

}   // !server::client_handler::controllers;