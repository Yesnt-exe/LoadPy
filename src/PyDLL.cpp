#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>
#include "../include/PyDLL.hpp"

#ifdef __unix__
#include <unistd.h>
#else
#include <Windows.h>
#endif


namespace fs = std::filesystem;

using namespace std;

void create(string name){
    FILE* o = fopen(name.c_str(), "w");
    if(!o){
        return;
    }
    fclose(o);
}

int start_process(string pFile){
    #ifdef __unix__
    fs::path fPath(pFile);
    if(!fs::exists(fPath)){
        return 0;
    }
    if(access(pFile.c_str(), X_OK)){
        return 0; // file cannot be executed
    }
    pid_t pForked = fork();
    if(pForked == -1){
        return 0;
    }
    else if(pForked == 0){
        if(execl(pFile.c_str(), "", NULL) == -1){
           exit(1); 
        }
    }
    else{
        return 1;
    }
    return 1;
    #else
    fs::path fPath(pFile);
    if(!fs::exists(fPath)){
        return 0;
    }
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    BOOL result = CreateProcessA(pFile.c_str(), "", NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
    if(!result){
        return 0;
    }
    else{
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return 1;
    }
    #endif
}



string Yesntlibs::PythonException::what(){
    return this->content;
}
Yesntlibs::PythonException::PythonException(source src, std::string info){
    this->content = info;
    this->causedby = src;
}

Yesntlibs::PyEngine::PyEngine(std::string enginepath){
    if(!fs::exists(enginepath)){
fail:
    
        this->enginepath = "";
        this->channelpath = "";
        this->lockpath = "";
        return;
    }
    if(!start_process(enginepath)){
        goto fail;
    }
    string directory;

    size_t reveresfind = enginepath.rfind('\\');
    if (reveresfind != string::npos)
    {
        directory = enginepath.substr(0, reveresfind);
    }
    else{
        directory = ".";
    }
    this->enginepath = enginepath;
    this->lockpath = directory + "/.pylock";
    this->channelpath = directory + "/.pychannel";
    return;
}

Yesntlibs::PyInstance::PyInstance(PyEngine * engine){
    if(!engine){
fail:
        return;
    }
    this->eng = engine;
    return;
}

void Yesntlibs::PyInstance::start(){
    if(!this->eng){
        return;
    }
    if(!this->eng->valid()){
        return;
    }
    if(this->check_running()){
        return;
    }
    int __IGNORE = start_process(this->eng->enginepath);
}

void Yesntlibs::PyInstance::kill(){
    if(!this->eng){
        return;
    }
    this->send_command("kill", "");
    auto t1 = chrono::high_resolution_clock::now();
    while (fs::exists(this->eng->lockpath))
    {
        this_thread::sleep_for(chrono::milliseconds(1));
        auto delay = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t1).count();
        if(delay >= 2000 ){
            break;
        }
    }
    return;
}

bool Yesntlibs::PyInstance::check_running(){
    if(!this->eng){
        return false;
    }
    if(this->eng->enginepath == ""){
        return false;
    }
    if(!this->send_command("check_running", "")){
        return false;
    }
    auto t1 = chrono::high_resolution_clock::now();
    while(fs::exists(this->eng->lockpath)){
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto dur = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t1);
        if(dur.count() >= 2500){
            remove(this->eng->lockpath.c_str());
            remove(this->eng->channelpath.c_str());
            return false; // waiting for more than 2.5 seconds, failed
        }
    }
    string cmd;
    string val;
    this->receive_command(cmd, val);
    if(cmd == "check_running" && val == "true"){
        return 1;
    }
    return 0;
}

int Yesntlibs::PyInstance::send_command(std::string command, std::string value){
    if(!this->eng){
        return 0;
    }
    if(fs::exists(this->eng->lockpath)){
        return 0;
    }
    string & channelpath = this->eng->channelpath;
    string & lockpath = this->eng->lockpath;
    ofstream writer;
    writer.open(channelpath, ofstream::out);
    writer << command << endl << value << endl;
    writer.close();
    create(lockpath.c_str());
    return 1;
}

int Yesntlibs::PyInstance::receive_command(std::string& command, std::string& value){
    if(!this->eng){
        return 0;
    }
    if(!fs::exists(this->eng->channelpath)){
        return 0;
    }
    ifstream reader(this->eng->channelpath, ifstream::in);
    if(!getline(reader, command)){
        command = "NULL";
    }
    if(!getline(reader, value)){
        command = "NULL";
    }
    reader.close();
    return 1;
}

Yesntlibs::PyInstance::~PyInstance(){
    this->kill();
}


Yesntlibs::PyModule Yesntlibs::PyInstance::import(string path){
    if(!fs::exists(path)){
        return 0;
    }
    if(!this->eng){
        return 0;
    }
    if(this->eng->channelpath == ""){
        return 0;
    }
    if(!this->send_command("import", fs::canonical(path).generic_string())){
        return 0;
    }
    auto t1 = chrono::high_resolution_clock::now();
    while(fs::exists(this->eng->lockpath)){
        this_thread::sleep_for(chrono::milliseconds(1));
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - t1).count();
        if(elapsed >= 3500){
            remove(this->eng->lockpath.c_str());
            remove(this->eng->channelpath.c_str());
            return 0;
        }
    }
    string chn;
    string val;
    if(!this->receive_command(chn, val)){
        return 0;
    }
    if(chn == "import_response" && val != "fail"){
        return (PyModule)atoi(val.c_str());
    }
    return 0;
}

bool Yesntlibs::PyEngine::valid(){
    return this->enginepath == "" ? false : fs::exists(this->enginepath);
}

Yesntlibs::PyEngine::~PyEngine(){
    remove(this->lockpath.c_str());
    remove(this->channelpath.c_str());
}

string Yesntlibs::PyInstance::call(std::string function, std::vector<string>args, PyModule m, int64_t call_waittime){
    if(!this->eng){
        throw PythonException(PythonException::PYENGINE, "Invalid PyEngine pointer!");
    }
    if(!this->check_running()){
        throw PythonException(PythonException::PYENGINE, "PyEngine is not running!");
    }
    string real = function + ":";
    for(size_t i = 0; i<args.size(); i++){
        if(i != 0){
            real += ";;";
        }
        real += args[i];
    }
    real += ":";
    real += to_string(m);
    if(!this->send_command("call", real)){
        return "";
    }
    else{
        auto start_time = chrono::high_resolution_clock::now();
        while(fs::exists(this->eng->lockpath)){
            this_thread::sleep_for(chrono::milliseconds(1));
            int64_t elapsed = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start_time).count();
            if(elapsed > call_waittime && call_waittime != -1 && call_waittime != 0){
                remove(this->eng->lockpath.c_str());
                throw PythonException(PythonException::source::SCRIPT, "Python took too long to respond!");
            }
        }
    }
    string val;
    string nm;
    if(!this->receive_command(nm, val)){
        throw PythonException(PythonException::source::INTERNAL, "Failed to recive command!");
    }
    return val;
}
