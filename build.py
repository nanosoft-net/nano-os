# -*- coding: utf-8 -*-

'''

Copyright(c) 2017 Cedric Jimenez

This file is part of Nano-Build.

Nano-Build is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Nano-Build is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Nano-Build.  If not, see <http://www.gnu.org/licenses/>.

'''


####################################################
####Imports
import sys, os
from argparse import ArgumentParser


####################################################
####Data types


class BuildParameters(object):
    '''
        Contains the software parameters
    '''

    def __init__(self):
        '''
            Constructor
        '''

        self.root_dir = ""
        ''' Root directory '''
        
        self.target = ""
        ''' Build target '''

        self.packages = []
        ''' Packages to build '''

        self.list_packages = False
        ''' Indicate if the packages must be listed '''
        
        self.build_cmd = []
        ''' Build command '''

        self.header_dep = True
        ''' Header dependency check '''
        
        self.verbose = False
        ''' Verbose mode '''
        
        self.build_jobs = 1
        ''' Number of build jobs '''
        
        return



####################################################
####Functions

def checkargs(args, params):
    '''
        Check the command line parameters

        @param args: command line parameters
        @type args: string[]
        
        @param params: parameter storage class
        @type params: BuildParameters

        @return: if the parameters are valid : True, if not : False
        @rtype: boolean

    '''

    # Retrieve the parameters
    parser_description = "Nano-Build v1.0"
    parser = ArgumentParser(description=parser_description)
    parser.add_argument('-t', nargs=1, metavar='target', required=True,
                        help='[string] Target name')
    parser.add_argument('-p', nargs='+', metavar='package',
                        required=True, help='[string] Packages to build')
    parser.add_argument('-c', nargs='+', metavar='command',
                        required=True, help='[string] Build command, ex: all|all+|clean|clean+')
    parser.add_argument('-j', nargs=1, metavar='jobs',
                        required=False, help='[int] Number of build jobs')
    parser.add_argument('-l', action='store_true',
                        required=False, help='List all available packages and targets')
    parser.add_argument('-v', action='store_true',
                        required=False, help='Verbose mode')
    parser.add_argument('-d', action='store_true',
                        required=False, help='Header dependency check')

    # Check the parameters count
    if len(args) < 2:
        print "No input parameters specified"
        parser.print_help()
        return False
    
    # Display the software inline help
    if (("-h" in args) or ("--help" in args)):
        parser.print_help()
        return False
    
    # Parse the parameters 
    if ("-l" in args):
        params.list_packages = True
    else:              
        args = parser.parse_args()
        params.target = args.t[0]
        params.packages = args.p
        params.build_cmd = args.c
        if args.j != None:
            params.build_jobs = int(args.j[0])
        params.list_packages = args.l
        params.verbose = args.v
        params.header_dep = args.d
        

    # All parameters are valid
    return True

def listPackages(params, print_result):
    '''
        List available packages
        
        @param params: parameter storage class
        @type params: BuildParameters
        
        @param print_result: indicate if the package list must be displayed
        @type print_result: bool
        
    '''
    
    # List all packages
    build_dir = os.path.join(params.root_dir, "build")
    packages = []
    package_name = ""
    recursiveListPackages(packages, package_name, build_dir)
    
    # List all targets
    targets = []
    for dirname, dirnames, filenames in os.walk(build_dir):
        for filename in filenames:
            if filename.endswith(".target"):
                targets.append(filename[:len(filename)-7])
    
    # Print results
    if print_result:
        print ""
        print "Packages list:"
        for package in packages:
            print " - " + package
        print ""
        print "Target list:"
        for target in targets:
            print " - " + target
        
    return (packages, targets)

def recursiveListPackages(packages, package_name, current_dir):
    '''
        List the packages recursively
        
        @param packages: list of the packages
        @type packages: string[]
        @param package_name: current package name
        @type package_name: string
        @param current_dir: current directory
        @type current_dir: string
        
    '''
    
    for dirname, dirnames, filenames in os.walk(current_dir):
        if( ("makefile" in filenames) and ("makefile.inc" in filenames) ):
            packages.append(package_name[1:])
        else:
            if len(dirnames) != 0:
                for dir in dirnames:
                    package = package_name + "." + dir
                    recursiveListPackages(packages, package, os.path.join(dirname, dir))
        break
    
    return

def buildPackages(params):
    '''
        Build selected packages
        
        @param params: parameter storage class
        @type params: BuildParameters
        
    '''
    
    successful_build_count = 0
    total_build_count = 0
    
    for package in params.packages:
        
        # Check if package name contains wildcard
        packages_to_build = []
        if package.endswith("*"):
            (packages, targets) = listPackages(params, False)
            package = package[:len(package) - 1]
            for pack in packages:
                if pack.startswith(package):
                    packages_to_build.append(pack)
        else:
            packages_to_build.append(package)
            
        # Build packages
        for package_to_build in packages_to_build:
            total_build_count = total_build_count + 1
            print ""
            try:
                
                # Set build directory
                dir = os.path.join(params.root_dir, "build", package_to_build.replace('.', os.sep))
                
                # Construct the build command
                command = "make -C " + dir + " -k" + " TARGET=" + params.target
                if params.build_jobs > 1:
                    command = command + " -j " + str(params.build_jobs) + " --output-sync=target"
                for cmd in params.build_cmd:
                    command = command + " " + cmd
                if params.header_dep:
                    command = command + " HEADER_DEP=yes"
                if params.verbose:
                    command = command + " DISPLAY=yes"
                    print command
                else:
                    command = command + " --no-print-directory"
                ret = os.system(command)
                if ret == 0:
                    successful_build_count = successful_build_count + 1
                
            except:
                print ""
                print "=> Invalid package : " + package
            
    print ""
    print "=> Success : " + str(successful_build_count) + "/" + str(total_build_count)

    if( successful_build_count == total_build_count ):
        ret = True
    else:
        ret = False
        
    return ret

####################################################
####Software entry point

if  __name__ == '__main__':

    params = BuildParameters()
    params.root_dir = os.path.dirname(os.path.abspath(__file__))

    # Check the command line parameters
    if not checkargs(sys.argv, params):
        sys.exit(-1)

    # Check action
    if params.list_packages:
        
        listPackages(params, True)
        ret = True
        
    else:
        
        ret = buildPackages(params)
        
    if ret:
        sys.exit(0)
    else:
        sys.exit(-1)

       

