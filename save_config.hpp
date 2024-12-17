#include <string>

/**
 * @brief Class to save and load configuration values
 * 
 * This class is used to save and load configuration values to a file
 */
class SaveConfig {
   public:
    SaveConfig(std::string filename);
    void save(std::string name, std::string value);
    std::string load(std::string name);

   private:
    std::string _filename;
};