:: Build.bat
:: %1  Project name, e.g. BlinkSample
:: %2  Target platform, supported: mega,uno,win32,win64
:: %3  Compiler, supported: avrgcc, msvc
:: %4  Base software, supported: arduino, none
:: %4  test framework, supported: googletest, none
:: %6  Command, e.g. all, clean

:: Search directories for Makefile.gmk (upwards). Stop if found.
:: :SearchArduinoDir
:: IF EXIST Makefile.gmk GOTO AtArduinoDir
:: cd ..
:: GOTO SearchArduinoDir

:: :AtArduinoDir

:: Prepare Microsoft C++ toolset if required.
:: See https://learn.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-170
:: Use if /I "%3" == "msvc" if case insensitive comparison is required.
if "%3" == "msvc" (
  if "%2" == "win32" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
  )
  if "%2" == "win64" (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
  )
)

:: make -k (or --keep-going)
::   Continue as much as possible after an error. While the target that failed, 
::   and those that depend on it, cannot be remade, the other prerequisites of 
::   these targets can be processed all the same.
:: MS Visual Studio: current directory is project directory (Arduino.vcproj)
make -s -f ./Build/make/Makefile.gmk ARG_PROJECT=%1 ARG_TARGET_ECU=%2 ARG_COMPILER=%3 ARG_BSW=%4 ARG_TESTFRAMEWORK=%5 %6
