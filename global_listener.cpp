#include "global_listener.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

/**
 * @brief Listens to a socket for a "trigger" message.
 * 
 * When the message is received, it calls the callback function.
 * 
 * @param callback Function to be called when the message is received.
 * @param runningFlag Flag to stop the listener.
 * @param socketPath Path to the socket file.
 */
void globalListener(std::function<void()> callback, std::atomic<bool>& runningFlag, std::string socketPath) {
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation error" << std::endl;
        return;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);
    unlink(socketPath.c_str());

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
        std::cerr << "Socket bind error" << std::endl;
        close(server_fd);
        return;
    }

    if (listen(server_fd, 5) == -1) {
        std::cerr << "Socket listen error" << std::endl;
        close(server_fd);
        unlink(socketPath.c_str());
        return;
    }

    while (runningFlag) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd == -1) {
            if (!runningFlag) break;
            std::cerr << "Socket conn error" << std::endl;
            continue;
        }

        char buffer[256];
        int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            std::string command(buffer);

            command.erase(command.find_last_not_of(" \n\r\t") + 1);

            if (command == "trigger") {
                std::cout << "socket callback" << std::endl;
                callback();
            }
        }

        close(client_fd);
    }

    close(server_fd);
    unlink(socketPath.c_str());
    std::cout << "Global listener shutdown" << std::endl;
}

/**
 * @brief Stops the listener by connecting to the socket.
 * 
 * @param socketPath Path to the socket file.
 */
void stopListener(std::string socketPath) {
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd != -1) {
        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        std::strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);

        connect(client_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        close(client_fd);
    }
}
