

struct GPS_coordinates { 
	char x[10]; 
	char y[10]; 
	char z[10]; 

}; 


void interpret_payload(int length, unsigned char payload[], int packet_type){ 
	//char c[1000]; 

	//int a = c[0]
	//a = a <<4; 
	//a+=c[1]; 
	//int state = 0; 
command: 
	if (packet_type == 1) //command 
		{ 
			unsigned char type = payload[0]; 
				if (type == packet_type){ 
					//state = 0; 
					continue; 
				}
				else if (type == 2) { 
					goto status; 
					//state++; 
				}
				else if (type == 3){ 
					goto image; 
				}

				//if (state )

			int flight_mode = payload[1]; 
			int gpslist_size = payload[2]; 

			GPS_coordinates list = payload[3]; 
			

		}
status: 
	else if (packet_type == 2) //status 
		{ 
			unsigned char type = payload[0]; 
				if (type == packet_type){ 
					//state = 0; 
					continue; 
				}
				else if (type == 1) { 
					goto command; 
					//state++; 
				}
				else if (type == 3){ 
					goto image; 
				}

				//if (state )

			int flight_mode = payload[1]; 
			int gpslist_size = payload[2]; 

			unsigned short batteryLev = payload[3];
			GPS_coordinates list = payload[4]; 
			char gyro_data[5] = payload[5]
			char accel_data[5]  = payload[6]; 
			char mag_data[5] = payload[7]; 
			GPS_coordinates current_pos = payload[8]; 
			char ultra_data[5] = payload[9]; 
			


		}
image: 
	else if (packet_type == 3)//image 
		{

			unsigned char packet_type = payload[1]; 
				if (type == packet_type){ 
					//state = 0; 
					continue; 
				}
				else if (type == 1) { 
					goto command; 
					//state++; 
				}
				else if (type == 2){ 
					goto status; 
				}

			unsigned char image_type = payload[2]; 
			unsigned short image_size = payload[3]; 
			char c[5000];
			   int fd, length;
			   int buff_size = 83;
			   c[0] = 6;
			   c[1] = 6;
			   c[2] = 6;
			   c[3] = 100;
			   fd=open("test.jpg",O_RDONLY);
			   if(fd == -1){
			       cout<<"pic not found\n";
			       return;
			   }
			   length=lseek(fd,0,SEEK_END);
			   close(fd);
			   fd=open("test.jpg",O_RDONLY);
			   c[4] = length >> 8;
			   c[5] = length & 255;
			   write(tty_fd, c, 6);
			   cout<<"sending pic "<<length<<'\n';
			   while(length > 0 ){
			       /*
			       if(write(tty_fd, c, length)){
			           length = 0;
			       }
			       */
			       if(length >= buff_size){
			           if(read(fd,c,buff_size) <=0){
			               cout<<"pic read failed "<<length<<" ";
			           }
			           c[buff_size]='\0';
			           if(write(tty_fd, c, buff_size)){
			               length -=buff_size;
			           }
			           else cout<<"write pic fail\n";
			           usleep(1000);
			       }
			       else{ 
			           if(read(fd,c,length) <=0 ){
			               cout<<"pic read failed2 "<<length<<"\n";
			           }
			           c[length] = '\0';
			           write(tty_fd, c, length);
			           length = 0;
			       }
			   }
			   c[0] = 169;
			   c[1] = 22;
			   c[2] = 243;
			    write(tty_fd, c, 3);
			   cout<<"pic sent\n";
			   close(fd);


		}

	else 
		{ 
			cout << "Packet type not recognized\n"; 

		}
} 