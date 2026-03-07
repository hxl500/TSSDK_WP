#!/bin/bash

err_handler() {
	ret=$?
	[ "$ret" -eq 0 ] && return

	echo "ERROR: Running ${FUNCNAME[1]} failed!"
	echo "ERROR: exit code $ret from ${BASH_SOURCE[1]} line: ${BASH_LINENO[0]}"
	echo "    $BASH_COMMAND"
	exit $ret
}
trap 'err_handler' ERR
set -eE

