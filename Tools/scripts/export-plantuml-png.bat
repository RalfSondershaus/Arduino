rem ============================================================
rem export-plantuml-png.bat
rem Exports all PlantUML (.puml) diagrams found under
rem docs\components to PNG files using plantuml.jar.
rem Download plantuml.jar from: https://plantuml.com/download.
rem ============================================================
@echo off
setlocal

rem Project root (script is in Tools/scripts)
set ROOT=%~dp0..\..

rem Input and output folders
set IN_DIR=%ROOT%\docs\components
set OUT_DIR=%ROOT%\docs\components

rem Path to plantuml.jar (adjust to your machine)
set PLANTUML_JAR=W:\Tools\plantuml\plantuml-1.2026.2.jar

if not exist "%OUT_DIR%" mkdir "%OUT_DIR%"

rem Optional: if dot.exe is not in PATH, uncomment and set:
rem set GRAPHVIZ_DOT=C:\Program Files\Graphviz\bin\dot.exe

echo Exporting PlantUML to PNG...
java -Djava.awt.headless=true -jar "%PLANTUML_JAR%" -tpng -charset UTF-8 -o "%OUT_DIR%" "%IN_DIR%\**\*.puml"

if errorlevel 1 (
  echo Export failed.
  exit /b 1
)

echo Done. PNG files are in:
echo %OUT_DIR%
exit /b 0