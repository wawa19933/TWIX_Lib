//============================================================================
// Name        : UDP_Server.cpp
// Author      : Artem Kirkov
// Version     : beta 1-1.02
// Copyright   : All rights reserved
// Description : Packets retranslator
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
#include <vector>
#include <chrono>
#include "socket.h"
#include "myutils.h"

using namespace std;
using namespace std::chrono;

void synchronizeClocks(Socket *socket);
void ts1Bar(Socket *socket);

int main()
{
    Socket *socket = new Socket(Socket::Server);
    socket->bindToPort();

    cout << " going to receive\n";

    while(1)
    {
        synchronizeClocks(socket);
/*
/////////////////////////////////   OWD server side     //////////////////////////////
vector<double> vecOwd (3);
vector<double> real_owd;
double Delta_TS = 0;
double tstamp1;
int counter = 0;
int Loss_packet = 0;
int Recv_packet = 0;
int Numbr_packet;
high_resolution_clock time_stamp;
for(int i=0;i<100;++i) {

double OWD = 0;
int recv= recvfrom(s, &vecOwd[0], sizeof(vecOwd), 0, (struct sockaddr *)&client, &l);
    if (recv<0)
            {
                cout << "Error reading from socket" << endl;
            }

        auto t_stamp = time_stamp.now();
        double tstamp2 = duration_cast<milliseconds>(t_stamp.time_since_epoch()).count();
        ++counter;
        tstamp1 = vecOwd.front();
        Delta_TS = vecOwd[1];
        Numbr_packet = vecOwd.back();
if (Numbr_packet != counter)
        {

             Loss_packet    ++;

        }
else
        {

            Recv_packet ++;

        }

if (Delta_TS < 0)
        {

             OWD = (tstamp2-tstamp1) - Delta_TS;
        }
else
        {
            OWD = (tstamp2-tstamp1) + Delta_TS;
        }

    real_owd.push_back(OWD);
//        cout << "TimeStamp: " << tstamp1 << endl;
//         cout << "Delta TS: " << Delta_TS << endl;

        if(sendto(s,&real_owd[0],sizeof(real_owd),0,(struct sockaddr *)&client,l)<0)
                    {
                        perror("Sending error\n");
                    }
        cout << "OWD: " << OWD << "\tms" << endl;
        vector<double>().swap(real_owd);
}
            cout << "Received packets: " << Recv_packet << endl;
            cout << "Lost packets: "    << Loss_packet << endl;
*/
    }

    return 0;
}

void ts1Bar(Socket *socket)
{
    Socket::Header header;
    char *buffer = new char[PACKET_SIZE];

    cout << "2) Sending timestamp for delta time calculation" << endl;
    while(true)
    {
        if(socket->hasPendingData(10000)) {
            popData(socket->readData(), header);

            if(header.command == 'O') {
                high_resolution_clock TimeStamp;
                double ts2 = duration_cast<milliseconds>(TimeStamp.now().time_since_epoch()).count();
                double ts_bar = ts2 - header.timeStamp;

                header.timeStamp = ts_bar;
                pushData(header, buffer);
                socket->writeData(buffer, PACKET_SIZE);
            }
            else if(header.command == 'F')
                return;
        }
    }
}

void synchronizeClocks(Socket *socket)
{
    struct Socket::Header header;
    char *buffer = new char[PACKET_SIZE];

    cout << "1) Echo for RTT..." << endl;
    for(;;)
    {
        if(socket->hasPendingData(0)) {
            popData(socket->readData(), header);

            if(header.command == 'F')
                break;

            cout << "Packet " << header.packetNum << " has been received from " << socket->peerHostIp() << endl;

            int written = 0;
            pushData(header, buffer);
            written = socket->writeData(buffer, PACKET_SIZE);

            cout << written << " bytes are sent!" << endl;
        }
    }

    ts1Bar(socket);
}




