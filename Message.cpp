#include "Message.h"


Message::Message() {
    message_type = UNDEFINED;
    message = "";
}

Message::Message(MessageType msg_type) {
    message_type = msg_type;
    message = "";
}

Message::Message(MessageType msg_type, string msg) {
    message_type = message_type;
    message = msg;
}

string Message::make_str_msg(){
    string str_msg = "";
    switch (this->message_type) {
        case JOIN :
            str_msg = "JOIN";
            break;
        
    }
    str_msg += this->message;
    return str_msg;
}
