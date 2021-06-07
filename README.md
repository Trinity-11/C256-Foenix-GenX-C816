# C256 Foenix GenX C816 Kernel

## About

This project contains the new portable kernel for the Foenix line of computers.
This is a simple kernel for the Foenix U, U+, FMX, and GenX computers and should be
portable to any CPU module for the Gen X for which a C89 compiler is available.

## Design Concepts

The first goal of this kernel is to have runnable code ready for the new GenX computer when the prototypes are available
and to be readily portable to the various CPU modules that will be made available for that computer.
A secondary goal is that this kernel should be able to run on the main Foenix computers: FMX, U, U+, and GenX.
Finally, the kernel should be easy to use and easy for people to extend and should not get in their way in writing games and demos.

To this end, this kernel will be kept as simple as possible: providing access to those portions of the hardware that a programmer
might not want to deal with immediately while not requiring a user program to interface to numerous services and managers to get
anything done. If a game wants to take over the entire machine, this kernel will permit that.

To this end, the following features (or lack of feature) will included in this kernel:
* File access to SD card, PATA hard drive, and floppy drive.
* Support for partitions, different cluster sizes, long file names, and directories.
* Support for PS/2 keyboard, with support for different layouts
* Support for PS/2 mouse
* A framework for interrupt handling to allow programs to handle specific interrupts without necessarily taking over the entire computer
* A framework for supporting block devices to allow a user or program to install their own block device (e.g. ZIP drive)
* A framework for supporting channel or streaming devices to allow programs:
    * Read and write to the console, serial ports, and other character or byte stream type devices
    * Read and write files on a byte or character basis
    * Add support for custom channels (e.g. Ethernet TCP streams)
* Single tasking (for the moment)

## Building

To build kernel on Windows machines, you will need Western Design Center's C compiler for the 65816.
Actually building the project consists of just running the `WDCMAKE.BAT` batch file.

## Structure

The project is broken out over several folders:
* `build` -- This is where all object files will be stored during the build process, as well as the final result files.
* `src` -- This is where all the C source files will be:
    * `cli` -- Code for a very simple built-in command line interface
    * `dev` -- All device specific code should be here, broken out by device: clock, codec, PS2 devices, PSG, SID, text screen, etc.
        * `block` -- The block device abstraction goes here, as well as implementation of block devices (_e.g._ SD, HDD, floppy)
        * `channel` -- The channel abstraction goes here, as well as implementation of channel devices (_e.g._ console, serial, parallel, MIDI, etc.)
    * `fatfs` -- Location for the FATfs file system code (from the FATfs project)
    * `include` -- Location for common include files
    * `rsrc` -- Location for resource files (fonts, images, etc.)
    * `sys` -- Location for CPU specific code, assembly files, boot strapping code
    * `types` -- Utility types for the kernel

## Credits

Portions of this project are from the FatFs project: http://elm-chan.org/fsw/ff/00index_e.html. Their copyright information included in the source code.
