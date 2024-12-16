#include "data_image.hpp"


Image::Image(std::vector<unsigned char> imageData, std::function<void()> toggleVisibility, std::function<void(int)> removeItem)
    : _imageData(imageData), toggleVisibility(toggleVisibility), removeItem(removeItem) {

    event_box = Gtk::make_managed<Gtk::EventBox>();
    event_box->set_size_request(200, 100);
    event_box->get_style_context()->add_class("list-row");

    auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_HORIZONTAL);
    box->set_size_request(200, 100);
    box->set_vexpand(false);
    box->get_style_context()->add_class("box-row");

    // auto image = Gdk::Pixbuf::create_from_data(_imageData.data(), Gdk::COLORSPACE_RGB, false, 8, 200, 100, 200 * 3);

    // auto image_widget = Gtk::make_managed<Gtk::Image>(image);
    // image_widget->set_size_request(200, 100);

    auto delete_icon = Gtk::make_managed<Gtk::Image>("edit-clear-symbolic");

    auto delete_button = Gtk::make_managed<Gtk::Button>();
    delete_button->set_image(*delete_icon);
    delete_button->set_relief(Gtk::RELIEF_NONE);
    delete_button->set_tooltip_text("Apagar imagem");

    delete_button->signal_clicked().connect([this]() {
        this->removeItem(this->index);
    });

    // box->pack_start(*image_widget, Gtk::PACK_EXPAND_WIDGET);
    box->pack_end(*delete_button, Gtk::PACK_SHRINK);

    event_box->add(*box);

    event_box->add_events(Gdk::BUTTON_PRESS_MASK);
    event_box->signal_button_press_event().connect(
        sigc::mem_fun(*this, &Image::paste)
    );
}

bool Image::paste(GdkEventButton*) {
    cout << "Colando imagem" << endl;
    auto clipboard = Gtk::Clipboard::get();
    clipboard->set_image(Gdk::Pixbuf::create_from_data(_imageData.data(), Gdk::COLORSPACE_RGB, false, 8, 200, 100, 200 * 3));
    clipboard->store();

    toggleVisibility();
    return true;
}


