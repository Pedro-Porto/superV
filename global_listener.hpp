#ifndef GLOBAL_LISTENER_HPP
#define GLOBAL_LISTENER_HPP

#include <functional>
#include <atomic>
#include <string>

void globalListener(std::function<void()> callback, std::atomic<bool>& runningFlag, std::string socketPath);
void stopListener(std::string socketPath);

#endif // GLOBAL_LISTENER_HPP
