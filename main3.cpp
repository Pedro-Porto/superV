#include "clipboard_listener.hpp"
#include <atomic>
#include <iostream>
#include <thread>

std::atomic<bool> keepRunning(true);

int main() {
    auto onCopy = [](const std::string& content) {
        std::cout << "Clipboard atualizado: " << content << std::endl;
    };

    ClipboardListener listener(onCopy, keepRunning);

    std::thread clipboardThread([&]() {
        listener.start();
    });

    std::cout << "Pressione Enter para encerrar o programa..." << std::endl;
    std::cin.get();

    keepRunning = false;

    if (clipboardThread.joinable()) {
        clipboardThread.join();
    }

    return 0;
}
