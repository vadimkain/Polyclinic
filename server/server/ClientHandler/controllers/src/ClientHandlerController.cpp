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

void ClientHandlerController::start(void) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("called");

    catch_new_connection();
}

void ClientHandlerController::catch_new_connection(void) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("called");

    while (m_SERVER_STARTER_MODEL->socket().m_socket_fd) {
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
    common::Socket client_socket;
    client_socket.m_socket_fd = client_fd;
    socklen_t client_address_len = sizeof(client_socket.m_sock_address);
    getsockname(client_socket.m_socket_fd, reinterpret_cast<sockaddr*>(&client_socket.m_sock_address), &client_address_len);

    // inet_ntoa(client_socket.m_sock_address.sin_addr);
    BLOG_INFO("connected client ip: ", inet_ntoa(client_socket.m_sock_address.sin_addr), "; client port: ", client_socket.m_sock_address.sin_port, 
        "; client fd: ", client_socket.m_socket_fd
    );

    client->set_socket(std::move(client_socket));
    m_client_handler_model_container.insert(client);

    std::thread(&ClientHandlerController::read_data, this, client).detach();
}

}   // !server::client_handler::controllers;