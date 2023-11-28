#pragma once

#include <string>
#include <deque>

void log(std::string message);
// get deque of log messages by reference
std::deque<std::string>& getLogs();
