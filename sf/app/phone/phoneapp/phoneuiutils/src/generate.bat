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
rem Description: Generate string tables and move headers.
rem

perl %EPOCROOT%epoc32\tools\stringtable.pl .\phoneloggerviewcommands.st
perl %EPOCROOT%epoc32\tools\stringtable.pl .\phonestatestrings.st
perl %EPOCROOT%epoc32\tools\stringtable.pl .\phonestatestringsgsm.st

perl ..\internal\tools\add_copyright.pl

move /Y phoneloggerviewcommands.h  ..\inc\phoneloggerviewcommands.h
move /Y phonestatestrings.h ..\inc\phonestatestrings.h 
move /Y phonestatestringsgsm.h ..\inc\phonestatestringsgsm.h 