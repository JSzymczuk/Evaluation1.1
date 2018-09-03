#include "Logger.h"

std::vector<String> Logger::_logs = std::vector<String>();

void Logger::log(String text) { _logs.push_back(text); }

void Logger::printLogs() { for (String s : _logs) { std::cout << s << std::endl; } }

void Logger::clear() { _logs.clear(); }