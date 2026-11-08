# Load with: source /path/to/programming/tools/compro.sh
# Supports Bash (WSL) and Zsh (macOS).

if [ -n "${BASH_VERSION-}" ]; then
  if [ "${BASH_SOURCE[0]}" = "$0" ]; then
    printf '%s\n' '[FAIL] load this file with: source tools/compro.sh' >&2
    exit 2
  fi
  _compro_source=${BASH_SOURCE[0]}
elif [ -n "${ZSH_VERSION-}" ]; then
  case ${ZSH_EVAL_CONTEXT-} in
    *:file) ;;
    *)
      printf '%s\n' '[FAIL] load this file with: source tools/compro.sh' >&2
      exit 2
      ;;
  esac
  _compro_source=${(%):-%x}
else
  printf '%s\n' '[FAIL] compro.sh requires Bash or Zsh.' >&2
  return 2 2>/dev/null || exit 2
fi

_compro_dir=${_compro_source%/*}
if [ "$_compro_dir" = "$_compro_source" ]; then
  _compro_dir=.
fi
_COMPRO_ROOT=$(CDPATH= cd -- "$_compro_dir/.." && pwd -P) || return
unset _compro_source _compro_dir

cplisten() (
  cd -- "$_COMPRO_ROOT" || return
  command python3 tools/samples.py "$@"
)

_compro_run() (
  local _compro_mode=$1
  shift
  cd -- "$_COMPRO_ROOT" || return

  # Help should not generate files or compile a program.
  case ${1-} in
    -h|--help)
      command python3 "tools/$_compro_mode.py" "$@"
      return
      ;;
  esac

  command mkdir -p .build || return
  local _compro_output
  _compro_output=$(command mktemp .build/output.XXXXXX) || return
  trap 'command rm -f "$_compro_output"' EXIT

  # Keep the previous submission file intact if expansion fails.
  command python3 tools/expander.py main.cpp > "$_compro_output" || return
  command mv -f "$_compro_output" output.cpp || return
  printf '%s\n' '[INFO] generated output.cpp'
  command python3 "tools/$_compro_mode.py" "$@"
)

cprun() {
  _compro_run test "$@"
}

dbrun() {
  _compro_run run "$@"
}

cpsave() (
  cd -- "$_COMPRO_ROOT" || return
  command python3 tools/archive.py "$@"
)
