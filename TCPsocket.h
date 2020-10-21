#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <iostream> 
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <queue>

using namespace std;

#define MAXBUFLEN 1024
#define LOSS_RATE 0

class TCPsocket {
    public:
        unsigned long byte_sent;
        unsigned long byte_received;

        queue<string> messages;

        void bind_server(string port);
        void send_message(string ip, string port, string message);
        TCPsocket();
};

#endif