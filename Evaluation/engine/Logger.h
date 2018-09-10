#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include "main/Configuration.h"

class Logger {
public:
	static std::vector<String> loggerIgnore;

	static void log(const String& text);
	static void logIfNotIgnored(const String& key, const String& text);
	static void printLogs();
	static void clear();
	static bool isLogging();
	static bool loggerIgnores(const String& name);
	static void startLogging();
	static void stopLogging();

private:
	static std::vector<String> _logs;
	static bool _isLogging;
	static std::mutex _mtx;
	static size_t _notIgnoredIdx;

	static void pushLog(const String& text);
};
