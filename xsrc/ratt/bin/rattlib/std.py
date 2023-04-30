""" Basic/standard functions 
"""

import config
import utils
from rattlib import output


#------------------------------------------------------------------------------
def load( script_name, file_extension=config.g_ratt_file_extension ):
    """ Dynamically loads the specifies script. 'script_name' is a string of 
        script's file name WITHOUT a file extension. 'file_extension' appended
        to 'script_name' to complete the script's file name.
        
        The same script can be 'loaded' multiple times, however only one 
        instance of the module is imported into python.

        The method returns the 'module object' for the loaded script.
    """
    m,e = utils.import_file( script_name+file_extension, config.g_script_paths )
    if ( m == None ):
        output.writeline( "Error loading script: (). [{}]".format( e ) )
        return None

    return m

def run( script_name, current_passcode = config.g_passed, file_extension=config.g_ratt_file_extension ):
    """ Dynamically loads and then runs the script.  The method returns the
        pass/fail result of the script. 
        
        If 'current_passcode' is config.g_failed, nothing is done and 
        config.g_failed is returned.
    """
    if ( current_passcode != config.g_passed ):
        output.writeline_verbose( f"INFO: Skipping running {script_name} because passcode is 'failed'" )
        return config.g_failed

    tc = load(script_name, file_extension)
    if ( tc == None ):
        return config.g_failed 
    
    r = config.g_failed
    try:
        output.writeline_verbose( f"INFO: running {script_name}" )
        r = tc.run()
    except Exception as e:
        output.writeline(f'Error: Script {script_name} failed to run. {e}')
        
    return r

#
def shell( cmd ):
    """ Executes the specified command. 'cmd' is string of that contains the 
        command with any optional arguments.
        
        The method returns a tuple with the exit code and output from the 
        command.
    """    
    exitcode, result = utils.run_shell( cmd )
    output.writeline_verbose( result )
    return exitcode, result