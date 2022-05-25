RadeonLoad
==========

This is a fork of RadeonTop that has most of the features removed. 
It prints text output lines at regular itervals, and is aimed more at compute applications over graphics.

Example output: 

    GPU: POLARIS12 on bus 43
    
         gpu     vram   temp    vram gb temp C      mclk% mclk ghz    sclk% sclk ghz
    -------- -------- ------   -------- ------   -------- -------- -------- --------
       1.67%   69.83%  73.1%      1.328   65.8     43.49%    0.652   83.91%    1.023
       1.67%   69.83%  73.1%      1.328   65.8     43.49%    0.652   83.91%    1.023
       1.67%   69.83%  72.0%      1.328   64.8     32.10%    0.481   66.00%    0.805
       1.67%   69.83%  71.8%      1.328   64.7     35.85%    0.538   79.10%    0.964
       0.00%   69.83%  71.9%      1.328   64.7     37.79%    0.567   70.22%    0.856
       1.67%   69.83%  71.6%      1.328   64.5     38.40%    0.576   74.29%    0.906
       0.83%   69.83%  71.2%      1.328   64.1     35.11%    0.527   55.10%    0.672
       0.83%   69.83%  71.3%      1.328   64.2     31.56%    0.473   81.39%    0.992
       0.83%   69.83%  71.3%      1.328   64.1     33.72%    0.506   68.31%    0.833

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


Simply start radeonload and it auto-selects the first supported GPU:

    ./radeonload


Running radeonload on a bus 0f:

    ./radeonload -b 0f


Writing values to stdout instead of showing a GUI:

    ./radeonload -d -


Getting all options:

    ./radeonload --help


Building
--------

#### Prerequisites
* all run time prerequisites with dev files
* gcc / clang
* pkgconf

### Building
If all prerequisites are fullfilled, it can be build by simply running:

    gmake

