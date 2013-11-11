::
:: Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
:: All rights reserved.
:: This component and the accompanying materials are made available
:: under the terms of "Eclipse Public License v1.0"
:: which accompanies this distribution, and is available
:: at the URL "http://www.eclipse.org/legal/epl-v10.html".
::
:: Initial Contributors:
:: Nokia Corporation - initial contribution.
::
:: Contributors:
::
:: Description:
::

@echo off

set QM_FILE_US=fmradio_en_US.qm
set QM_FILE=fmradio_en.qm
set QM_LOCATION=..\..\bin\%QM_FILE_US%

ECHO Creating QM file
call lrelease -idbased fmradio.ts -qm %QM_LOCATION%

IF EXIST \epoc32\tools\epoc.bat (
    echo Copying files to epoc32
    xcopy /y fmradio.ts \epoc32\include\platform\qt\translations\fmradio_en.ts
    xcopy /y fmradio.ts \epoc32\include\platform\qt\translations\fmradio_en_US.ts
    
    xcopy /y %QM_LOCATION% \epoc32\data\z\resource\qt\translations\%QM_FILE%
    xcopy /y %QM_LOCATION% \epoc32\release\winscw\udeb\z\resource\qt\translations\%QM_FILE%
    xcopy /y %QM_LOCATION% \epoc32\release\winscw\urel\z\resource\qt\translations\%QM_FILE%
    xcopy /y %QM_LOCATION% \epoc32\data\z\resource\qt\translations\%QM_FILE_US%
    xcopy /y %QM_LOCATION% \epoc32\release\winscw\udeb\z\resource\qt\translations\%QM_FILE_US%
    xcopy /y %QM_LOCATION% \epoc32\release\winscw\urel\z\resource\qt\translations\%QM_FILE_US%
)
