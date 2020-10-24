#ifndef MASTER_RECORD_H
#define MASTER_RECORD_H

#include <string>
#include <vector>
#include <map>
#include "Record.h"

class Master_record{
    public:
        vector<Record*> file_entry;
        map<string, int> member_token;
        void process_put(string msg);
        Master_record();
};

#endif