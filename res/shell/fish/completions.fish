# fish completions for pstate-frequency


set -l noopt "not __fish_contains_opt -s G -s S -s c -s r -s p -s g -s m -s n -s t get set plan governor max min turbo"

# Disable file completions
complete -f -c pstate-frequency

complete -f -c pstate-frequency -n "$noopt" -s H -l "help" -d "Display help"
complete -f -c pstate-frequency  -n "$noopt" -s V -l "version" -d "Display application version"

# [get] options
set -l get_layer "__fish_contains_opt -s G get"
complete -f -c pstate-frequency -n "$noopt" -s G -l "get" -d "Access current CPU values"
complete -f -c pstate-frequency -n "$get_layer" -s c -l current -d "Display the current user set CPU values"
complete -f -c pstate-frequency -n "$get_layer" -s r -l real -d "Display the real time CPU frequences"

# [set] options
set -l set_layer "__fish_contains_opt -s S set"

function __fish_pstate-frequency_plans
    set -l POWER_PLAN_CONFIG_DIR "/etc/pstate-frequency.d"
    for plan in "$POWER_PLAN_CONFIG_DIR/*"
        string match -r "\\d+\\-(\\w+)\\.plan" -- "$plan" | awk 'FNR == 2 {print}'
    end
end

function __fish_pstate-frequency_governors
    set -l SYSTEM_CPU_DIR "/sys/devices/system/cpu"
    set -l SYSTEM_CPU_GOVERNORS "$SYSTEM_CPU_DIR/cpu0/cpufreq/scaling_available_governors"
    command cat "$SYSTEM_CPU_GOVERNORS" | tr ' ' '\n'
end

function __fish_pstate-frequency_turbo_values
    printf "%s\t%s\n" "on" "Turn turbo boost on"
    printf "%s\t%s\n" "off" "Turn turbo boost off"
    printf "1\n0\n"
end

complete -f -c pstate-frequency -n "$noopt" -s S -l "set" -d "Modify current CPU values"
complete -f -c pstate-frequency -n "$set_layer" -s p -l plan -d "Set a predefined power plan"
complete -f -c pstate-frequency -n "$set_layer; and __fish_contains_opt -s p plan" -kxa "(__fish_pstate-frequency_plans)"
complete -f -c pstate-frequency -n "$set_layer" -s g -l governor -d "Set the cpufreq governor"
complete -f -c pstate-frequency -n "$set_layer; and __fish_contains_opt -s g governor" -kxa "(__fish_pstate-frequency_governors)"
complete -f -c pstate-frequency -n "$set_layer" -s m -l max -r -d "Modify current CPU max frequency"
complete -f -c pstate-frequency -n "$set_layer" -s n -l min -r -d "Modify current CPU min frequency"
complete -f -c pstate-frequency -n "$set_layer" -s t -l turbo -d "Modify current CPU turbo boost"
complete -f -c pstate-frequency -n "$set_layer; and __fish_contains_opt -s t turbo" -kxa "(__fish_pstate-frequency_turbo_values)"

