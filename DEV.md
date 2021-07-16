# Environment setup
## Linux
* See UrchinEngine environment setup [here](https://github.com/petitg1987/urchinEngine/blob/master/DEV.md)
* Install library glfw3 (static library is not provided with package 'libglfw3-dev')
  ```
  sudo apt install libegl1-mesa-dev libffi-dev libwayland-bin libwayland-dev libxrandr-dev x11proto-randr-dev
  rm /tmp/glfw3/ -rf && mkdir -p /tmp/glfw3/ && cd /tmp/glfw3/
  wget -P /tmp/glfw3/ https://github.com/glfw/glfw/releases/download/3.3.4/glfw-3.3.4.zip
  unzip glfw-3.3.4.zip && cd /tmp/glfw3/glfw-3.3.4
  cmake -DBUILD_SHARED_LIBS=OFF .
  make
  sudo make install
  ```

## Windows
* N/A

# CLion configuration
## Build configuration
* Add CMake profiles (File > Settings -> Build,Execution,Deployment > CMake)
  * Profile **DebugO0**:
    * Name: `DebugO0`
    * Build type: `Debug`
    * CMake options: `-DUSE_O0=true`
    * Generation path: `.build/debug`
    * Build options: `-j 10`
  * Profile **DebugO0Asan**:
    * Name: `DebugO0Asan`
    * Build type: `Debug`
    * CMake options: `-DUSE_O0=true -DUSE_ASAN=true`
    * Generation path: `.build/debug-asan`
    * Build options: `-j 10`    
  * Profile **DebugO3**:
    * Name: `DebugO3`
    * Build type: `Debug`
    * CMake options: `-DUSE_O3=true`
    * Generation path: `.build/debugO3`
    * Build options: `-j 10`
* Add CMake applications
  * Application **urchinEngineTest**:
    * Target/executable: `urchinEngineTest`
  * Application **urchinEngineTest (win)**:
    * Target/executable: `urchinEngineTest`
    * Program arguments: `--windowed`
  * Application **urchinEngineTest (win, dbg)**:
    * Target/executable: `urchinEngineTest`
    * Program arguments: `--windowed --debug`

## Clang analyzer
* In File > Settings > Languages & Frameworks > C/C++ > Clangd > Clang Errors and Warnings: `-Wno-unused-variable,-Wno-infinite-recursion,-Werror=implicit-function-declaration,-Wno-shadow-field-in-constructor-modified,-Wno-shadow-ivar,-Wuninitialized,-Wunused-label,-Wunused-lambda-capture,-Wno-shadow`