#ifndef CLIPBOARD_LISTENER_HPP
#define CLIPBOARD_LISTENER_HPP

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <atomic>
#include <variant>
#include <functional>
#include <string>

class ClipboardListener {
public:
    ClipboardListener(std::function<void(const std::variant<std::string, std::vector<unsigned char>>, const std::string)> callback, std::atomic<bool>& runningFlag);
    ~ClipboardListener();

    void start();

private:
    std::function<void(const std::variant<std::string, std::vector<unsigned char>>, const std::string)> onCopyCallback;
    std::atomic<bool>& runningFlag;

    bool printSelection(Display* display, Window window, const char* bufname, const char* fmtname);
    void watchSelection(Display* display, Window window, const char* bufname);
};

#endif // CLIPBOARD_LISTENER_HPP
