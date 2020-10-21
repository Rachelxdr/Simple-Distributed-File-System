#ifndef MESSAGESTYPES_H
#define MESSAGESTYPES_H

// enum MessageType {JOIN, HEARTBEAT, SWREQ, SWRESP, JOINRESPONSE};
enum MessageType {JOIN, MEM_INFO, QUERY, RESPONSE, UNDEFINED};
enum LogType {JOINGROUP, UPDATE, FAIL, LEAVE, REMOVE, GOSSIPTO, GOSSIPFROM, BANDWIDTH, MEMBERS};

#endif //MESSAGESTYPES_H