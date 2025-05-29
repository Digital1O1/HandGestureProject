# [Reference](https://github.com/jbeder/yaml-cpp)

# Why 'make install' matters after building from source
## Executing the commands below
```bash
mkdir build
cd build
cmake ..
make
```
- Only builds the library in the local `build` directory 
- The system doesn't know where the headers or compiled libraries are at 

## This is where `make install` comes into play
### `make install` does three things
1) Copies header files to : `/usr/local/include`
   - This allows us to 
     ```cpp
     #include <yaml-cpp/yaml.h>
     ```
2) Copies compiled libraries (.a or .so) to : `/usr/local/lib`
3) Registers paths 
    - In `/usr/local` with the system
    - This allows other programs the ability to link when using the default compiler flag like
      - `-lyaml-cpp
      - pkg-config --cflags yaml-cpp
      - pkg-config --libs  yaml-cpp

## Why `/usr/local`
- `/usr/` is for software provided by the system package manager (e.g. apt, dnf)
- `/usr/local/` is for software you installed manually from source, so it doesn’t interfere with the package-managed stuff
- Tools like g++, ld, and pkg-config will automatically search in `/usr/local/include` and `/usr/local/lib`
- Which is why it's safe and the standard to install libraries build from source to `/usr/local`