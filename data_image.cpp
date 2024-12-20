#include "data_image.hpp"
#include "keyboard_emulator.hpp"

/**
 * @brief Construct a new Image:: Image object
 * 
 * Creates a event box with the image and a delete button to add to the interface
 * @param imageData The image data - vector of unsigned char
 * @param type The image type
 * @param toggleVisibility Function to toggle the visibility of the main window
 * @param removeItem Function to remove the item from the history
 */
Image::Image(std::vector<unsigned char> imageData, std::string type,
             std::function<void()> toggleVisibility,
             std::function<void(int)> removeItem)
    : _imageData(std::move(imageData)),
      _type(type),
      toggleVisibility(std::move(toggleVisibility)),
      removeItem(std::move(removeItem)) {
    event_box = new Gtk::EventBox();
    event_box->set_size_request(200, 100);
    event_box->get_style_context()->add_class("list-row");

    auto box =
        Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_HORIZONTAL);
    box->set_size_request(200, 100);
    box->set_vexpand(false);
    box->get_style_context()->add_class("box-row");

    try {
        auto loader = Gdk::PixbufLoader::create();
        loader->write(_imageData.data(), _imageData.size());
        loader->close();

        auto original_pixbuf = loader->get_pixbuf();

        const int size = 80;
        int crop_width = std::min(original_pixbuf->get_width(),
                                  original_pixbuf->get_height());
        int offset_x = (original_pixbuf->get_width() - crop_width) / 2;
        int offset_y = (original_pixbuf->get_height() - crop_width) / 2;

        auto cropped_pixbuf = Gdk::Pixbuf::create_subpixbuf(
            original_pixbuf, offset_x, offset_y, crop_width, crop_width);
        auto scaled_pixbuf =
            cropped_pixbuf->scale_simple(size, size, Gdk::INTERP_BILINEAR);

        auto image_widget = Gtk::make_managed<Gtk::Image>(scaled_pixbuf);
        image_widget->set_size_request(size, size);

        box->pack_start(*image_widget, Gtk::PACK_SHRINK);
    } catch (const std::exception& e) {
        std::cerr << "Error loading image: " << e.what() << std::endl;
    }

    auto delete_icon = Gtk::make_managed<Gtk::Image>("edit-clear-symbolic");

    auto delete_button = Gtk::make_managed<Gtk::Button>();
    delete_button->set_image(*delete_icon);
    delete_button->set_relief(Gtk::RELIEF_NONE);
    delete_button->set_tooltip_text("Apagar imagem");

    delete_button->signal_clicked().connect(
        [this]() { this->removeItem(this->index); });

    box->pack_end(*delete_button, Gtk::PACK_SHRINK);

    event_box->add(*box);

    event_box->add_events(Gdk::BUTTON_PRESS_MASK);
    event_box->signal_button_press_event().connect(
        sigc::mem_fun(*this, &Image::paste));
}

/**
 * @brief Paste the image to the clipboard
 * 
 * Uses Gtk Clipboard to add the image to the clipboard
 */
bool Image::paste(GdkEventButton*) {
    try {
        auto loader = Gdk::PixbufLoader::create();
        loader->write(_imageData.data(), _imageData.size());
        loader->close();

        auto pixbuf = loader->get_pixbuf();

        auto clipboard = Gtk::Clipboard::get();
        clipboard->set_image(pixbuf);
        clipboard->store();

        toggleVisibility();

        removeItem(index);

        Glib::signal_timeout().connect_once([this]() {
            ctrlV();
        }, 50);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error pasting image: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Destroy the Image:: Image object
 * 
 * Deletes the event box
 */
Image::~Image() {
    if (event_box) {
        delete event_box;
        event_box = nullptr;
    }
}
