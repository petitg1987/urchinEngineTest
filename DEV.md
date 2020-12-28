# Environment setup
## Linux
* See [DEV.md](https://github.com/petitg1987/urchinEngine/blob/master/DEV.md) of UrchinEngine
* Install library glfw3 (static library is not provided with package 'libglfw3-dev')
  ```
  sudo apt install libegl1-mesa-dev libffi-dev libwayland-bin libwayland-dev libxrandr-dev x11proto-randr-dev
  rm /tmp/glfw/ -rf && mkdir -p /tmp/glfw/ && cd /tmp/glfw/
  wget -P /tmp/glfw/ https://github.com/glfw/glfw/releases/download/3.3.2/glfw-3.3.2.zip
  unzip glfw-3.3.2.zip && cd /tmp/glfw/glfw-3.3.2
  cmake -DBUILD_SHARED_LIBS=OFF .
  make
  sudo make install
  ```

## Windows
* N/A

## CLion configuration
* Add CMake profiles (File > Settings -> Build,Execution,Deployment > CMake)
  * Profile **DebugO0**:
    * Name: `DebugO0`
    * Build type: `Debug`
    * CMake options: `-DUSE_O0=true`
    * Generation path: `.build/debug`
    * Build options: `-j 12`
  * Profile **DebugO3**:
    * Name: `DebugO3`
    * Build type: `Debug`
    * CMake options: `-DUSE_O3=true`
    * Generation path: `.build/debugO3`
    * Build options: `-j 12`
* Add CMake applications
  * Application **urchinEngineTest**:
    * Target/executable: `urchinEngineTest`
    * Program arguments: `--windowed`
  * Application **urchinEngineTest (fullscreen)**:
    * Target/executable: `urchinEngineTest`
