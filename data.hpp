#ifndef DATA_HPP
#define DATA_HPP

#include <string>
#include <gtkmm.h>

using namespace std;

class Data {  // abstract class
   public:
    virtual bool paste(GdkEventButton* event) = 0;
    virtual ~Data() = default;
    virtual void setIndex(int index) = 0;
    virtual int getIndex() = 0;
    Gtk::EventBox *event_box;
};

#endif // DATA_HPP