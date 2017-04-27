#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h> 
#include <netdb.h>
#include <cstring> 
#include <iostream> 
#include <string.h>



#define BUFSIZE 1000
#define TM_PACKETS_TO_SEND 100
//#define DEST_ADDR "127.0.0.1"

using namespace std;
void output(string logfile)
{
	//ofstream outputFile(logfile);
	//outputFile << "Connecting to the server";
}

int receiveimage(int socket, string imagefile)
{
	int buffersize=0;
	int receive_size =0;
	int size=0;
	int r_size;
	int w_size;
	int b;

	char imagearray[10241];


	FILE *image;

	
	b=read(socket,&size, sizeof(int));
	
	while(b<0)
	{
		b=read(socket,&size, sizeof(int));
	}

	char buffer[] = "Got it";

	b = write(socket, &buffer, sizeof(int));
	while(b<0)
	{
		b = write(socket, &buffer, sizeof(int));		
	}
	const char *z = imagefile.c_str();
	image = fopen(z, "w");

	if(image==NULL)
	{
		printf("Error has occured, Image file could not be opened\n");
		return -1;
	}

	int need_exit = 0;
	struct timeval timeout = {12,0};

	fd_set fds;
	int buffer_fd;
	int buffer_out;

	while(receive_size < size)
	{
		FD_ZERO(&fds);
		FD_SET(socket,&fds);
		buffer_fd = select(FD_SETSIZE,&fds, NULL,NULL,&timeout);

		if(buffer_fd<0)
		{
			cout << "File Descriptor Problem." << endl;;
		}
		if(buffer_fd==0)
		{
			cout<< "Buffer timeout" << endl;
		}
		if(buffer_fd > 0)
		{
			
			r_size=read(socket,imagearray,10241);
			
			while(r_size<0)
			{
				r_size=read(socket,imagearray,10241);
			}

       		w_size = fwrite(imagearray,1,r_size,image);
  

       		if(r_size!=w_size)
       		{
       			cout << "Error" << endl;
       		}
       		 receive_size += r_size;
		}
	}
	fclose(image);
  	printf("Image successfully Received!\n");
  	return 1;
}



int main(int argc, char* argv[])
{	
	string temp;
	string temp1;
	string ip;
	string port;
	string logfile;
	string imagefile;
	const char *ip_add;
	const char *c;
	temp1 = argv[1];
	if(!temp1.compare("u"))
	{
		if(argc < 4)
   		{
        	cout << "Usage is: -u -s portno -l logfile" << endl;
        	return 1;
    	}
    	else
    	{
		    ip = argv[2];
		    ip.erase(remove_if(ip.begin(),ip.end(),::isspace),ip.end());
		    ip_add = ip.c_str();
		    port = argv[3];
		    port.erase(remove_if(port.begin(),port.end(),::isspace),port.end());
		    c = port.c_str();
		    logfile = argv[4];


		    cout << "Using UDP sockets" << endl;
        	cout << "Port Number: " << port << endl;
        	cout << "Saved to: " << logfile << endl;
    	}
	}
	if(!temp1.compare("t"))
	{
		if(argc < 6)
   		{
        	cout << "Usage is: -t -s serverIP -p portno -l logfile -I imagefile" << endl;
        	return 1;
    	}
    	else
    	{
    		ip = argv[2];
		    ip.erase(remove_if(ip.begin(),ip.end(),::isspace),ip.end());
		    ip_add = ip.c_str();
		    port = argv[3];
		    port.erase(remove_if(port.begin(),port.end(),::isspace),port.end());
		    c = port.c_str();
		    logfile = argv[4];
		    imagefile = argv[5];

		    cout << "Using TCP sockets" << endl;
        	cout << "Port Number: " << port << endl;
        	cout << "Saved to: " << logfile << endl;
        	cout << "Image file: " << imagefile << endl; 
    	}
	}

	
	int fd; 
	int status;
	unsigned char buf[BUFSIZE];

	string name;
	string uscid;

	cout << "Please Enter Name" << endl;
	getline(cin,name);
	cout << "Please Enter Your USCID" << endl;
	getline(cin,uscid);

	name = "Name " + name;
	uscid = "USCID " + uscid;


	int name_len = strlen(name.c_str());
	string space = "\n";
	string mesg;
	if(!temp1.compare("u"))
	{
		string namelen = "StringLength " + to_string(name_len - 5);
		mesg = uscid + space + name + space + namelen;
	}
	if(!temp1.compare("t"))
	{
		mesg = uscid + space + name;
	}
	//const char *a;
	//a = name.c_str();
	//const char *b;
	//b = uscid.c_str();
	//const char *d;
	//d = to_string(name_len).c_str();

	char *msg = new char[mesg.length()+1];
    strcpy(msg,mesg.c_str());
	int len; 


	struct addrinfo hints;
	struct addrinfo *res;  // will point to the results

	memset(&hints, 0, sizeof hints); // make sure the struct is empty
	hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
	if(!temp1.compare("t"))
	{
		hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
		cout << "test" << endl;
	}
	else
	{
		hints.ai_socktype = SOCK_DGRAM;
	}

	// get ready to connect
	if ((status = getaddrinfo(ip_add, c, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
	


	if((fd=socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) //creates the socket
	{
		perror("error with socket");
		return 0;
	}

	/*if((bind(fd, res->ai_addr, res->ai_addrlen))==-1) //associate the socket with a port on your local machine
    {
        cout <<"bind problem" << endl;
    }*/
    if(connect(fd,res->ai_addr,res->ai_addrlen)==-1)
    {
    	cout << "connect problem" << endl;
    }

    len = strlen(msg);
    send(fd,msg,len,0);

    recv(fd,buf,BUFSIZE,0);
    receiveimage(fd,imagefile);


    output(logfile);

	/*memset((char *)&myaddr, 0, sizeof(myaddr)); //binds the socket to a valid IP address and specific port
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(PORT); //what port to use, pick any available port
	myaddr.sin_addr.s_addr = inet_addr(DEST_ADDR);*/

	/*if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0)
	{
		perror("bind failed");
		return 0;
	}*/

	//i NEED TO CONNECT() dont need to bind	

	/*for (i=0;i<TM_PACKETS_TO_SEND;i++)
	{
    	printf("Sending packet %d\n",i);
    	//printf(buf, "This is packet %d\n", i);
    	errorCode = sendto(fd, buf, BUFSIZE,0,(struct sockaddr *)&myaddr,sizeof(myaddr));
    	if(errorCode < 0)
    	{
    		cout << "problem with sending" << endl;
    		break;
    	}
    	i++;
    	
    }*/

    close(fd);
    return 0;
}