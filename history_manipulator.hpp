#ifndef __HISTORY_MANIPULATOR__H__
#define __HISTORY_MANIPULATOR__H__

#include <gtkmm.h>
#include <list>
#include "data_text.hpp"


class MainWindow; // foward declaration

class HistoryManipulator {
    public:
    HistoryManipulator(MainWindow *window) : window(window) {};
    void add(std::string text);
    void remove(int index);
    list<Text*> history;
    private:
    MainWindow *window;
};

#endif  // __HISTORY_MANIPULATOR__H__