#include "log.h"
#include <string>
#include <deque>

#define MAX_LOGS 20
// declare deque of log messages
std::deque<std::string> logs;

void log(std::string message) {
  // add message to deque
  logs.push_front(message);
  // if deque is too long, remove oldest message
  if (logs.size() > MAX_LOGS) {
    logs.pop_back();
  }
}

// get deque of log messages
std::deque<std::string>& getLogs() {
  return logs;
}
