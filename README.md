# LoadPy
Load python modules (.py) in C++

# Build
Requirements : 
    
- g++ compiler
 - python3.X.X with pyinstaller (pip install pyinstaller/sudo pip3 install pyinstaller)

---
 - Linux:
    
    Navigate to LoadPy/build/
    
        ./build.sh
    
    Output is located at LoadPy/lib/

- Windows:
    
    Im working on windows version
    
# Example

   Navigate to LoadPy/example/

- Linux
  
        ./build_example.sh

- Windows
    - Im working on windows version

# How to use?

You can view example code in LoadPy/example/example.cpp

Declare new PyEngine, argument is file path to python engine (PyEngine is created in LoadPy/lib/)

PyEngine engine("LoadPy/lib/Engine.elf")

Declare new PyInstance, argument is pointer to PyEngine

PyInstance python(&engine)

- To import a module you need to call

PyModule m = PyInstance::import(modulepath)

Invalid import returns 0 as PyModule

- now you can call functions using

PyInstance::call(function name,arguments, module, max time function will wait for python response (in milliseconds))

- you can start/stop PyInstance using PyInstance::kill and PyInstance::start


PyInstance can throw PythonException

It has 3 possible sources, you can find them in
PythonException::source

To get exception text call PythonException::what