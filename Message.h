#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <iostream>

#include "Message_type.h"

using namespace std;
class Message {
    public:
        MessageType message_type;
        string message;

        Message();
        Message(MessageType msg_type);
        Message(MessageType msg_type, string msg);

        string make_str_msg(); 
};

#endif