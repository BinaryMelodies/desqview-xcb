
# XCB bindings for DESQview/X

## Overview

This project started out by attempting to reverse engineer the Xlib bindings for DJGPP 1.08, which is mostly based on the MIT X11R4 code base.
However once enough information was gathered to supplement what is already documented in Ralf Brown's Interrupt List, the goal pivoted to providing a modern, open source interface to DESQview/X for modern compilers.

The DESQview/X specific code is found in a single file and it is a very simple shim over the DESQview/X API.
No reverse engineered code was included in it it. The rest of the code has been adjusted for the new target (including removing mutexes and authentication). Unlike the original binding, this one runs in 16-bit real mode and implements the leaner XCB library in the hopes of having a lower memory footprint.

## Links and references

* [X Window System, Version 11, Release 4](https://www.x.org/wiki/X11R4/), on which the original bindings were apparently based on
* [XCB libraries](https://xcb.freedesktop.org/), including the two repositories used: [https://gitlab.freedesktop.org/xorg/lib/libxcb], [https://gitlab.freedesktop.org/xorg/proto/xcbproto]
* [XCB programming tutorials](https://www.x.org/releases/X11R7.7/doc/libxcb/tutorial/index.html), which were adapted for the tests under the `test` folder
* [DESQview/X Software Ports](https://www.cs.cmu.edu/~bmm/dvx.html), some interesting information on DESQview/X
* [Ralf Brown's Interrupt List](https://www.cs.cmu.edu/~ralf/files.html) with two online browsable versions: [http://www.ctyme.com/rbrown.htm], [https://www.delorie.com/djgpp/doc/rbinter/]

