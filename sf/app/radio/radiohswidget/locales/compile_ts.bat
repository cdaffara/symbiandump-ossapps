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

set QM_FILE=radiohswidget.qm
set QM_FILE_EN_US=radiohswidget_en_us.qm
set QM_LOCATION=./


ECHO Creating QM file
call lrelease -idbased radiohswidget.ts -qm %QM_LOCATION%%QM_FILE%
REM call lrelease -idbased radiohswidget_en_us.ts -qm %QM_LOCATION%%QM_FILE_EN_US%
