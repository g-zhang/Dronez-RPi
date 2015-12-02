#ifndef XbeeInterface_h
#define XbeeInterface_h

#include <string.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

void xbee_main();
#define MODEM "/dev/ttyXBEE"
#define MODEM_PIC "/dev/ttyXBEE"
#define BAUDRATE B9600
int set_serial(int which);
void send_data(const void *c, int size, const char &code, const int &which);
void read_in();
void parse(char *data, int size, char type);
void send_pic(const char *name);

static int xbee_comm = set_serial(0);
//static int xbee_pic = xbee_comm; // undo when you send pic

class Queue_send{
public:
	Queue_send( void *c,const int &size_in, char code_in)
	{
		is_pic = false;
		code = code_in;
		size = size_in;
		send[0] = 133;
    		send[1] = 133;
    		send[2] = 133;
    		send[3] = code;
    		send[4] = size >> 8;
    		send[5] = size & 255;
    		memcpy(&send[6], c, size);
 		size += 9;
 		send[size-3] = 169;
    		send[size-2] = 22;
    		send[size-1] = 243;
	}
	Queue_send(std::string name) : pic_name(name), is_pic(true){}
	void send_data(){
		if(!is_pic){
			std::cout << "Xbee sent: " << size - 9 <<std::endl;
			write(xbee_comm,send,size);
		}
		else
			send_pic(pic_name.c_str());
		return;
	};
private:
	char send[200];
	char code;
	int size;
	std::string pic_name;
	bool is_pic;
};

#endif
