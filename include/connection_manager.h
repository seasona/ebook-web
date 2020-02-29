#pragma once

#include "connection.h"
#include <set>

namespace Jhttp {

// manage connections to easily start connection and stop connection when the
// server shut down
class ConnectionManager {
public:
    ConnectionManager(const ConnectionManager&) = delete;
    ConnectionManager& operator=(const ConnectionManager&) = delete;

    ConnectionManager() = default;
    ~ConnectionManager() = default;

    void start(ConnectionPtr c);

    void stop(ConnectionPtr c);

    void stopAll();

private:
    std::set<ConnectionPtr> connections_;
};

}  // namespace Jhttp