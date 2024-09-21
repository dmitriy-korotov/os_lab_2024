cd lab2
ls
clear
cd src/
clear
cd swap/
clear
gcc -o swap main.c swap.
gcc -o swap main.c swap.c
./swap
clear
./swap
cd ../revert_string/
cler
clear
gcc -o revert revert_string.c main.c ../swap/swap.c -I../swap
./revert
./revert string
clear
ls
clear
gcc -o revert.o revert_string.c ../swap/swap.c -I../swap
gcc -c revert_string.c ../swap/swap.c -I../swap -o revert.o
gcc -c revert_string.c ../swap/swap.c -I../swap
ls
ar rcs swap.o revert_string.o -o librevert.a
ar rcs librevert.a swap.o revert_string.o
gcc -o revert_static main.c -L. -lrevert
./revert_static 
./revert_static string
clear
gcc -c revert_string.c ../swap/swap.c -I../swap -fPIC
gcc --shared revert_string.o swap.o -o librevert.so
gcc main.c
gcc -c main.c -o main.o
gcc --shared revert_string.o swap.o -o librevert_dynamic.so
rm librevert.so 
gcc main.o -L. -lrevert_dynamic
./a.out 
rm a.out 
gcc main.o -L. -lrevert_dynamic -o revert_dynamic
export LD_LIBRARY_PATH=$(pwd)/
./revert_dynamic 
./revert_dynamic string
sudo apt -y install libcunit1 libcunit1-doc libcunit1-dev
cd ../tests/
clear
gcc -c tests.c -o test.o
gcc -c tests.c -L../revert_string -lrevert_dynamic -o test.o
gcc -c tests.c -I../revert_string -L../revert_string -lrevert_dynamic -o test.o
ls
rm test.o 
gcc -c tests.c -I../revert_string test.o
gcc -c tests.c -I../revert_string -o test.o
ls
rm test.o
rm tests.o
gcc -c tests.c -I../revert_string -o tests.o
gcc tests.o -L../revert_string -lrevert_dynamic -o tests
gcc tests.o -L../revert_string -lrevert_dynamic -lcunit -o tests
./tests
history