#!/usr/bin/env python
##############################################################################
# Copyright (c) 2013, Lawrence Livermore National Security, LLC.
# Produced at the Lawrence Livermore National Laboratory
#
# Written by Martin Schulz et al <schulzm@llnl.gov>
# LLNL-CODE-645436
# All rights reserved.
#
# This file is part of the GREMLINS framework.
# For details, see http://scalability.llnl.gov/gremlins.
# Please read the LICENSE file for further information.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the disclaimer below.
#
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the disclaimer (as
#       noted below) in the documentation and/or other materials
#       provided with the distribution.
#
#     * Neither the name of the LLNS/LLNL nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE
# LIVERMORE NATIONAL SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##############################################################################

import optparse
import sys
import os

def parseOptions():
    usage = "usage: %prog [options] <program> <injection_lib/GREMLIN>"
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
    program=""
    if(args[1] !="" ):
    	program += "LD_PRELOAD="+args[1]
    program += " mpirun "+ args[0]

    print program
    #os.environ["LD_LIBRARY_PATH"] = args[1]
    #os.environ["LD_PRELOAD"] = args[1]

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
