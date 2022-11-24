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
#include <thread>


using namespace std;

void read(int newFD, char *msg)
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

void write(int newFD, char *buff)
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

void init(int sockFD, int newFD)
{
    char msg[1500];
    cout << "------------------------------\n";
    cout << "awaiting client message...\n";
    thread readMSG(&read,newFD, &msg); //initiating thread for read()
    thread writeMSG(&write,newFD, &msg); //initiating thread for write()
}


int main(int argc, char *argv[])
{
    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(argc != 2)
    {
        cout << "please input port num \n";
    }
    
    //using arguments to get port num
    int portNum = atoi(argv[1]); // this will give an error if debuged using compielr. The pointer is NULL because no argument was put in. 
    servAddr.sin_port = htons(portNum);
    

    //keeping track of socket descriptor
    int sockSd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockSd < 0)
    {
        cout << "Error establishing socket (exiting,err 140)\n";
        exit(0);
    }
    int bindstat = bind(sockSd, (struct sockaddr*)&servAddr, sizeof(servAddr));
    if(bindstat < 0)
    {
        cout << "erorr binding socket (exiting, err 141\n";
        exit(0);
    }
    cout << "waiting for connection...\n";
    listen(sockSd, 10);

    //making new structs for the incoming connection
    sockaddr_in newSockAddr;
    socklen_t newSockAddrSize = sizeof(newSockAddr);

    int newCSD = accept(sockSd,(sockaddr *)&newSockAddr, &newSockAddrSize);
    if(newCSD < 0)
    {
        cout << "could not accept request from client (error 142, exiting)\n";
        exit(0);
    }
    cout << "Connected with client!\n";

    init(sockSd, newCSD);
    //All that is commented below this is summarized into the function above.
    // while(true)
    // {
    //     cout << "------------------------------\n";
    //     cout << "awaiting client message...\n";
    //     memset(&msg, 0, sizeof(msg));
    //     recv(newCSD, (char*)&msg, sizeof(msg),0);    
    //     if(!strcmp(msg, "exit()"))
    //     {
    //         cout << "exit code received\n";
    //         break;
    //     }
    //     cout << "Client: " << msg << endl;
    //     string data; //data to be sent
    //     getline(cin, data);
    //     memset(&msg, 0, sizeof(msg)); // clearing the buffer before sending
    //     strcpy(msg, data.c_str());
    //     if(data == "exit()")
    //     {
    //         memset(&msg, 0, sizeof(msg));
    //         string servMsg = "Server has closed connection";
    //         strcpy(msg, servMsg.c_str());
    //         send(newCSD, (char*)&msg, strlen(msg), 0);
    //         cout << "connection closing...\n";
    //         break;
    //     }
    //     send(newCSD, (char*)&msg, strlen(msg), 0);
    // }
    //closing the socket and ending the server
    close(newCSD);
    close(sockSd);
    cout << "-----------------------\n";
    cout << "session closed \n";
    cout << "bye bye\n";

}