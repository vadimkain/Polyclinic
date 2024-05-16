#ifndef POLYCLINIC_SERVER_SERVER_SERVERSTARTER_MODELS_ISERVERSTARTERMODEL_HPP
#define POLYCLINIC_SERVER_SERVER_SERVERSTARTER_MODELS_ISERVERSTARTERMODEL_HPP

#include "Socket.hpp"

namespace server::serverstarter::models {

class IServerStarterModel {
public:
    virtual common::Socket socket(void) const = 0;
    virtual void set_socket(const common::Socket& val) = 0;
    virtual void set_socket(common::Socket&& val) = 0;
};

}   // !server::serverstarter::models;

#endif  // !POLYCLINIC_SERVER_SERVER_SERVERSTARTER_MODELS_ISERVERSTARTERMODEL_HPP;