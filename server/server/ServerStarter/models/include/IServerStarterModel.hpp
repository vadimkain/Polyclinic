#ifndef POLYCLINIC_SERVER_SERVER_SERVERSTARTER_MODELS_ISERVERSTARTERMODEL_HPP
#define POLYCLINIC_SERVER_SERVER_SERVERSTARTER_MODELS_ISERVERSTARTERMODEL_HPP

#include <cinttypes>
#include <atomic>

namespace server::serverstarter::models {

class IServerStarterModel {
public:
    virtual std::int32_t socket_fd() const noexcept = 0;
    virtual void set_socket_fd(std::int32_t val) noexcept = 0;
};

}   // !server::serverstarter::models;

#endif  // !POLYCLINIC_SERVER_SERVER_SERVERSTARTER_MODELS_ISERVERSTARTERMODEL_HPP;