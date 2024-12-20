#ifndef __DATA_IMAGE__H__
#define __DATA_IMAGE__H__
#include <gtkmm.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>


#include <chrono>
#include <iostream>
#include <thread>

#include "data.hpp"
/**
 * @brief Image data class
 * 
 * Inherits from Data class
 */
class Image : public Data {
   public:
    Image(std::vector<unsigned char> imageData, std::string type,
          std::function<void()> toggleVisibility,
          std::function<void(int)> removeItem);
    ~Image() override;
    bool paste(GdkEventButton *event);
    void setIndex(int index) override { this->index = index; };
    int getIndex() const override { return this->index; };
    std::vector<unsigned char> *getContent() { return &_imageData; };
    const std::string &getType() const override { return _type; };
    Gtk::EventBox *getEventBox() const override { return event_box; };
    bool operator==(int otherIndex) const { return this->index == otherIndex; }

   private:
    Gtk::EventBox *event_box;
    std::vector<unsigned char> _imageData;
    std::string _type;
    std::function<void()> toggleVisibility;
    std::function<void(int)> removeItem;
    int index;
};

#endif  // __DATA_IMAGE__H__