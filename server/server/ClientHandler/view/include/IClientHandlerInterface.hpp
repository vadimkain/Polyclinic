#ifndef POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_VIEW_ICLIENTHANDLERINTERFACE_HPP
#define POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_VIEW_ICLIENTHANDLERINTERFACE_HPP

#include "Signal"
#include "Socket.hpp"

namespace server::client_handler::view {

class IClientHandlerInterface {
public:
    common::Signal<common::Socket> on_client_disconnected;
};

}   // !server::client_handler::view;

#endif  // !POLYCLINIC_SERVER_SERVER_CLIENTHANDLER_VIEW_ICLIENTHANDLERINTERFACE_HPP;