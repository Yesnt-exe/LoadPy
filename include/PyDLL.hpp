#include <stdio.h>
#include <string>
#include <vector>
#include <map>


namespace Yesntlibs{

typedef uint16_t PyModule;

class PythonException{
private:
    std::string content;
public:
    std::string what();
    enum source{
        SCRIPT = 0, PYENGINE = 1, INTERNAL = 2
    };
    source causedby;
    PythonException(source src, std::string info);
};

class PyEngine{
public:
    bool valid();
    std::string lockpath;
    std::string channelpath;
    std::string enginepath;
    PyEngine(std::string enginepath);
    ~PyEngine();
};

class PyInstance{
private:
    PyEngine * eng;
    int send_command(std::string command, std::string value);
    int receive_command(std::string& command, std::string& value);
public:
    PyInstance(PyEngine * engine);
    bool check_running();
    // 0 = failed import
    PyModule import(std::string path);
    std::string call(std::string function, std::vector<std::string>args, PyModule m, int64_t call_waittime = -1);
    void start();
    void kill();
    ~PyInstance();
};
};
