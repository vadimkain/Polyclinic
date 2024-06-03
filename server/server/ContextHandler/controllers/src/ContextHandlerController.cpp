#include "ContextHandlerController.hpp"

#include "config.hpp"

#include "Logger.hpp"

namespace server::context_handler::controllers {


ContextHandlerController::ContextHandlerController(std::weak_ptr<models::IContextHandlerModel> model,
    std::weak_ptr<view::IContextHandlerInterface> view
)   : m_context_handler_model{model.lock()}
    , m_context_handler_interface{view.lock()}
    , on_open_uri_slot{std::bind(&ContextHandlerController::on_open_uri, this, std::placeholders::_1)}
{
    BDECLARE_TAG_SCOPE("ContextHandlerController", __FUNCTION__);
    BLOG_INFO("contructor called");

    init();
}

void ContextHandlerController::init() {
    common::connect(&m_context_handler_interface->open_uri, &on_open_uri_slot);
}

void ContextHandlerController::on_open_uri(std::string uri) {
    BDECLARE_TAG_SCOPE("ContextHandlerController", __FUNCTION__);
    BLOG_INFO("uri = ", uri);

    m_context_handler_model->set_current_uri(uri);
    m_context_handler_model->set_current_page_file(common::config::DEFAULT_HTML_FILE);

    auto current_page_path = m_context_handler_model->current_uri() + m_context_handler_model->current_page_file();
    
    // m_context_handler_interface->page_address_updated.emit(current_page_path);
}


}   // !server::context_handler::controllers;