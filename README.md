pstate-frequency
================

Program to adjust CPU frequencies using the Intel P-State driver (3.13 onwards)

pstate-frequency is able to adjust the Intel p-state driver values for the maximum running frequency 
and the state of turbo boost. It is able to dynamically set a scaling frequency range and
artificially limit the range that your CPU will run at.

If you notice that your CPU seems limited or not able to run at its max frequency, try 
using ther performance or max-performance plan. If you notice it is running too hot or too fast
all the time, perhaps try to powersave plan.

Requirements
============

Intel P-State driver (included in kernel 3.13 and onwards I believe)
glibc with GNU getopt.h

A C compiler of some kind. I use clang, so that is what is set by default in
config.mk

However, you are welcome to use whatever works for you. I have found success using both
clang and gcc, and have not tested builds using other compilers. Take note that any build errors 
that arise from the use of other compilers are most likely not the result of any of the code.

Notes
=====

So this program is really ugly.

I wrote it basically to be used on my computer and so far, it only works on mine.
If you're reading this and you happen to also have an MSI GE60 0ND-042US, then you're in luck.

Otherwise if you want to use this properly you may need to do some extra configuring.

Update as of 09/17/2014:

After shamelessly copying code from the i7z project, which in turn directly pulled code off from a 
kernel mailing list, the ability to determine the max CPU frequency at runtime has been added.
The maximum number of CPU's has also been determined from using information in /proc/cpuinfo.


Usage
=====

Call the function differently depending on how you wish to use it.

To read CPU values, a normal user can be used.
To write CPU values, root must be used.

The -g option reads current CPU values.

The -s option allows you to write the max frequency with -m integer value, 

the turbo state with -t 0 for Turbo, 1 for No Turbo

or set a predefined power plan with -p 1, 2, 3

The powersave plan limits your CPU to its lowest frequency possible.

The performance plan allows full range of the CPU not including Turbo Boost capabilities.

The max-performance plan allows full range of the CPU including Turbo Boost capabilities.

