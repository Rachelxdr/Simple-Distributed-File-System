#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <iostream>


using namespace std;
class Message {
    public:
        string message_type;
        string message;

        Message();
        Message(string msg_type);
        Message(string msg_type, string msg);

        string make_str_msg(); 
};

#endif