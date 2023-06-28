# basys3-8bit-workshop-cpu-projects

## Overview
This project contains a set of Basys3 versions of the [8 Bit Workshop](https://8bitworkshop.com/) CPU projects, 
and a set of template files needed to create any Basys3 version of an 8 Bit Workshop CPU project.

## How to build and install projects
 1. Move to the project directory.
 2. Build and install the project with: `make install`
 3. Remove all generated files with: `make clean`

## Creating a new Basys3 version of an 8 Bit Workshop VGA project
 1. Copy the "template" directory under "cpu" to a new project directory.
 2. Rename the "project.tcl" and "project.v" files to the name of the project.
 3. In the Makefile, do the following:
    - Replace "<project name>" with the name of the project.
    - Define all of the 8 Bit Workshop project-specific .v files in "PROJ_SOURCES".
 4. In the .tcl file, do the following:
    - Replace "<project name>" with the name of the project.
 5. In the .v file, do the following:
    - Replace "<project name>" with the name of the project.
    - Replace "<project-specific top module>" with the name of the top module in the 8 Bit Workshop project.
 6. Copy all of the 8 Bit Workshop project-specific .v files to the new directory.

## Known issues
 - The 8 Bit Workshop and Basys3 verilog compilers are different and it is possible 
   that 8 Bit Workshop verilog files will not compile. Errors will either show up 
   on the command line or in file, "build/status."
 - The Basys3 complier does not allow the incrementation operator, "n++," so it must be changed to "n=n+1".
 - Output ports must be defined as "reg" if they are used in synchronous assignments.
    
