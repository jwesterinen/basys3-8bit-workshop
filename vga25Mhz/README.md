# basys3-8bit-workshop-vga-projects

## Overview
This project contains a set of Basys3 versions of the [8 Bit Workshop](https://8bitworkshop.com/) VGA projects, 
and a set of template files needed to create any Basys3 version of an 8 Bit Workshop VGA project.

## How to build and install projects
 1. Move to the project directory.
 2. Build and install the project with: `make install`
 3. Remove all generated files with: `make clean`

## Creating a new Basys3 version of an 8 Bit Workshop VGA project
 1. Create a directory under "vga25Mhz" with the same name as the project.
 2. Copy all files in the "template" directory to the new directory.
 3. Rename the "project.tcl" and "project.v" files to the name of the project.
 4. In the Makefile, do the following:
    - Replace "<project name>" with the name of the project.
    - Define all of the 8 Bit Workshop project-specific .v files in "PROJ_SOURCES".
 5. In the .tcl file, do the following:
    - Replace "<project name>" with the name of the project.
 6. In the .v file, do the following:
    - Replace "<project name>" with the name of the project.
    - Replace "<project-specific top module>" with the name of the top module in the 8 Bit Workshop project.
 7. Copy all of the 8 Bit Workshop project-specific .v files to the new directory.

## Known issues
 - The 8 Bit Workshop and Basys3 verilog compilers are different and it is possible 
   that 8 Bit Workshop verilog files will not compile. Errors will either show up 
   on the command line or in file, "build/status."
 - The Basys3 complier does not allow the incrementation operator, "n++," so it must be changed to "n=n+1".
 - Output ports must be defined as "reg" if they are used in synchronous assignments.
    
