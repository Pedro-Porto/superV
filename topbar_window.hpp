#ifndef TOPBARWINDOW_HPP
#define TOPBARWINDOW_HPP

#include <gtkmm.h>
#include <libayatana-appindicator/app-indicator.h>
#include <functional>

/**
 * @brief Class to create a topbar window
 * 
 * This class is used to create a topbar window with an indicator that can be used to control the application
 */
class TopBarWindow {
public:
    TopBarWindow(const std::function<void()>& callback);
    ~TopBarWindow();

private:
    void setupIndicator();
    void setupMenu();

    AppIndicator* indicator;
    Gtk::Menu menu;
    std::function<void()> toggleOverlayCallback;
};

#endif // TOPBARWINDOW_HPP