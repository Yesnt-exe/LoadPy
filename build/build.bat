cd ../src/Python/
pyinstaller --onefile --noconsole Engine.py
mkdir ../../lib/
move dist/Engine ../../lib/Engine.elf
del dist
del build
del __pycache__
del Engine.spec
cd ..
g++ PyDLL.cpp -Ofast -c --static -std=c++20 -o ../lib/PyDLL.o
cd ../build/
