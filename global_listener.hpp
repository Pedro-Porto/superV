#ifndef GLOBAL_LISTENER_HPP
#define GLOBAL_LISTENER_HPP

#include <functional>
#include <atomic>

void globalListener(std::function<void()> callback, std::atomic<bool>& runningFlag);
void stopListener();

#endif // GLOBAL_LISTENER_HPP
