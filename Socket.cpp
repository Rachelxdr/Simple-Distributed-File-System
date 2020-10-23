#include "Socket.h"

Socket::Socket() {
    byte_sent = 0;
    byte_received = 0;
}

void Socket::bind_server(string port){
    // Node* my_node = (Node*) node;
    int socket_fd;
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    // hints.ai_socktype = SOCK_STREAM;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    
    int s = getaddrinfo(NULL, port.c_str(), &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(1);
    }
    struct addrinfo *find_valid = result;

    while (find_valid != NULL) {
        if (socket_fd = socket(find_valid->ai_family, find_valid->ai_socktype, find_valid->ai_protocol) == -1) {
            perror("bindServer: socket");
            continue;
        }
        // size_t b = bind(socket_fd, find_valid->ai_addr, find_valid->ai_addrlen);
        
        if (int b = ::bind(socket_fd, find_valid->ai_addr, find_valid->ai_addrlen) < 0) {
            close(socket_fd);
            perror("bindServer: bind");
            continue;
        }
        find_valid = find_valid->ai_next;
        break;
    }
    
    if(find_valid == NULL) {
        fprintf(stderr, "failed to bind socket\n");
        exit(1);
    }
    
    freeaddrinfo(result);
    char buf[MAXBUFLEN];
    struct sockaddr_storage src_addr;
    socklen_t src_addr_len = sizeof(src_addr);
    bzero(buf, sizeof(buf));
    int num_bytes;
    cout <<"thread created" <<endl;
    cout<<"socket fd: "<<socket_fd<<endl;
    
    while((num_bytes = recvfrom(socket_fd, buf, MAXBUFLEN - 1 ,0, (struct sockaddr *)&src_addr,(socklen_t*)&src_addr_len)) > 0) {
        //design different data structure for membershiplist and files.

        this->byte_received += num_bytes;
        buf[num_bytes] = '\0';
        this->qMessages.push(buf);
        
        struct sockaddr_in* src_addr_info = (struct sockaddr_in*)&src_addr;

        printf("[RECEIVED MESSAGE]: message received: %s from %s\n", buf, inet_ntoa(src_addr_info->sin_addr));
        // string msg_to_log = my_node->time_util() + " Received " + to_string(num_bytes) + " bytes " + "from " + inet_ntoa(src_addr_info->sin_addr) + " : " + PORT "\n";
        // my_node->node_logger->log_message(msg_to_log);

        bzero(buf, sizeof(buf));
    }
    close(socket_fd);
    // printf("%s", buf);

}

void Socket::send_message(string ip, string port, string msg) {
    // If current node is master, it doesn't send the join information
    // if (msg_to_send->message_type == "JOIN" && this->is_master == true) {
    //     // cout << "master"<<endl;
    //     return;
    // }
    // 
    // string log_msg = this->time_util() + " " + this->self_member_id + " sent to " + MASTER + ":" + PORT + "\n";
    // this->node_logger->log_message(log_msg);
    // cout<<"target ip: "<<ip<<"target port: "<<port<<endl;
    
    int sock_fd;
    struct addrinfo hints, *servinfo, *p;
    int num_bytes;
    
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    // hints.ai_socktype = SOCK_STREAM;

    // cout << "calling get addr info"<<endl;
    int get_addr_info_ret = getaddrinfo(ip.c_str(), port.c_str(), &hints, &servinfo);
    if (get_addr_info_ret != 0) {
        cout << "get addr info error"<<endl;
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(get_addr_info_ret));
        return;
    }

    // cout << "calling socket"<<endl;
    for (p = servinfo; p != NULL; p = p->ai_next) {
        sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock_fd == -1) {
            perror("create send socket");
            continue;
        } 
        break;
    }

    // cout << "finished socket loop"<<endl;
    if (p == NULL) {
        // cout << "send socket not found"<<endl;
        perror("failed to bind send socket");
        return;
    }
    
    //process and send message
    
    struct sockaddr_in* result_addr = (struct sockaddr_in*) p->ai_addr;
    num_bytes = sendto(sock_fd, msg.c_str(), strlen(msg.c_str()), 0,p->ai_addr, p->ai_addrlen);
    
    cout <<"byte sent "<< num_bytes<<endl;
    if (num_bytes == -1){
        perror("error sending message");
        exit(1);
    }
    // cout<< "message sent: "<< msg <<endl;

    // Log sending information
    // string msg_to_log = this->time_util() + " " + "sent " + msg + " to  " + ip + ":" + port +"\n"; 
    // this->node_logger->log_message(msg_to_log);


    freeaddrinfo(servinfo);
    close(sock_fd);
}