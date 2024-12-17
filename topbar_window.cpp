#include "topbar_window.hpp"

TopBarWindow::TopBarWindow(const std::function<void()>& callback)
    : toggleOverlayCallback(callback) {
    setupIndicator();
}

TopBarWindow::~TopBarWindow() {
    if (indicator) {
        g_object_unref(indicator);
    }
}

void TopBarWindow::setupIndicator() {
    indicator = app_indicator_new(
        "clipboard-overlay", 
        "edit-paste", 
        APP_INDICATOR_CATEGORY_APPLICATION_STATUS
    );

    app_indicator_set_status(indicator, APP_INDICATOR_STATUS_ACTIVE);
    setupMenu();
}

/**
 * @brief Setup the menu for the indicator
 * 
 * Adds the menu items to the indicator on the topbar
 */
void TopBarWindow::setupMenu() {
    auto toggle_overlay_item = Gtk::make_managed<Gtk::MenuItem>("Toggle Overlay");
    toggle_overlay_item->signal_activate().connect([this]() {
        toggleOverlayCallback();
    });
    menu.append(*toggle_overlay_item);
    toggle_overlay_item->show();

    auto quit_item = Gtk::make_managed<Gtk::MenuItem>("Quit");
    quit_item->signal_activate().connect([this]() {
        if (indicator) {
            app_indicator_set_status(indicator, APP_INDICATOR_STATUS_PASSIVE);
            g_object_unref(indicator);
            indicator = nullptr;
        }

        auto app = Gtk::Application::get_default();
        if (app) {
            app->quit();
        }
    });
    menu.append(*quit_item);
    quit_item->show();

    app_indicator_set_menu(indicator, GTK_MENU(menu.gobj()));
}
