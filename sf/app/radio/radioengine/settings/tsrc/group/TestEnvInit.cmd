@REM
@REM Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
@REM All rights reserved.
@REM This component and the accompanying materials are made available
@REM under the terms of "Eclipse Public License v1.0"
@REM which accompanies this distribution, and is available
@REM at the URL "http://www.eclipse.org/legal/epl-v10.html".
@REM
@REM Initial Contributors:
@REM Nokia Corporation - initial contribution.
@REM
@REM Contributors:
@REM
@REM Description:  Wins Command file for Settings Test Module
@REM
@REM
@echo Initializing test environment for settingstestmodule...
copy ..\conf\SettingsTestModule.cfg \epoc32\wins\c\TestFramework\SettingsTestModule.cfg
copy ..\conf\SettingsTestModule.cfg \epoc32\winscw\c\TestFramework\SettingsTestModule.cfg
copy ..\conf\SettingsTestModule.cfg \epoc32\data\z\system\data\SettingsTestModule.cfg
copy ..\init\TestFramework.ini \epoc32\wins\c\TestFramework\TestFramework.ini
copy ..\init\TestFramework.ini \epoc32\winscw\c\TestFramework\TestFramework.ini
copy ..\init\TestFramework.ini \epoc32\data\z\system\data\TestFramework.ini
copy ..\conf\2002FF52.txt \epoc32\data\z\private\10202be9\2002FF52.txt
copy ..\conf\2002FF52.txt \epoc32\release\winscw\udeb\z\private\10202be9\2002FF52.txt
copy ..\conf\2002FF52.txt \epoc32\release\winscw\urel\z\private\10202be9\2002FF52.txt
del \epoc32\winscw\c\private\10202be9\persists\2002FF52.cre
@echo Test environment initialization for settingstestmodule... Finished.