#ifndef DATA_TEXT_HPP
#define DATA_TEXT_HPP
#include <gtkmm.h>
#include <iostream>

#include "data.hpp"

class Text : public Data {
   public:
    Text(std::string text, std::string type, std::function<void()> toggleVisibility, std::function<void(int)> removeItem);
    ~Text() override;
    bool paste(GdkEventButton* event);
    void setIndex(int index) override { this->index = index; };
    int getIndex() const override { return this->index; };
    const std::string& getContent() const { return _text; };
    const std::string& getType() const override { return _type; };
    Gtk::EventBox *getEventBox() const override { return event_box; };
    
   private:
    Gtk::EventBox *event_box;
    std::string _text;
    std::string _type;
    std::function<void()> toggleVisibility;
    std::function<void(int)> removeItem;
    std::string cleanText();
    int index;
    void sendKey(int fd, int keycode, bool press);
    bool wait_for_device(const char* device_path, int timeout_ms);
};

#endif // DATA_TEXT_HPP


