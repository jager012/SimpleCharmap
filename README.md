# Simple Charmap
Just a simple character map.

## Installation

### Windows
- Download the latest release installer from [here](https://github.com/jager012/SimpleCharmap/releases).

### Linux
- #### Install build prerequisites:
  - C++ build environment (gcc + make)
  - Qt6 + qmake6
- Clone the repository
- ```qmake6 && make```
- ```sudo make install```

The binary and all the required files (translations and characters metadata) will be installed to `/opt/SimpleCharmap/`.

## Building on Windows
Using MSVC is recommended. You could either generate a Visual Studio project by running ```qmake -t vcapp``` inside the VS **x64 Native Tools Command Prompt** or build the application straight from the VS prompt by running `qmake` with no options and then `nmake` or `jom`. The generated Makefile will automatically handle every additional step to end up with a fully working build inside the `release/` folder of your source tree.

You can use MinGW-w64 or any other toolchain supported by qmake. If you use MSVC, the qmake script will put a copy of the VC++ Redistributable setup file in the `release/` folder.

## Credits
- Qt Software Foundation
- Papirus Development Team
