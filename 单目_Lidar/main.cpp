#include <iostream>
#include <unistd.h>
#include "DataMerge.h"

int main() {
	DataMerge dataMerge;
	usleep(2*1000*1000);
	cout << "totalFrameNum = " << dataMerge.totalFrameNum << endl;

	while (dataMerge.startFramePos < dataMerge.totalFrameNum) {
		usleep(100*1000);//�ȴ��߳�ִ�н���
	}

	return 0;
}
