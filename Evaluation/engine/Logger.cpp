#include "Logger.h"
#include "engine/CommonFunctions.h"

bool Logger::_isLogging = AreLogsVisible;

std::vector<String> Logger::_logs = std::vector<String>();

std::mutex Logger::_mtx;

std::vector<String> Logger::loggerIgnore = std::vector<String>({
	"updateOrientation",
	"updateMovement",
	"setPreferredVelocityAndSafeGoal",
	"updateSpotting",
	"getActorsInViewAngle",
	"getVelocityObstacles",
	"computeCandidates",
	"selectVelocity",
	"checkMovement",
	"updateCurrentAction",
	"updateMovement",
	"updateWeapons"
	});

size_t Logger::_notIgnoredIdx = loggerIgnore.size();

bool Logger::loggerIgnores(const String& name) {
	return common::indexOf<String>(loggerIgnore, name) != _notIgnoredIdx;
}

void Logger::pushLog(const String& text) {
		_mtx.lock();
		_logs.push_back(text);
		_mtx.unlock();
}

void Logger::log(const String& text) { 
	if (_isLogging) {
		pushLog(text);
	}
}

void Logger::logIfNotIgnored(const String& key, const String& text) {
	if (_isLogging && !loggerIgnores(key)) {
		pushLog(text);
	}
}

void Logger::printLogs() { 
	if (_isLogging) {
		_mtx.lock();
		for (String s : _logs) {
			std::cout << s << std::endl;
		}
		_mtx.unlock();
	}
}

void Logger::clear() {
	_mtx.lock();
	_logs.clear();
	_mtx.unlock();
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