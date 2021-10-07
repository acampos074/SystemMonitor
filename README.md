# System Monitor Project

## Overview
This is the second project from the Udacity [C++](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213) course. The initial source code template can be found [here](https://github.com/udacity/CppND-System-Monitor). This program contains the implementation of the `System`, `Process`, and `Processor` classes, as well as functions within the `LinuxParser` namespace.

Below is the resulting output of the System Monitor.

![alt-text](https://raw.githubusercontent.com/acampos074/SystemMonitor/master/figures/SystemMonitor1006.png)

## License
The source code is released under an [MIT license.](https://opensource.org/licenses/MIT)
## Installation
1. Clone the project repository:
`git clone https://github.com/acampos074/SystemMonitor.git`
2. Build the project:
`make build`

This project uses [Make](https://www.gnu.org/software/make/) to build the project. The Makefile has four targets:
* `build` compiles the source code and generates the executable file
* `format` applies [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) to style the source code
* `debug` compiles the source code, generates an executable file, and debugging symbols
* `clean` deletes the `build/` directory

## Dependencies
* [ncurses](https://invisible-island.net/ncurses/announce.html) is a library used to output text-based graphical output in the terminal.


`sudo apt install libncurses5-dev libncursesw5-dev`

## Usage
Run the executable:

`./build/monitor`
