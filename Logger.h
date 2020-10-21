#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>

#define LOG_FILE "log.txt"

using namespace std;

class Logger {
    public:
        string log_file_name;

        Logger();
        void log_message(string message);
    
    private:
        void write_to_log(string s);

};


#endif