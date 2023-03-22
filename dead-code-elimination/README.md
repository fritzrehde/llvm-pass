# Dead Code Elimination

Remove code that does not affect the program result (i.e. that has no side effects and no usages).

## Steps to run

1. Build LLVM (or install from official package repositories; on Arch Linux: `sudo pacman -S llvm llvm-libs`)
2. Find the llvm installation directory (using `llvm-config --prefix`; `/usr` on Arch Linux when installing from the package repos) and set it in the [`CMakeLists.txt`](CMakeLists.txt).
3. Build the project
```sh
# currently in "dead-code-elimination" directory
export LLVM_DIR=/usr
mkdir build
cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..
make
```
4. Run the `DeadCodeEliminationPass` pass with `opt` using the generated shared library `libDeadCodeEliminationPass.so` (`libDeadCodeEliminationPass.dylib` on MacOS)
```sh
# currently in "dead-code-elimination" directory
opt -load-pass-plugin ./build/libDeadCodeEliminationPass.so -passes=dead-code-elimination -S ./inputs/example-[1|2].ll
```
