cd ../build/
./build.sh
cd ..
cd example
g++ example.cpp -Ofast -std=c++20 ../lib/PyDLL.o -o  example.elf
echo Done
