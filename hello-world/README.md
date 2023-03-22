# Hello World

Print out the name of non-external functions that exist in the program; No modification of the program (adapted from the guide [HelloWorld: Your First Pass](https://github.com/banach-space/llvm-tutor#helloworld-your-first-pass))

## Steps to run

1. Build LLVM (or install from official package repositories; on Arch Linux: `sudo pacman -S llvm llvm-libs`)
2. Find the llvm installation directory (using `llvm-config --prefix`; `/usr` on Arch Linux when installing from the package repos) and set it in the [`CMakeLists.txt`](CMakeLists.txt).
3. Build the project
```sh
# currently in "hello-world" directory
export LLVM_DIR=/usr
mkdir build
cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..
make
```
4. Prepare the input file
```sh
# currently in "hello-world" directory
clang -O1 -S -emit-llvm ./inputs/input_hello_world.c -o input_hello_world.ll
```
5. Run the `HelloWorld` pass with `opt` using the generated shared library `libHelloWorld.so` (`libHelloWorld.dylib` on MacOS)
```sh
# currently in "hello-world" directory
opt -load-pass-plugin ./build/libHelloWorld.so -passes=hello-world -disable-output ./inputs/input_hello_world.ll
```
