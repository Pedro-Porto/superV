#include "data_text.hpp"
#include "keyboard_emulator.hpp"

/**
 * @brief Construct a new Text:: Text object
 * 
 * Creates a event box with the text and a delete button to add to the interface
 * @param text The text to be displayed
 * @param type The text type
 * @param toggleVisibility Function to toggle the visibility of the main window
 * @param removeItem Function to remove the item from the history
 */
Text::Text(std::string text, std::string type, std::function<void()> toggleVisibility, std::function<void(int)> removeItem)
    : _text(text), _type(type), toggleVisibility(toggleVisibility), removeItem(removeItem) {

    event_box = new Gtk::EventBox();
    event_box->set_size_request(200, 100);
    event_box->get_style_context()->add_class("list-row");

    auto box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::ORIENTATION_HORIZONTAL);
    box->set_size_request(200, 100);
    box->set_vexpand(false);
    box->get_style_context()->add_class("box-row");

    std::string textToShow = cleanText();
    auto label = Gtk::make_managed<Gtk::Label>(textToShow);
    label->set_line_wrap(true);
    label->set_line_wrap_mode(Pango::WrapMode::WRAP_CHAR);
    label->set_max_width_chars(30);
    label->set_xalign(0.0);
    label->set_ellipsize(Pango::ELLIPSIZE_END);
    label->set_lines(3);

    auto delete_icon = Gtk::make_managed<Gtk::Image>("edit-clear-symbolic");

    auto delete_button = Gtk::make_managed<Gtk::Button>();
    delete_button->set_image(*delete_icon);
    delete_button->set_relief(Gtk::RELIEF_NONE);
    delete_button->set_tooltip_text("Apagar texto");

    delete_button->signal_clicked().connect([this]() {
        this->removeItem(this->index);
    });

    box->pack_start(*label, Gtk::PACK_EXPAND_WIDGET);
    box->pack_end(*delete_button, Gtk::PACK_SHRINK);

    event_box->add(*box);

    event_box->add_events(Gdk::BUTTON_PRESS_MASK);
    event_box->signal_button_press_event().connect(
        sigc::mem_fun(*this, &Text::paste)
    );
}

/**
 * @brief Cleans the text from new lines
 * 
 * @return std::string The cleaned text
 */
std::string Text::cleanText() {
    std::string cleanText = _text;
    cleanText.erase(std::remove(cleanText.begin(), cleanText.end(), '\n'), cleanText.end());
    return cleanText;
}

bool Text::paste(GdkEventButton*) {
    cout << "Colando: " << _text << endl;
    auto clipboard = Gtk::Clipboard::get();
    clipboard->set_text(_text);
    clipboard->store();

    toggleVisibility();
    removeItem(index);

    Glib::signal_timeout().connect_once([this]() {
        ctrlV();
    }, 100);
    return true;
}

/**
 * @brief Destroy the Text:: Text object
 * 
 */
Text::~Text() {
    if (event_box) {
        delete event_box;
        event_box = nullptr;
    }
}
