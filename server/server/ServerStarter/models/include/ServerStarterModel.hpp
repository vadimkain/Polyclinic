#ifndef POLYCLINIC_SERVER_SERVER_SERVERSTARTER_MODELS_SERVERSTARTERMODEL_HPP
#define POLYCLINIC_SERVER_SERVER_SERVERSTARTER_MODELS_SERVERSTARTERMODEL_HPP

#include "IServerStarterModel.hpp"

#include <atomic>

namespace server::serverstarter::models {

class ServerStarterModel : IServerStarterModel {
public:
    ServerStarterModel(void);

    std::int32_t socket() const noexcept override;
    void set_socket(std::int32_t val) noexcept override;

private:
    std::atomic<std::int32_t> m_socket_fd;
};

}   // !server::serverstarter::models;

#endif  // !POLYCLINIC_SERVER_SERVER_SERVERSTARTER_MODELS_SERVERSTARTERMODEL_HPP;