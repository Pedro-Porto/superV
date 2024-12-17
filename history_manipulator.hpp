#ifndef __HISTORY_MANIPULATOR__H__
#define __HISTORY_MANIPULATOR__H__

#include <gtkmm.h>

#include <list>

#include "data_text.hpp"

class MainWindow;  // foward declaration
class SaveHistory; // foward declaration

class HistoryManipulator {
   public:
    HistoryManipulator(MainWindow *window, SaveHistory *saveHistory);
    void add(const std::string text, const std::string type);
    void add(const std::vector<unsigned char> imageData,
             const std::string type);
    void remove(int index);
    const std::list<Data *> &getHistory() const { return history; };
    HistoryManipulator& operator=(SaveHistory& saveHistory);

   private:
    SaveHistory *saveHistory;
    MainWindow *window;
    list<Data *> history;
    void cropData();
};

#endif  // __HISTORY_MANIPULATOR__H__