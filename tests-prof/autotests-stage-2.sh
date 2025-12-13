set +e

dokill=true
if [ "$#" -ge 1 -a "$1" = "-n" ]; then
  dokill=false
  shift
fi

ran_daemon=false

contains () {
  local e match="$1"
  shift
  for e; do [ "$e" = "$match" ] && return 0; done
  return 1
}

if ! command -v python3 >/dev/null 2>/dev/null; then
  printf "The command `python3` not exist. Please install python.\n" >&2
  exit 1
fi

if contains "1" "$@"; then
  python3 test-jalon-1.py
  ran_daemon=true
fi

if [ "$#" -eq 0 ] || contains "2" "$@"  || contains "2c" "$@" ; then
  ./run-tadmor-tests-jalon-2.sh
fi


if [ "$#" -eq 0 ] || contains "2" "$@"  || contains "2d" "$@" ; then
  python3 run-erraid-tests-jalon-2.py
fi

if $dokill && $ran_daemon ; then
  killall -q -u "$USER" erraid >/dev/null 2>/dev/null
  sleep 1
  killall -q -9 -u "$USER" erraid >/dev/null 2>/dev/null
  true
fi
