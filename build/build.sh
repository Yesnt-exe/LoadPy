cd ../src/Python/
pyinstaller --onefile --noconsole Engine.py
mv dist/Engine ../../lib/Engine.elf
rm dist -r
rm build -r
rm __pycache__ -r
rm Engine.spec
cd ..
g++ PyDLL.cpp -Ofast -c --static -std=c++20 -o ../lib/PyDLL.o
cd ../build/
