#ifndef __HISTORY_MANIPULATOR__H__
#define __HISTORY_MANIPULATOR__H__

#include <gtkmm.h>
#include <list>
#include "data_text.hpp"


class MainWindow; // foward declaration

class HistoryManipulator {
    public:
    HistoryManipulator(MainWindow *window) : window(window) {};
    void add(const std::string text, const std::string type);
    void add(const std::vector<unsigned char> imageData, const std::string type);
    void remove(int index);
    list<Data*> history;
    private:
    MainWindow *window;
};

#endif  // __HISTORY_MANIPULATOR__H__