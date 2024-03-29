#include <iostream>
#include "../sharedvars.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "XbeeInterface.h"
#include "xbee_parsing.h"
#include <thread>
#include <queue>

using namespace std;

void xbee_main() {
    cout << "xbee thread started" << endl;
    //unique_lock<mutex> infosendULock(SharedVars::infosendLock);
    //infosendULock.lock();
    while(true){
	//infosendULock.lock();
	//while(!SharedVars::infosend.empty()){
	//	SharedVars::infosendCv.wait(infosendULock);
	//}
	SharedVars::infosendLock.lock();
        if(SharedVars::infosend.size()){
            SharedVars::infosend.front().send_data();
            SharedVars::infosend.pop_front();
	    SharedVars::infosendLock.unlock();
        } else {
	    SharedVars::infosendLock.unlock();
	    sleep(2);
	}
	//infosendULock.unlock();
    }
    //infosendULock.unlock();
}

void read_in(){
    cout << "xbee read_in thread started" << endl;
    char buffer[5];
    int state = 0;
    int size = 0;
    int which_mes = 0;
    int i_data = 0;
    char type = '1';
    char data[5000];
    while(1){
        while (read(xbee_comm,buffer,1)>0){
            bool restart = true;
            //int temp = buffer[0];
            //cout<<temp<<" "<<flush;
            if (buffer[0] == 169 && state == 4){
                state++;
                restart = false;
            }
            else if (state == 5) {
                if (buffer[0] == 22)
                {
                    state++;
                    restart = false;
                }
                else state = 4;
            }
            else if (state == 6) {
                if (buffer[0] == 243){
                    restart = false;
                    state++;
                    if (i_data == size){
                        cout<<"Parsing\n";
                        SharedVars::infosendLock.lock();
                        SharedVars::infosend.push_front(
                            Queue_send( &which_mes, sizeof(which_mes),
                                'r')
                        );
                        SharedVars::infosendLock.unlock();
                        SharedVars::infosendCv.notify_one();
                        parse(data, size, type);
                    }
                    else{
                        cout<<"Data Wrong size packet dropped :(\n";
                    }
                    state = 0;
                    i_data = 0;
                }
                else
                    state = 4;
            }
            if (state >= 4)
            {
                if (i_data < size)
                {
                    data[i_data] = buffer[0];
                    i_data++;
                }
                else if(restart)
                {
                    i_data = 0;
                    state = 0;
                }
            }
             else if (state == 3)
            {
                type = buffer[0];

                for (int i = 0; i < 3;)
                {
                    if(read(xbee_comm,&buffer[i],1) > 0){
                        which_mes = buffer[0];
                        size = buffer[1];
                        size = size << 8;
                        size += buffer[2] & 255;
                        //cout<<"size"<<size<<endl;
                        i++;
                    }
                }
                state++;
            }
            else if (buffer[0] == 133 && state < 3)
                state++;
        }
        sleep(1);
    }
}

void send_data(const void *c, int size, const char &code, const int &which){
    cout<<size<<endl;
	char send[size + 8];
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
    /*
    for(int i = 0; i < size; i++){
        int temp = send[i];
        cout<<temp<<" ";
    }
    cout<<endl;

    for(int i = 0; i < size; i++){
        cout<<i<<" ";
    } for debugging*/
    cout<<endl;
    write(which,send,size);
    /*for(int i= 0; i < size;i+=100){
        if(i + 100 > size){
            write(xbee_comm,&send[i],size - i);
        }
        else
            write(xbee_comm,&send[i],100);
        usleep(50);
    }*/
}

void send_pic(const char *name){
    int fd, length;
    fd = open(name ,O_RDONLY);
    if(fd == -1){
        cout << "Xbee: pic not found" << endl;
        close(fd);
        return;
    }
    length=lseek(fd,0,SEEK_END);
    lseek(fd,0,SEEK_SET);
    char pic_info[length];
    read(fd, pic_info, length);
    send_data(pic_info, length,'p', xbee_comm);
    close(fd);
}

void parse(char *data, int size, char type){
    if(type == 'g'){
        gps_parse(data, size);
    }
    else if(type == 'f'){
        flight_mode(data,size);
    }

}

int set_serial(int which){
    struct termios tio;
    struct termios stdio;
    //struct termios old_stdio;
    int tty_fd;//, flags;
    //tcgetattr(STDOUT_FILENO,&old_stdio);
    memset(&stdio,0,sizeof(stdio));

    stdio.c_iflag=0;
    stdio.c_oflag=0;
    stdio.c_cflag=0;
    stdio.c_lflag=0;
    stdio.c_cc[VMIN]=1;
    stdio.c_cc[VTIME]=0;

    //tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
    //tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking
    memset(&tio,0,sizeof(tio));
    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag=CS8|CREAD|CLOCAL|CRTSCTS;           // 8n1, see termios.h for more information
    //tio.c_cflag=CS8|CREAD|CLOCAL;
    tio.c_lflag=0;
    tio.c_cc[VMIN]=1;
    tio.c_cc[VTIME]=5;
    if(which == 0){
        if((tty_fd = open(MODEM , O_RDWR | O_NONBLOCK)) == -1){
            printf("Error while opening\n"); // Just if you want user interface error control
            return -1;
        }
        else{
    	   cout<<"Xbee opened on "<<MODEM<<'\n';
        }
    }
    else{
        if((tty_fd = open(MODEM_PIC , O_RDWR | O_NONBLOCK)) == -1){
            printf("Error while opening\n"); // Just if you want user interface error control
            return -1;
        }
        else{
           cout<<"Xbee opened on "<<MODEM_PIC <<'\n';
        }
    }
    cfsetospeed(&tio,BAUDRATE);
    cfsetispeed(&tio,BAUDRATE);            // baudrate is declarated above
    tcsetattr(tty_fd,TCSANOW,&tio);
    return tty_fd;
}
