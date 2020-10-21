#include "Node.h"


void* send_sock_create(void* node){
    Node* my_node = (Node*)node;
    my_node->join_system();
    // while(my_node->node_mode == "active") {
        
    // }

}




void* server_sock_create(void* node){
    Node* my_node = (Node*) node;
    int socket_fd;
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
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
        bind(socket_fd, find_valid->ai_addr, find_valid->ai_addrlen);
        // if (b < 0) {
        //     perror("bindServer: bind");
        //     continue;
        // }
        break;
    }
    
    if(find_valid == NULL) {
        fprintf(stderr, "failed to bind socket\n");
        exit(1);
    }
    
    freeaddrinfo(result);
    char buf[MAXBUFLEN];
    struct sockaddr_storage src_addr;
    int src_addr_len = sizeof(src_addr);
    bzero(buf, sizeof(buf));
    int num_bytes;
    
    while((num_bytes = recvfrom(socket_fd, buf, MAXBUFLEN - 1 ,0, (struct sockaddr *)&src_addr,(socklen_t*)&src_addr_len)) > 0) {
        //design different data structure for membershiplist and files.
        
    }
    printf("%s", buf);

}
