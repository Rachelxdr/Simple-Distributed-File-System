#include "File.cpp"
#include "Logger.cpp"
#include "Node.cpp"
#include "Member.cpp"
#include "Thread.cpp"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc > 2) {
        cout << "Invalid format" <<endl;
        exit(1);
    }
    Node* my_node;
    if (argc == 1){
        my_node = new Node();
    } else {
        my_node = new Node(true);
    }

    string cmd;
    bool joined = false;
    while(1) {
        cin >> cmd;
        if (cmd == "join"){
            my_node->activate();
            joined = true;
        }

    }
}