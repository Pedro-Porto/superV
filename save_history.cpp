#include "save_history.hpp"
#include "history_manipulator.hpp"

SaveHistory::SaveHistory(std::string filename)
    : _filename(filename) {}

void SaveHistory::save(HistoryManipulator& history) {
    std::ofstream file(_filename, std::ios::out);

    if (!file) {
        std::cerr << "File error" << std::endl;
        return;
    }

    for (auto& h : history.getHistory()) {
        if (auto text = dynamic_cast<Text*>(h)) {
            file << text->getType().size() << text->getType()
                 << text->getContent().size() << text->getContent();
        } else if (auto image = dynamic_cast<Image*>(h)) {
            file << image->getType().size() << image->getType()
                 << image->getContent()->size();
            file.write(reinterpret_cast<const char*>(image->getContent()->data()), image->getContent()->size());
        }
    }
    file.close();
}

void SaveHistory::load(HistoryManipulator& history) {
    std::ifstream file(_filename, std::ios::in);

    if (!file) {
        std::cerr << "File error" << std::endl;
        return;
    }

    while (file) {
        size_t typeSize, contentSize;
        std::string type;
        std::string content;

        if (!(file >> typeSize)) break;

        type.resize(typeSize);
        file.read(&type[0], typeSize);

        if (!(file >> contentSize)) break;

        content.resize(contentSize);
        file.read(&content[0], contentSize);

        if (type == "UTF8_STRING" || type == "STRING") {
            history.add(content, type);
        } else {
            history.add(
                std::vector<unsigned char>(content.begin(), content.end()),
                type);
        }
    }

    file.close();
}

SaveHistory& SaveHistory::operator=(HistoryManipulator& history) {
    save(history);
    return *this;
}

void SaveHistory::deleteFile() { std::remove(_filename.c_str()); }
