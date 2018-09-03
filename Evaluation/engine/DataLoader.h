#pragma once

#include <fstream>
#include <map>
#include <sstream>
#include "Logger.h"

typedef std::map<String, String> PropertyValues;

template <typename Type> class DataLoader {

public:
	virtual bool load(const PropertyValues& data, Type& result) = 0;

protected:
	template <typename T> bool setProperty(T* property, String name, T conversion(const String&)) {
		if (_data.find(name + ':') == _data.end()) {
			std::cout << "Property '" + name + "' is missing." << std::endl;
			return false;
		}
		try { *property = conversion(_data[name + ':']); }
		catch (std::exception e) {
			std::cout << "Property '" + name + "' is invalid." << std::endl;
			return false;
		}
		return true;
	}

	static int toInt(const String& string) { return std::stoi(string); }
	static float toFloat(const String& string) { return std::stof(string); }
	static bool toBool(const String& string) { return !string.compare("True"); }
	static String toString(const String& string) { return string; }

	void setData(const PropertyValues& data) { _data = data; }

private:
	PropertyValues _data;
};

