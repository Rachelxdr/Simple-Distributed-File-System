#ifndef RECORD_H
#define RECORD_H

#include <string>
#include <vector>

#define INIT "init"
#define PROCESS "process"
#define READY "ready"
#define DELETE "delete"

class Record {
    public:
        string file_name;
        string primary_replica_id;
        string backup_replica1_id;
        string backup_replica2_id;
        string backup_replica3_id;
        string status;
        // unsigned long size;

        Record();
        Record(string file_name, string primary_id);

};

#endif