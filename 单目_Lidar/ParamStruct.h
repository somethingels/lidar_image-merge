#pragma once
#include <string>

using namespace std;

struct InitParam {
	string imgSrcPath;
	string pcdSrcPath;

	string imgSavePath;
	string pcdSavePath;

	int startFramePos;
};
