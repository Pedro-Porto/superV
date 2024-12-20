#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <cairomm/context.h>
#include <gtkmm.h>

#include <string>
class HistoryManipulator;  // foward declaration

/**
 * @brief Class to create the main window
 * 
 * This class is used to create the main window of the application
 */
class MainWindow : public Gtk::Window {
   public:
    MainWindow();
    void toggleVisibility();
    void updateItems(HistoryManipulator *history);

   protected:
    bool onLostFocus(GdkEventFocus *event);
    Gtk::ScrolledWindow scrolled_window;
    Gtk::Box app_container;
    Gtk::Box box;

};

#endif  // WINDOW_HPP

