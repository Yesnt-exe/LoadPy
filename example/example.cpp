#include "../include/PyDLL.hpp"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){
    try{
        cout << "Starting PyEngine" << endl;
        #ifdef __unix__
        Yesntlibs::PyEngine engine("../lib/Engine.elf");
        #else
        Yesntlibs::PyEngine engine("../lib/Engine.exe");
        #endif  
        cout << "Establishing communication with engine..." << endl;
        Yesntlibs::PyInstance python(&engine); 
        int eStatus = engine.valid();
        int pyStatus = python.check_running();
        cout << "Engine status : " << eStatus << "\nInstance status : " << pyStatus << endl;
        if(!eStatus || !pyStatus){
            cout << "Verificaiton failed!" << endl;
            return 1;
        } 
        cout << "Importing..." << endl;
        Yesntlibs::PyModule imported = python.import("./library.py");
        if(!imported){
            cout << "Failed to import module!" << endl;
            return 1;
        }
        cout << "Calling : " << python.call("tocall", {"a1", "a2"}, imported, 2500) << '\n';
        return 0;
    }
    catch(Yesntlibs::PythonException e){
        cout << "An exception has occured\n";
        cout << e.what() << endl;
        return 1;
    }
}  