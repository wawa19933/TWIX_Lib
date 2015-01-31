//============================================================================
// Name        : UDP_Client
// Author      : Artem Kirkov
// Version     : beta 1-1.02
// Copyright   : All rights reserved
// Description : Calculate RTT for a packets
//============================================================================
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include "socket.h"
#include "myutils.h"

using namespace std;
using namespace std::chrono;

void calculateOWD(int number);
void getStableRegion(vector<double> &data, vector<double> &outliers);
double synchronizeClocks(Socket *socket, int number);

int main(int argc, char *argv[])
{
    Socket *sock = new Socket(Socket::Client);

////////////// Getting IP and port from command line://///////////////////////

    if(argc != 2) {
        perror("Type IP address and Port");
        exit(1);
    }

//    char *ip_addr = argv[1];
//	int port_num = atoi(argv[2]);
//	hostent * record = gethostbyname(ip_addr);
//	in_addr * addrptr = (in_addr *) record->h_addr;

//    s = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
//	server.sin_addr = *addrptr;
//	server.sin_family=AF_INET;
//	server.sin_port=htons(port_num);
    /*if(s < 0) {
        perror("Socket creation failed");
    }
    cout << "Socket has been successfully created";*/

//    socklen_t l = sizeof(client);
//	socklen_t m = sizeof(server);

    if (!sock->bindToAddress(argv[1]))
       return errno;

    cout << "Going to send:" << endl;
//	cout << "\n Sending 100 messages 1 byte each and wait for acknowledgment\n";

    synchronizeClocks(sock, 100);

 /*
    for(i = 0; i < 100; i++)
	{
        sock->writeData(data, PACKET_SIZE);

//        if(sendto(s,data, sizeof(data), 0, (struct sockaddr *)&server, m) < 0) {
//            perror("Sending error\n");
//        }

        auto t1 = std::chrono::high_resolution_clock::now();

        if(sock->hasPendingData(1000))
            memcpy(buff, sock->readData(), PACKET_SIZE);
        else
            cout << "Packet has been lost!" << endl;

//        if(recvfrom(s, buff, sizeof(buff),0,(struct sockaddr *)&client,&l) < 0) {
//            perror("Receive failed\n");
//        }

        auto t2 = std::chrono::high_resolution_clock::now();

        //////////// A floating point milliseconds type///////////////

        using FpMilliseconds = std::chrono::duration <float, std::chrono::milliseconds::period>;
        static_assert(std::chrono::treat_as_floating_point <FpMilliseconds::rep>::value, "Floating error: ");
        auto rtt = FpMilliseconds(t2-t1);
        vec.push_back(rtt.count());



        //rtt_sum+=rtt.count();
	///////////////////////// Output of RTT //////////////////////////
        std::cout <<"Packets received from:"<< sock->peerHostIp() <<"\tRTT = "<< rtt.count() << " milliseconds\n";
    }

//////////////////////// Quartiles calculation ///////////////////
    quartiles(vec);



///////////////////////////Calculation of Delta T////////////////////////////////////
    if(sendto(s,&owd_bar,sizeof(owd_bar),0,(struct sockaddr *)&client,l)<0) {
        perror("Sending error\n");
    }




//////////////////////////////      OWD calculation     /////////////////////////////////////////////////////
   std::vector<double> v  ;
   vector<double> owd (1) ;
   int count = 0;
    for (i = 0; i < 100; ++i)
    {
        double OWD;
        ++count;
        high_resolution_clock TimeStamp;
        auto t_stamp = TimeStamp.now();
        double TS1 = duration_cast<milliseconds>(t_stamp.time_since_epoch()).count();
        v.push_back(TS1);
        v.push_back(Delta_TS);
        v.push_back(count);
        //cout << "sending TS1:" << TS1 << endl;
        if(sendto(s,&v[0], sizeof(v), 0, (struct sockaddr *)&server, m) < 0)
        {
            perror("Error during sending packets \n");
        }

      vector<double>().swap(v);

      int r = recvfrom(s, &owd[0], sizeof(owd), 0, (struct sockaddr *)&client, &l);
        if (r < 0) {
            cout << "Error reading from socket" << endl;
        }

     OWD = owd.front();
     printf("OWD: %.8f\n", OWD);
     cout.flush();
    }
*/

return 0;
}

void getStableRegion(vector<double> &data, vector<double> &outliers)
{
    cout << "\n===== Finding of stable region for RTTs ======" << endl;
    sort(data.begin(), data.end());
    int sz = data.size();
    double Q1 = 0.25 * sz;
    double Q2 = 0.5 * sz;
    double Q3 = 0.75 * sz;

    cout.precision(3);
    cout << "Number of elemnts: " << sz << endl;
    cout << "25% "<< data.at(Q1) << " 50% "<<data.at(Q2)<< " 75% "<< data.at(Q3) << endl << endl;

    double IQR = data.at(Q3) - data.at(Q1);
    double upper = data.at(Q3) + 1.5*IQR, lower = data.at(Q1) - 1.5*IQR;
    cout << "IQR: " << IQR << " (" << lower << ", " << upper << ")" << endl;

    cout << "Outliers: ";
    for(int i = 0; i < sz; ++i) {
        if(data[i] < lower || data[i] > upper) {
            cout << data[i] << " ";
            outliers.push_back(data[i]);
            data.erase(data.begin() + i);
            sz = data.size();
        }
    }
    cout << endl << endl;
}

void calculateOWD(int number)
{

}

double owdBar(vector<double> &data)
{
    double rtt_sum = 0;
    double aver = 0;
    double owd_bar = 0;

    for (int i = 0; i < data.size(); ++i)
    {
        rtt_sum += data.at(i);
    }

    aver    = rtt_sum / data.size();
    owd_bar = aver / 2;

    cout << "Average RTT is: " << aver << "\tOWD is: " << owd_bar << endl;

    return owd_bar;
}

double synchronizeClocks(Socket *socket, int number)
{
    struct Socket::Header header;
    char *buffer = new char[PACKET_SIZE];
    vector<double> timestamps, outliers;

    for(int i = 0; i < number;)
    {
        /* Sending Packet */
        header.packetNum = ++i;
        if(i == number + 1)
            header.command = 'F';
        else
            header.command = 'S';
        pushData(header, buffer);

        if(socket->canWrite())
            socket->writeData(buffer, PACKET_SIZE);

        auto t1 = std::chrono::high_resolution_clock::now();

        /* Receiving ECHO */
        if(socket->hasPendingData(10000)) {
            popData(socket->readData(), header);

            if(header.packetNum != i)
                cout << "Packet " << i << " has been lost! Last is " << header.packetNum  << endl;
            else {
                auto t2 = std::chrono::high_resolution_clock::now();

                //////////// A floating point milliseconds type///////////////
                using FpMilliseconds = std::chrono::duration <float, std::chrono::milliseconds::period>;
                static_assert(std::chrono::treat_as_floating_point <FpMilliseconds::rep>::value, "Floating error: ");

                auto rtt = FpMilliseconds(t2 - t1);

                cout << header.packetNum << ") RTT is: " << rtt.count() << " ms" << endl;
                timestamps.push_back(rtt.count());
            }
        }
    }

    cout << "2) Calculating stable regions..." << endl;
    getStableRegion(timestamps, outliers);

    cout << "3) Calculating OWD via average RTT / 2..." << endl;
    double owd_bar = owdBar(timestamps);

    cout << "4) Getting delta time via timestamp from server..." << endl;
    double deltaTs = 0;
    while(true)
    {
        header.packetNum = ++number;
        header.command = 'O';       // OWD command
        header.timeStamp = owd_bar;
        pushData(header, buffer);

        socket->writeData(buffer, PACKET_SIZE);

        high_resolution_clock tm;
        double TS1 = duration_cast<milliseconds> (tm.now().time_since_epoch()).count();

        cout.precision(20);
        cout << "Timestamp is:\t" << TS1 << endl;

        if(socket->hasPendingData(10000)) {
            popData(socket->readData(), header);

            deltaTs = TS1 - header.timeStamp;

            cout << "Received timestamp:\t" << header.timeStamp << endl;
            cout << "Delta T:\t" << deltaTs << " ms" << endl;

            header.command = 'F';
            pushData(header, buffer);
            socket->writeData(buffer, PACKET_SIZE);

            break;
        }
    }

    return deltaTs;
}



