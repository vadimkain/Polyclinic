#include "ContextHandlerController.hpp"
#include "config.hpp"

#include "Logger.hpp"

#include <sys/stat.h>
#include <unistd.h>
#include <sys/sendfile.h>

namespace server::context_handler::controllers {


ContextHandlerController::ContextHandlerController(std::weak_ptr<models::IContextHandlerModel> model,
    std::weak_ptr<view::IContextHandlerInterface> view
)   : m_context_handler_model{model.lock()}
    , m_context_handler_interface{view.lock()}
    , on_open_uri_slot{std::bind(&ContextHandlerController::on_open_uri, this, std::placeholders::_1, std::placeholders::_2)}
{
    BDECLARE_TAG_SCOPE("ContextHandlerController", __FUNCTION__);
    BLOG_INFO("contructor called");

    init();
}

void ContextHandlerController::init() {
    common::connect(&m_context_handler_interface->open_uri, &on_open_uri_slot);
}

void ContextHandlerController::on_open_uri(std::string uri, common::Socket socket) {
    BDECLARE_TAG_SCOPE("ContextHandlerController", __FUNCTION__);
    BLOG_INFO("uri = ", uri);

    m_context_handler_model->set_current_uri(uri);
    
    std::string request = "HTTP/1.1 200 Ok\r\nContent-Type: text/html\r\n\r\n";
    std::string image_path = "./frontend_web/index.html";

    int fdimg = open(image_path.c_str(), O_RDONLY);

    ::write(socket.m_socket_fd, request.c_str(), strlen(request.c_str()));

    // m_context_handler_model->set_current_page_file(common::config::DEFAULT_HTML_FILE);

    auto current_page_path = m_context_handler_model->current_uri() + m_context_handler_model->current_page_file();

    if(fdimg < 0){
        BLOG_INFO("Cannot open file path: ", image_path, ". Error: ", fdimg);
        printf("Cannot Open file path : %s with error %d\n", image_path, fdimg); 
    }

    struct stat stat_buf;  /* hold information about input file */
     
    fstat(fdimg, &stat_buf);
    int img_total_size = stat_buf.st_size;
    int block_size = stat_buf.st_blksize;
    if(fdimg >= 0){
        ssize_t sent_size;

        while(img_total_size > 0){
              int send_bytes = ((img_total_size < block_size) ? img_total_size : block_size);
              int done_bytes = ::sendfile(socket.m_socket_fd, fdimg, NULL, block_size);
              img_total_size = img_total_size - done_bytes;
            //}
        }
        if(sent_size >= 0){
            BLOG_INFO("Send file: ", image_path);
            // printf("send file: %s \n" , image_path);
        }
        close(fdimg);
    }
    
    // m_context_handler_interface->page_address_updated.emit(current_page_path);
}


}   // !server::context_handler::controllers;