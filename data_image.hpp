#ifndef __DATA_IMAGE__H__
#define __DATA_IMAGE__H__

#include <gtkmm.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <thread>
#include <chrono>

#include <iostream>

#include "data.hpp"

class Image : public Data {
   public:
    Image(std::vector<unsigned char> imageData, std::string type, std::function<void()> toggleVisibility, std::function<void(int)> removeItem);
    ~Image() override;
    bool paste(GdkEventButton* event);
    void setIndex(int index) override { this->index = index; };
    int getIndex() override { return this->index; };
    private:
    std::vector<unsigned char> _imageData;
    std::string _type;
    std::function<void()> toggleVisibility;
    std::function<void(int)> removeItem;
    int index;
};

#endif  // __DATA_IMAGE__H__