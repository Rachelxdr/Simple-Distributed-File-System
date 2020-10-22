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
#include <queue>


#include "File.h"
#include "Logger.h"
#include "Member.h"
#include "Message.h"

#define T_period 500000 // in microseconds
#define T_timeout 3 // in T_period
#define T_cleanup 3 // in T_period
#define MASTER "172.22.158.5"
// #define MASTER "127.0.0.1"
#define MAX_NUM_TARGET 4
#define PORT "6000"
#define MAXBUFLEN 1024
#define FAIL 0
#define ACTIVE 1
using namespace std;

void* send_sock_create(void* node);
void* server_sock_create(void* node);
void receive_msg(void* node);
class Node {
    public:
        string self_member_id;
        Member self_member;
        int hb_counter;
        int local_time;
        time_t start_time;
        map<string,tuple<int, int, int> > mem_list; //member -> (hb_count, timestamp, fail_flag, role)
        bool is_master; // might not need this
        string master_id;
        Logger* node_logger;
        // vector<File> local_files;
        struct hostent* host;
        string node_mode;
        int bytes_received;
        int round;
        queue<string> qMessages;

        void activate();
        void join_system();
        string time_util();
        int get_message();
        void failure_detection();
        void update_mem_list();
        vector<string> get_gossip_targets();
        void send_pings(vector<string> targets);
        int get_time();
        void update_time();

        Node();
        
    private:
        string pack_membership_list();
        void send_message(string ip, string port, Message* message);
        void read_message(string msg);
        vector <string> splitString(string s, string delimiter);
        void process_hb(string message);
        struct hostent* get_host();

};

#endif