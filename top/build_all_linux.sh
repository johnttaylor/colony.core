#!/bin/bash
# This script is used by the CI/Build machine to build the Linux test
# project on a Linux box
#
# usage: build_all_linux.sh 
#
set -e

# Get the root directory
HERE="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Put the current directory into the command path (simplifies invoking the nqbp.py scripts)
export PATH=$PATH:./

# Configure NQBP
export NQBP_XPKG_MODEL=mixed
export NQBP_PKG_ROOT=$HERE
export NQBP_WORK_ROOT=$HERE/..
export NQBP_XPKGS_ROOT=$HERE/xsrc
export NQBP_BIN=$NQBP_XPKGS_ROOT/nqbp

alias bob="$HERE/xsrc/nqbp/other/bob.py"
alias chuck="$HERE/xsrc/nqbp/other/chuck.py"

# Build all linux unit tests
cd tests
bob -v linux --bld-all -gt
