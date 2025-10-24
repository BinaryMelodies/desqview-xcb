
# XCB bindings for DESQview/X

## Overview

This project started out by attempting to reverse engineer the Xlib bindings for DJGPP 1.08, which is mostly based on the MIT X11R4 code base.
However once enough information was gathered to supplement what is already documented in Ralf Brown's Interrupt List, the goal pivoted to providing a modern, open source interface to DESQview/X for modern compilers.

Instead of the Xlib libraries, this project is a port of the newer and leaner XCB library, that compiles to a 16-bit real mode MS-DOS executable via the Open Watcom and IA-16 GCC compilers.

The DESQview/X specific code is found in a single file and it is a very simple shim over the DESQview/X API.
No reverse engineered code was included in it it.
The rest of the code comes from the XCB project and has been adjusted for the new target.

## Compilation

Running `make` will create an `out/watcom` and an `out/gcc` folder, containing the libraries for the Open Watcom and IA-16 GCC compilers.
It will also compile the tests as MS-DOS small model executables under the same folders that can be directly run under DESQview/X.
Alternatively, `make watcom` will only generate the Watcom binaries and `make gcc` the IA-16 GCC binaries.
The command `make native` will compile the examples for the host using the host's version of GCC.

The generated executables can be launched from within DESQview/X, for example from a DOS window.

## Modifications

Most of the code is based on the XCB libraries.
The xcbproto repository is linked as a submodule, and the libxcb libraries have been modified, most notably in the compilation process.
A custom Makefile is provided, and the repository is preconfigured for the Watcom and IA-16 GCC compilers.

The files `xcb_dos.h` and `xcb_dos.c` are DOS and DESQview/X specific and provide new code.
It exposes the DESQview socket API, but unlike the original Xlib bindings, this interface is simplified specifically for use with XCB.
Most other files have seen minor adjustments, most notably to remove calls to the thread library and the overhauled code to connect to DESQview/X, including removal of authentication.
Some files had to be adjusted to compile with the Watcom compiler.

The xproto.c generated file had to be split up to several files.
This was required because the IA-16 GCC compiler tried to include the entire object file during linking, and it would run out of memory for the limited small memory model text segment.

## Status

While the ported library compiles and works with the examples, it has not been thoroughly tested.
Much of the comfort of a full UNIX system is missing as well, since only the core libraries have been ported.
However, as a proof of concept, it is complete and unlikely to need updates.
The author releases this code in the hopes that others will find it useful and/or educational.
**Use at your own risk, as it comes without any warranty.**

## Links and references

* [X Window System, Version 11, Release 4](https://www.x.org/wiki/X11R4/), on which the original bindings were apparently based on
* [XCB libraries](https://xcb.freedesktop.org/), including the two repositories used: <https://gitlab.freedesktop.org/xorg/lib/libxcb>, <https://gitlab.freedesktop.org/xorg/proto/xcbproto>
* [XCB programming tutorials](https://www.x.org/releases/X11R7.7/doc/libxcb/tutorial/index.html), which were adapted for the tests under the `test` folder
* [DESQview/X Software Ports](https://www.cs.cmu.edu/~bmm/dvx.html), some interesting information on DESQview/X
* [Ralf Brown's Interrupt List](https://www.cs.cmu.edu/~ralf/files.html) with two online browsable versions: <http://www.ctyme.com/rbrown.htm>, <https://www.delorie.com/djgpp/doc/rbinter/>
* [Open Watcom 1.9](https://openwatcom.org/) and the [Open Watcom 2.0 fork](https://open-watcom.github.io/)
* [The IA-16 GCC toolchain](https://gitlab.com/tkchia/build-ia16) maintained by TK Chia

