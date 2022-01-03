# LoadPy
Load python modules (.py) in C++

# Build
### Requirements
[pyinstaller](https://pypi.org/project/pyinstaller/)

[MinGW](https://sourceforge.net/projects/mingw/) (if you use Windows)

- All the requirements must included in the PATH variable.

--------------------------
Navigate to LoadPy/build/

Use `build.sh` or `build.bat` depends on your system.

Output is located at LoadPy/lib/
    
# Example
Navigate to LoadPy/example/

Use `./build_example.sh` or `build_example.bat` depends on your system.

# How to use?

You can view example code in LoadPy/example/example.cpp

Declare a new PyEngine, argument is file path to python engine (PyEngine is created in LoadPy/lib/)
```c++
PyEngine engine("LoadPy/lib/Engine.elf")
```
Declare new PyInstance, argument is pointer to PyEngine
```c++
PyInstance python (&engine)
```
To import a module you need to call
```c++
PyModule m = PyInstance::import(modulepath)
```
Invalid import returns 0 as PyModule

Now you can call functions using
```c++
PyInstance::call(function name, arguments, module, max time function will wait for python response (in milliseconds))
```
- you can start or stop PyInstance using `PyInstance::start` and `PyInstance::kill`


PyInstance can throw *PythonException*

It has 3 possible sources, you can find them in
```c++
PythonException::source
```
To get exception text call `PythonException::what`
