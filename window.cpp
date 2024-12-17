#include "window.hpp"
#include "history_manipulator.hpp"

MainWindow::MainWindow() {
    set_app_paintable(true);
    auto screen1 = get_screen();
    auto visual = screen1->get_rgba_visual();
    if (visual) {
        gtk_widget_set_visual(Gtk::Widget::gobj(), visual->gobj());
    } else {
        g_warning("RGBA not found");
    }

    auto css_provider = Gtk::CssProvider::create();
    css_provider->load_from_data(R"(
            .box-row {
                padding: 10px;
            }
            .list-row {
                background-color: #404040;
                border-radius: 10px;
                color: white;
                padding: 10px;
            }
            .box-container {
                border-radius: 20px;
                margin: 5px;
                padding: 5px;
            }
            .app-container {
                background-color: #2a2a2a;
                border-radius: 20px;
                padding: 10px;
            }
        )");
    auto screen = Gdk::Display::get_default()->get_default_screen();
    Gtk::StyleContext::add_provider_for_screen(
        screen, css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);

    set_default_size(300, 400);
    set_decorated(false);
    set_position(Gtk::WIN_POS_CENTER);

    app_container.set_orientation(Gtk::ORIENTATION_VERTICAL);
    app_container.get_style_context()->add_class("app-container");

    scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    scrolled_window.add(box);

    box.set_orientation(Gtk::ORIENTATION_VERTICAL);
    box.set_spacing(10);

    box.get_style_context()->add_class("box-container");

    app_container.pack_start(scrolled_window, Gtk::PACK_EXPAND_WIDGET);
    add(app_container);

    signal_focus_out_event().connect(sigc::mem_fun(*this, &MainWindow::onLostFocus), false);

    show_all_children();
    hide();
}

bool MainWindow::onLostFocus(GdkEventFocus*) {
    hide();
    return false;
}

void MainWindow::toggleVisibility() {
    if (is_visible()) {
        hide();
    } else {
        show_all();
    }
}

void MainWindow::updateItems(HistoryManipulator *history) {
    std::cout << "Updating items..." << std::endl;
    box.foreach([&](Gtk::Widget& child) {
        box.remove(child);
    });
    std::cout << "Removed all children" << std::endl;
    for (auto it = history->getHistory().rbegin(); it != history->getHistory().rend(); ++it) { // reverse loop
        box.pack_start(*(*it)->getEventBox(), Gtk::PACK_SHRINK);
    }
    std::cout << "Packed all children" << std::endl;
    show_all_children();
}
