# fish completions for pstate-frequency

set -l noopt "not __fish_contains_opt -s G -s S -s c -s r -s p -s g -s m -s n -s t get set loop plan governor max min turbo"

# Disable file completions
complete -c pstate-frequency -f

complete -c pstate-frequency -n "$noopt" -s H -l "help" -d "Display help"
complete -c pstate-frequency  -n "$noopt" -s V -l "version" -d "Display application version"

# [get] options
set -l get_layer "__fish_contains_opt -s G get"
complete -c pstate-frequency -n "$noopt" -s G -l "get" -d "Access current CPU values"
complete -c pstate-frequency -n "$get_layer" -s c -l current -d "Display the current user set CPU values"
complete -c pstate-frequency -n "$get_layer" -s r -l real -d "Display the real time CPU frequences"
complete -c pstate-frequency -n "$get_layer" -l loop -r -d "Continually query real time CPU frequencies with a delay of <time> between loops."

# [set] options
set -l set_layer "__fish_contains_opt -s S set"
complete -c pstate-frequency -n "$noopt" -s S -l "set" -d "Modify current CPU values"
complete -c pstate-frequency -n "$set_layer" -s p -l plan -r -d "Set a predefined power plan <name>"
complete -c pstate-frequency -n "$set_layer" -s g -l governor -r -d "Set the cpufreq governor <name>"
complete -c pstate-frequency -n "$set_layer" -s m -l max -r -d "Modify current CPU max frequency <number|offset>"
complete -c pstate-frequency -n "$set_layer" -s n -l min -r -d "Modify current CPU min frequency <number|offset>"
complete -c pstate-frequency -n "$set_layer" -s t -l turbo -r -d "Modify current CPU turbo boost <number|name>"

