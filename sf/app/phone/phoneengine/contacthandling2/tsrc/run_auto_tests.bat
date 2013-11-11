rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem

@echo off
setlocal
goto :startup 

::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: function DEFAULT
::
:: Modify this function
::  - TESTED_SRC can be changed to match tested code.
::    Specifies path from testcode group directory to
::    tested code
::  - Add test directories to be runned
::    ie. "call :build ut_projectdirectory" or use
::    "for /f %%a in ('dir /b ut_*') do call :build %%a"
::    which compiles, instruments and runs all test directories
::    starting with "ut_".
::::::::::::::::::::::::::::::::::::::::::::::::::::::
:DEFAULT
set TESTED_SRC=..\..\..\src\*

for /f %%a in ('dir /b ut_*') do call :build %%a
if [%DOMODULESTESTS%] EQU [TRUE] (
for /f %%a in ('dir /b mt_*') do call :build %%a
)


call :finish
goto :END

::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: function STARTUP
::
:: Initializing script
::::::::::::::::::::::::::::::::::::::::::::::::::::::
:STARTUP

set BUILDTESTS=TRUE
set RUNTESTS=TRUE
set REMOVETESTS=TRUE
set INSTRUMENT=TRUE
set REMOVEINSTRUMENT=TRUE
set DOMODULESTESTS=TRUE
set SBS_CALL=sbs --config winscw_udeb.test --keepgoing BUILD
set PATH_TO_DLL=\epoc32\release\winscw\udeb
set PATH_TO_MOVE_DLL=\epoc32\release\winscw\udeb\z\sys\bin
set PATH_TO_COVERAGE_DATA=\coverage_data


if not exist %PATH_TO_DLL%\eunitexerunner.exe  ( 
echo EUnit not installed! 
goto end
)

if not exist %PATH_TO_COVERAGE_DATA% (
mkdir %PATH_TO_COVERAGE_DATA%
)

if [%1] EQU [] ( goto default )

call :%1
call :%2
call :%3
call :%4
call :%5
echo Running tests = %RUNTESTS%
echo Instrumenting tests = %INSTRUMENT%
goto default

:/NOCLEANUP
set REMOVEINSTRUMENT=FALSE
set REMOVETESTS=FALSE
goto end

:/NORUN
set RUNTESTS=FALSE
set REMOVEINSTRUMENT=FALSE
set REMOVETESTS=FALSE
goto end

:/NOINSTRUMENT
set INSTRUMENT=FALSE
goto end

:/NOBUILD
set BUILDTESTS=FALSE
goto end

:/ONLYUNITTESTS
set DOMODULESTESTS=FALSE
goto end

::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: function FINISH
::
:: Finishes run_auto_tests.bat
::::::::::::::::::::::::::::::::::::::::::::::::::::::
:FINISH
call :move

if [%RUNTESTS%] EQU [TRUE] (
call :runtests
)

if [%REMOVETESTS%] EQU [TRUE] (
call :removetests
)

if [%INSTRUMENT%] EQU [TRUE] (
call :calculatecoverage
)

if [%REMOVEINSTRUMENT%] EQU [TRUE] (
call :removecoverage
)
goto end

::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: function BUILD
::    param directory
::
:: builds test from given directory
::::::::::::::::::::::::::::::::::::::::::::::::::::::
:BUILD
setlocal
if [%BUILDTESTS%] EQU [FALSE] ( goto end )
echo Building %1
if not exist %1 ( 
echo %1 Not found! 
goto end
)

pushd .
call cd %1\group
call sbs --config winscw_udeb.test --keepgoing CLEAN
if [%INSTRUMENT%] EQU [TRUE] (
call ctcwrap -n %PATH_TO_COVERAGE_DATA%\%1 -i d -C "EXCLUDE=*" -C "NO_EXCLUDE=%TESTED_SRC%" "%SBS_CALL%"
) else (
call %SBS_CALL%
)

popd
endlocal
goto end


::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: function MOVE
::
:: moves compiled tests to eunit directory
::::::::::::::::::::::::::::::::::::::::::::::::::::::
:MOVE
echo Moving tests
call move %PATH_TO_DLL%\ut_*.dll %PATH_TO_MOVE_DLL%
call move %PATH_TO_DLL%\mt_*.dll %PATH_TO_MOVE_DLL%
goto end


::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: function RUNTESTS
::
:: Runs tests in eunit directory
::::::::::::::::::::::::::::::::::::::::::::::::::::::
:RUNTESTS
echo Running tests
call %PATH_TO_DLL%\eunitexerunner.exe -dtextshell --
copy \epoc32\winscw\c\shared\EUnit\Logs\EUnit_log.xml \lastrun_EUnit_log.xml
goto end

::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: function CALCULATECOVERAGE
::
:: Calculates test coverage. Generates html
::::::::::::::::::::::::::::::::::::::::::::::::::::::
:CALCULATECOVERAGE
echo Calculating coverage
ctcpost %PATH_TO_COVERAGE_DATA%\*.sym -p - | ctcmerge -i - -o profile.txt
call ctc2html -t 70 -i profile.txt -o \coverage_result -nsb
goto end

::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: function REMOVETESTS
::
:: Removes tests from eunit directory
::::::::::::::::::::::::::::::::::::::::::::::::::::::
:REMOVETESTS
echo Removing Test dlls
call sleep 3
call del %PATH_TO_MOVE_DLL%\t_*.dll
call del %PATH_TO_MOVE_DLL%\mt_*.dll
call del %PATH_TO_MOVE_DLL%\ut_*.dll
call del %PATH_TO_MOVE_DLL%\it_*.dll
goto end

::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: function REMOVECOVERAGE
::
:: Removes data from coverage directory
::::::::::::::::::::::::::::::::::::::::::::::::::::::
:REMOVECOVERAGE
echo Removing deleting coverage data
call del %PATH_TO_COVERAGE_DATA%\*.sym
call del %PATH_TO_COVERAGE_DATA%\*.dat
call del profile.txt
goto end

::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: function END
::
:: Prints done
::::::::::::::::::::::::::::::::::::::::::::::::::::::
:END
echo Done!