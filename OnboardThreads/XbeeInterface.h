#ifndef XbeeInterface_h
#define XbeeInterface_h

#include <string.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void xbee_main();
#define MODEM "/dev/ttyUSB0"
#define MODEM_PIC "/dev/ttyUSB0"
#define BAUDRATE 9600    
int set_serial(int which);
void send_data(const void *c, int size, const char &code, const int &which);
void read_in();
void parse(char *data, int size, char type);
void send_pic(const char *name);

static int xbee_comm = set_serial(0);
static int xbee_pic = set_serial(1); // undo when you send pic


//template <typename T>

class Queue_send{
public:
	Queue_send( void *c,const int &size_in, char code_in)
	{
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
	void send_data(){
		write(xbee_comm,send,size);
		return;
	};
private:
	char send[200];
	char code;
	int size;
};

/*
class Pic_send : public Queue_send{
public:
	Pic_send(std::string name_in) : pic_name(name_in){}
	virtual void send() const{
		char temp[pic_name.size()];
		std::memcpy(temp, pic_name.c_str(),pic_name.size());
		send_pic(temp);
	}
private:
	std::string pic_name;
};

template <typename T>
class Struct_send : public Queue_send{
public:
	Struct_send(T info_in, char code_in) : info(info_in), code(code_in){}
	virtual void send() const{
		send_data (&info, sizeof(info), code);
	}
private:
	T info;
	char code;
};
*/
#endif
