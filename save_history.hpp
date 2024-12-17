#include <fstream>
#include <iostream>

#include "data.hpp"
#include "data_image.hpp"
#include "data_text.hpp"

class HistoryManipulator; // foward declaration

/**
 * @brief Class to save and load history
 * 
 * This class is used to save and load history to a file
 */
class SaveHistory {
   public:
    SaveHistory(std::string filename);
    void deleteFile();
    SaveHistory& operator=(HistoryManipulator& history);
    void save(HistoryManipulator& history);
    void load(HistoryManipulator& history);

   private:
    std::string _filename;
};
