#include "Node.h"


void* send_sock_create(void* node){
    Node* my_node = (Node*)node;
    my_node->join_system();
    while(my_node->node_mode == "active") {
        cout<<"[ROUND]: "<<my_node->round<<endl;
        cout<<"[TOTAL MEM]" << my_node->total_mem<<endl;
        my_node->round++;
        my_node->get_message();

        if (my_node->node_mode == "fail") {
            pthread_exit(NULL);
        }

        my_node->failure_detection();
        my_node->hb_counter++;
        // cout<<"local time before: "<<my_node->local_time<<endl;
        my_node->update_time();
        // cout<<"local time after: "<<my_node->local_time<<endl;
        my_node->update_mem_list();

        vector<string> targets = my_node->get_gossip_targets();
        my_node->send_pings(targets);
        usleep(T_period);
    }



    pthread_exit(NULL);

}




void* server_sock_create(void* node){
    receive_msg(node);
    pthread_exit(NULL);
    
}
void receive_msg(void* node){
    Node* my_node = (Node*) node;
    int socket_fd;
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    // hints.ai_socktype = SOCK_STREAM;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    
    int s = getaddrinfo(NULL, PORT, &hints, &result);
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

        my_node->bytes_received += num_bytes;
        buf[num_bytes] = '\0';
        my_node->qMessages.push(buf);
        
        struct sockaddr_in* src_addr_info = (struct sockaddr_in*)&src_addr;

        printf("message received from: %s\n", inet_ntoa(src_addr_info->sin_addr));
        string msg_to_log = my_node->time_util() + " Received " + to_string(num_bytes) + " bytes " + "from " + inet_ntoa(src_addr_info->sin_addr) + " : " + PORT "\n";
        my_node->node_logger->log_message(msg_to_log);

        bzero(buf, sizeof(buf));
    }
    close(socket_fd);
    // printf("%s", buf);

}
