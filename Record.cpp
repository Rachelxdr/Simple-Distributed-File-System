#include "Record.h"

Record::Record(){
    file_name = "";
    primary_replica_id = "";
    backup_replica1_id = "";
    backup_replica2_id = "";
    backup_replica3_id = "";
    status= INIT;
    size = 0;
}

Record::Record(string file_name, string primary_id){
    file_name = file_name;
    primary_replica_id = primary_id;
    backup_replica1_id = "";
    backup_replica2_id = "";
    backup_replica3_id = "";
    status= INIT;
    // size = 0;
}