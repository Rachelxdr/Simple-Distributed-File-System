#include "Member.h"

Member::Member(string node_ip, string node_port, int node_time, int node_hb){
    ip_address = node_ip;
    port = node_port;
    timestamp = node_time;
    hb_counter = node_hb;
    failed_flag = 0;
}

Member::Member(string node_ip, string node_port, int node_time){
    ip_address = node_ip;
    port = node_port;
    timestamp = node_time;
    hb_counter = 0;
    failed_flag = 0;
}

Member::Member(string node_ip, string node_port){
    ip_address = node_ip;
    port = node_port;
    timestamp = 0;
    hb_counter = 0;
    failed_flag = 0;
}

Member::Member(){
    ip_address = "";
    port = "";
    timestamp = 0;
    hb_counter = 0;
    failed_flag = 0;
}

string Member::make_member_id() {
    string id = ip_address + ":" + port + ":" + to_string(timestamp);
    return id;
}