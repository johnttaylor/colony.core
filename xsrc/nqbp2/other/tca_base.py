#=============================================================================
# Helper script for analysis code coverage result generated by the gcc compiler
#=============================================================================

#
import sys   
import os
import subprocess
import fnmatch
import platform
#
from nqbplib.docopt.docopt import docopt
from nqbplib import utils


#
usage = """ 
(T)est (C)overage (A)nalyzer
===============================================================================
Usage: tca [options] rpt [<args>...]
       tca [options] -d OFILE
       tca [options] -m PATTERN
       tca [options] -c
       tca [options] --clean
       tca [options] merge <SRC> <DST>

Arguments:
  rpt              Uses 'gcovr' to generate a summary report of the code 
                   coverage.  The optional '<args>' are passed directory to
                   'gcovr' 
  -d OFILE         Generates human readable '.gcov' for the specified 'OFILE' in
                   the project/object tree. The '.gcov' files are placed in the
                   current working directory
  -m PATTERN       Searches the file(s) identified by 'PATTERN' for code lines
                   that did NOT execute.  You must run 'tca -d OFILE' before
                   using this option.
  -c               Removes only the '.gcov' files. Note: You do NOT have 
                   re-compile or run the executable again before rerunning this 
                   script.
  --all            TCA configures 'gcovr' to exclude unreachable arcs. The --all 
                   option includes all 'arcs'.  The exclusion of 'arcs' is 
                   attempt to improve branch coverage metrics for C++ code.
  -z, --clean      Removes ALL code coverage related files.  You will have to
                   RE-COMPILE and RE-RUN the executable before running this script 
                   again.
  merge            Merges the SRC code coverage output in single/combined DST
                   coverage output file.
  SRC              The source coverage output file to merge (i.e test coverage
                   for a single unit test).
  DST              The destination coverage output file to merge into (i.e. the
                   combined coverage output file).    

Options:
  --ci             Uses the repository root instead of the src/ directory. This
                   is a work-around for Jenkins-Cobertura plugin
  -w LINES         Size of the 'window' when displaying the output when using
                   the '-m' option.  [Default: 2].
  -h, --help       Display command help.
        
         
Examples:
    # Generates the default code coverage report
    tca.py rpt     
    
    # Generates the branch coverage report
    tca.py rpt --branches
    
    # Generates XML formatted output
    tca.py rpt --xml-pretty
    
    # Generates detailed HTML output
    tca.py rpt --html --html-details -o coverage.html
    
    # Gets the list of 'gcovr' of command line options
    tca.py rpt -h
    
    # Filters the output to a single component directory
    tca.py rpt -f .*Cpl/Container.*

    # Combine the code coverage output from multiple unit tests
    tca.py rpt --json coverage.json
    tca.py merge coverage.json combined.json

NOTES:
    o TCA assumes you used the gcc compiler to create you executable.
    o When compiling you must use the following options:
        '-fprofile-arcs -ftest-coverage'
    o When linking you must use the following options:
        '-fprofile-arcs -lgcov'
    o You must first run your executable BEFORE running TCA
    o It is RECOMMENDED that you run 'tca --clean' before you compile or run the
      executable.
    

"""

#------------------------------------------------------------------------------
# Parse command line
def run(argv):
    # Process command line args...
    args = docopt(usage, version="0.0.1", options_first=True)
    
    # get the package root
    pkg = os.environ.get('NQBP_PKG_ROOT')
    gcovr_root = f'-r {pkg}/src'
    if args['--ci']:
        gcovr_root = f'--filter .*src/.* -r {pkg}'

    # setup excludes and other options
    arcopt = ' --exclude-unreachable-branches --decisions ' # Setup 'arc' excludes for C++ code (see https://gcovr.com/en/stable/faq.html) 
    if ( args['--all'] ):
        arcopt = ''
    excludes = '--exclude=.*_0test.* --exclude=.*/xsrc/.* --exclude=.*src/Catch.* --exclude=.*src/Cpl/Json/Arduino.h --exclude=.*src/Cpl/Json/ArduinoHelpers.cpp --exclude=.*src/Cpl/Type/enum.h --exclude-unreachable-branches --exclude-lines-by-pattern .*CPL_SYSTEM_TRACE.* --exclude-lines-by-pattern .*CPL_SYSTEM_ASSERT.*'
    gcovr_args = f'{excludes} --gcov-ignore-parse-errors=negative_hits.warn -j 8 {arcopt} {gcovr_root} --object-directory . '
    

    # Generate summary
    if (args['rpt']):
        python = 'python'
        cmd  = f"{python} -m gcovr {gcovr_args} {' '.join(args['<args>']) if args['<args>'] else ''} ."
        if (args['<args>']):
            first = args['<args>'][0]
            if (first == '-h' or first == '--help'):
                cmd = '{} -m gcovr --help'.format(python)
        print(cmd)        
        run_shell(cmd, True)

    # Generate human readable .gcov files
    elif(args['-d']):
        cmd = 'gcov ' + args['-d']
        run_shell(cmd)
        
    # Search file
    elif (args['-m']):
        search( os.getcwd(), args['-m'], args['-w'] )
        
    # Clean ALL
    elif (args['--clean']):
        clean( os.path.join(pkg,"projects"), ['*.gcov', '*.gcda', '*.gcno'] )
        clean(os.path.join(pkg,"tests"), ['*.gcov', '*.gcda', '*.gcno'])
        
    # Clean just .gcov files
    elif (args['-c']):
        clean( os.getcwd(), ['*.gcov'] )
        
    # Merge code coverage output files
    elif (args['merge']):
        merge_files( gcovr_args, args['<SRC>'], args['<DST>'] )

#------------------------------------------------------------------------------
def run_shell(cmd, verbose_flag=False, on_err_msg=None):
    if (verbose_flag):
        p = subprocess.Popen(cmd, shell=True)
    else:
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    r = p.communicate()
    if (p.returncode != 0 and on_err_msg != None):
        exit(on_err_msg)
    
    r0 = '' if r[0] == None else r[0].decode()
    r1 = '' if r[1] == None else r[1].decode()
    return (p.returncode, "{} {}".format(r0,r1))


#------------------------------------------------------------------------------
def clean(root, filters):
    for root, dirs, files in os.walk(root):
        for x in filters:
            for f in fnmatch.filter(files,x):
                os.remove(os.path.join(root,f))

#------------------------------------------------------------------------------
def search(path, filter, wsize):
    for file in os.listdir(path):
        if (fnmatch.fnmatch(file,filter)):
            if (os.path.isfile(file)):
                print()
                print("FILE: {}".format(file))
                print("-" * 80)
                _searchfile(file, wsize)
                
                
def _searchfile(file, window_size):
    window = []
    wsize = int(window_size)
    with open(file) as fd:

        # Housekeeping
        pre_marker = True
        pre_count = 0
        post_marker = False
        post_count = 0
        marker = False

        for line in fd:
            # Test for marker
            if (line.strip().startswith('####')):
                window.append(line)
                pre_marker = False
                pre_count = 0
                post_marker = True                                 
                post_count = 0

            # Manage window pre-window
            elif (pre_marker):
                pre_count = pre_count + 1
                window.append(line)
                if (pre_count > wsize):
                    del window[0]

            # Manage window post-window
            elif (post_marker):
                post_count = post_count + 1
                window.append(line)
                if (post_count >= wsize):
                    _print_window(window)
                    post_marker = False
                    pre_marker = True
                    pre_count = len(window)
                    delta = pre_count - wsize - 1
                    for x in range(delta):
                        del window[0]
                        pre_count = pre_count - 1
                    while(pre_count > wsize):
                        del window[0]
                        pre_count = pre_count - 1
            
        # dump the window if file end before the window did
        if (post_marker):
            _print_window(window)
        
        
def _print_window(lines):
    for l in lines:
        print(l, end=' ')
    print()    

#------------------------------------------------------------------------------
import shutil
def merge_files(gcovr_args, src, dst):
    if (not os.path.isfile(src)):
        exit("ERROR: Source file '{}' does not exist".format(src))
    if (not os.path.isfile(dst)):
        # Create the destination file with a empty json payload
        with open(dst, 'w') as fd:
            fd.write('{"files": [], "gcovr/format_version": "0.6"}')
        
    tmp_file = '__temp_combined_coverage.json'
    cmd = f'python -m gcovr {gcovr_args} -a {src} -a {dst} --json {tmp_file}'
    
    print(cmd)
    run_shell(cmd, True, f"ERROR: Failed to merge code coverage files {src} and {dst}")
    shutil.copyfile(tmp_file, dst)  # Copy src to dst before merging
    os.remove(tmp_file)  # Remove the temporary file
    print(f"Updated code coverage file: {dst}")