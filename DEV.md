# Environment setup
## Linux
* See UrchinEngine environment setup [here](https://github.com/petitg1987/urchinEngine/blob/master/DEV.md)
* Install library glfw3 (static library is not provided with package 'libglfw3-dev')
  ```
  sudo apt install libegl1-mesa-dev libffi-dev libwayland-bin libwayland-dev libxrandr-dev x11proto-randr-dev libxinerama-dev libxcursor-dev libxi-dev
  rm /tmp/glfw3/ -rf && mkdir -p /tmp/glfw3/ && cd /tmp/glfw3/
  wget -P /tmp/glfw3/ https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.zip
  unzip glfw-3.3.8.zip && cd /tmp/glfw3/glfw-3.3.8
  cmake -DBUILD_SHARED_LIBS=OFF .
  make
  sudo make install
  ```

## Windows
* *To define...*

# CLion configuration
## Build configuration
* Add CMake profiles (File > Settings -> Build,Execution,Deployment > CMake)
  * Profile **Debug**:
    * Name: `Debug`
    * Build type: `Debug`
    * Build directory: `.build/debug`
    * Build options: `-j 14`
  * Profile **DebugSanitizer**:
    * Name: `DebugSanitizer`
    * Build type: `Debug`
    * CMake options: `-DUSE_SANITIZER=true`
    * Build directory: `.build/debug-sanitizer`
    * Build options: `-j 14`
* Add CMake applications
  * Application **urchinEngineTest**:
    * Target/executable: `urchinEngineTest`
  * Application **urchinEngineTest (win)**:
    * Target/executable: `urchinEngineTest`
    * Program arguments: `--windowed`
  * Application **urchinEngineTest (win, dev)**:
    * Target/executable: `urchinEngineTest`
    * Program arguments: `--windowed --dev`
