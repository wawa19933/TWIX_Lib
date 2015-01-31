#include "socket.h"
#include <iostream>
#include <string.h>
#include <arpa/inet.h>

Socket::Socket(Types t) :
    sockType(t), currentState(Disconnected)
{
    peerAddress = new addrinfo;
    clearMemory(*peerAddress);

    buffer = new char[PACKET_SIZE];
}

bool Socket::bindToPort(const char *servicePort)
{
	struct addrinfo hints;
	struct addrinfo *result;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET;		//
	hints.ai_socktype = SOCK_DGRAM;	//
    hints.ai_flags = AI_PASSIVE;	// Hints for ANY IPv4 addresses

    if(getaddrinfo(0, servicePort, &hints, &peerAddress)) {
		std::cout << "Cannot create addrinfo structure for ANY_IP address!" << std::endl;
        return false;
	}

    for(result = peerAddress; result != 0; result = result->ai_next)
	{
		descriptor = socket(result->ai_family, result->ai_socktype, result->ai_protocol);		// Socket initialization
		if(descriptor < 0) {																	// Error information
            std::cerr << "Failed while creating socket!" << std::endl;
		}

		if(!bind(descriptor, result->ai_addr, result->ai_addrlen)) {
			break;
		}

		close(descriptor);
	}
    if(result == 0) {
        std::cerr << "Cannot bind the socket to ANY_IP!" << std::endl;
        return false;
    }
	else {
        currentState = Listening;
        std::cout << "Server is waiting for data on port " << servicePort << std::endl;
	}

    return true;
}

bool Socket::bindToAddress(const char *hostAddr, const char *servicePort)
{
	struct addrinfo *result;
    struct addrinfo hints;
    clearMemory(hints);

	hints.ai_family = AF_INET;		//
	hints.ai_socktype = SOCK_DGRAM;	//

    if(getaddrinfo(hostAddr, servicePort, &hints, &peerAddress)) {
        std::cerr << "Cannot create addrinfo structure for " << hostAddr << std::endl;
        return false;
	}

    hints.ai_flags = AI_PASSIVE;
    addrinfo *local;
    getaddrinfo("localhost", servicePort, &hints, &local);

    for(result = peerAddress; result != 0; result = result->ai_next)
    {
        descriptor = socket(result->ai_family, result->ai_socktype, result->ai_protocol);		// Socket initialization
        if(descriptor < 0) {																	// Error information
            std::cout << "Failed while creating socket!" << std::endl;
        }

        bool loc = true;
        for(int i = 2; i < 6; ++i) {
            if(local->ai_addr->sa_data[i] != result->ai_addr->sa_data[i]) {
                loc = false;
                break;
            }
        }
        if(loc)
            break;

        if(!bind(descriptor, result->ai_addr, result->ai_addrlen)) {
            break;
        }

        std::cout << "Cannot bind socket! - " << strerror(errno) << std::endl;
        close(descriptor);
    }

    peerAddress = result;
    if(peerAddress == 0) {
        std::cerr << "Error while connecting to the " << hostAddr << std::endl;
        return false;
    }
	else {
        currentState = Connected;
        getnameinfo(peerAddress->ai_addr, peerAddress->ai_addrlen, peerName, 300, 0, 0, 0);
        getnameinfo(peerAddress->ai_addr, peerAddress->ai_addrlen, peerIP, INET_ADDRSTRLEN, 0, 0, NI_NUMERICHOST);

        std::cout << "Binded to the " << peerName << '(' << peerIP << ')' << std::endl;
	}

    return true;
}

int Socket::writeData(const char *data, size_t sz)
{
    ssize_t size = sendto(descriptor, data, sz, MSG_DONTWAIT, peerAddress->ai_addr, peerAddress->ai_addrlen);
    if(size < 0) {
        std::cerr << "Error while sending!" << std::endl;
        return 0;
    }

    return size;
}

const char* Socket::readData()
{
    int size = recvfrom(descriptor, buffer, PACKET_SIZE, MSG_DONTWAIT, peerAddress->ai_addr, &peerAddress->ai_addrlen);
	if(size == -1) {
        std::cout << "recvfrom error - " << strerror(errno) << std::endl;
        return 0;
	}

	char host[255], service[64];
    if(getnameinfo(peerAddress->ai_addr, peerAddress->ai_addrlen, host, 255, service, 64, NI_NUMERICSERV))
//		std::cout << "Received " << size << " bytes from " << host << std::endl;
    getnameinfo(peerAddress->ai_addr, peerAddress->ai_addrlen, peerName, 300, 0, 0, 0);
    getnameinfo(peerAddress->ai_addr, peerAddress->ai_addrlen, peerIP, INET_ADDRSTRLEN, 0, 0, NI_NUMERICHOST);

    return buffer;
}

Socket::States Socket::state()
{
    return currentState;
}

Socket::Types Socket::type()
{
    return sockType;
}

bool Socket::hasPendingData(int timeout)
{
    watched.fd = descriptor;
    watched.events = POLLIN | POLLRDNORM;
    watched.revents = 0;

    int result = poll(&watched, 1, timeout);

    if(result < 0) {
        std::cout << "Error with poll() in Socket::hasPendingData()" << std::endl;
        return false;
    }
    else if(result == 0) {
        return false;
    }

    return true;
}

bool Socket::canWrite()
{
    struct pollfd fd;
    fd.fd = descriptor;
    fd.events = POLLWRBAND;
    fd.revents = 0;

    int res = poll(&fd, 1, 0);
    if(res < 0) {
        std::cerr << "Error in poll() Socket::canWrite()" << std::endl;
        return false;
    }
    else if(res == 0) {
        return false;
    }

    return true;
}

