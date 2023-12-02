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

void logff(char* message, float val1, float val2) {
  char buffer[100];
  int int1 = val1*100.0;
  int int2 = val2*100.0;
  sprintf(buffer, "%s %d %d", message, int1, int2);
  // add message to deque
  logs.push_front(buffer);
  // if deque is too long, remove oldest message
  if (logs.size() > MAX_LOGS) {
    logs.pop_back();
  }
}

void logi(char* message, int val1) {
  char buffer[100];
  sprintf(buffer, "%s %d", message, val1);
  // add message to deque
  logs.push_front(buffer);
  // if deque is too long, remove oldest message
  if (logs.size() > MAX_LOGS) {
    logs.pop_back();
  }
}

// get deque of log messages
std::deque<std::string>& getLogs() {
  return logs;
}
