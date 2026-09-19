#!/usr/bin/env bats
#
# $ bats tests/pstate-frequency.bats

setup() {
  PSTATE_BIN="${BATS_TEST_DIRNAME}/../pstate-frequency"

  export HOME="${BATS_TEST_TMPDIR}/home"
  mkdir -p "${HOME}"

  # TODO(Peter): Do we need to setup a fake /sys and stuff here? How would this work
}

@test "version prints the version and exits 0" {
  run "${PSTATE_BIN}" version
  [ "${status}" -eq 0 ]
  [[ "${output}" == "pstate-frequency <"* ]]
}

@test "no arguments prints usage and exits 0" {
  run "${PSTATE_BIN}"
  [ "${status}" -eq 0 ]
  [[ "${output}" == *"[general]"* ]]
}

@test "unknown command prints usage and exits 0" {
  run "${PSTATE_BIN}" bogus
  [ "${status}" -eq 0 ]
  [[ "${output}" == *"[general]"* ]]
}
