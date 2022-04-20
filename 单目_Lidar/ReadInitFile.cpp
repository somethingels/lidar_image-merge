
#include "ReadInitFile.h"

ReadInitFile::ReadInitFile() {
	std::map<string, string> param = ReadInitFile::read("dataInit.ini");
	params = ReadInitFile::paramsMapImpl(param);
}

map<string, string> ReadInitFile::read(string filePath) {
	map<string, string> retMap;
	ifstream in(filePath);
	if (!in.is_open()) {
		std::cout << "Init file read error." << std::endl;
		return retMap;
	}
	string line;
	while (getline(in, line)) {
		if (!line.empty()) {
			if ('#' == line[0]) {
				continue;
			}
			vector<string> v0 = this->split(line, "#");
			vector<string> v = this->split(v0[0], "=");
			if (!v.empty() && v.size() >= 2) {
				retMap.insert(pair<string, string>(this->trim(v[0]), this->trim(v[1])));
			} else {
				std::cout << "invalid line : " << line.c_str() << std::endl;
			}
		}
	}
	return retMap;
}

std::vector<std::string> ReadInitFile::split(const std::string &str, const std::string &pattern) {
	char *strc = new char[strlen(str.c_str()) + 1];
	strcpy(strc, str.c_str());
	std::vector<std::string> resultVec;
	char *tmpStr = strtok(strc, pattern.c_str());
	while (tmpStr != NULL) {
		resultVec.push_back(std::string(tmpStr));
		tmpStr = strtok(NULL, pattern.c_str());
	}
	delete[] strc;
	return resultVec;
}

std::string &ReadInitFile::trim(std::string &s) {
	if (!s.empty()) {
		const string &delim = " \t";
		s.erase(0, s.find_first_not_of(delim));
		s.erase(s.find_last_not_of(delim) + 1);
	}
	return s;
}

/*
 * 参数映射
 * */
InitParam ReadInitFile::paramsMapImpl(std::map<string, string> param) {
	InitParam initParam{};
	initParam.imgSrcPath = param.at("imgSrcPath");
	initParam.pcdSrcPath = param.at("pcdSrcPath");

	initParam.imgSavePath = param.at("imgSavePath");
	initParam.pcdSavePath = param.at("pcdSavePath");

	initParam.startFramePos = stoi(param.at("startFramePos"));

	return initParam;
}

const InitParam &ReadInitFile::getParams() const {
	return params;
}

void ReadInitFile::setParams(const InitParam &_params) {
	ReadInitFile::params = _params;
}
