# UrchinEngineTest
Test application using Urchin engine

## CLion configuration
- Add CMake profiles (File > Settings -> Build,Execution,Deployment > CMake)
  - Profile **DebugO0**:
    - Name: `DebugO0`
    - Build type: `Debug`
    - CMake options: `-DUSE_O0=true`
    - Generation path: `.build/debug`
    - Build options: `-j 12`
  - Profile **DebugO3**: 
    - Name: `DebugO3`
    - Build type: `Debug`
    - CMake options: `-DUSE_O3=true`
    - Generation path: `.build/debugO3`
    - Build options: `-j 12`
- Add CMake applications
  - Application **urchinEngineTest**:
    - Target/executable: `urchinEngineTest`
    - Program arguments: `--windowed`
  - Application **urchinEngineTest (fullscreen)**:
    - Target/executable: `urchinEngineTest`
