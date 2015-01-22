# pstate-frequency


Easily control Intel p-state driver on Linux (3.9 and upwards)

pstate-frequency is able to adjust the Intel p-state driver values for the minimum  
and maximum scaling frequencies and the state of turbo boost.

### Requirements


+ Intel P-State driver (included in kernel 3.9 and upwards).  
Enable it by adding **intel_pstate=enable** to the boot command line.  
If the p-state driver is not present in your system, the application will still  
work on a basic level using the default **cpufreq** driver.

## Optional

+ **msr-tools** package to provide the **wrmsr** binary
+ The **msr** kernel module which should also be provided by the **msr-tools** package.
+ *Please note that*
**msr-tools**
*may be named diffrently on your distribution of choice.*

### Installation


The installation process follows the basic *make, make install* process.  
While building, there are a couple of options that one may configure or change:  
+ The C++ compiler used (defaults to c++, though clang++ is recommended)  
+ The directory to install to (defaults to /usr/local)  
+ Whether or not to install bash completion (defaults to Yes)
+ Whether or not to install zsh completion (defaults to Yes)
+ Whether or not to install a udev rule which monitors the state of the power supply  
and sets the power plan automatically to powersave when on battery and the power plan  
to performance when plugged into via AC adapter (defaults to Yes)

### Usage


The binary by default is named **pstate-frequency**  
When called without any options, the program will display something like the following:  
![](https://raw.githubusercontent.com/pyamsoft/pstate-frequency/master/assets/img/pstate-frequency_example_run.png)

When called with the *-g --get* option, the program will display something like the following:
![](https://raw.githubusercontent.com/pyamsoft/pstate-frequency/master/assets/img/pstate-frequency_example_get.png)

When called with the *-s --set* option, the program will display something like the following:
![](https://raw.githubusercontent.com/pyamsoft/pstate-frequency/master/assets/img/pstate-frequency_example_set.png)

The *-g --get* option can be called by a normal user, and will display the current values  
for the CPU  minimum scaling frequency, the CPU maximum scaling frequency, the state of Turbo Boost.

The *-s --set* option can only be called by a user with root permissions.  
The *-s --set* option also takes one or more of these flags as necessary arguments:  
+ **-m --max** Adjust the maximum scaling frequency of the CPU
+ **-n --min** Adjust the minimum scaling frequency of the CPU
+ **-t --turbo** Adjust the current state of Turbo Boost (0 is ON, 1 is OFF)
+ **-p --plan** Adjust the maximum scaling and Turbo Boost to a preset plan.

There are three user power plans:  
1. **powersave (1)** Sets the minimum and maximum scaling frequencies to the lowest available  
and disables Turbo Boost.  
2. **performance (2)** Sets the minimum scaling frequency to the lowest available,  
the maximum scaling frequency to the highest available non-turbo frequency and disables Turbo Boost.  
3. **max-performance (3)** Sets the minimum and maximum scaling frequencies to the  
highest available frequency taking into account Turbo Boost frequencies, and enables Turbo Boost.  

There is also a power plan meant to be used with the udev rule:
1. **auto (0)** If the computer's main powersource is online, then the *performance* plan is set.  
If the main powersource is offline, then the *powersave* plan is set.  


# Understanding pstate-frequency

pstate-frequency is just a convenient frontend for pre-existing software. It interacts with  
and modifies variables that are then read by other drivers or modules to effect system  
performance. Any problems with the system which may have indirectly resulted from the  
usage of pstate-frequency, but are not related to the output or input handling in  
pstate-frequency itself, should be presented to the respective authors.


# Questions


Questions or issues should be either posted in the issue section of this repository,  
or directed by email to pyamsoft @ \<pyam.soft@gmail.com\>
