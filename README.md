# pstate-frequency

Easily control Intel p-state driver on Linux (3.9 and upwards)

pstate-frequency is able to adjust the Intel p-state driver values for the  
minimum and maximum scaling frequencies and the state of turbo boost.


### Requirements

+ Intel P-State driver (included in kernel 3.9 and upwards).  
Enable it by adding **intel_pstate=enable** to the boot command line.  
If the p-state driver is not present in your system, the application will  
still work using the default **acpi-cpufreq** driver.

pstate-frequency is distribution independent, and should run on all standard  
versions of Linux based operating systems which include a recent kernel (3.9+)  
and come with the set of expected core utilities, including *ls*, *grep*,  
*cat*, *wc*, *cut*, and *modprobe*. The system must also support the use of  
the **proc** and **sys** mounts, as found on a typical modern system. These  
attributes are assumed on any system attempting to use the pstate-frequency  
frontend, and support towards any system which does not conform to these  
normally expected features cannot be given at this time.

You are also expected to read and understand the settings in the configuration  
file: config.mk. Undertaking modification of this file is up to the user, and  
support for any kind of customization that deviates from the shipped  
configuration file will not be supported.


### Installation

The installation process follows the basic *make, make install* process.  

While building, there are a couple of options that one may configure or  
change:  
+ The C++ compiler used (defaults to g++, though clang++ is recommended)
+ The directory to install to (defaults to /usr/local)
+ Whether or not to install bash completion (defaults to Yes)
+ Whether or not to install zsh completion (defaults to Yes)
+ Whether or not to install a udev rule which monitors the state of the power  
supply and sets the power plan automatically to powersave when on battery and  
the power plan to performance when plugged into via AC adapter (defaults to Yes)


#### Distribution Specific Installations

At this time, only ArchLinux is officially supported via the Arch User Repository (AUR).

pstate-frequency comes with a PKGBUILD provided in the assets directory.  
The PKGBUILD will pull the latest version from the master repository on GitHub  
and compile it for any recent version of ArchLinux.  

Debian based distributions are currently unofficially supported through the  
**checkinstall** program and this is currently the recommended way to install  
the package until an actual deb package can be constructed.

RedHat based distributions should also look to using **checkinstall** or a  
similar kind of program to produce an RPM for use on such systems.


### Versioning

The versioning is rather arbitrary. The scheme is as follows:  
	X.Y.Z

X is the major version number, it will only be updated when the program is  
significantly updated. Y is the minor version number, it will be updated when  
features are added to existing versions. Z is the bugfix version number, it  
will be updated generally whenever a bug is fixed or patched.

The addition of a ".git" tag after the bugfix version number signifies that  
the current version is nebulus, and is in the process of fixing a bug or  
patching a feature that would otherwise lead to the incrementation of the  
bugfix version number.

### Usage

The binary by default is named **pstate-frequency**  
When called without any options, the program will display something like  
the following:  
![](https://raw.githubusercontent.com/pyamsoft/pstate-frequency/master/assets/img/pstate-frequency_example_run.png)

When called with the *-G --get* option, the program will display something  
like the following:  
![](https://raw.githubusercontent.com/pyamsoft/pstate-frequency/master/assets/img/pstate-frequency_example_get.png)

When called with the *-S --set* option, the program will display something  
like the following:  
![](https://raw.githubusercontent.com/pyamsoft/pstate-frequency/master/assets/img/pstate-frequency_example_set.png)

The *-G --get* option can be called by a normal user.  
The *-G --get* option also takes one or more of these flags as necessary  
arguments:  
+ **-c --current** Display the current user-requested CPU values
+ **-r --real** Display real-time CPU frequencies

The *-S --set* option can only be called by a user with root permissions.  
The *-S --set* option also takes one or more of these flags as necessary  
arguments:  
+ **-m --max** Adjust the maximum scaling frequency of the CPU
+ **-n --min** Adjust the minimum scaling frequency of the CPU
+ **-o --gov** Adjust the current CPU scaling governor
+ **-t --turbo** Adjust the current state of Turbo Boost (0 is ON, 1 is OFF)
+ **-p --plan** Adjust the maximum scaling and Turbo Boost to a preset plan.

There are three user power plans:  
1. **powersave (1)** Sets the minimum and maximum scaling frequencies to the  
lowest available and disables Turbo Boost.  
2. **performance (2)** Sets the minimum scaling frequency to the  
lowest available, the maximum scaling frequency to the highest available  
non-turbo frequency and disables Turbo Boost.  
3. **max-performance (3)** Sets the minimum and maximum scaling frequencies  
to the highest available frequency taking into account Turbo Boost  
frequencies, and enables Turbo Boost.  

There is also a power plan meant to be used with the udev rule:  
1. **auto (0)** If the computer's main powersource is online, then the  
*performance* plan is set. If the main powersource is offline, then the  
*powersave* plan is set.  


### Understanding pstate-frequency

pstate-frequency is just a convenient frontend for pre-existing software.  
It interacts with and modifies variables that are then read by other drivers  
or modules to affect system performance. Any problems with the system which  
may have indirectly resulted from the usage of pstate-frequency, but are not  
related to the output or input handling in pstate-frequency itself, should be  
presented to the respective authors.

Intel's thermald is also capable of adjusting the pstate driver periodically  
via the monitoring of system temperatures. pstate-frequency should in theory  
be able to work around this as it limits the actual CPU frequencies, not just  
the values of the pstate driver itself. This being said, any issues that may  
arise as a result of using the thermald program with pstate-frequency will  
not be supported, though users are welcome to report them. This does not  
mean users are unable to use thermald and pstate-frequency, just understand  
that pstate-frequency may override the automatic values set by thermald, and  
thermald may attempt to override the manually set values of pstate-frequency.

One particular setting of thermald that may conflict with pstate-frequency  
depending on one's desired use cases is that thermald by default will gain  
absolute control over the Turbo settings for the intel_pstate driver.  
This means that thermald will override any user set values for the  
no_turbo variable in pstate-frequency. One solution to this problem  
if a user wishes to use pstate-frequency to control turbo boost is to  
comment out or remove the intel_pstate line in the default thermald  
configuration file: **/etc/thermald/thermal-cpu-cdev-order.xml**.


#### pstate-frequency doesn't work

Sometimes you will notice that even when a frequency is requested using  
pstate-frequency, the CPU will still run at higher, or lower  
frequencies. This is explained by the kernel documentation snippet  
below:  

*For contemporary Intel processors, the frequency is controlled by the  
processor itself and the P-states exposed to software are related to  
performance levels.  The idea that frequency can be set to a single  
frequency is fiction for Intel Core processors. Even if the scaling  
driver selects a single P state the actual frequency the processor  
will run at is selected by the processor itself.*

This means that although a certain value may be requested through the  
pstate-frequency software interface, the actual frequency that the CPU  
will run at is decided solely by the hardware itself.  
pstate-frequency is only able to offer "suggestions" as to what  
frequency should be run, not make strict rules.

### Additional Notes

If you are attempting to use the udev rule but pstate-frequency fails to  
change the frequencies when the power state has changed, please open an issue  
on GitHub. I believe this is an issue that is due to the way udev lays out the  
structures of certain systems requiring a different or additional udev rule  
to function properly. This new rule can only be instated after a large amount  
of testing, and any help that you could provide by stating your system as well  
as any output you get when running **udevadm monitor** would be of great help.


## Questions

Questions or issues should be either posted in the issue section of this  
repository, or directed by email to pyamsoft @ pyam.soft@gmail.com

## Issues

Check the issues page on GitHub for any notes about outstanding or existing  
issues. If you encounter a problem with pstate-frequency of which no such  
issue already exists please feel free to help the developer by creating an  
issue ticket.

## License

GPL v2
