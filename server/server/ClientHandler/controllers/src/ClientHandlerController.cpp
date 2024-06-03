#include "ClientHandlerController.hpp"
#include "ClientHandlerModel.hpp"

#include "config.hpp"
#include "EnumStringConvertor.hpp"
#include "HttpHeaders.hpp"
#include "Socket.hpp"
#include "Logger.hpp"

#include <thread>
#include <unordered_map>

#include <sys/stat.h>
#include <unistd.h>
#include <sys/sendfile.h>

namespace server::client_handler::controllers {

ClientHandlerController::ClientHandlerController(std::weak_ptr<serverstarter::models::IServerStarterModel> server_model,
    std::weak_ptr<context_handler::view::IContextHandlerInterface> context_handler_interface
)   : m_SERVER_STARTER_MODEL{server_model}, m_context_handler_interface{context_handler_interface.lock()}
    // , on_page_address_updated_slot{std::bind<void(std::string)>(&ClientHandlerController::on_page_address_updated, this, std::placeholders::_1)}
    , on_page_address_updated_slot{std::bind(&ClientHandlerController::on_page_address_updated, this, std::placeholders::_1)}
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

    init();

    catch_new_connection();
}

void ClientHandlerController::init() {
    common::connect(&m_context_handler_interface->page_address_updated, &on_page_address_updated_slot);

}

void ClientHandlerController::on_page_address_updated(std::string address) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);
    BLOG_INFO("Page address = ", address);
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

    BLOG_INFO("client: ", client->socket().to_string());

    client->set_socket(std::move(client_socket));
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
        // TODO: create signal-slot for disconnect client;
    } else {
        BLOG_ERROR("Error in reading data: ", client->socket().latest_error());
    }
}

void ClientHandlerController::handle_http_request(std::weak_ptr<models::IClientHandlerModel> weak_client, const common::HttpHeaders &header) {
    BDECLARE_TAG_SCOPE("ClientHandlerController", __FUNCTION__);

    auto client = weak_client.lock();

    BLOG_INFO("client: ", client->socket().to_string(), "; http request type = ", common::EnumStringConvertor::init()->to_string(header.method), "; http body = ", header.body);
    // m_context_handler_interface->request_to_open_uri(header.uri);
    
    std::string request = "HTTP/1.1 200 Ok\r\nContent-Type: text/html\r\n\r\n";
    std::string image_path = "./frontend_web/index.html";

    ::write(client->socket().m_socket_fd, request.c_str(), strlen(request.c_str()));

    int fdimg = open(image_path.c_str(), O_RDONLY);
    
    if(fdimg < 0){
        printf("Cannot Open file path : %s with error %d\n", image_path, fdimg); 
    }

    struct stat stat_buf;  /* hold information about input file */
     
    fstat(fdimg, &stat_buf);
    int img_total_size = stat_buf.st_size;
    int block_size = stat_buf.st_blksize;
    //printf("image block size: %d\n", stat_buf.st_blksize);  
    //printf("image total byte st_size: %d\n", stat_buf.st_size);
    if(fdimg >= 0){
        ssize_t sent_size;

        while(img_total_size > 0){
            //if(img_total_size < block_size){
             //   sent_size = sendfile(fd, fdimg, NULL, img_total_size);
            //}
            //else{
            //    sent_size = sendfile(fd, fdimg, NULL, block_size);
            //}          
            //img_total_size = img_total_size - sent_size;
        
            //if(sent_size < 0){
             //   printf("send file error --> file: %d, send size: %d , error: %s\n", fdimg, sent_size, strerror(errno));
             //   img_total_size = -1;
              int send_bytes = ((img_total_size < block_size) ? img_total_size : block_size);
              int done_bytes = ::sendfile(client->socket().m_socket_fd, fdimg, NULL, block_size);
              img_total_size = img_total_size - done_bytes;
            //}
        }
        if(sent_size >= 0){
            BLOG_INFO("Send file: ", image_path);
            // printf("send file: %s \n" , image_path);
        }
        close(fdimg);
    }
}

}   // !server::client_handler::controllers;