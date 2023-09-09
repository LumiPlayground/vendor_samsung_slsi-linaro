#!/bin/sh

# The intention with this script is to have a simple way to test BT
# on/off. The script should be run on the target device. This is done by
# opening and closing the h4 interface. If this works without error we
# consider it a pass, if an error code returns on the open call we
# consider it a fail and exit.
#
# A specific number of iteations can be provided as an optinal argument.

if [ -z "$1" ]; then
    ITERATIONS=3
else
    ITERATIONS=$1
fi

case $ITERATIONS in
    ''|*[!0-9]*) echo error: Argument is not an integer; exit 1 ;;
    *) echo Running $ITERATIONS iterations ;;
esac

for i in `seq $ITERATIONS`; do
    echo -n Iteration "$i: $OUTPUT"
    OUTPUT=`timeout 0.5 cat /dev/scsc_h4_0 2>&1`
    case "$OUTPUT" in
        *no\ devices/emulators\ found) echo ""; exit 1 ;;
        */dev/scsc_h4_0:\ Bad\ address) echo ""; exit 1 ;;
        */dev/scsc_h4_0:\ Invalid\ argument) echo ""; exit 1 ;;
        */dev/scsc_h4_0:\ I/O\ error) echo ""; exit 1  ;;
        *) echo OK ;;
    esac
    # We expect to be be able to start BT essentially immediately after
    # we successfully have closed the h4 interface, but we have a small
    # delay here as the intention with this script is not to expose
    # inner race conditions in the driver.
    sleep .2
done

