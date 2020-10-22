#include "Node.h"
using namespace std;

Node::Node() {
    self_member_id = "";
    hb_counter = 0;
    local_time = 0;
    is_master = false;
    // host = this->get_host();
    node_logger = new Logger();
    node_mode = "inactive";
    bytes_received = 0;
     // need to get local ip address
    
}

string Node::pack_membership_list(){
    string mem_info = "";
    for (auto& element: this->mem_list) {
        string key = element.first;
        tuple <int, int, int> value = element.second;
        mem_info += key+ "," + to_string(get<0>(value)) + "," + to_string(get<1>(value)) + "," + to_string(get<2>(value)) + "," + this->master_id + ";";
    }
    return mem_info;
}


int Node::get_message() {
    queue<string> all_message(this->qMessages);
    for (int i = 0; i < all_message.size(); i++) {
        read_message(all_message.front());
    }
    
}

void Node::send_message(string ip, string port, Message* msg_to_send) {
    // If current node is master, it doesn't send the join information
    if (msg_to_send->message_type == "JOIN" && this->is_master == true) {
        cout << "master"<<endl;
        return;
    }
    // 
    // string log_msg = this->time_util() + " " + this->self_member_id + " sent to " + MASTER + ":" + PORT + "\n";
    // this->node_logger->log_message(log_msg);

    
    int sock_fd;
    struct addrinfo hints, *servinfo, *p;
    int num_bytes;
    
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

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
    string msg = msg_to_send->make_str_msg();
    // cout << "sending message" << endl;
    num_bytes = sendto(sock_fd, msg.c_str(), strlen(msg.c_str()), 0,p->ai_addr, p->ai_addrlen);
    
    cout <<"byte sent "<< num_bytes<<endl;
    if (num_bytes == -1){
        perror("error sending message");
    }
    // cout<< "message sent: "<< msg <<endl;

    // Log sending information
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

vector<string> Node::splitString(string s, string delimiter) {
    vector<string> result;
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;

	while ((pos_end = s.find (delimiter, pos_start)) != string::npos) {
		token = s.substr (pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		result.push_back (token);
	}

	result.push_back (s.substr (pos_start));
	return result;

}


void Node::process_hb(string message) {
    vector<string> mem_info = splitString(message, ";"); //id, hb, time, flag, masterid
    for (string mem: mem_info) {
        if (mem.size() == 0) {
            continue;
        }
        vector<string> elem_list = splitString(mem, ",");
        string id = elem_list[0];
        int hb = stoi(elem_list[1]);
        int time = stoi(elem_list[2]);
        int flag = stoi(elem_list[3]);
        string mas_id = elem_list[4];

        // information of myself
        if (id.compare(this->self_member_id) == 0) {
            // kill myself if detected as failed by others
            if (flag == FAIL) {
                // suicide
                this->node_mode = "fail";
                cout << "Good bye world" <<endl;
                string msg_to_log = this->time_util() + " At " + to_string(this->local_time) + this->self_member_id + " Committed Suicide \n";
                this->node_logger->log_message(msg_to_log);
                return;
            } 
            // update master information
            if (mas_id.compare(this->master_id) != 0) {

                cout << "Update master id " << this->master_id << " to " << mas_id <<endl;
                string msg_to_log = this->time_util() + " At " + to_string(this->local_time) + "Update master id " + this->master_id + " to " + mas_id + "\n";
                this->node_logger->log_message(msg_to_log);
                this->master_id = mas_id;

            }
            continue;
        } 
        map<string, tuple<int, int, int>>::iterator it;
        it = this->mem_list.find(id);
        // new member
        if (it == this->mem_list.end() && flag == ACTIVE) {
            this->mem_list[id] = make_tuple(hb, this->local_time, flag);
        
            cout << "New member: "<< id << " at " << this->local_time <<endl;
            string msg_to_log = this->time_util() + " At " + to_string(this->local_time) + id + " joined membership list \n";
            this->node_logger->log_message(msg_to_log);
        } else if (it != this->mem_list.end()) {
            tuple <int, int, int> mem_to_change = this->mem_list[id];
            if (get<2>(mem_to_change) == ACTIVE) {
                if (flag == FAIL) {
                    get<2>(mem_to_change) = FAIL;
                    get<1>(mem_to_change) = this->local_time;
                    cout << "Member received as failed "<< id << " at " << this->local_time <<endl;
                    string msg_to_log = this->time_util() + " At " + to_string(this->local_time) + id + " received as failed \n";
                    this->node_logger->log_message(msg_to_log);
                } else {
                    int current_hb = get<0>(mem_to_change);
                    if (hb > current_hb) {
                        current_hb = hb;
                        get<1>(mem_to_change) = this->local_time;
                        string msg_to_log = this->time_util() + " At " + to_string(this->local_time) + " update hb counter " + id + "\n";
                        this->node_logger->log_message(msg_to_log);
                    }

                }
            }
        }

    }
    return;

    
}


// message format: "TYPE==message"
void Node::read_message(string msg){

    vector<string> splited_msg = splitString(msg, "==");
    string type = splited_msg[0];
    string message = splited_msg[1]; // id, hb, time, flag, masterid; id, hb, time, flag, masterid...

    // if receives "JOIN" message, current node is master
    if (type == "JOIN") {
        vector<string> other_info = splitString(message, ",");
        string other_id = other_info[0];
        vector<string> new_mem_info = splitString(other_id, ":");
        string new_ip = new_mem_info[0];
        string new_port = new_mem_info[1];
        string mem_info = pack_membership_list();
        Message* msg_to_send = new Message("PING", mem_info);
        send_message(new_ip, new_port, msg_to_send);

    } else if(type == "PING") {
        process_hb(message);

    }

}

void Node::join_system(){
    
    this->node_mode = "active";
    
    //Log join info
    string message_to_log = this->time_util() + " JOIN: " + this->self_member_id + "\n";
    this->node_logger->log_message(message_to_log);

    // Ping master so that other members know the ndoe join
    Member master(MASTER, PORT);
    string mem_info = pack_membership_list();
    cout << "mem_info: "<<mem_info<<endl;
    Message* msg_to_send = new Message("JOIN", mem_info);
    cout << "type: "<< msg_to_send->message_type <<endl;
    
    //send to master
    send_message(MASTER, PORT, msg_to_send);
    return;
}


void Node::activate(){
    // create send thread
    int send_thread_ret = pthread_create(&this->send_thread, NULL, send_sock_create, (void*)this);
    

   
    // int recv_thread_ret = pthread_create(&this->receive_thread, NULL, server_sock_create, (void*)this);
    
}


int main(int argc, char* argv[]) {
    if (argc != 1 && argc != 2) {
        std::cout << "Invalid format" <<endl;
        exit(1);
    }
    cout<< "running" <<endl;
    // create a node
    Node* my_node = new Node();
    
    // Assign master
    if (argc != 1){
        my_node->is_master = true;
        cout <<"master: "<< my_node->is_master <<endl;
    }
    // set up host name for current node
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
    my_node->host = hp;


    //create node self member
    Member own(inet_ntoa(*(struct in_addr*)my_node->host->h_addr_list[0]), PORT, my_node->local_time, my_node->hb_counter);
    cout <<"debug" <<endl;
    my_node->self_member = own;

    // create node receive thread
    int recv_thread_ret = pthread_create(&my_node->receive_thread, NULL, server_sock_create, (void*)my_node);
    
    //create node member id
    time(&my_node->start_time);
    my_node->self_member_id = own.ip_address + ":" + PORT + ":" + to_string(my_node->start_time);
    my_node->mem_list[my_node->self_member_id] = make_tuple(my_node->hb_counter, my_node->local_time, ACTIVE);
    if (my_node->is_master) {
        my_node->master_id = my_node->self_member_id;
    }
    // read inputs and process
    string cmd;
    bool joined = false;
    while(1) {
        std::cin >> cmd;
        if (cmd == "join"){
            my_node->activate();

        }
        pthread_join(my_node->send_thread, NULL);
        pthread_join(my_node->receive_thread, NULL);

    }
    
}