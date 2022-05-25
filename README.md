RadeonLoad
==========

This is a fork of RadeonTop that has most of the features removed. 
It prints text output lines at regular itervals, and is aimed more at compute applications over graphics.

Builds and runs on FreeBSD, but requires gmake.

Requires access to /dev/dri/cardN files or /dev/mem (root privileges).


Supported cards
---------------

R600 and up, even Southern Islands should work fine.
Works with both the open drivers and AMD Catalyst.

For the Catalyst driver, only the mem path is currently supported - this
means it won't run on the default Ubuntu kernels that block /dev/mem.

The total GPU utilization is also valid for OpenCL loads; the other blocks
are only useful in GL loads.

#### Prerequisites

* libdrm
* libncurses
* libpciaccess


Simply start radeontop and it auto-selects the first supported GPU:

    ./radeontop


Running radeontop on a bus 0f:

    ./radeontop -b 0f


Writing values to stdout instead of showing a GUI:

    ./radeontop -d -


Getting all options:

    ./radeontop --help


Building
--------

#### Prerequisites
* all run time prerequisites with dev files
* gcc / clang
* pkgconf

### Building
If all prerequisites are fullfilled, it can be build by simply running:

    gmake

