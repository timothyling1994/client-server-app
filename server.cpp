#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>  
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <cstring> 
#include <iostream> 
#include <string.h>
#include <algorithm> 
#include <fstream> 
#include <stdio.h>

using namespace std;

#define BUFLEN 1000
//#define NPACK 1000
//#define PORT "1153"
#define DEST_ADDR "127.0.0.1"
#define BACKLOG 10

void output(string logfile, string host, string clientport, string port,char *buf, int loops, bool imagebool, string imagefile1)
{
    ofstream outputFile(logfile);
    outputFile << "Server started on: " << " at port " << port << endl;
    outputFile << "Received client connection from hostname " << " port: " << clientport << endl;
    outputFile << buf << endl;
    outputFile << "sending random string length " << loops << endl;
    outputFile << "received string length " << sizeof(buf) << endl;
    if(imagebool)
    {
        outputFile <<"sending image file " << imagefile1 << endl;
    }
    outputFile << "terminating server..." << endl;

}

int sendimage(int socket,string image)
{
    FILE *picfile;
    const char *pic = image.c_str();
    picfile = fopen(pic, "r");
    if(picfile == NULL)
    {
        printf("Error opening image");
    }
    
    int send_buffer_size = 200000;
    char send_buffer[10000];
    char read_buffer[300];
    int read_size;
    int size;
    int bytes_r;


    fseek(picfile,0,SEEK_END); //repositions stream indicator
    size = ftell(picfile); //gets size of the file
    rewind(picfile);

    if(size == EOF)
    {
        cout<<"problem w/stream"<<endl;
    }


    write(socket,(void *)&size,sizeof(int)); //write out given size of the image


    //do
    //{
    bytes_r=read(socket,&read_buffer,301);
    //}
    //while(bytes_r<0);
    while(bytes_r<0)
    {
        bytes_r=read(socket,&read_buffer,301);
    }
    if(feof(picfile))
    {
        printf("Error: End of File Reached\n");
    }
    else
    {
        while(!feof(picfile))
        {
            read_size = fread(send_buffer,1,sizeof(send_buffer)-1,picfile);
            
            bytes_r = write(socket,send_buffer,read_size);
            
            while(bytes_r<0)
            {
                bytes_r = write(socket,send_buffer,read_size);
            }

            bzero(send_buffer,sizeof(send_buffer));
        }
    }
}



int main(int argc, char* argv[])
{


    string temp1 = argv[1];
    string imagefile;
    if(!temp1.compare("u"))
    {
        if(argc < 4)
        {
            cout << "Usage is: -u -p portno -l logfile" << endl;
            return 1;
        }
    }
    if(!temp1.compare("t"))
    {
        if(argc<5)
        {
            cout <<"Usage is: -u -p portno -l logfile -I imagefile" << endl;
            return 1;
        }
        else
        {
            imagefile=argv[4];
        }
    }

    
    string port = argv[2];
    port.erase(remove_if(port.begin(),port.end(),::isspace),port.end());
    const char *c = port.c_str();
    string logfile = argv[3];

    if(!temp1.compare("u"))
    {
        cout << "Using UDP sockets" << endl;
        cout << "Port Number: " << port << endl;
        cout << "Saved to: " << logfile << endl;
    }
    //struct sockaddr_in si_me, si_other;
    struct addrinfo hints;
    struct addrinfo *res;
    struct sockaddr_storage their_addr;
    struct sockaddr_in sin;
    socklen_t len1;
    socklen_t addr_size;
    char hoststr[NI_MAXHOST];
    char portstr[NI_MAXSERV];
    char hostserver[NI_MAXHOST];
    char portserver[NI_MAXSERV];
    string outputPort;
    string outputIP;
    bool imagebool;
    //char ip4[100];
    //int s, new_s, i, slen=sizeof(si_other), status;
    int socketfd; 
    int new_fd;
    string asciiString = "RSTRING ";

    int status;
    srand(time(NULL));
    //int loops = rand()%151 + 100;
    int loops = rand()%151 + 100;
    cout << loops << endl;
    for(int i=0;i<loops;i++)
    {
        int asciiVal = rand()%95 + 32;
    
        asciiString = asciiString+(char)asciiVal;
        //cout << (char)asciiVal << endl;

    }


    char *msg = new char[asciiString.length()+1];
    strcpy(msg,asciiString.c_str());
    //cout << msg << endl;
    char buf[BUFLEN];
    int len, bytes_sent;
    len = strlen(msg);
    
    //memset((char *) &si_me, 0, sizeof(si_me));
    //si_me.sin_family = AF_INET;
    //si_me.sin_port = htons(PORT);
    //si_me.sin_addr.s_addr = inet_addr(DEST_ADDR);
    //si_me.sin_addr.s_addr = htonl(INADDR_ANY);


    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    if(!temp1.compare("u"))
    {
        hints.ai_socktype = SOCK_DGRAM; // TCP stream sockets
    }
    else
    {
        hints.ai_socktype = SOCK_STREAM;
    }
    hints.ai_flags = AI_PASSIVE;     


    if ((status = getaddrinfo(NULL, c, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    if ((socketfd=socket(res->ai_family, res->ai_socktype, res->ai_protocol))==-1)
    {
        cout << "socket problem" << endl;
    }

    if((bind(socketfd, res->ai_addr, res->ai_addrlen))==-1) //associate the socket with a port on your local machine
    {
        cout <<"bind problem" << endl;
    }
    
    if(!temp1.compare("t"))
    {
        if(listen(socketfd,BACKLOG)==-1)
        {
            cout << "listening problem" << endl;
        }
        len = sizeof(sin);
        getsockname(socketfd,(struct sockaddr *)&sin,&len1);
        outputPort = ntohs(sin.sin_port);
        outputIP= ntohs(sin.sin_addr.s_addr);
        printf("port number %d\n",ntohs(sin.sin_port));
        //printf("ip number %d\n",ntohs(sin.sin_addr.s_addr));
        addr_size = sizeof their_addr;
        new_fd = accept(socketfd,(struct sockaddr *)&their_addr,&addr_size);


        if((recv(new_fd,buf,BUFLEN,0))==-1)
        {
            cout << "Recv problem" << endl;
        }

        if((send(new_fd,msg,len,0))==-1) // sEND AN IMAGE
        {
            cout << "Send problem" << endl;
        }
        sendimage(new_fd,imagefile);
        imagebool = true;
        fflush(stdout);

    
    }
    else
    {
        addr_size = sizeof their_addr;
        if((recvfrom(socketfd,buf,BUFLEN,0,(struct sockaddr *)&their_addr,&addr_size))==-1)
        {
            cout << "Recvfrom problem" << endl;
        }
        if((sendto(socketfd,msg,len,0,(struct sockaddr *)&their_addr,addr_size))==-1)
        {
            cout << "Sendto problem" << endl; 
        }
        cout << "reach" << endl;
        imagebool = false;

    }



    cout << buf << endl;
    getnameinfo((struct sockaddr *)&their_addr, addr_size, hoststr, sizeof(hoststr), portstr, sizeof(portstr), NI_NUMERICHOST | NI_NUMERICSERV);
    //getnameinfo((struct sockaddr *)&res,res->ai_addrlen,hostserver, sizeof(hostserver),portserver,sizeof(portserver),NI_NUMERICHOST | NI_NUMERICSERV);
    close(socketfd);
    fflush(stdout);
    string hoststring=hoststr;
    string portstring=portstr;
    output(logfile,hoststring,portstring,port, buf,loops,imagebool, imagefile);

    //listen(s, LISTENQ);
    //cout << "WAITING for connections" << endl;

    //new_s = accept(s,(struct sockaddr *)&si_other, (socklen_t*)sizeof(si_other));
    

    //for (i=0; i<NPACK; i++)
    //{
    /*while(1)    
    {   

        cout << "SOCKET" << new_s << endl;
        cout<< "SOCKET!" << s << endl;
        //if (recvfrom(new_s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, (socklen_t *)sizeof(si_other))==-1)
        //    cout <<"receive problems" << endl;
        printf("Received packet from %s:%d\nData: %s\n\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
    //}
    }

    close(s);*/
    freeaddrinfo(res);
    return 0;
}