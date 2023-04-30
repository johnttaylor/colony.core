""" Utility functions for outputting messages/information that get sent to 
    stdout as well as the log file
"""
import config

g_verbose = False
g_debug = False
g_fdout = None
g_logout = None
g_ident_string = ""

#------------------------------------------------------------------------------
def write(string, log_only=False, console_only=False ):
    """ Write/appends 'string' to the output stream.  See the truth table
        below for output behaivor.

                log_only    console_only    OUTPUT
                --------    ------------  | ------
                 false        false       | fdout and logout
                 true         false       | logout
                 false        true        | fdout
                 true         true        | NO output
    """
    if (g_fdout != None and log_only == False ):
        g_fdout.write(string)
        g_fdout.flush()
    if (g_logout != None and console_only == False ):
        g_logout.write(bytes(string,'utf-8'))
        g_logout.flush()

#
def writeline(string, log_only=False, console_only=False):
    """ Write/appends 'string' to the output stream AND adds a trailine
        newline. Note: 'log_only' must be false to output the string to the 
        console.
    """
    if (g_fdout != None and log_only == False):
        g_fdout.write(string + "\n")
    if (g_logout != None and console_only == False):
        g_logout.write(bytes(string + "\n", 'utf-8'))

#
def write_verbose(string, log_only=False, console_only=False):
    """ Same as write(), except the output is only 'enabled' when verbose
        output has been enabled.
    """

    if (g_verbose):
        write(string,log_only, console_only)

#
def writeline_verbose(string, log_only=False, console_only=False):
    """ Same as writeline(), except the output is only 'enabled' when verbose
        output has been enabled.
    """
    if (g_verbose ):
        writeline(string,log_only, console_only)

#
def write_debug(string, log_only=False, console_only=False):
    """ Same as write(), except the output is only 'enabled' when debug
        output has been enabled.
    """

    if (g_debug):
        write(string,log_only, console_only)

#
def writeline_debug(string, log_only=False, console_only=False):
    """ Same as writeline(), except the output is only 'enabled' when debug
        output has been enabled.
    """

    if (g_debug):
        writeline(string,log_only, console_only)


#------------------------------------------------------------------------------
def write_entry(module_name, function_name="", format_string=config.g_entry_banner, indent_prefix=config.indent_prefix):
    """ Writes the formated string - with the specified module/function name.
        The format string should assume that indent string is the first argument,
        the module_name is the second argument, and function_name is the third
        argument.

        The write_entry() and write_exit() calls MUST always been done pairs!
    """

    global g_ident_string
    writeline(format_string.format(g_ident_string, module_name, function_name))
    g_ident_string += indent_prefix

def write_exit(module_name, function_name="", format_string=config.g_exit_banner, indent_prefix=config.indent_prefix):
    """ Writes the formated string - with the specified module/funciton name.
        The format string should assume that indent string is the first argument,
        the module_name is the second argument, and function_name is the third
        argument

        The write_entry() and write_exit() calls MUST always been done pairs!
    """

    global g_ident_string
    g_ident_string = g_ident_string[:-len(indent_prefix)]
    writeline(format_string.format(g_ident_string, module_name, function_name))


#------------------------------------------------------------------------------
def set_verbose_mode(enabled):
    """ Enables/Disabled verbose output
    """

    global g_verbose
    g_verbose = enabled

#
def set_debug_mode(enabled):
    """ Enables/Disabled debug output
    """

    global g_debug
    g_debug = enabled

#
def set_output_fds(out_fd, log_fd):
    """ Sets/defines the opened file objects for console and log output
    """

    global g_fdout
    global g_logout
    g_fdout  = out_fd
    g_logout = log_fd


