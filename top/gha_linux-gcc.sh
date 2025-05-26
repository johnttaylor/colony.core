#!/usr/bin/env bash
set -e
set -x

# This script is used by the CI/Build machine to build the Linux projects
#
# The script ASSUMES that the working directory is the package root
#
# usage: build_linux.sh <bldnum>
# 

# setup the environment
source ./env.sh default


# Set the CI build flag
export NQBP_CI_BUILD="1"

# Build the Catch2 static library (just 64bit apps for now)
pushd projects
$NQBP_BIN/other/bob.py -v4 linux  -c --try posix64 
$NQBP_BIN/other/bob.py -v4 linux  --try cpp11 
popd

# Build all test linux projects (just 64bit apps for now)
pushd tests
$NQBP_BIN/other/bob.py -v4 linux -c --try posix64 --bldtime --bldnum $1
$NQBP_BIN/other/bob.py -v4 linux --try cpp11 --bldtime --bldnum $1

# Run unit tests
$NQBP_BIN/other/chuck.py -vt --match a.out --dir _posix64
$NQBP_BIN/other/chuck.py -v --match aa.out --dir _posix64
$NQBP_BIN/other/chuck.py -vt --match a.py --dir _posix64
$NQBP_BIN/other/chuck.py -v --match aa.py --dir _posix64
$NQBP_BIN/other/chuck.py -vt --match a.out --dir _cpp11
$NQBP_BIN/other/chuck.py -v --match aa.out --dir _cpp11
$NQBP_BIN/other/chuck.py -vt --match a.py --dir _cpp11
$NQBP_BIN/other/chuck.py -v --match aa.py --dir _cpp11
popd

# Generate code coverage metrics
MAGIC_DIR="$NQBP_PKG_ROOT/tests/Cpl/Checksum/_0test/linux/gcc"
COMBINED_CODE_COVERAGE_FILE="$NQBP_PKG_ROOT/cobertura.json"
COMBINED_CODE_COVERAGE_XML="$NQBP_PKG_ROOT/cobertura.xml"
if [ -f "$COMBINED_CODE_COVERAGE_FILE" ]; then
    rm -f "$COMBINED_CODE_COVERAGE_FILE"
fi
pushd tests
$NQBP_BIN/other/chuck.py -v --dir gcc --d2 linux --match tca.py args --ci rpt --json cobertura.json
$NQBP_BIN/other/chuck.py -v --dir gcc --d2 linux --match tca.py args --ci merge cobertura.json $COMBINED_CODE_COVERAGE_FILE
popd

# Convert the JSON data file to XML format
pushd $MAGIC_DIR
python tca.py --ci rpt -a $COMBINED_CODE_COVERAGE_FILE --xml $COMBINED_CODE_COVERAGE_XML
popd