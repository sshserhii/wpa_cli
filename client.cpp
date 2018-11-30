#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
using namespace std;
int main (){
    int Socket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(12344);
    SockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    connect(Socket,(struct sockaddr *)(&SockAddr),sizeof(SockAddr));
    char Buffer[] = "Hello";
    send(Socket,Buffer,sizeof(Buffer),MSG_NOSIGNAL);
    Buffer[2]='*';
    recv(Socket,Buffer,sizeof(Buffer),MSG_NOSIGNAL);
    shutdown(Socket,SHUT_RDWR);
    close(Socket);
    cout<<Buffer;

    return 0;
}
