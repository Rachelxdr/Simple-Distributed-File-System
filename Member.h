#ifndef MEMBER_H
#define MEMBER_H

#include <string>
#include <iostream>

using namespace std;
class Member {
    public:
        string ip_address;
        string port;
        int timestamp;
        int hb_counter;
        int failed_flag;
        Member();
        Member(string node_ip, string node_port, int node_time, int node_hb);
        Member(string node_ip, string node_port, int node_time);
        Member(string node_ip, string node_port);

        string make_member_id();

};


#endif