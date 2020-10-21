#include "Node.h"
// #include "Thread.cpp"
/*
struct hostent {
               char  *h_name;            // official name of host 
               char **h_aliases;         // alias list 
               int    h_addrtype;        // host address type 
               int    h_length;          // length of address 
               char **h_addr_list;       // list of addresses 
           }
*/

using namespace std;

Node::Node() {
    self_member_id = "";
    hb_counter = 0;
    local_time = 0;
    is_master = false;
    host = this->get_host();
    node_logger = new Logger();
    node_mode = "inactive";
     // need to get local ip address
    
}

string Node::pack_membership_list(){
    string mem_info = "";
    for (auto& element: this->mem_list) {
        string key = element.first;
        tuple <int, int, int> value = element.second;
        mem_info += key + to_string(get<0>(value)) + "," + to_string(get<1>(value)) + "," + to_string(get<2>(value));
    }
    return mem_info;
}

void Node::send_message(string ip, string port, Message* msg_to_send) {
    if (msg_to_send->message_type == JOIN && this->is_master == true) {
        return;
    }
    string log_msg = this->time_util() + " " + this->self_member_id + " sent to " + MASTER + ":" + PORT + "\n";
    this->node_logger->log_message(log_msg);
    int sock_fd;
    struct addrinfo hints, *servinfo, *p;
    int num_bytes;
    
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int get_addr_info_ret = getaddrinfo(ip.c_str(), port.c_str(), &hints, &servinfo);
    if (get_addr_info_ret != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(get_addr_info_ret));
        return;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock_fd == -1) {
            perror("create send socket");
            continue;
        } 
        break;
    }
    
    if (p == NULL) {
        perror("failed to bind send socket");
        return;
    }
    string msg = msg_to_send->make_str_msg();
    num_bytes = send(sock_fd, msg.c_str(), strlen(msg.c_str()), 0);

    if (num_bytes == -1){
        perror("error sending message");
    }
    cout<< "message sent: "<< msg <<endl;
    string msg_to_log = this->time_util() + " " + "sent " + msg + " to  " + MASTER + ":" + PORT +"\n"; 
    this->node_logger->log_message(msg_to_log);
    freeaddrinfo(servinfo);
    close(sock_fd);
    return;

}

string Node::time_util() {
    time_t result = time(nullptr);
    return asctime(localtime(&result));
}


struct hostent* Node::get_host(){
    char host[100] = {0};
    struct hostent *hp;
    if (gethostname(host, sizeof(host)) < 0){
        cout << "error: gethostname" <<endl;
        return 0;
    }

    if ((hp = gethostbyname(host)) == NULL) {
        cout << "error: gethostbyname" <<endl;
        return 0;
    }

    if (hp -> h_addr_list[0] == NULL) {
        cout << "error: no IP" << endl;
        return 0;
    }
    return hp;
}

void Node::join_system(){
    Member own(inet_ntoa(*(struct in_addr*)this->host->h_addr_list[0]), PORT, this->local_time, this->hb_counter);
    this->self_member = own;
    this->node_mode = "active";
    time(&this->start_time);
    this->self_member_id = own.ip_address + ":" + PORT + ":" + to_string(this->start_time);

    if (this->is_master){
        cout<<own.ip_address<<endl;
    }
    if (is_master) {
        this->master_id = self_member_id;
    }
    string message_to_log = this->time_util() + " JOIN: " + this->self_member_id + "\n";
    this->node_logger->log_message(message_to_log);
    // Ping master so that other members know
    Member master(MASTER, PORT);
    string mem_info = pack_membership_list();
  
    Message* msg_to_send = new Message(JOIN, mem_info);
    send_message(MASTER, PORT, msg_to_send);
}


void Node::activate(){

    int send_thread_ret = pthread_create(&this->send_thread, NULL, send_sock_create, (void*)this);
    

   
    int recv_thread_ret = pthread_create(&this->receive_thread, NULL, server_sock_create, (void*)this);
    
}


int main(int argc, char* argv[]) {
    if (argc != 1 && argc != 2) {
        std::cout << "Invalid format" <<endl;
        exit(1);
    }
    cout<< "running" <<endl;
    Node* my_node = new Node();

    if (argc != 1){
        my_node->is_master = true;
    }

    string cmd;
    bool joined = false;
    while(1) {
        std::cin >> cmd;
        if (cmd == "join"){
            my_node->activate();
            joined = true;
        }

    }
    pthread_join(my_node->send_thread, NULL);
    pthread_join(my_node->receive_thread, NULL);
}