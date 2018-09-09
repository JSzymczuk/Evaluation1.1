#include "Logger.h"

bool Logger::_isLogging = AreLogsVisible;
std::vector<String> Logger::_logs = std::vector<String>();

void Logger::log(String text) { 
	if (_isLogging) {
		_logs.push_back(text);
	}
}

void Logger::printLogs() { 
	if (_isLogging) {
		for (String s : _logs) {
			std::cout << s << std::endl;
		}
	}
}

void Logger::clear() { 
	_logs.clear();
}

void Logger::startLogging() {
	_isLogging = true; 
	_logs.clear(); 
}

void Logger::stopLogging() {
	_isLogging = false;
}

bool Logger::isLogging() {
	return _isLogging;
}