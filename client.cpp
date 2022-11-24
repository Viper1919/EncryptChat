#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>

using namespace std;

void read(int clientFD, char *msg)
{
    while(true)
    {
        memset(&msg, 0, sizeof(msg));
        size_t size = sizeof(msg);
        recv(newFD,(char*)&msg, size, 0);
        if(sizeof(&msg) != 0)
        {
            cout << "Client: " << msg << endl;
        }
        if(!strcmp(msg, "exit()"))
        {
            cout << "exit code received\n";
            break;
        }
    }
}

void write(int clientFD, char *buff)
{
    string data;
    while(true)
    {
        memset(&buff, 0, sizeof(buff));
        getline(cin, data);
        strcpy(buff, data.c_str());
        send(newFD, &buff, strlen(buff),0);
    }

}

void init(int clientFD)
{
    char msg[1500];
    string data = "Hello server";
    cout << "------------------------------\n";
    strcpy(msg, data.c_str());
    send(clientFD, (char*)&msg, strlen(msg), 0);
    thread readMSG(&read,newFD, &msg); //initiating thread for read()
    thread writeMSG(&write,newFD, &msg); //initiating thread for write()
}


int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        cerr << "ERROR: server_ip, port not included\n";
        exit(0);
    }
    char *servIp = argv[1];
    int portNum = atoi(argv[2]);
    char msg[1500];
    
    //finding host
    struct hostent* host = gethostbyname(servIp);
    
    //setting up connection addrs
    sockaddr_in SSockAddr;
    bzero((char*)&SSockAddr, sizeof(SSockAddr));
    SSockAddr.sin_family = AF_INET;
    SSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host ->h_addr_list));
    SSockAddr.sin_port = htons(portNum);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    
    int status = connect(clientSd, (sockaddr*)&SSockAddr,sizeof(SSockAddr)); //should return 0

    if(status < 0)
    {
        cout << "Could not connect to socket!\n";
        return 0;
    }
    cout << "Connected!\n";

    init(clientSd);
    
  
    close(clientSd);
    cout << "------------------------------------\n";
    cout << "Connection closed\n";
    return 0;
}