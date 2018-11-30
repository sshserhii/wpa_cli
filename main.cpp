#include <iostream>
#include <set>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <algorithm>
#include <poll.h>

using namespace std;

#define POLL_SIZE 2048

int set_nonblock(int fd){
    int flags;
#if defined(O_NONBLOCK)
    if(-1 == (flags = fcntl(fd,F_GETFL,0)))
            flags=0;
    return fcntl(fd,F_SETFL,flags | O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fd,FIOBIO,&flags);
#endif
}


int main(int argc, char *argv[])
{
    int MasterSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    set<int> SlaveSockets;

    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(12345);
    SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(MasterSocket,(struct sockaddr*)(&SockAddr),sizeof(SockAddr));

    set_nonblock(MasterSocket);

    listen(MasterSocket,SOMAXCONN);
    struct pollfd Set[POLL_SIZE];
    Set[0].fd = MasterSocket;
    Set[0].events = POLLIN;
    while(true){
        int index = 1;
        for(auto Iter = SlaveSockets.begin();Iter !=SlaveSockets.end();Iter++){
            Set[index].fd=*Iter;
            Set[index].events=POLLIN;
            index++;
        }
        unsigned int SetSize = 1 + SlaveSockets.size();
        poll(Set,SetSize,-1);

        for(int i =0;i<SetSize;i++){
            if(Set[i].revents & POLLIN){
                if(i){
                    static char Buffer[1024];
                    int RecvSize = recv(Set[i].fd,Buffer,1024,MSG_NOSIGNAL);
                                        if((RecvSize==0) && (errno != EAGAIN)){
                        shutdown(Set[i].fd,SHUT_RDWR);
                        close(Set[i].fd);
                        SlaveSockets.erase(Set[i].fd);
                    }
                    else if(RecvSize > 0){
                        send(Set[i].fd,Buffer,RecvSize,MSG_NOSIGNAL);
                        for(int i=0;i<RecvSize;i++)
                            cout<< Buffer[i];
                    }
                }
                else{
                    int SlaveSocket = accept(MasterSocket,0,0);
                    set_nonblock(SlaveSocket);
                    SlaveSockets.insert(SlaveSocket);
                }
            }
        }

}
        return 0;
}

