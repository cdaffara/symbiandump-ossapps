@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description:
@rem
@echo off
if "%1"=="" goto usage
if "%2"=="" goto usage

@echo on

md \epoc32\wins\c\test\sms
md \epoc32\wins\c\logs\sms
md \epoc32\wins\c\logs\etel
md \epoc32\wins\c\logs\msgtest
md \epoc32\wins\c\logs\schsend
md \epoc32\wins\c\logs\watcher

rd \smstest_%1 /s/q
md \smstest_%1 
md \smstest_%1\system\data
md \smstest_%1\system\libs
md \smstest_%1\test\sms
md \smstest_%1\logs\sms
md \smstest_%1\logs\msgtest
md \smstest_%1\logs\etel
md \smstest_%1\logs\schsend
md \smstest_%1\logs\watcher
copy \epoc32\release\%1\%2\t*.exe \smstest_%1
copy \epoc32\release\%1\%2\msvtestutils.dll \smstest_%1\system\libs
copy \epoc32\release\%1\%2\smstestutils.dll \smstest_%1\system\libs
copy \epoc32\wins\c\test\sms\*.* \smstest_%1\test\sms
REM copy \epoc32\wins\c\system\data\cdbv2.dat \smstest_%1\system\data\cdbv2.dat
REM copy \epoc32\wins\c\system\data\s*.esk \smstest_%1\system\data
REM copy \epoc32\wins\c\system\data\w*.esk \smstest_%1\system\data

@echo off

echo.
echo.
echo.
echo.
echo The SMCM test files are now stored in folder \smstest_%1
echo.
echo These files may now be copied onto a device for testing:
echo.
echo - Copy \smstest_%1\logs\ to c:\logs\ on the device
echo - Copy \smstest_%1\test\ to c:\test\ on the device
echo - Copy \smstest_%1\system\ to c:\system\ on the device
echo - The other files (including the test harnesses)
echo   may be left on the CF card.
echo.

goto end

:usage

@echo off
echo.
echo USAGE: test [platform] [variant]
echo.
echo e.g.:  test thumb urel
echo        test armi udeb
echo.

:end