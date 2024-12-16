#include "history_manipulator.hpp"
#include "window.hpp"
#include "data_text.hpp"


void HistoryManipulator::add(std::string text) {
    auto text_data = new Text(text, [this]()->void {
        window->toggleVisibility();
    }, [this](int index)->void {
        remove(index);
    });
    text_data->setIndex(history.size());
    history.push_back(text_data);
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
