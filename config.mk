# The MIT License (MIT)
#
# Copyright (c) 2016 Peter Kenji Yamanaka
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

##
# Name of the script
#
# You can change this, but note that it may have unforseen consequences
#
# For example, this does not currently change the content inside of
# any res/ files. Use at your own risk.
##
NAME=pstate-frequency

##
# Install prefix
#
# Understand that this prefix will only affect the destination of the
# installed pstate-frequency binary and the installed documentation
#
# The bash-completion, zsh-completion, udev rule, and systemd unit files
# expect to be installed into the /usr directory, and may not work if they
# are installed anywhere other than the /usr directory.
#
# [/usr/local] | /usr | other
##
PREFIX?=/usr/local

##
# power plan files
#
# Include default power plans
#
# 0 NO | [1 YES]
##
INCLUDE_POWER_PLANS?=1

##
# bash-completion
#
# Include a bash completion file
#
# 0 NO | [1 YES]
##
INCLUDE_BASH_COMPLETION?=1

##
# zsh-completion
#
# Include a zsh completion file
#
# [0 NO] | 1 YES
##
INCLUDE_ZSH_COMPLETION?=0

##
# systemd services
#
# Include a systemd unit
#
# 0 NO | [1 YES]
##
INCLUDE_SYSTEMD_UNIT?=1

##
# udev rules
#
# Include a udev rule which will set the pstate to powersave on battery and
# performance on AC power
#
# 0 NO | [1 YES]
##
INCLUDE_UDEV_RULE?=1

##
# Install README.md
#
# Installs a copy of the README.md file to
# $(PREFIX)/share/doc/pstate-frequency/README.md
#
# 0 NO | [1 YES]
##
INCLUDE_DOC?=1

##
# Install copy of the MIT License
#
# Installs a copy of the MIT License
# $(PREFIX)/share/doc/pstate-frequency/project
#
# 0 NO | [1 YES]
##
INCLUDE_LICENSE?=1

##
# Make Flags
#
# Do NOT modify these unless you know what you are doing
##
MAKEFLAGS:= $(MAKEFLAGS) --no-print-directory
