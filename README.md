# pstate-frequency

Easily control CPU p-state driver on Linux (3.9 and upwards)

pstate-frequency is able to adjust the CPU p-state driver values for the
minimum and maximum scaling frequencies and the state of turbo boost.

### Requirements

- The intel_pstate (included in kernel 3.9 and upwards) OR
  the amd_pstate driver in "passive" mode (included in kernel 5.17) OR
  the amd_pstate driver in "active-epp" mode (included in kernel 6.5) OR
  the acpi-cpufreq driver (you have this).  
  Other CPU scaling drivers are not supported and so the behavior of
  the pstate-frequency script when running with an unsupported CPU driver is undefined.

- coreutils  
  The standard GNU tools for things like cat, tr, grep, and awk. These are
  generally expected on all modern Linux based systems. If you are in a
  situation where your machine does not have these basic tools, support cannot
  be offered at this time.

- Reading Comprehension and Understanding  
  You are also expected to read and understand the settings in the configuration
  file: config.mk.

### Installation

The installation follows a basic process:

```
$ make edit
# make install
```

While building, there are a couple of options that one may configure or
change:

- The directory to install to (defaults to /usr/local)
- Install a set of default power plans (defaults to Yes)
- Install bash completion (defaults to Yes)
- Install zsh completion (defaults to No)
- Install fish completion (defaults to No)
- Install systemd related unit files (defaults to Yes)
- Install udev rules for automatic frequency setting (defaults to Yes)
- Install this README as documentation (defaults to Yes)

#### Distribution Specific Installations

At this time, only ArchLinux is officially supported via the Arch User
Repository (AUR).

pstate-frequency comes with a PKGBUILD provided in the assets directory.  
The _stable/PKGBUILD_ will pull the latest released version from GitHub
and compile it for ArchLinux.  
The _git/PKGBUILD_ will pull the latest version from the dev repository on
GitHub and compile it for ArchLinux.

Debian package management and RedHat package management based distributions
are currently unofficially supported through the **checkinstall** program
and this is the recommended way to install the package until an actual
package can be constructed. Please see the `docheckinstall` script in
`assets/checkinstall`

For those who are brave and running an RPM based system, a spec file exists at
`assets/rpm` which should build the latest release version. You will need all
of the usual `rpmdev` related tools and a working `rpmdev tree` already setup.
Since I do not actually run an RPM based machine (I use Arch btw), I won't be
able to ensure this file is kept working, so its a "best effort" basis. I have
at least verified that the rpmspec builds in a Fedora 39 container, for what
that is worth! See the original RPM spec inspiration
[here](https://github.com/pyamsoft/pstate-frequency/issues/25)

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

The script by default is named **pstate-frequency**  
When called without any options, the program will display something like
the following:  
[![Example Run](https://raw.githubusercontent.com/pyamsoft/pstate-frequency/main/assets/img/pstate-frequency_example_run.png)](https://github.com/pyamsoft/pstate-frequency)

When called with the _-G --get_ option, the program will display something
like the following:  
[![Example Get](https://raw.githubusercontent.com/pyamsoft/pstate-frequency/main/assets/img/pstate-frequency_example_get.png)](https://github.com/pyamsoft/pstate-frequency)

When called with the _-S --set_ option, the program will display something
like the following:  
[![Example Set](https://raw.githubusercontent.com/pyamsoft/pstate-frequency/main/assets/img/pstate-frequency_example_set.png)](https://github.com/pyamsoft/pstate-frequency)

The _-G --get_ option can be called by a normal user and can take various
flags as optional arguments:

- **-c --current** Display the current user-requested CPU values
- **-r --real** Display real-time CPU frequencies

_By default, the calling the -G option with no extra arguments is equivalent
to calling with the --current option_

The _-S --set_ option can only be called by a user with root permissions, and
takes various flags as required arguments:

- **--delay** Delays the execution of the set commands by 5 seconds.  
   This is naive but useful for system startup where various bits of the machine
  may not be ready until a certain amount of time has passed.

  _It is mainly used in the systemd unit_

- **-m --max** Adjust the maximum scaling frequency of the CPU  
   Can be specified as a plain number or in the syntax `+<number>`
  which will resolve to the minimum CPU frequency + `number`
- **-n --min** Adjust the minimum scaling frequency of the CPU  
   Can be specified as a plain number or in the syntax `+<number>`
  which will resolve to the minimum CPU frequency + `number`
- **-g --governor** Adjust the current CPU scaling governor.
  Must be the name of a valid CPU governor
- **-t --turbo** Adjust the current state of Turbo Boost.  
   Can be specified as `on` or `off`. Adjusts turbo boost for
  all cores.
- **-p --plan** Adjust the maximum scaling and Turbo Boost to a preset plan.  
   Must be specified as a plan name inside of `/etc/pstate-frequency.d/`
- **-e --epp** Adjust the current CPU Energy Performance Preference
  Must be the name of a valid EPP profile - other than "default"

_By default, calling the -S option with no extra arguments will return an
error_

##### Power Plans

Power Plans are convenience shortcuts which essentially alias to calling
pstate-frequency with various options for --max, --min, --turbo, --epp, and
--governor. They are not necessary for operation, but are recommended.

There are five default power plans:

1. **powersave** Sets the minimum scaling frequency to the lowest available
   frequency, the maximum scaling frequency to 30% above the minimum scaling
   frequency within the possible range, configures the scaling governor and
   energy performance preference to "powersave" equivalent settings, and
   disables Turbo Boost.
2. **balanced** Sets the minimum scaling frequency to the lowest available
   frequency, the maximum scaling frequency to the highest available frequency,
   configures the scaling governor and energy performance preference to
   "balanced" equivalent settings, and enables Turbo Boost.
3. **performance** Sets the minimum scaling frequency to the lowest available
   frequency, the maximum scaling frequency to the highest available frequency,
   configures the scaling governor and energy performance preference to
   "performance" equivalent settings, and enables Turbo Boost.
4. **max** Sets the minimum and maximum scaling frequencies to the highest
   available frequency taking into account Turbo Boost frequencies, configures
   the scaling governor and energy performance preference to "max" equivalent
   settings, and enables Turbo Boost.
5. **auto** If the computer's main powersource is online, then the
   _balanced_ plan is set. If the main powersource is offline, then the
   _powersave_ plan is set.

The power plans live in /etc/pstate-frequency.d/ and can be edited to change
how each performs. An example of the various available plan options can be
found in `res/plans/README`

Note that the power plans are simple shell script files which are sourced
by pstate-frequency at run time, potentially as the root user. As such, please
make sure that the plans do not contain any malicious code, or really, anything
other than the accepted variables. pstate-frequency does not ship with plans
that execute any code and you are responsible for what you configure your own
plans to do.

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

_For contemporary Intel processors, the frequency is controlled by the
processor itself and the P-states exposed to software are related to
performance levels. The idea that frequency can be set to a single
frequency is fiction for Intel Core processors. Even if the scaling
driver selects a single P state the actual frequency the processor
will run at is selected by the processor itself._

This means that although a certain value may be requested through the
pstate-frequency software interface, the actual frequency that the CPU
will run at is decided solely by the hardware itself.
pstate-frequency is only able to offer "suggestions" as to what
frequency should be run, not make strict rules.

## Troubleshooting

### Settings do not apply on resume

Please make sure that you are using the `pstate-frequency-sleep.service`
to apply the script again when the system resumes from suspend. Also see
issue [17](https://github.com/pyamsoft/pstate-frequency/issues/17)

### Additional Notes

If you are attempting to use the udev rule but pstate-frequency fails to
change the frequencies when the power state has changed, udev may be failing
to notice your change of power source. Please read and understand the udev
rule included in the res directory, before attempting to create a work
around rule for your particular system. Should you succeed in creating a
working udev rule for your system, please feel free to submit it to the
main project as a pull request on GitHub.

At this time, pstate-frequency does not make use of any of the new files
added to the intel_pstate driver in kernels 4 and up. pstate-frequency
will only make use of the following sys files:

With the intel_pstate driver:

- /sys/devices/system/cpu/intel_pstate/max_perf_pct
- /sys/devices/system/cpu/intel_pstate/min_perf_pct
- /sys/devices/system/cpu/intel_pstate/no_turbo

With acpi-cpufreq OR amd_pstate (passive):

- /sys/devices/system/cpu/cpufreq/boost

## Development

pstate-frequency is developed in the open on Github at:

```
https://github.com/pyamsoft/pstate-frequency
```

Thanks to [edo1](https://github.com/edo1) for a systemd service file
that allows picking a plan instead of relying on a hardcoded one.  
[Pull Request](https://github.com/pyamsoft/pstate-frequency/pull/35)

## Issues

Check the issues page on GitHub for any notes about outstanding or existing
issues. If you encounter a problem with pstate-frequency of which no such
issue already exists please feel free to help the developer by creating an
issue ticket.

## License

GPLv2

```
  The GPLv2 License

    Copyright (C) 2024  pyamsoft

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
```
