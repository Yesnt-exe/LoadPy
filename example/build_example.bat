@echo off
echo Building libraries...
cd ../build

cd ../src/Python/
pyinstaller --onefile --noconsole Engine.py
cd ../../
mkdir lib
cd src/Python
move .\dist\Engine.exe ..\..\lib\Engine.exe
del dist
del build
del __pycache__
del Engine.spec
cd ..
g++ PyDLL.cpp -Ofast -c --static -std=c++2a -o ../lib/PyDLL.o
cd ../example
echo Building example...
cd ..
cd example
g++ example.cpp -Ofast -std=c++2a ../lib/PyDLL.o -o example.exe
echo Done
