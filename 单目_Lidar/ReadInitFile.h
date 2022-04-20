#pragma once

#include <iostream>
#include <map>
#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include "ParamStruct.h"

using namespace std;

class ReadInitFile {
public:
	ReadInitFile();

	const InitParam &getParams() const;
	void setParams(const InitParam &_params);

private:
	InitParam params;
	map<string, string> read(string filePath);
	InitParam paramsMapImpl(std::map<string, string> param);

	std::vector<std::string> split(const std::string &str, const std::string &pattern);

	std::string &trim(std::string &s);
};

