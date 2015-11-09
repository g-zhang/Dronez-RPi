#ifndef XbeeInterface_h
#define XbeeInterface_h

#include <string.h>
#include <string>
#include <cstring>
#include <stdlib.h>

void xbee_main();
#define MODEM "/dev/ttyUSB0"
#define BAUDRATE 9600    
int set_serial();
void send_data(const void *c, int size, const char &code);
void read_in();
void parse(char *data, int size, char type);
void send_pic(const char *name);


//template <typename T>

class Queue_send{
public:
	Queue_send( void *info_in,const int &size_in, char code_in) :
		info(info_in), code(code_in), size(size_in){
			senddata = true;
		}
	Queue_send(std::string name_in) : pic_name(name_in){
		senddata = false;
	}
	void send(){
		if(senddata){
			send_data(info, size, code);
			return;
		}
		char temp[pic_name.size()];
		std::memcpy(temp, pic_name.c_str(),pic_name.size());
		send_pic(temp);

	};
private:
	bool senddata;
	void *info;
	char code;
	int size;
	std::string pic_name;
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
