#ifndef POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_VIEW_ICONTEXTHANDLERINTERFACE_HPP
#define POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_VIEW_ICONTEXTHANDLERINTERFACE_HPP

#include "Signal"
#include "Socket.hpp"

#include <string>

namespace server::context_handler::view {

class IContextHandlerInterface {
public:
    virtual void request_to_open_uri(std::string uri, common::Socket socket) = 0;

/*
 * Signals starts;
 */
public:
    common::Signal<std::string, common::Socket> open_uri;
    common::Signal<std::string, common::Socket> page_updated;

/*
 * Signals ends;
 */

};

}   // !server::context_handler::view;

#endif  // !POLYCLINIC_SERVER_SERVER_CONTEXTHANDLER_VIEW_ICONTEXTHANDLERINTERFACE_HPP;