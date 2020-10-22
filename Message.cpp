#include "Message.h"


Message::Message() {
    message_type = "";
    message = "";
}

Message::Message(string msg_type) {
    message_type = msg_type;
    message = "";
}

Message::Message(string msg_type, string msg) {
    message_type = msg_type;
    message = msg;
}

string Message::make_str_msg(){
    string str_msg = this->message_type + "==" + this->message;
    
    // str_msg += this->message;
    cout <<"message to send: "<<str_msg<<endl;
    return str_msg;
}
