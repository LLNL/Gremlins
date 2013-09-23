#!/usr/bin/env python

import optparse
import sys
import os

def parseOptions():
    usage = "usage: %prog [options] <program> <injection_lib>"
    parser = optparse.OptionParser(usage)
    
    parser.add_option("-e","--error-rate", dest="RATE",
                      help="Set error rate (errors/hour).", default="0")
    
    parser.add_option("-i","--iter-back", dest="IT",
                      help="Set number of iterations to recover", default="0")
    
    parser.add_option("-t","--print-injection-time", dest="PRINT_INJ_TIMES",
                      action="store_true", default=False,
                      help="Print next injection time.")
    
    (options, args) = parser.parse_args()
    if len(sys.argv) == 1:
        parser.print_help()
        exit()
        
    return (options, args)


def main():
    # Parse options
    (options, args) = parseOptions()
    program = args[0]

    os.environ["LD_LIBRARY_PATH"] = args[1]

    # Set enviroment variables
    errorRate = options.RATE
    os.environ["FAULT_RATE"] = errorRate
    
    iterBack = options.IT
    os.environ["ROLLBACK_ITERATIONS"] = iterBack
    
    if (options.PRINT_INJ_TIMES == True):
        os.environ["PRINT_INJECTION_TIME"] = "1"
    
    # Run program
    os.system(program)
    
main()
