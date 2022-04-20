
#include "Util.h"

// 创建多级目录
int Util::mkdir_r(const char *path) {
	if (path == nullptr) {
		return -1;
	}
	if (access(path, 0) != -1) {
		return -1;
	}
	char *temp = strdup(path);
	char *pos = temp;

	/* 去掉开头的 './' 或 '/' */
	if (strncmp(temp, "/", 1) == 0) {
		pos += 1;
	} else if (strncmp(temp, "./", 2) == 0) {
		pos += 2;
	}

	/* 循环创建目录 */
	for (; *pos != '\0'; ++pos) {
		if (*pos == '/') {
			*pos = '\0';
			mkdir(temp, 0755);
			//printf("for %s\n", temp);
			*pos = '/';
		}
	}

	/* 如果最后一级目录不是以'/'结尾，
	遇到'\0'就中止循环，
	不会创建最后一级目录 */
	if (*(pos - 1) != '/') {
		//printf("if %s\n", temp);
		mkdir(temp, 0755);
	}
	free(temp);
	return 0;
}

int Util::scan_one_dir(const char *path, vector<string> &vecStr) {
	if (NULL == path) {
		cout << " path is null ! " << endl;
		return -1;
	}

	struct stat s{};
	lstat(path, &s);
	if (!S_ISDIR(s.st_mode)) {
		return -2;
	}

	struct dirent *filename;
	DIR *dir;
	dir = opendir(path);
	if (NULL == dir) {
		return -3;
	}

	while ((filename = readdir(dir)) != NULL) {
		if (strcmp(filename->d_name, ".") == 0 ||
		    strcmp(filename->d_name, "..") == 0) {
			continue;
		}

		char wholePath[1024] = {0};
		sprintf(wholePath, "%s/%s", path, filename->d_name);
		string tempStr = filename->d_name;
		vecStr.push_back(tempStr);
		//cout << "wholePath= " << wholePath << endl;
	}

	for (int i = 0; i < vecStr.size() - 1; ++i) {
		for (int j = i + 1; j < vecStr.size(); ++j) {
			if (vecStr.at(i) > vecStr.at(j)) {
				string tempStr = vecStr.at(i);
				vecStr.at(i) = vecStr.at(j);
				vecStr.at(j) = tempStr;
			}
		}
	}

	free(filename);
	free(dir);
	return 1;
}

void Util::CopyFile(const char *sourceFile, const char *destFile) {
	std::string command = "cp ";
	command += sourceFile;
	command += " ";
	command += destFile;//cp /home/file1 /root/file2
	system((char *) command.c_str());//
}

void Util::MoveFile(const char *sourceFile, const char *destFile) {
	std::string command = "mv ";
	command += sourceFile;
	command += " ";
	command += destFile;//mv /home/file1 /root/file2
	system((char *) command.c_str());//
}
