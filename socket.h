#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <poll.h>
#include "myutils.h"

#define PACKET_SIZE 508

class Socket
{
public:
    struct Header {
        int packetNum;
        double timeStamp;
        char command;
    };

    enum States {
        Listening   = 1,
        Connected   = 2,
        Disconnected= 0
    };

    enum Types {
        Client = 1,
        Server = 2
    };

    Socket(Types t = Server);

    bool bindToPort(const char *servicePort = "18778");
    bool bindToAddress(const char *hostAddr, const char *servicePort = "18778");
    int writeData(const char* data, size_t sz);
	const char *readData();

    States state();
    Types type();
    const char *peerHostIp() { return peerIP; }
    const char *peerHostName() { return peerName; }
    bool hasPendingData(int timeout = 100);
    bool canWrite();

private:
    short port;
	int descriptor;
    struct addrinfo *peerAddress;
    struct pollfd watched;
	char *buffer;
    char peerName[300];
    char peerIP[INET_ADDRSTRLEN];

    Types sockType;
    States currentState;
};

#endif // SOCKET_H
