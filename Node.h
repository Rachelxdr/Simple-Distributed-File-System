#ifndef NODE_H
#define NODE_H

#include <string.h>
#include <map>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <pthread.h>
#include <time.h>
#include <tuple>
#include <ctime>


#include "File.h"
#include "Logger.h"
#include "Member.h"
#include "Message.h"

#define MASTER "10.192.118.174"
#define PORT "6000"
#define MAXBUFLEN 1024

using namespace std;

void* send_sock_create(void* node);
void* server_sock_create(void* node);
class Node {
    public:
        string self_member_id;
        Member self_member;
        int hb_counter;
        int local_time;
        time_t start_time;
        map<string,tuple<int, int, int> > mem_list; //member -> (hb_count, timestamp, fail_flag)
        bool is_master; // might not need this
        string master_id;
        Logger* node_logger;
        // vector<File> local_files;
        struct hostent* host;
        pthread_t send_thread;
        pthread_t receive_thread;
        string node_mode;

        void activate();
        int join_system(Member introducer);
        string time_util();
        Node();
        
    private:
        string pack_membership_list();
        void send_message(string ip, string port, Message* message);

        

        struct hostent* get_host();

};

#endif