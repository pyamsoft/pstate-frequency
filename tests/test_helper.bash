#!/bin/sh

PSTATE_BIN="${BATS_TEST_DIRNAME}/../pstate-frequency"

##
# Check that we can unshare, by actually attempting to unshare lol
pf_can_unshare_user() {
  unshare --user --map-root-user --mount -- \
    unshare --user --map-user="$(id -u)" --map-group="$(id -g)" -- true >/dev/null 2>&1
}

##
# Path to test fixture directory
pf_fixture_root() {
  printf -- '%s/fixture' "${BATS_TEST_TMPDIR}"
}

##
# Global setup of fixture system
pf_setup_fixture() {
  fixture="$(pf_fixture_root)"

  mkdir -p "${fixture}/sys/devices/system/cpu/cpufreq"
  mkdir -p "${fixture}/sys/class/power_supply"
  mkdir -p "${fixture}/etc/pstate-frequency.d"
  mkdir -p "${fixture}/proc"
  touch "${fixture}/proc/cpuinfo"

  unset fixture
}

##
# Add fake CPU
# $1 cpu number
# $2 max frequency
# $3 min frequency
# $4 scaling driver
pf_add_cpu() {
  pf_add_cpu__idx="$1"
  pf_add_cpu__max_khz="$2"
  pf_add_cpu__min_khz="$3"
  pf_add_cpu__driver="${4:-acpi-cpufreq}"

  pf_add_cpu__scaling_max_khz="${pf_add_cpu__max_khz}"
  pf_add_cpu__scaling_min_khz="${pf_add_cpu__min_khz}"
  pf_add_cpu__governor="powersave"
  pf_add_cpu__available_governors="performance powersave"

  pf_add_cpu__dir="$(pf_fixture_root)/sys/devices/system/cpu/cpu${pf_add_cpu__idx}/cpufreq"
  mkdir -p "${pf_add_cpu__dir}"

  printf -- '%s\n' "${pf_add_cpu__max_khz}" >"${pf_add_cpu__dir}/cpuinfo_max_freq"
  printf -- '%s\n' "${pf_add_cpu__min_khz}" >"${pf_add_cpu__dir}/cpuinfo_min_freq"
  printf -- '%s\n' "${pf_add_cpu__scaling_max_khz}" >"${pf_add_cpu__dir}/scaling_max_freq"
  printf -- '%s\n' "${pf_add_cpu__scaling_min_khz}" >"${pf_add_cpu__dir}/scaling_min_freq"
  printf -- '%s\n' "${pf_add_cpu__scaling_max_khz}" >"${pf_add_cpu__dir}/scaling_cur_freq"
  printf -- '%s\n' "${pf_add_cpu__driver}" >"${pf_add_cpu__dir}/scaling_driver"
  printf -- '%s\n' "${pf_add_cpu__governor}" >"${pf_add_cpu__dir}/scaling_governor"
  printf -- '%s\n' "${pf_add_cpu__available_governors}" >"${pf_add_cpu__dir}/scaling_available_governors"

  pf_add_cpu__cpuinfo="$(pf_fixture_root)/proc/cpuinfo"
  printf -- 'processor\t: %s\n' "${pf_add_cpu__idx}" >>"${pf_add_cpu__cpuinfo}"

  unset pf_add_cpu__idx
  unset pf_add_cpu__max_khz
  unset pf_add_cpu__min_khz
  unset pf_add_cpu__scaling_max_khz
  unset pf_add_cpu__scaling_min_khz
  unset pf_add_cpu__driver
  unset pf_add_cpu__governor
  unset pf_add_cpu__available_governors
  unset pf_add_cpu__dir
  unset pf_add_cpu__cpuinfo
}

##
# Kill a CPU
# $1 cpu number
# $2 target file
pf_remove_cpu_file() {
  rm -f "$(pf_fixture_root)/sys/devices/system/cpu/cpu$1/cpufreq/$2"
}

##
# Setup "intel_pstate"
# $1 max
# $2 min
# $3 turbo
pf_add_intel_pstate() {
  pf_add_intel_pstate__dir="$(pf_fixture_root)/sys/devices/system/cpu/intel_pstate"

  mkdir -p "${pf_add_intel_pstate__dir}"
  printf -- '%s\n' "$1" >"${pf_add_intel_pstate__dir}/max_perf_pct"
  printf -- '%s\n' "$2" >"${pf_add_intel_pstate__dir}/min_perf_pct"
  printf -- '%s\n' "$3" >"${pf_add_intel_pstate__dir}/no_turbo"

  unset pf_add_intel_pstate__dir
}

##
# Setup "amd_pstate"
# $1 turbo boost
pf_add_amd_pstate_turbo() {
  pf_add_amd_pstate_turbo__dir="$(pf_fixture_root)/sys/devices/system/cpu/amd_pstate"

  mkdir -p "${pf_add_amd_pstate_turbo__dir}"
  printf -- '%s\n' "$1" >"${pf_add_amd_pstate_turbo__dir}/cpb_boost"

  unset pf_add_amd_pstate_turbo__dir
}

##
# Setup "acpi-cpufreq"
# $1 turbo boost
pf_add_generic_boost() {
  printf -- '%s\n' "$1" >"$(pf_fixture_root)/sys/devices/system/cpu/cpufreq/boost"
}

##
# All input is parsed into the plan file
#
# $1 plan name
pf_add_plan() {
  cat >"$(pf_fixture_root)/etc/pstate-frequency.d/$1"
}

# Runs inside a private mount namespace with fixtures set up
pf_run() {
  pf_run__mode="$1"
  shift

  # We always enter a user namespace as a fake root to set up the mounts.
  # The "non root" mode then nests another user namespace to drop back to our own user.
  pf_run__drop=""
  if [ "${pf_run__mode}" != "root" ]; then
    pf_run__drop="unshare --user --map-user=$(id -u) --map-group=$(id -g) --"
  fi

  ##
  # Syntax here is sh -c "blahblah" (arg$0), ($arg1) ...
  # in this case, arg$0 is ignored since we don't use $0 in our embedded script
  #
  # shellcheck disable=SC2016,SC2086
  run unshare --user --map-root-user --mount -- sh -c '
    fixture="$1"
    pstate_bin="$2"
    drop="$3"

    shift
    shift
    shift

    mount --bind "${fixture}/sys/devices/system/cpu" /sys/devices/system/cpu || exit 99
    mount --bind "${fixture}/sys/class/power_supply" /sys/class/power_supply || exit 99
    mount --bind "${fixture}/proc/cpuinfo" /proc/cpuinfo || exit 99

    mount -t tmpfs tmpfs /etc || exit 99
    mkdir -p /etc/pstate-frequency.d || exit 99
    cp -a "${fixture}/etc/pstate-frequency.d/." /etc/pstate-frequency.d/ 2>/dev/null

    exec ${drop} "${pstate_bin}" "$@"
  ' __test_shell__ "$(pf_fixture_root)" "${PSTATE_BIN}" "${pf_run__drop}" "$@"

  unset pf_run__mode
  unset pf_run__drop
}
