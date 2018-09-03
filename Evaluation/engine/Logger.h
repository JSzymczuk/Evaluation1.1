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
private:
	static std::vector<String> _logs;
};
