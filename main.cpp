#include <cairomm/context.h>
#include <gtkmm.h>
#include <vector>

class MainApp : public Gtk::Window {
   public:
    MainApp() {
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
            .list-row {
                background-color: lightgray;
                border-radius: 10px;
                color: black;
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

        scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC,
                                   Gtk::POLICY_AUTOMATIC);
        scrolled_window.add(box);

        box.set_orientation(Gtk::ORIENTATION_VERTICAL);
        box.set_spacing(10);

        box.get_style_context()->add_class("box-container");

        for (int i = 0; i < 20; ++i) {
            auto event_box =
                Gtk::make_managed<Gtk::EventBox>();

            event_box->set_size_request(200, 100);
            event_box->get_style_context()->add_class("list-row");

            auto label = Gtk::make_managed<Gtk::Label>(std::to_string(i + 1));
            event_box->add(*label);

            box.pack_start(*event_box, Gtk::PACK_SHRINK);
        }
        app_container.pack_start(scrolled_window, Gtk::PACK_EXPAND_WIDGET);
        add(app_container);
        
        show_all_children();
    }

   protected:
    Gtk::ScrolledWindow scrolled_window;
    Gtk::Box box;
    Gtk::Box app_container;
    
};

int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create(argc, argv);
    MainApp janela;

    return app->run(janela);
}
