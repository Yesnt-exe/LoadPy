@echo off
echo Run from build/ directory!
cd ../src/Python/
pyinstaller --onefile --noconsole Engine.py
cd ../../
mkdir lib
cd src/Python
move .\dist\Engine.exe ..\..\lib\Engine.exe
rmdir dist
rmdir build
rmdir __pycache__
del Engine.spec
cd ..
g++ PyDLL.cpp -Ofast -c --static -std=c++2a -o ../lib/PyDLL.o
cd ../build
