#ifndef DATA_TEXT_HPP
#define DATA_TEXT_HPP
#include <gtkmm.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <thread>
#include <chrono>

#include <iostream>

#include "data.hpp"

class Text : public Data {
   public:
    Text(std::string text, std::string type, std::function<void()> toggleVisibility, std::function<void(int)> removeItem);
    ~Text() override;
    bool paste(GdkEventButton* event);
    void setIndex(int index) override { this->index = index; };
    int getIndex() override { return this->index; };
   private:
    std::string _text;
    std::string _type;
    std::function<void()> toggleVisibility;
    std::function<void(int)> removeItem;
    std::string cleanText();
    int index;
};

#endif // DATA_TEXT_HPP


