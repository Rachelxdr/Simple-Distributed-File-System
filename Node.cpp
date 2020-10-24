#include "Node.h"
using namespace std;

Node::Node() {
    self_member_id = "";
    hb_counter = 0;
    local_time = 0;
    is_master = false;
    // host = this->get_host();
    node_logger = new Logger();
    node_mode = INACTIVE_NODE;
    bytes_received = 0;
    round = 0;
     // need to get local ip address
    
}

string Node::pack_membership_list(){
    string mem_info = "";
    for (auto& element: this->mem_list) {
        string key = element.first;
        tuple <int, int, int> value = element.second;
        mem_info += key+ "||" + to_string(get<0>(value)) + "," + to_string(get<1>(value)) + "," + to_string(get<2>(value)) + ";";
    }
    return mem_info;
}


int Node::get_message() {
    queue<string> all_message(this->qMessages);
    this->qMessages = queue<string>();
    int size = all_message.size();
    for (int i = 0; i < size; i++) {
        // cout<<"cur message"<<all_message.front()<<endl;
        read_message(all_message.front());
        all_message.pop();
    }

    return 0;
}

vector<string> Node::get_gossip_targets(){
    vector<string> all_members;
    for (auto& element: this->mem_list){
        string mem_id = element.first;
        int mem_flag = get<2>(element.second);
        if (mem_id != this->self_member_id && mem_flag == ACTIVE){
            all_members.push_back(mem_id);
        }

    }

    if (all_members.size() <= MAX_NUM_TARGET) {
        return all_members;
    }
    vector<string> chosen_members;
    for (int i = 0; i <= MAX_NUM_TARGET; i++) {
        int random_num = rand() % all_members.size();
        chosen_members.push_back(all_members[random_num]);
        all_members.erase(all_members.begin() + random_num);
    }
    return chosen_members;
    

}

void Node::send_pings(vector<string> targets) {
    string mem_info = pack_membership_list();
    // cout<< "mem_info" <<mem_info <<endl;
    // cout<<"time in mem_list: "<<to_string(get<1>(this->mem_list[this->self_member_id])) <<endl;

    Message* msg_to_send = new Message("PING", mem_info);
    for(string target_id : targets) {
        vector<string> id_info = splitString(target_id, ":");
        string target_ip = id_info[0];
        string target_port = id_info[1];
        send_message(target_ip, target_port, msg_to_send);
    }

}

void Node::receive_msg(){
    // Node* my_node = (Node*) node;
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
    // cout <<"thread created" <<endl;
    // cout<<"socket fd: "<<socket_fd<<endl;
    
    while((num_bytes = recvfrom(socket_fd, buf, MAXBUFLEN - 1 ,0, (struct sockaddr *)&src_addr,(socklen_t*)&src_addr_len)) > 0) {
        //design different data structure for membershiplist and files.

        this->bytes_received += num_bytes;
        buf[num_bytes] = '\0';
        this->qMessages.push(buf);
        
        struct sockaddr_in* src_addr_info = (struct sockaddr_in*)&src_addr;

        // printf("[RECEIVE]: message received from: %s\n", inet_ntoa(src_addr_info->sin_addr));
        string msg_to_log = this->time_util() + "[RECEIVE]: Received " + to_string(num_bytes) + " bytes " + "from " + inet_ntoa(src_addr_info->sin_addr) + " : " + PORT "\n";
        this->node_logger->log_message(msg_to_log);

        bzero(buf, sizeof(buf));
    }
    close(socket_fd);
    // printf("%s", buf);

}

void Node::send_message(string ip, string port, Message* msg_to_send) {
    // If current node is master, it doesn't send the join information
    if (msg_to_send->message_type == "JOIN" && this->is_master == true) {
        // cout << "master"<<endl;
        return;
    }
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
    string msg = msg_to_send->make_str_msg();
    struct sockaddr_in* result_addr = (struct sockaddr_in*) p->ai_addr;
    num_bytes = sendto(sock_fd, msg.c_str(), strlen(msg.c_str()), 0,p->ai_addr, p->ai_addrlen);
    
    cout <<"byte sent "<< num_bytes<<endl;
    if (num_bytes == -1){
        perror("error sending message");
    }
    // cout<< "message sent: "<< msg <<endl;

    // Log sending information
    cout <<"[SEND]: to" << ip << ":"<<port<<endl;
    string msg_to_log = this->time_util() + " " + "[SEND]: sent to  " + ip + ":" + port +"\n"; 
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

void Node::failure_detection(){
    vector<string> to_remove;
    for (auto& mem : this->mem_list) {
        string mem_id = mem.first;
        int mem_hb = get<0>(mem.second);
        int mem_time = get<1>(mem.second);
        int mem_flag = get<2>(mem.second);
        // myself, skip

        if (mem_id.compare(this->self_member_id) == 0) {
            continue;
        }
        if (mem_flag == ACTIVE) {
            if (this->local_time - mem_time > T_timeout) {
                tuple <int, int, int> new_info(mem_hb, this->local_time, FAIL);
                this->mem_list[mem_id] = new_info;
                cout << "[FAILURE DETECTION]: member "<< mem_id <<" detected as failed "<<endl;
                string msg_to_log = this->time_util() + " " + mem_id + "[FAILURE DETECTION]: detected as fail at local time " + to_string(this->local_time) + "\n";
                this->node_logger->log_message(msg_to_log);
            }
        } else {
            if (this->local_time - mem_time > T_cleanup) {
                auto it = this->mem_list.find(mem_id);
                if (it != this->mem_list.end()) {
                   
                    to_remove.push_back(mem_id);
                }
            }
        }
        for (uint i = 0; i < to_remove.size(); i++) {
            auto it = this->mem_list.find(to_remove[i]);
            if (it != this->mem_list.end()) {
                cout << "[REMOVE]: Removed " + mem_id + " at local time " + to_string(this->local_time) << endl;
                string msg_to_log = this->time_util() + "[REMOVE]: removed " + mem_id + " at local time " + to_string(this->local_time) + "\n";
                this->node_logger->log_message(msg_to_log);
                this->mem_list.erase(it);
            }
        }
    }
} 

void Node::update_mem_list(){
    tuple <int, int, int> new_self_info(this->hb_counter, this->local_time, ACTIVE);
    this->mem_list[this->self_member_id] = new_self_info;
}

void Node::process_hb(string message) {
    vector<string> mem_info = splitString(message, ";"); //id, hb, time, flag, masterid
    for (string mem: mem_info) {
        if (mem.size() == 0) {
            continue;
        }
        // cout<<"mem: "<<mem<<endl;
        vector<string> elem_entry = splitString(mem, "||");
        string id = elem_entry[0];
        // cout<<"id: "<<id<<endl;
        string mem_count = elem_entry[1];
        // cout <<"mem_count: "<<mem_count<<endl;
        vector<string> elem_list = splitString(mem_count, ",");
        int hb = stoi(elem_list[0]);
        int time = stoi(elem_list[1]);
        int flag = stoi(elem_list[2]);
        // string mas_id = elem_list[4];
        // cout <<"finished splitting" <<endl;
        // information of myself
        if (id.compare(this->self_member_id) == 0) {
            // kill myself if detected as failed by others
            if (flag == FAIL) {
                // suicide
                this->node_mode = FAILED_NODE;
                cout << "Good bye world" <<endl;
                string msg_to_log = this->time_util() + "[SUICIDE]: At " + to_string(this->local_time) + this->self_member_id + " Committed Suicide \n";
                this->node_logger->log_message(msg_to_log);
                return;
            } 
            // update master information
            // if (this->is_master == false && mas_id.compare(this->master_id) != 0) {

            //     cout << "Update master id " << this->master_id << " to " << mas_id <<endl;
            //     string msg_to_log = this->time_util() + " At " + to_string(this->local_time) + "Update master id " + this->master_id + " to " + mas_id + "\n";
            //     this->node_logger->log_message(msg_to_log);
            //     this->master_id = mas_id;

            // }
            continue;
        } 
        map<string, tuple<int, int, int>>::iterator it;
        it = this->mem_list.find(id);
        // new member
        if (it == this->mem_list.end() && flag == ACTIVE) {
            this->mem_list[id] = make_tuple(hb, this->local_time, flag);
            this->total_mem++;
            cout << "[NEW MEMBER]: "<< id << " at " << this->local_time <<endl;
            string msg_to_log = this->time_util() + " At " + to_string(this->local_time) + id + " joined membership list \n";
            this->node_logger->log_message(msg_to_log);
        } else if (it != this->mem_list.end()) {
            tuple <int, int, int> mem_to_change = this->mem_list[id];
            if (get<2>(mem_to_change) == ACTIVE) {
                if (flag == FAIL) {
                    if (id == this->master_id) {

                        cout <<"[FAILED INFO MESSAGE]: master failed" <<endl;


                    } else{
                        tuple <int, int, int> updated_info(hb, this->local_time, FAIL);
                        this->mem_list[id] = updated_info;
                        cout << "[FAILED INFO MESSAGE]: Member received as failed "<< id << " at " << this->local_time <<endl;
                        string msg_to_log = this->time_util() + "[RECEIVED FAIL]: At " + to_string(this->local_time) + id + " received as failed \n";
                        this->node_logger->log_message(msg_to_log);
                    }
                    
                } else {
                    int current_hb = get<0>(mem_to_change);
                    if (hb > current_hb) {
                        tuple <int, int, int> updated_info(hb, this->local_time, ACTIVE);
                        this->mem_list[id] = updated_info;
                        string msg_to_log = this->time_util() + "[UPDATE HB]: At " + to_string(this->local_time) + " update hb counter " + id + "\n";
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
    // cout <<"type: "<<type<<endl;
    // if receives "JOIN" message, current node is master
    // cout<<"finished parsing"<<endl;
    if (type == "JOIN") {
        vector<string> other_info = splitString(message, ",");
        string other_id = other_info[0];
        vector<string> new_mem_info = splitString(other_id, ":");
        string new_ip = new_mem_info[0];
        string new_port = new_mem_info[1];
        string mem_info = pack_membership_list();
        Message* msg_to_send = new Message("PING", mem_info);
        // cout<<"new_ip "<<new_ip<<" new_port "<<new_port<<endl;
        send_message(new_ip, new_port, msg_to_send);

    } else if(type == "PING") {
        process_hb(message);

    }

}

int Node::get_time() {
    time_t time_buf;
    time(&time_buf);
    return static_cast<int>(time_buf);
}

void Node::update_time() {
    time_t time_buf;
    time(&time_buf);
    this->local_time = static_cast<int>(time_buf);
}

void Node::join_system(){
    
    this->node_mode = ACTIVE_NODE;
    
    //Log join info
    this->total_mem = 1;
    this->local_time = get_time();
    string message_to_log = this->time_util() + "[JOIN]: JOIN: " + this->self_member_id + "\n";
    this->node_logger->log_message(message_to_log);

    // Ping master so that other members know the ndoe join
    Member master(MASTER, PORT);
    string mem_info = pack_membership_list();
    // cout << "mem_info: "<<mem_info<<endl;
    Message* msg_to_send = new Message("JOIN", mem_info);
    // cout << "type: "<< msg_to_send->message_type <<endl;
    
    //send to master
    if (this->is_master == false) {
        send_message(MASTER, PORT, msg_to_send);
    }
    return;
}

void Node::show_members(){
    string mem_str = "===========================  MEMBERSHIP INFORMATION ===========================\n";
    for (auto& elem : this->mem_list) {
        string cur_mem_info = "";
        string mem_id = elem.first;
        tuple <int, int, int> mem_info = elem.second;
        cur_mem_info = "Member ID: " + mem_id + "hb_counter: " + to_string(get<0>(mem_info)) + "local_timestamp: " + to_string(get<1>(mem_info)) +  "flag: ";
        if (get<2>(mem_info) == FAIL) {
            cur_mem_info += "FAIL\n";
        } else {
            cur_mem_info += "ACTIVE\n";
        }
        mem_str += cur_mem_info;

    }
    mem_str += "===============================================================================";
    cout << mem_str << endl;
}


int main(int argc, char* argv[]) {
    pthread_t send_thread;
    pthread_t receive_thread;
    if (argc != 1 && argc != 2) {
        std::cout << "Invalid format" <<endl;
        exit(1);
    }
    // cout<< "running" <<endl;
    // create a node
    Node* my_node = new Node();
    
    // Assign master
    if (argc != 1){
        my_node->is_master = true;
        // cout <<"master: "<< my_node->is_master <<endl;
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
    // cout <<"debug" <<endl;
    my_node->self_member = own;

    // create node receive thread
    int recv_thread_ret = pthread_create(&receive_thread, NULL, server_sock_create, (void*)my_node);
    if (recv_thread_ret != 0) {
            cout << "Error:unable to create thread," << recv_thread_ret << endl;
            exit(-1);
    }
    //create node member id
    time(&my_node->start_time);
    my_node->self_member_id = own.ip_address + ":" + PORT + ":" + to_string(my_node->start_time);
    my_node->mem_list[my_node->self_member_id] = make_tuple(my_node->hb_counter, my_node->local_time, ACTIVE);
    if (my_node->is_master) {
        my_node->master_id = my_node->self_member_id;
    } else {
        my_node->reelect_token = my_node->get_time() + rand() % 100;
    }
    // read inputs and process
    string cmd;
    bool joined = false;
    int *ret;
    while(1) {
        cin >> cmd;
        if (cmd == "join"){
            int send_thread_ret = pthread_create(&send_thread, NULL, send_sock_create, (void*)my_node);
            if (send_thread_ret != 0) {
                cout << "Error:unable to create thread," << send_thread_ret << endl;
				exit(-1);
            }
            joined = true;
        } else if (cmd == "leave") {
            if (joined) {
                my_node->node_mode = INACTIVE_NODE;
                
                pthread_join(send_thread, (void**) ret);

                cout <<"At "<<my_node->local_time<<" "<< my_node->self_member_id<<" is leaving"<<endl;
                string msg_to_log = my_node->time_util() + " [LEAVE]:" + my_node->self_member_id + " left group\n";
                my_node->node_logger->log_message(msg_to_log);
                sleep(2);
                joined = false;
            }
        } else if (cmd == "id") {
            cout << "@@@@@ Node ID: "<< my_node->self_member_id<<endl;
        } else if (cmd == "member") {
            my_node->show_members();
        }
       cout<<cmd<<endl;
    //    cout.flush();
        // pthread_join(my_node->receive_thread, NULL);

    }
    pthread_kill(receive_thread, SIGUSR1);
    if (joined) {
        pthread_kill(send_thread, SIGUSR1);
    }
    pthread_exit(NULL);
    return 0;
    
}