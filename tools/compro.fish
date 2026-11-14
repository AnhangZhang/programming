# Load with: source /path/to/programming/tools/compro.fish
# Run the shared implementation in Bash without changing fish's directory.
set -l _compro_bash_source (path resolve (path dirname (status filename))/compro.sh)

function _compro_dispatch --inherit-variable _compro_bash_source
    command bash -c 'source "$1" || exit; shift; "$@"' compro "$_compro_bash_source" $argv
end

function cplisten
    _compro_dispatch cplisten $argv
end

function cprun
    _compro_dispatch cprun $argv
end

function dbrun
    _compro_dispatch dbrun $argv
end

function cpsave
    _compro_dispatch cpsave $argv
end
