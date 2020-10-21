#ifndef FILE_H
#define FILE_H

#include <fstream>
#include "Node.h"

using namespace std;

class File {
    public:

        string file_name;
        // vector<Node> replicas;
        ofstream file_stream;


        File(string file_name);
        

};

#endif