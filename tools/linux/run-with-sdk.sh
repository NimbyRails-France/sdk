#!/bin/sh
set -eu
if [ "$#" -lt 2 ]; then
    printf '%s\n' 'Usage: run-with-sdk.sh /absolute/SDK.so /absolute/nimbyrails [arguments...]' >&2
    exit 2
fi
sdk=$1
shift
case "$sdk" in /*) ;; *) printf '%s\n' 'An absolute SDK path is required' >&2; exit 2 ;; esac
test -f "$sdk"
test -x "$1"
export NRF_LINUX_OBSERVATION=1
export LD_PRELOAD="$sdk${LD_PRELOAD:+:$LD_PRELOAD}"
exec "$@"
