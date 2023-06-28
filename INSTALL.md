## Install rayraw-soft
### Build example

```
mkdir rayraw-soft
cd  rayraw-soft
mkdir build install

git clone https://github.com/spadi-alliance/rayraw-soft.git rayraw-soft.src.git 

cmake \
   -S ./rayraw-soft.src.git \
   -B ./build \
   -DCMAKE_INSTALL_PREFIX=./install \
   -DCMAKE_PREFIX_PATH=<your hul_core_lib install dir>/install
cd build
make
make install
```

### Practical usage

We recommend to make a symbolic link the to hul-core-lib install directory inside the rayraw-soft install directory.
