#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "main/Configuration.h"

class Logger {
public:
	static void log(String text);
	static void printLogs();
	static void clear();
	static bool isLogging();
	static void startLogging();
	static void stopLogging();
private:
	static std::vector<String> _logs;
	static bool _isLogging;
};
