#include <fstream>
#include <string>
#include <ctime>

class Logger
{
private:
    std::ofstream log_file;
    std::string log_level;
    std::string filename;

    std::string sep = "\t|";

public:

    Logger(const std::string &filename, const std::string level = "INFO") :
     filename(filename), log_level(level) {
        log_file.open(filename, std::ios::app);
    }

    ~Logger() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }

    void log(const std::string& message, const std::string& ip) {
        time_t ticks = time(NULL);
        log_file << ctime(&ticks) << sep
                 << message << sep
                 << ip << std::endl;
    }
};