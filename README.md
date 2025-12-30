# gravity-simulation

Using the equations from [here](https://orbital-mechanics.space) as a guide.

Project is licensed under the [GNU Affero General Public License](https://www.gnu.org/licenses/agpl-3.0.en.html#license-text).

## To Do
- Add checking if init values make sense for "relative" frame
- Black Holes? Raytracing?
- Make the orbit trails configurable
- Add textures?
- Make the bodies spin? (would need textures)
- Fix the memory leaks. Especially in the YAML parser
- Make the camera auto-track an object


## Guide

### Dependencies

- OpenGL 4.3+
- GCC 14+ / Clang 18+

### Compiling from Source

This projcet uses [CMake](https://cmake.org/download/) to handle the build process. Any version from 3.15 -> 4.21 _should_ work.


#### Linux

1. Install the following libs:
```
sudo apt install libglfw3-dev libyaml-dev pkg-config
```

2. `mkdir -p build`
3. `cd build`
4. `cmake ..`
5. `make` 

If successful, a binary named `sim` will the saved to `out/`. If there are errors, ensure you have all the dependencies installed.

#### Windows

**NOTICE** MSVC is currently **not** supported as a compiler on windows. Please use Clang instead. This is due to the use of Variable Length Arrays in the program.

Windows compilation is slightly different than Linux, and requries more steps.

1. Install [MSYS2](https://www.msys2.org/)
    - Install Clang in MSYS2 by following [this guide.](https://www.mingw-w64.org/getting-started/msys2-llvm/)
2. Install [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/overview)
3. Install the following with vcpkg:
```
vpckg install libyaml glfw3
```
4. Run `vcpkg integrate install`
    - Note the output of this command. Find the part similar to: `-DCMAKE_TOOLCHAIN_FILE=C:/Users/<USERNAME>/Desktop/vcpkg/scripts/buildsystems/vcpkg.cmake` You will need to run cmake with this.
5. Install [Ninja](https://ninja-build.org/)
6. Navigate back to the project's root directory
7. Run cmake:

``` 
cmake -B  build -S . -G "Ninja" -DCMAKE_C_COMPILER=C:\msys64\mingw64\bin\clang.exe -DCMAKE_TOOLCHAIN_FILE=C:/Users/caleb/Desktop/vcpkg/scripts/buildsystems/vcpkg.cmake
```

**NOTE** If you're getting incompatabiility errors, try running this to force the output to be compatible with the GCC ABI.
```
 cmake -B  build -S . -G "Ninja" -DCMAKE_C_COMPILER=C:\msys64\mingw64\bin\clang.exe -DCMAKE_TOOLCHAIN_FILE=C:/Users/caleb/Desktop/vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_C_FLAGS="-target x86_64-pc-windows-gnu" -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=lld"
 ```

8. `cd build` and then `cmake --build .`

If everything works without any errors, there should now be a `sim.exe` file in the `out/` directory in the project's root.


### CLI Flags

| Flag | Function                                      | Arguments               |
|------|-----------------------------------------------|-------------------------|
| -m   | Set the frame of reference for the simulation | inertial, relative, cog, n-body|                                                             
| -t   | Set the time delta between steps              | Floats > 0.0            |
| -d   | Enable debug printing to console              | N/A                     |
| -h   | Open the help menu                            | N/A                     |
| -n   | Define the number of bodies to render         | 0 - N                   |
| -3   | Render the scene in 3d                        | N/A                     |


### Inititalization YAML

The initial values for each body is defined in a YAML file, which must be named `init.yaml` and placed in the directory you are running the simulator from. Refer to the `init.example.yaml` for a starting place. 

The different keys that exist are:

- Name: The name of the body. This is only used to differentiate them in parsing at this time.
- Mass: The mass of the body in KG. Scientific notation is allowed. (Ex. 10E3, 1.65E5)
- Radius: The radius of the body in KGs. Scientific notation is allowed. (Ex. 10E3, 1.65E5)
- Position: Two (or three) values seperated by a comma to represent the position vector of the body in meters. Scientific notation is allowed. (Ex. 10E3, 1.65E5)
- Velocity: Two (or three) values seperated by a comma to represent the velocity vector of the body in meters. Scientific notation is allowed. (Ex. 10E3, 1.65E5)
- Color: Hex color to define the color of the body
- Type: (currently only star is supported)
    - Star - Gives the body a 100% ambient glow and illuminates other bodies


### Moving the camera in 3d

In 3D, we can move the camera using WASD, and Shift/Control to move along the Y-axis.
You can rotate the scene with Q/E, I/K, and J/L
You can adjust the speed to be 1x, 2x, or 5x by pressing 1,2, or 5 respectively.

### Known Issues
- Compiling with an optimization setting higher than -O0 can cause crashes on Nvidia GPUs