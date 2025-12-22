# Build.sh
# $1  Project name, e.g. App/BlinkSample
# $2  Target ECU, supported: mega,nano,win32,win64
# $3  Compiler, supported: avrgcc, gcc, msvc
# $4  Base software, supported: arduino, win, none
# $5  test framework, supported: googletest, unity, none
# $6  Command, e.g. all, clean

# make -k (or --keep-going)
#   Continue as much as possible after an error. While the target that failed, 
#   and those that depend on it, cannot be remade, the other prerequisites of 
#   these targets can be processed all the same.
# make -i (or --ignore-errors)
#   When you run make with the -i or --ignore-errors flag, errors are ignored in all recipes of all rules. 
#   A rule in the makefile for the special target .IGNORE has the same effect, if there are no prerequisites. 
#   This is less flexible but sometimes useful.
# make -d (or --debug[=options])
#   Print debugging information in addition to normal processing. The debugging information says which files
#   are being considered for remaking, which file-times are being compared and with what results, which files 
#   actually need to be remade, which implicit rules are considered and which are applied—everything interesting 
#   about how make decides what to do. The -d option is equivalent to ‘--debug=a’ (see below).

# After each shell invocation returns, make looks at its exit status. If the shell completed successfully 
# (the exit status is zero), the next line in the recipe is executed in a new shell; after the last line 
# is finished, the rule is finished.
# If there is an error (the exit status is nonzero), make gives up on the current rule, and perhaps on all rules.
# Sometimes the failure of a certain recipe line does not indicate a problem. For example, you may use 
# the mkdir command to ensure that a directory exists. If the directory already exists, mkdir will report an 
# error, but you probably want make to continue regardless.
#
# To ignore errors in a recipe line, write a �-� at the beginning of the line�s text (after the initial tab). 
# The �-� is discarded before the line is passed to the shell for execution.

# MS Visual Studio: current directory is project directory (Arduino.vcproj)
make -s -f ./Build/make/Makefile.gmk ARG_PROJECT=$1 ARG_TARGET_ECU=$2 ARG_COMPILER=$3 ARG_BSW=$4 ARG_BSW_TESTFRAMEWORK=$5 $6
