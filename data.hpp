#ifndef DATA_HPP
#define DATA_HPP

#include <string>
#include <gtkmm.h>

using namespace std;

/**
 * @brief Abstract class for data
 * 
 * This class is used to create a common interface for the data classes Text and Image
 */
class Data {  // abstract class
   public:
    virtual bool paste(GdkEventButton* event) = 0;
    virtual ~Data() = default;
    virtual void setIndex(int index) = 0;
    virtual int getIndex() const = 0;
    virtual const std::string& getType() const = 0;
    virtual Gtk::EventBox *getEventBox() const = 0;
    virtual bool operator==(int index) const = 0;
};

#endif // DATA_HPP