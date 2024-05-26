#include "ContextHandlerController.hpp"
#include "Logger.hpp"

namespace server::context_handler::controllers {


ContextHandlerController::ContextHandlerController(std::weak_ptr<models::IContextHandlerModel> model)
    : m_context_handler_model{model.lock()}
{
    BDECLARE_TAG_SCOPE("ContextHandlerController", __FUNCTION__);
    BLOG_INFO("contructor called");
}

void ContextHandlerController::open_default_page_by_uri(const std::string& uri) {
    BDECLARE_TAG_SCOPE("ContextHandlerController", __FUNCTION__);
    BLOG_INFO("uri = ", uri);

    m_context_handler_model->set_current_uri(uri);
}


}   // !server::context_handler::controllers;