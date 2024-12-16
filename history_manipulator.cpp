#include "history_manipulator.hpp"
#include "window.hpp"
#include "data_text.hpp"
#include "data_image.hpp"


void HistoryManipulator::add(const std::string text, const std::string type) {
    cout << "Clipboard: (" << type << ") " << text << endl;
    auto text_data = new Text(text, type, [this]()->void {
        window->toggleVisibility();
    }, [this](int index)->void {
        remove(index);
    });
    text_data->setIndex(history.size());
    history.push_back(text_data);
    std::cout << history.back()->event_box->get_height() << endl;
    
    window->updateItems(this);
}

void HistoryManipulator::add(const std::vector<unsigned char> imageData, const std::string type) {
    std::cout << "Clipboard: (" << type << ") " << imageData.size() << " bytes" << std::endl;
    auto image_data = new Image(imageData, type, [this]()->void {
        window->toggleVisibility();
    }, [this](int index)->void {
        remove(index);
    });
    image_data->setIndex(history.size());
    history.push_back(image_data);
    window->updateItems(this);
}

void HistoryManipulator::remove(int index) {
    if (index < 0 || index >= history.size()) {
        return;
    }

    auto it = std::remove_if(history.begin(), history.end(),
        [index](auto* item) {
            if (item->getIndex() == index) {
                delete item;
                return true;
            }
            return false;
        });

    history.erase(it, history.end());

    for (auto* item : history) {
        if (item->getIndex() > index) {
            item->setIndex(item->getIndex() - 1);
        }
    }

    window->updateItems(this);
}
