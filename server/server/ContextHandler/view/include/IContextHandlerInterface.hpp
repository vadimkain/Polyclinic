#ifndef POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_VIEW_ICONTEXTHANDLERINTERFACE_HPP
#define POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_VIEW_ICONTEXTHANDLERINTERFACE_HPP

#include <string>

namespace server::context_handler::view {

class IContextHandlerInterface {
public:
    virtual void request_to_open_uri(std::string uri) = 0;
};

}   // !server::context_handler::view;

#endif  // !POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_VIEW_ICONTEXTHANDLERINTERFACE_HPP;