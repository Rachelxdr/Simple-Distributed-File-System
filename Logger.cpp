#include "Logger.h"


Logger::Logger() {
    log_file_name = LOG_FILE;
}

void Logger::write_to_log(string message){
    FILE *fp;
    fp = fopen(log_file_name.c_str(), "a");
    fprintf(fp, "%s",message.c_str());
    fclose(fp);
    return;
}

void Logger::log_message(string message) {
    write_to_log(message);
    return;
}