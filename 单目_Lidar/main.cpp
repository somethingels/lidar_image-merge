#include <iostream>
#include <unistd.h>
#include "DataMerge.h"

int main() {
	DataMerge dataMerge;
	usleep(2*1000*1000);
	cout << "totalFrameNum = " << dataMerge.totalFrameNum << endl;

	while (dataMerge.startFramePos < dataMerge.totalFrameNum) {
		usleep(100*1000);//等待线程执行结束
	}

	return 0;
}
