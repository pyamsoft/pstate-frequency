# pstate-frequency

Easily control Intel p-state driver on Linux (3.9 and upwards)

pstate-frequency is able to adjust the Intel p-state driver values for the  
minimum and maximum scaling frequencies and the state of turbo boost.


[![Build Status](https://travis-ci.org/pyamsoft/pstate-frequency.svg?branch=master)](https://travis-ci.org/pyamsoft/pstate-frequency)


### Requirements

+ Intel P-State driver (included in kernel 3.9 and upwards).  
Enable it by adding **intel_pstate=enable** to the boot command line.  
Starting with version 2.0.0, only intel_pstate will be suported. There  
are plenty of other tools that can support acpi-cpufreq and other CPU  
drivers.

+ coreutils  
The standard GNU tools for things like cat, wc, grep, and cut. These are  
generally expected on all modern Linux based systems. If you are in a  
situation where your machine does not have these basic tools, support cannot  
be offered at this time.

+ Reading Comprehension and Understanding  
You are also expected to read and understand the settings in the configuration  
file: config.mk.


### Optional Dependencies

+ x86_energy_perf_policy  
This is used to set the CPU to normal performance policy upon suspend and
resume. It is used to currently work around an assumed bug whereby on resume  
the system is set to performance policy on one core. It can help fix issues  
where upon resume of the system, the CPU runs at frequencies higher than  
that which is advertised to be available.


### Installation

The installation follows a basic process:  
```
# make edit
# make bin
# make install
```

While building, there are a couple of options that one may configure or  
change:  
+ The C compiler used (defaults to gcc)  
+ The directory to install to (defaults to /usr/local)  
+ Install bash completion (defaults to Yes)  
+ Install zsh completion (defaults to No)  
+ Install systemd related unit files (defaults to Yes)  
+ Install udev rules for automatic frequency setting (defaults to Yes)  
+ Install this README as documentation (defaults to Yes)  
+ Install a copy of the source code as documentation (defaults to Yes)  

Also note that the config.mk file specifies by default the option  
*-march=native* which can only be used by newer version of clang and  
gcc version including and above 4.9


#### Distribution Specific Installations

At this time, only ArchLinux is officially supported via the Arch User  
Repository (AUR).

pstate-frequency comes with a PKGBUILD provided in the assets directory.  
The *stable/PKGBUILD* will pull the latest released version from GitHub  
and compile it for ArchLinux.  
The *git/PKGBUILD* will pull the latest version from the dev repository on  
GitHub and compile it for ArchLinux.  

Debian package management and RedHat package management based distributions  
are currently unofficially supported through the **checkinstall** program  
and this is the recommended way to install the package until an actual deb  
package can be constructed.

Any other package management based distributions can look to using  
**checkinstall** but are otherwise entirely unsupported by the developer at  
this time.


### Versioning

The versioning is rather arbitrary. The scheme is as follows:  
        X.Y.Z

X is the major version number, it will only be updated when the program is  
significantly updated. Y is the minor version number, it will be updated when  
features are added to existing versions. Z is the bugfix version number, it  
will be updated generally whenever a bug is fixed or patched.

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

The *-G --get* option can be called by a normal user and can take various  
flags as optional arguments:  
+ **-c --current** Display the current user-requested CPU values  
+ **-r --real** Display real-time CPU frequencies  

*By default, the calling the -G option with no extra arguments is equivalent  
to calling with the --current option*

The *-S --set* option can only be called by a user with root permissions, and  
takes various flags as required arguments:  
+ **-m --max** Adjust the maximum scaling frequency of the CPU  
+ **-n --min** Adjust the minimum scaling frequency of the CPU  
+ **-g --governor** Adjust the current CPU scaling governor  
+ **-t --turbo** Adjust the current state of Turbo Boost (0 is ON, 1 is OFF)  
+ **-p --plan** Adjust the maximum scaling and Turbo Boost to a preset plan.  

*By default, calling the -S option with no extra arguments will return an  
error*


##### Power Plans

Power Plans are convenience shortcuts which essentially alias to calling  
pstate-frequency with various options for --max, --min, --turbo, and  
--governor. They are not necessary for operation, but are recommended.

There are three user recommended power plans:  
1. **powersave (1)** Sets the minimum and maximum scaling frequencies to the  
lowest available and disables Turbo Boost.  
2. **balanced (2)** Sets the minimum scaling frequency to the  
lowest available, the maximum scaling frequency to the highest available  
non-turbo frequency and disables Turbo Boost.  
3. **performance (3)** Sets the minimum and maximum scaling frequencies  
to the highest available frequency taking into account Turbo Boost  
frequencies, and enables Turbo Boost.  
4. **auto (0)** If the computer's main powersource is online, then the  
power plan defined by the user configurable *AUTO_POWER_PLAN_AC* or the  
*balanced* plan is set. If the main powersource is offline, then the  
power plan defined by the user configurable *AUTO_POWER_PLAN_BAT* or the  
*powersave* plan is set.  


### Understanding pstate-frequency

pstate-frequency is just a convenient frontend for pre-existing software.  
It interacts with and modifies variables that are then read by other drivers  
or modules to affect system performance. Any problems with the system which  
may have indirectly resulted from the usage of pstate-frequency, but are not  
related to the output or input handling in pstate-frequency itself, should be  
presented to the respective authors.


##### Intel thermald

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


##### x86_energy_perf_policy

Additional control may be achieved through the use of the  
x86_energy_perf_policy or equivalent binaries. Setting the  
x86_energy_perf_policy to the 'normal' mode can help pstate-frequency  
maintain stricter control of CPU frequencies, and this can sometimes  
help in keeping frequencies under control after system suspend and resume.  
There is a systemd unit included with pstate-frequency by default called  
x86_energy_perf_policy-sleep.service which will set the policy to normal  
upon system suspend/resume.


### Additional Notes

If you are attempting to use the udev rule but pstate-frequency fails to  
change the frequencies when the power state has changed, udev may be failing  
to notice your change of power source. Please read and understand the udev  
rule included in the res directory, before attempting to create a work  
around rule for your particular system. Should you succeed in creating a  
working udev rule for your system, please feel free to submit it to the  
main project as a pull request on GitHub.

At this time, pstate-frequency does not make use of any of the new files  
added to the pstate-driver in kernels 4 and up. The pstate-frequency program  
will only make use of the following sys files:  
+ /sys/devices/system/cpu/intel_pstate/max_perf_pct  
+ /sys/devices/system/cpu/intel_pstate/min_perf_pct  
+ /sys/devices/system/cpu/intel_pstate/no_turbo  

## Questions

Questions or issues should be either posted in the issue section of this  
repository, or directed by email to pyamsoft @ pyam(dot)soft(at)gmail(dot)com

## Issues

Check the issues page on GitHub for any notes about outstanding or existing  
issues. If you encounter a problem with pstate-frequency of which no such  
issue already exists please feel free to help the developer by creating an  
issue ticket.

## License

GPL v2
