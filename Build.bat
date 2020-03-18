:: Build.bat
:: %1  Project name
:: %2  Command, e.g. all, clean

:: An argument that contains ‘=’ specifies the value of a variable: ‘v=x’ sets the value of the
:: variable v to x. If you specify a value in this way, all ordinary assignments of the same
:: variable in the makefile are ignored; we say they have been overridden by the command line
:: argument.

:: Search directories for Makefile.gmk. Stop if found.
:SearchArduinoDir
IF EXIST Makefile.gmk GOTO AtArduinoDir
cd ..
GOTO SearchArduinoDir

:AtArduinoDir
make -s -f ./Makefile.gmk ARG_PROJECT=%1 %2
