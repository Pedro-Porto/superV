#include "global_listener.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

#define SOCKET_PATH "/tmp/global_listener_socket"
void globalListener(std::function<void()> callback, std::atomic<bool>& runningFlag) {
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Erro ao criar o socket");
        return;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    unlink(SOCKET_PATH);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == -1) {
        perror("Erro ao associar o socket");
        close(server_fd);
        return;
    }

    if (listen(server_fd, 5) == -1) {
        perror("Erro ao escutar no socket");
        close(server_fd);
        unlink(SOCKET_PATH);
        return;
    }

    std::cout << "Global listener ativo no socket: " << SOCKET_PATH << std::endl;

    while (runningFlag) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd == -1) {
            if (!runningFlag) break;
            perror("Erro ao aceitar conexão no socket");
            continue;
        }

        std::cout << "Conexão recebida no socket!" << std::endl;

        char buffer[256];
        int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            std::string command(buffer);

            command.erase(command.find_last_not_of(" \n\r\t") + 1);

            std::cout << "Mensagem recebida: [" << command << "]" << std::endl;

            if (command == "trigger") {
                std::cout << "Chamando o callback..." << std::endl;
                callback();
            } else {
                std::cerr << "Comando não reconhecido: [" << command << "]" << std::endl;
            }
        } else {
            std::cerr << "Nada lido do cliente ou erro!" << std::endl;
        }

        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    std::cout << "Global listener encerrado." << std::endl;
}

void stopListener() {
    int client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd != -1) {
        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        std::strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

        connect(client_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        close(client_fd);
    }
}