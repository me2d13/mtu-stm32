#pragma once

#include <string>
#include <deque>

void log(std::string message);
void logff(char*  message, float val1, float val2);
void logi(char* message, int val1);
// get deque of log messages by reference
std::deque<std::string>& getLogs();
