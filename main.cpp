#include <gtkmm.h>
#include <libayatana-appindicator/app-indicator.h>
#define X11_NO_MIT_SHM
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <atomic>
#include <iostream>
#include <thread>
#include <mutex>
#include <csignal>
#include "clipboard_listener.hpp"
#include "global_listener.hpp"
#include "history_manipulator.hpp"
#include "topbar_window.hpp"
#include "window.hpp"
#include "save_history.hpp"
#include "save_config.hpp"

std::atomic<bool> keepRunning(true);
Glib::Dispatcher toggleDispatcher;
std::mutex stateMutex;

void signalHandler(int) {
    std::cout << "\nGraceful shutdown..." << std::endl;
    keepRunning = false;
    auto app = Gtk::Application::get_default();
    if (app) {
        app->quit();
    }
}


int main(int argc, char* argv[]) {
    std::signal(SIGINT, signalHandler);

    SaveConfig config("superv.conf");

    bool saveToHistory = config.load("SAVE_HISTORY") == "true";

    if (saveToHistory) {
        std::cout << "Saving history to file" << std::endl;
    } else {
        std::cout << "Not saving history to file" << std::endl;
    }

    auto app = Gtk::Application::create(argc, argv, "org.example.overlay");

    Gtk::Window dummyWindow;
    dummyWindow.set_default_size(1, 1);
    dummyWindow.set_decorated(false);
    dummyWindow.set_opacity(0.0);
    dummyWindow.hide();

    MainWindow mainWindow;

    HistoryManipulator history(&mainWindow);

    SaveHistory saveHistory("history.bin", history);

    if (saveToHistory) {
        saveHistory.load();
    }

    auto toggleOverlay = [&mainWindow]() { mainWindow.toggleVisibility(); };

    auto topBarWindow = std::make_unique<TopBarWindow>(toggleOverlay);



    toggleDispatcher.connect([&]() {
        std::lock_guard<std::mutex> lock(stateMutex);
        mainWindow.toggleVisibility();
    });

    ClipboardListener clipboardListener(
    [&history, &saveToHistory, &saveHistory](const std::variant<std::string, std::vector<unsigned char>> content, const std::string type) {
        if (std::holds_alternative<std::string>(content)) {
            history.add(std::get<std::string>(content), type);
        } else if (std::holds_alternative<std::vector<unsigned char>>(content)) {
            history.add(std::get<std::vector<unsigned char>>(content), type);
        }
        if (saveToHistory) {
            saveHistory.save();
        }
    },
    keepRunning);

    std::thread clipboardThread([&]() {
        while (keepRunning) {
            clipboardListener.start();
        }
    });

    std::thread globalKeyListenerThread(globalListener, [&]() {
        toggleDispatcher.emit();
    }, std::ref(keepRunning));

    app->signal_shutdown().connect([&]() {
        std::cout << "Shutting down application..." << std::endl;

        // stop listeners
        keepRunning = false;

        // trigger x11 event to stop loop
        Display* display = XOpenDisplay(nullptr);
        if (display) {
            Window root = DefaultRootWindow(display);
            Atom clipboard = XInternAtom(display, "CLIPBOARD", False);
            XSetSelectionOwner(display, clipboard, root, CurrentTime);
            XCloseDisplay(display);
        }

        stopListener();

        if (globalKeyListenerThread.joinable()) {
            globalKeyListenerThread.join();
        }

        if (clipboardThread.joinable()) {
            clipboardThread.join();
        }

        topBarWindow.reset();
    });

    return app->run(dummyWindow);
}
