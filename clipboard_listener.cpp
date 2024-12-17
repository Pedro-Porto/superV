#include "clipboard_listener.hpp"

#include <X11/Xatom.h>
#include <limits.h>

#include <cassert>
#include <chrono>
#include <cstring>
#include <iostream>
#include <thread>

/**
 * @brief Construct a new Clipboard Listener:: Clipboard Listener object
 * source for implementation: https://stackoverflow.com/questions/8755471/x11-wait-for-and-get-clipboard-text
 */
ClipboardListener::ClipboardListener(
    std::function<
        void(const std::variant<std::string, std::vector<unsigned char>>,
             const std::string)>
        callback,
    std::atomic<bool>& flag)
    : onCopyCallback(callback), runningFlag(flag) {}

ClipboardListener::~ClipboardListener() {}

/**
 * @brief Prints the selection to the callback function
 */
bool ClipboardListener::printSelection(Display* display, Window window,
                                       const char* bufname,
                                       const char* fmtname) {
    char* result;
    unsigned long ressize, restail;
    int resbits;
    Atom bufid = XInternAtom(display, bufname, False),
         fmtid = XInternAtom(display, fmtname, False),
         propid = XInternAtom(display, "XSEL_DATA", False),
         incrid = XInternAtom(display, "INCR", False);
    XEvent event;

    XSelectInput(display, window, PropertyChangeMask);
    XConvertSelection(display, bufid, fmtid, propid, window, CurrentTime);
    do {
        XNextEvent(display, &event);
    } while (event.type != SelectionNotify ||
             event.xselection.selection != bufid);

    if (event.xselection.property) {
        XGetWindowProperty(display, window, propid, 0, LONG_MAX / 4, True,
                           AnyPropertyType, &fmtid, &resbits, &ressize,
                           &restail, (unsigned char**)&result);
        if (fmtid != incrid) {
            std::string type(fmtname);
            if (type == "UTF8_STRING" || type == "STRING") {
                std::string content(result, ressize);
                onCopyCallback(content, type);
            } else {
                std::vector<unsigned char> content(
                    (unsigned char*)result, (unsigned char*)result + ressize);
                onCopyCallback(content, type);
            }
            XFree(result);
        }

        if (fmtid == incrid) {
            do {
                do {
                    XNextEvent(display, &event);
                } while (event.type != PropertyNotify ||
                         event.xproperty.atom != propid ||
                         event.xproperty.state != PropertyNewValue);

                XGetWindowProperty(display, window, propid, 0, LONG_MAX / 4,
                                   True, AnyPropertyType, &fmtid, &resbits,
                                   &ressize, &restail,
                                   (unsigned char**)&result);
                std::string content(result, ressize);
                XFree(result);
                std::string type(fmtname);
                if (type == "UTF8_STRING" || type == "STRING") {
                    std::string content(result, ressize);
                    onCopyCallback(content, type);
                } else {
                    std::vector<unsigned char> content(
                        (unsigned char*)result,
                        (unsigned char*)result + ressize);
                    onCopyCallback(content, type);
                }
            } while (ressize > 0);
        }
        return true;
    }
    return false;
}

/**
 * @brief Watches the selection
 * If there is a change in the selection, it calls printSelection
 * Must run in a separate thread (or it will block the main thread)
 */
void ClipboardListener::watchSelection(Display* display, Window window,
                                       const char* bufname) {
    int event_base, error_base;
    XEvent event;
    Atom bufid = XInternAtom(display, bufname, False);

    assert(XFixesQueryExtension(display, &event_base, &error_base));
    XFixesSelectSelectionInput(display, DefaultRootWindow(display), bufid,
                               XFixesSetSelectionOwnerNotifyMask);

    while (runningFlag) {
        if (XPending(display)) {
            XNextEvent(display, &event);

            if (event.type == event_base + XFixesSelectionNotify &&
                ((XFixesSelectionNotifyEvent*)&event)->selection == bufid) {
                if (!printSelection(display, window, bufname, "image/png") &&
                    !printSelection(display, window, bufname, "image/jpeg") &&
                    !printSelection(display, window, bufname, "UTF8_STRING")) {
                    printSelection(display, window, bufname, "STRING");
                }
            }
        } else {
            // delay to avoid high CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

/**
 * @brief Starts the clipboard listener
 */
void ClipboardListener::start() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "X11 not available" << std::endl;
        return;
    }

    unsigned long color = BlackPixel(display, DefaultScreen(display));
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0,
                                        0, 1, 1, 0, color, color);

    watchSelection(display, window, "CLIPBOARD");

    XDestroyWindow(display, window);
    XCloseDisplay(display);
}
