@echo off
setlocal

set type="%~1"
set execpath="./tests/bin/Debug/tests.exe"

if %type%=="" (
	echo Missing argument
	echo Usage: build, run, debug
	exit /b 1
)

if "%~2" NEQ "" (
	echo Error: Expected one eargument
	echo Usage: build, run, debug
	exit /b 1
)

if %type%=="build" (
	echo Building the project...
	premake5 gmake
	gnumake config=debug
	exit /b 0
) else if %type%=="run" (
	echo Building and running the project...
	premake5 gmake
	gnumake config=debug
	%execpath%

	exit /b 0
) else if %type%=="debug" (
	echo Building and running using debugger...
	premake5 gmake
	gnumake config=debug
	raddbg --run %execpath%

	exit /b 0
) else (
	echo Invalid argument: %type%
	echo Usage: build, run, debug
	exit /b 1
)

endlocal
