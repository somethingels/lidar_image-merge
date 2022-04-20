#pragma once
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <unistd.h>

using namespace std;

class Util {
public:
	static int mkdir_r(const char *path);// 创建多级目录

	static int scan_one_dir( const char * path, vector<string> &vecStr);// 列出一个目录下所有文件

	static void CopyFile(const char *sourceFile, const char *destFile);

	static void MoveFile(const char *sourceFile, const char *destFile);
};

