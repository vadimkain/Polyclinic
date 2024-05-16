#include "ClientHandlerController.hpp"
#include "ClientHandlerModel.hpp"

#include "Socket.hpp"

#include "Logger.hpp"

#include <thread>

#include <sys/socket.h>
#include <arpa/inet.h>

namespace server::client_handler::controllers {

ClientHandlerController::ClientHandlerController(std::weak_ptr<serverstarter::models::IServerStarterModel> server_model)
    : m_SERVER_STARTER_MODEL{server_model}
{
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("constructor called on thread #", std::this_thread::get_id());
}

ClientHandlerController::~ClientHandlerController(void) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("destructor called on thread #", std::this_thread::get_id());
    for (auto &client : m_client_handler_model_container) {
        client->socket().close();

        if (!client->socket().is_valid()) {
            BLOG_DEBUG("client with fd ", client->socket().m_socket_fd, " is closed");
        } else {
            BLOG_WARNING("Impossible to close the client with fd ", client->socket().m_socket_fd);
        }
    }
}

void ClientHandlerController::start(void) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("called");

    catch_new_connection();
}

void ClientHandlerController::catch_new_connection(void) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("called");

    auto server_socket = m_SERVER_STARTER_MODEL->socket();
    while (server_socket.m_socket_fd > 0) {
        BLOG_DEBUG("waiting for connection");

        auto client_socket = server_socket.accept();
        if (client_socket.m_socket_fd < 0) {
            BLOG_ERROR("impossible to connect to client: ", client_socket.latest_error());
            continue;
        }

        handle_connect(client_socket);
    }
}

void ClientHandlerController::read_data(std::weak_ptr<models::IClientHandlerModel> client) {

}

void ClientHandlerController::send_data(std::weak_ptr<models::IClientHandlerModel> client) {

}

void ClientHandlerController::handle_connect(const common::Socket& client_socket) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("called");

    auto client = std::make_shared<models::ClientHandlerModel>();

    // inet_ntoa(client_socket.m_sock_address.sin_addr);
    BLOG_INFO("connected client ip: ", inet_ntoa(client_socket.m_sock_address.sin_addr), "; client port: ", client_socket.m_sock_address.sin_port, 
        "; client fd: ", client_socket.m_socket_fd
    );

    client->set_socket(std::move(client_socket));
    m_client_handler_model_container.insert(client);

    std::thread(&ClientHandlerController::read_data, this, client).detach();
}

}   // !server::client_handler::controllers;