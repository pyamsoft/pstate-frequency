#!/usr/bin/env bats
#
# $ bats tests/root.bats

load test_helper

_CPU_MIN=100000
_CPU_MAX=1000000

setup() {
  if ! pf_can_unshare_user; then
    skip "unprivileged user namespaces (unshare --user) are not available"
  fi

  pf_setup_fixture
  pf_add_cpu 0 "${_CPU_MAX}" "${_CPU_MIN}"
  pf_add_cpu 1 "${_CPU_MAX}" "${_CPU_MIN}"
  pf_add_generic_boost 0
}

@test "-S -m writes the max frequency to every core" {
  pf_run root -S -m 80
  [ "${status}" -eq 0 ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq")" -eq "$((_CPU_MAX * 8 / 10))" ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpu1/cpufreq/scaling_max_freq")" -eq "$((_CPU_MAX * 8 / 10))" ]
}

@test "-S -n writes the min frequency to every core" {
  pf_run root -S -n 20
  [ "${status}" -eq 0 ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq")" -eq "$((_CPU_MIN * 2))" ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpu1/cpufreq/scaling_min_freq")" -eq "$((_CPU_MIN * 2))" ]
}

@test "-S -g writes the governor to every core" {
  pf_run root -S -g performance
  [ "${status}" -eq 0 ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor")" = "performance" ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpu1/cpufreq/scaling_governor")" = "performance" ]
}

@test "-S -t on sets the generic boost file to 1" {
  pf_run root -S -t on
  [ "${status}" -eq 0 ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpufreq/boost")" = "1" ]
}

@test "-S -t off sets the generic boost file to 0" {
  pf_add_generic_boost 1

  pf_run root -S -t off
  [ "${status}" -eq 0 ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpufreq/boost")" = "0" ]
}

@test "-S -t on sets intel_pstate no_turbo to 0" {
  pf_add_intel_pstate 100 0 1

  pf_run root -S -t on
  [ "${status}" -eq 0 ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/intel_pstate/no_turbo")" = "0" ]
}

@test "-S -t off sets intel_pstate no_turbo to 1" {
  pf_add_intel_pstate 100 0 0

  pf_run root -S -t off
  [ "${status}" -eq 0 ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/intel_pstate/no_turbo")" = "1" ]
}

@test "-S -p applies a full manual plan" {
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpufreq/boost")" = "0" ]

  pf_add_plan "02-balanced.plan" <<'EOF'
PLAN_CPU_MAX="90"
PLAN_CPU_MIN="10"
PLAN_CPU_TURBO="on"
PLAN_CPU_CPUFREQ_GOVERNOR="performance"
EOF

  pf_run root -S -p balanced
  [ "${status}" -eq 0 ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor")" = "performance" ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpufreq/boost")" = "1" ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq")" -eq "$((_CPU_MAX * 9 / 10))" ]
}

@test "-S -p resolves a hyphenated custom plan name" {
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpufreq/boost")" = "0" ]

  pf_add_plan "10-my-custom-plan.plan" <<'EOF'
PLAN_CPU_TURBO="on"
EOF

  pf_run root -S -p my-custom-plan
  [ "${status}" -eq 0 ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpufreq/boost")" = "1" ]
}

@test "a leaked PLAN_ env var does not leak into a plan" {
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpufreq/boost")" = "0" ]

  pf_add_plan "20-turbo-only.plan" <<'EOF'
PLAN_CPU_TURBO="on"
EOF

  export PLAN_CPU_MAX="99"
  pf_run root -S -p turbo-only
  unset PLAN_CPU_MAX

  [ "${status}" -eq 0 ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpufreq/boost")" = "1" ]
  [ "$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq")" -eq "${_CPU_MAX}" ]
}

@test "-S -m and -n together keeps min below max" {
  pf_run root -S -m 30 -n 50
  [ "${status}" -eq 0 ]
  pf_run__min="$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq")"
  pf_run__max="$(cat "$(pf_fixture_root)/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq")"
  [ "${pf_run__min}" -lt "${pf_run__max}" ]
}
