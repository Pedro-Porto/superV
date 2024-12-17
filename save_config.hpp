#include <string>

class SaveConfig {
   public:
    SaveConfig(std::string filename);
    void save(std::string name, std::string value);
    std::string load(std::string name);

   private:
    std::string _filename;
};