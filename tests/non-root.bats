#!/usr/bin/env bats
#
# $ bats tests/non-root.bats

load test_helper

_CPU_MIN=100000
_CPU_MAX=1000000

setup() {
  if ! pf_can_unshare_user; then
    skip "unprivileged user namespaces (unshare --user) are not available"
  fi

  if [ "$(id -u)" -eq 0 ]; then
    skip "run this suite as non-root (use bin/test)"
  fi

  pf_setup_fixture
  pf_add_cpu 0 "${_CPU_MAX}" "${_CPU_MIN}"
}

@test "-V prints the version and exits 0" {
  pf_run nonroot -V
  [ "${status}" -eq 0 ]
  [[ "${output}" == "pstate-frequency <"* ]]
}

@test "-H prints usage and exits 0" {
  pf_run nonroot -H
  [ "${status}" -eq 0 ]
  [[ "${output}" == "pstate-frequency <"* ]]
  [[ "${output}" == *"[general]"* ]]
}

@test "no arguments prints usage and exits 0" {
  pf_run nonroot
  [ "${status}" -eq 0 ]
  [[ "${output}" == *"[general]"* ]]
}

@test "unknown command prints usage and exits 0" {
  pf_run nonroot bogus
  [ "${status}" -eq 0 ]
  [[ "${output}" == *"[general]"* ]]
}

@test "-G -c reports current values for a generic acpi-cpufreq CPU" {
  pf_add_generic_boost 1

  pf_run nonroot -G -c
  [ "${status}" -eq 0 ]
  [[ "${output}" == *"CPU_DRIVER   -> acpi-cpufreq"* ]]
  [[ "${output}" == *"CPU_GOVERNOR -> powersave"* ]]
  [[ "${output}" == *"TURBO        -> 1 [ON]"* ]]
  [[ "${output}" == *"EPP          -> unsupported"* ]]
  [[ "${output}" == *"CPU_MIN      -> 10% [100MHz]"* ]]
  [[ "${output}" == *"CPU_MAX      -> 100% [1000MHz]"* ]]
}

@test "-G -c reports current values for an intel_pstate CPU" {
  pf_add_cpu 0 "${_CPU_MAX}" "${_CPU_MIN}" intel_pstate
  pf_add_intel_pstate 100 0 0

  pf_run nonroot -G -c
  [ "${status}" -eq 0 ]
  [[ "${output}" == *"TURBO        -> 0 [ON]"* ]]
}

@test "-G -c reports current values for an amd_pstate CPU" {
  pf_add_cpu 0 "${_CPU_MAX}" "${_CPU_MIN}" amd-pstate-epp
  pf_add_amd_pstate_turbo 1

  pf_run nonroot -G -c
  [ "${status}" -eq 0 ]
  [[ "${output}" == *"TURBO        -> 1 [ON]"* ]]
}

@test "-G -r prints one CPU line per fixture core" {
  pf_add_cpu 1 "${_CPU_MAX}" "${_CPU_MIN}"
  pf_add_cpu 2 "${_CPU_MAX}" "${_CPU_MIN}"

  pf_run nonroot -G -r
  [ "${status}" -eq 0 ]
  [[ "${output}" == *"CPU[0]"* ]]
  [[ "${output}" == *"CPU[1]"* ]]
  [[ "${output}" == *"CPU[2]"* ]]
}

@test "-G -r ignores core with a missing scaling_cur_freq" {
  pf_add_cpu 1 "${_CPU_MAX}" "${_CPU_MIN}"
  pf_remove_cpu_file 1 scaling_cur_freq

  pf_run nonroot -G -r
  [ "${status}" -eq 0 ]
  [[ "${output}" == *"CPU[0]"* ]]
  [[ "${output}" != *"CPU[1]"* ]]
}

@test "-S  is blocked by the root check" {
  pf_run nonroot -S -m 80
  [ "${status}" -ne 0 ]
  [[ "${output}" == *"You must be root."* ]]

  pf_run nonroot -S -m abc
  [ "${status}" -ne 0 ]
  [[ "${output}" == *"max argument 'abc' is not a number"* ]]
  [[ "${output}" != *"You must be root."* ]]

  pf_run nonroot -S -t bogus
  [ "${status}" -ne 0 ]
  [[ "${output}" == *"turbo argument 'bogus' is invalid"* ]]
  [[ "${output}" != *"You must be root."* ]]
}
