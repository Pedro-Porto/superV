#include <fstream>
#include <iostream>

#include "data.hpp"
#include "data_image.hpp"
#include "data_text.hpp"
#include "history_manipulator.hpp"

class SaveHistory {
   public:
    SaveHistory(std::string filename, HistoryManipulator &history);
    void save();
    void load();
    void deleteFile();

   private:
    std::string _filename;
    HistoryManipulator &_history;
};
