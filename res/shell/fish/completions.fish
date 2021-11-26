# Disable file completions
complete -c pstate-frequency -f

complete -c pstate-frequency -s H -l "help" -d "Display help"
complete -c pstate-frequency -s V -l "version" -d "Display application version"

# [get] options
complete -c pstate-frequency -s G -l "get" -d "Access current CPU values"
complete -c pstate-frequency -n "__fish_contains_opt -s G get" -s c -l current -d "Display the current user set CPU values"
complete -c pstate-frequency -n "__fish_contains_opt -s G get" -s r -l real -d "Display the real time CPU frequences"
complete -c pstate-frequency -n "__fish_contains_opt -s G get" -l loop -r -d "Continually query real time CPU frequencies with a delay of <time> between loops."

# [set] options
complete -c pstate-frequency -s S -l "set" -d "Modify current CPU values"
complete -c pstate-frequency -n "__fish_contains_opt -s S set" -s p -l plan -r -d "Set a predefined power plan <name>"
complete -c pstate-frequency -n "__fish_contains_opt -s S set" -s g -l governor -r -d "Set the cpufreq governor <name>"
complete -c pstate-frequency -n "__fish_contains_opt -s S set" -s m -l max -r -d "Modify current CPU max frequency <number|offset>"
complete -c pstate-frequency -n "__fish_contains_opt -s S set" -s n -l min -r -d "Modify current CPU min frequency <number|offset>"
complete -c pstate-frequency -n "__fish_contains_opt -s S set" -s t -l turbo -r -d "Modify current CPU turbo boost <number|name>"
