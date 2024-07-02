# line-counter
A simple line counter tool that can be installed and run from the terminal.

## Installation
1. In `CMakeLists.txt`, specify where you want it to be installed to in the directory argument of `install(...)`
2. Create a folder to build to. I like to just create a `build` folder from the root directory. CD to the build folder.
3. Create CMake files from the build folder: `cmake -G "Unix Makefiles" ../` (The `../` is the dir CMakeLists.txt is in, also can change `Unix Makefiles` to whatever)
4. Build the project from the build folder: `cmake --build .`
5. Install the project from the build folder: `cmake --install .`
6. Edit environment variables if needed (add path to installed executable to PATH)
