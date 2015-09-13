#include <stdio.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART
#include <iostream>

using namespace std;

int main () {
	cout << "hello world" << endl;
	int aqfd = -1;
	aqfd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
	if(aqfd == -1) {
		cout << "Error could not open com port to AQ board" << endl;
		return -1;
	}
	
	cout << aqfd << endl;
	
	unsigned char c = 's';
	if(write(aqfd, &c, sizeof(unsigned char)) < 0) {
		cout << "Error in sending bytes" << endl;
	}

	if(aqfd != -1) {
		unsigned char rxbuf[256];
		int rxlen = read(aqfd, (void*)rxbuf, 255);
		if(rxlen < 0) {
			cout << "Error in read" << endl;
		} else if(rxlen == 0) {
			cout << "No data in buf" << endl;
		} else {
			printf("%i bytes read: %s\n", rxlen, rxbuf);
		}
	}

	close(aqfd);
	return 0;
}
