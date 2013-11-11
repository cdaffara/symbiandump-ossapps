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



ECHO "Running ABLD reallyclean"
call abld reallyclean
ECHO "Removing Gallery files (glx*.*)"
call del /F /S %EPOCROOT%epoc32\glx*.*
ECHO "Removing Slideshow files (shw*.*)"
call del /F /S %EPOCROOT%epoc32\shw*.*
ECHO "Removing Gallery test files (t_*glx*.*)"
call del /F /S %EPOCROOT%epoc32\t_*glx*.*
ECHO "Removing Slideshow test files (t_*shw*.*)"
call del /F /S %EPOCROOT%epoc32\t_*shw*.*
ECHO "Removing Build directory, and object files in there"
call del /F /S /Q %EPOCROOT%epoc32\BUILD\Photos\*.*
call rmdir /S /Q %EPOCROOT%epoc32\BUILD\Photos